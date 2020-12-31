/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC CSM and LM Numerical Integrator and Guidance

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

#include "CSMLMGuidanceSim.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "saturn.h"
#include "rtcc.h"

CSMLMPoweredFlightIntegration::CSMLMPoweredFlightIntegration(RTCC *r, PMMRKJInputArray &T, int &I, EphemerisDataTable *E, RTCCNIAuxOutputTable *A) :
	RTCCModule(r),
	TArr(T),
	IERR(I),
	Eph(E),
	Aux(A)
{
	TLARGE = 10e80;
}

void CSMLMPoweredFlightIntegration::PMMRKJ()
{
	VECTOR3 RSAVE, VSAVE;
	double TSAVE;

	PCINIT();

	//Thruster switch to 0
	KTHSWT = 0;
	//Error indicator to 0
	IERR = 0;
	//Initialize starting weight
	WT = TArr.CAPWT;
	//Initialize starting position vector
	R = RP = TArr.sv0.R;
	V = VP = TArr.sv0.V;
	T = 0.0;
	TPREV = T;
	DTPREV = 0.0;

	TLOP = TBM;
	TI = TBM;
	MPHASE = 1;

	TCO = 0.0;
	KEND = 0;
	TPREV = TBM;

	DVGO = abs(TArr.DVMAN);
	THRUST = THPS[MPHASE - 1];
	WTLRT = WDOTPS[MPHASE - 1] * TArr.WDMULT;
	THX = THRUST;

	//Calculate pure ullage time, total ullage minus overlap
	DTSPAN[0] = TArr.DTU - DTSPAN[8];
	if (DTSPAN[0] < 0.0)
	{
		DTSPAN[0] = 0.0;
	}
	TBI = TBM + DTSPAN[0];
	if (TArr.MANOP <= 2)
	{
		Tg = TLARGE;
	}
	else
	{
		Tg = TBI + DTSPAN[5];
	}

	RSAVE = R;
	VSAVE = V;
	TSAVE = T;

	TNEXT = TLARGE;
	DT = DTSPAN[0];
	TAU = TBI;

	//Integrate to Tau for PCGUID short burn logic
	PCRUNG(Eph, WeightTable);

	//Save free-flight state vector at ignition
	sv_ff.R = R;
	sv_ff.V = V;
	sv_ff.GMT = TArr.sv0.GMT + T;
	sv_ff.RBI = TArr.sv0.RBI;

	CalcBodyAttitude();

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS || TArr.MANOP > 3)
	{
		PCGUID();
	}

	KTHSWT = 1;

	R = RSAVE;
	V = VSAVE;
	T = TSAVE;
	if (TArr.KEPHOP != 0)
	{
		TNEXT = TArr.DTOUT;
	}

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
		goto PMMRKJ_LABEL_20A;
	}
	else
	{
		sv1.R = R;
		sv1.V = V;
		sv1.GMT = TArr.sv0.GMT + T;
		sv1.RBI = TArr.sv0.RBI;
		TEND = TLARGE;
		TBI = TBM;
		if (DTMAN > 0)
		{
			TI = TBM + DTMAN;
			TEND = TI;
			goto PMMRKJ_LABEL_9A;
		}
		else
		{
			if (TArr.DVMAN != 0)
			{
			PMMRKJ_LABEL_8B:
				TI = TLARGE;
				goto PMMRKJ_LABEL_9A;
			}
			else
			{
				if (TArr.MANOP == 4 || TArr.MANOP == 5)
				{
					if (KGN == 5)
					{
						goto PMMRKJ_LABEL_8C;
					}
					else
					{
						DTMAN = TLARGE;
						goto PMMRKJ_LABEL_8B;
					}
				}
				else
				{
				PMMRKJ_LABEL_8E:
					KGN = 5;
					goto PMMRKJ_LABEL_8C;
				}
			}
		}
	}

PMMRKJ_LABEL_8C:
	TBI = TBM;
	DT = 0.0;
	TI = T;
	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
		DTSPAN[7] = 0.0;
		goto PMMRKJ_LABEL_15B;
	}
	else
	{
		goto PMMRKJ_LABEL_12B;
	}

PMMRKJ_LABEL_9A:
	TE = T + STEP;
	if (TE > TNEXT)
	{
		TE = TNEXT;
	}
	if (T != Tg)
	{
		if (TE > Tg)
		{
			TE = Tg;
		}
	}
	else
	{
		IATT = 0;
	}
	if (TE <= TI)
	{
		DT = TE - T;
	}
	else
	{
		DT = TI - T;
	}
	if (DTMAN > 0)
	{
		goto PMMRKJ_LABEL_11A;
	}
	if (TArr.DVMAN == 0.0)
	{
		goto PMMRKJ_LABEL_8E;
	}
	if (TArr.DVMAN > 0)
	{
		T_c = -abs(THRUST);
	}
	else
	{
		T_c = -abs(THX);
	}
	DTGO = WT / WTLRT * (1.0 - exp(WTLRT*DVGO / T_c)) - DTSPAN[7];
	goto PMMRKJ_LABEL_11B;
PMMRKJ_LABEL_11A:
	if (MPHASE == 7)
	{
		goto PMMRKJ_LABEL_11C;
	}
	DTGO = TEND - T;
PMMRKJ_LABEL_11B:
	if (DTGO > DT)
	{
		goto PMMRKJ_LABEL_11C;
	}
	DT = DTGO;
	if (DT < 0)
	{
		DT = 0.0;
	}
	goto PMMRKJ_LABEL_12A;
PMMRKJ_LABEL_11C:
	if (TE < TI)
	{
		goto PMMRKJ_LABEL_21A;
	}
	else
	{
		TAU = TI;
		goto PMMRKJ_LABEL_12B;
	}

PMMRKJ_LABEL_12A:
	TAU = T + DT;
	TI = TAU;
	KEND = 1;
	if (MPHASE == 2 || MPHASE == 3)
	{
		DTUL = TAU - TBM;
	}
PMMRKJ_LABEL_12B:
	PCRUNG(Eph, WeightTable);
	if (KGN == 5 || IERR != 0)
	{
		goto PMMRKJ_LABEL_22C;
	}
	if (KEND == 0)
	{
		goto PMMRKJ_LABEL_15A;
	}
	KEND = 0;
	if (MPHASE == 1 || MPHASE == 8)
	{
		goto PMMRKJ_LABEL_22A;
	}
	MPHASE = 7;
PMMRKJ_LABEL_13B:
	TI = T + DTSPAN[7];
	THRUST = THRUST / THPS[9] * THPS[7];
	WTLRT = WTLRT / WDOTPS[9] * WDOTPS[7] * TArr.WDMULT;
	TEND = TLARGE;
PMMRKJ_LABEL_14B:
	MPHASE++;
PMMRKJ_LABEL_14C:
	THX = THRUST * cos(P_G)*cos(Y_G);
	KTHSWT = 1;
	if (KGN == 5)
	{
		goto PMMRKJ_LABEL_12B;
	}
	goto PMMRKJ_LABEL_9A;
PMMRKJ_LABEL_15A:
	if (MPHASE == 1)
	{
		goto PMMRKJ_LABEL_15B;
	}
	else if (MPHASE == 2)
	{
		goto PMMRKJ_LABEL_18B;
	}
	else if (MPHASE == 3)
	{
		goto PMMRKJ_LABEL_18D;
	}
	else if (MPHASE == 4)
	{
		goto PMMRKJ_LABEL_18C;
	}
	else if (MPHASE == 5 || MPHASE == 6)
	{
		goto PMMRKJ_LABEL_18E;
	}
	else if (MPHASE == 7)
	{
		goto PMMRKJ_LABEL_13B;
	}
	else
	{
		goto PMMRKJ_LABEL_22B;
	}
PMMRKJ_LABEL_15B:
	IJ = 0;
	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS && TArr.IC == 13 && TArr.LMDESCJETT <= TBM)
	{
		goto PMMRKJ_LABEL_15C;
	}
	goto PMMRKJ_LABEL_16A;
PMMRKJ_LABEL_15C:
	IJ = 1;
PMMRKJ_LABEL_16A:
	if (TArr.IC == 1)
	{
		WC = WT;
	}
	else if (TArr.IC == 4 || TArr.IC == 12)
	{
		WL = WT;
	}
	if (TArr.MANOP == 1)
	{
		A_T = A_T_in;
	}
	if (TArr.DVMAN > 0)
	{
		DTTOC = WDOTPS[7] / WDOTPS[9] * DTSPAN[7];
	}
	TI = T + DTSPAN[1];
	sv1.R = R;
	sv1.V = V;
	sv1.GMT = TArr.sv0.GMT + T;
	sv1.RBI = TArr.sv0.RBI;
	if (TArr.KTRIMOP == -1)
	{
		IA = 1;
		pRTCC->GIMGBL(WC, WL, P_G, Y_G, THRUST, WTLRT, TArr.ThrusterCode, TArr.IC, IA, IJ, TArr.DOCKANG);
		IA = -1;
	}
	else
	{
		IA = 0;
		pRTCC->GetSystemGimbalAngles(TArr.ThrusterCode, P_G, Y_G);
	}
	PGBI = P_G;
	YGBI = Y_G;
	goto PMMRKJ_LABEL_18F;
PMMRKJ_LABEL_18B:
	TI = T + DTSPAN[2];
PMMRKJ_LABEL_18F:
	THRUST = THPS[MPHASE];
	WTLRT = WDOTPS[MPHASE] * TArr.WDMULT;
	goto PMMRKJ_LABEL_14B;
PMMRKJ_LABEL_18D:
	if (TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
	PMMRKJ_LABEL_18C:
		TI = T + DTSPAN[MPHASE];
		goto PMMRKJ_LABEL_18F;
	}
	else
	{
	PMMRKJ_LABEL_18E:
		if (DTMAN <= 0)
		{
			TEND = TLARGE;
		}
	}
	TI = TEND;
	THRUST = THPS[9];
	WTLRT = WDOTPS[9] * TArr.WDMULT;
	MPHASE = 7;
	goto PMMRKJ_LABEL_14C;

PMMRKJ_LABEL_20A:
	if (KGN == 5)
	{
		goto PMMRKJ_LABEL_8C;
	}
	if (!(TArr.MANOP == 1 || TArr.MANOP == 2))
	{
		if (DTMAN <= 0)
		{
			if (TArr.DVMAN == 0.0)
			{
				DTMAN = TLARGE;
			}
		}
	}
	TI = TBI;
	TEND = TBI + DTMAN;
	if (DTSPAN[0] != 0.0)
	{
		goto PMMRKJ_LABEL_9A;
	}
	T = TBI;
	goto PMMRKJ_LABEL_15B;
PMMRKJ_LABEL_21A:
	TAU = TE;
	PCRUNG(Eph, WeightTable);
	if (IERR != 0)
	{
		goto PMMRKJ_LABEL_22C;
	}
	if (MPHASE == 8 || TCO == 0.0)
	{
		goto PMMRKJ_LABEL_9A;
	}
	TI = TCO;
	if (TI < T)
	{
		TI = T;
	}
	TEND = TCO;
	if (KGN == 4)
	{
		KGN = 0;
	}
	goto PMMRKJ_LABEL_9A;
PMMRKJ_LABEL_22A:
	DTMANE = T - TBI;
	goto PMMRKJ_LABEL_22C;
PMMRKJ_LABEL_22B:
	DTMANE = T - TBI - DTSPAN[7];
PMMRKJ_LABEL_22C:
	sv2.R = R;
	sv2.V = V;
	sv2.GMT = TArr.sv0.GMT + T;
	sv2.RBI = TArr.sv0.RBI;
	if (Aux && TArr.KAUXOP)
	{
		Aux->A_T = A_T_out;
		Aux->DT_B = DTMANE;
		Aux->DT_TO = DTTOC;
		Aux->DV = DV;
		Aux->DV_C = DVX;
		Aux->DV_cTO = DVTOX;
		Aux->DV_TO = DVTO;
		Aux->DV_U = DV_ul;
		Aux->P_G = PGBI;
		Aux->Y_G = YGBI;

		if (TArr.sv0.RBI == BODY_EARTH)
		{
			Aux->CSI = 0;
			Aux->RBI = BODY_EARTH;
		}
		else
		{
			Aux->CSI = 2;
			Aux->RBI = BODY_MOON;
		}
		Aux->R_1 = TArr.sv0.R;
		Aux->V_1 = TArr.sv0.V;
		Aux->GMT_1 = TArr.sv0.GMT;
		Aux->R_BI = sv1.R;
		Aux->V_BI = sv1.V;
		Aux->GMT_BI = sv1.GMT;
		Aux->R_BO = sv2.R;
		Aux->V_BO = sv2.V;
		Aux->GMT_BO = sv2.GMT;
		if (TArr.KEPHOP >= 1)
		{
			Eph->table.push_back(sv2);
		}
		Aux->sv_FF = sv_ff;
		Aux->V_G = VGN;
		Aux->X_B = X_B;
		Aux->Y_B = Y_B;
		Aux->Z_B = Z_B;
		Aux->WTENGON = WTENGON;
		Aux->WTEND = WT;
		Aux->MainFuelUsed = MAINFUELUSED;
		Aux->RCSFuelUsed = RCSFUELUSED;
		Aux->W_CSM = TArr.CSMWT;
		Aux->W_LMA = TArr.LMAWT;
		Aux->W_LMD = TArr.LMDWT;
		Aux->W_SIVB = TArr.SIVBWT;
	}
}

void CSMLMPoweredFlightIntegration::PCINIT()
{
	KGN = 0;
	STEP = 2.0;
	WTLIM = 0.0;
	DV = 0.0;
	DVX = 0.0;
	DVTOX = 0.0;
	DVTO = 0.0;
	P_G = 0.0;
	Y_G = 0.0;
	PGBI = 0.0;
	YGBI = 0.0;
	DV_ul = 0.0;
	X_B = Y_B = Z_B = _V(0, 0, 0);
	Kg = 0;
	IJ = 0;
	DTMANE = 0.0;
	A_T_in = TArr.AT;

	RCSFUELUSED = 0.0;
	MAINFUELUSED = 0.0;
	DTMAN = TArr.DTMAN;
	for (int i = 0;i < 10;i++)
	{
		THPS[i] = 0.0;
		WDOTPS[i] = 0.0;
	}
	for (int i = 0;i < 9;i++)
	{
		DTSPAN[i] = 0.0;
	}
	for (int i = 0;i < 4;i++)
	{
		XK[i] = 0.0;
	}

	IATT = 0;
	if (TArr.MANOP == 0)
	{
		return;
	}

	SIGN = 1.0;

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS4 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		SIGN = -1.0;
	}

	if (TArr.IC == 1)
	{
		WC = TArr.CSMWT;
		WL = 0.0;
		WS = 0.0;
	}
	else if (TArr.IC == 12)
	{
		WC = 0.0;
		WL = TArr.LMAWT + TArr.LMDWT;
		WS = 0.0;
	}
	else if (TArr.IC == 13)
	{
		WC = TArr.CSMWT;
		WL = TArr.LMAWT + TArr.LMDWT;
		WS = 0.0;
	}
	else if (TArr.IC == 3)
	{
		WC = TArr.CSMWT;
		WL = 0.0;
		WS = TArr.SIVBWT;
	}
	else if (TArr.IC == 14)
	{
		WC = 0.0;
		WL = TArr.LMAWT + TArr.LMDWT;
		WS = TArr.SIVBWT;
	}
	else
	{
		WC = TArr.CSMWT;
		WL = TArr.LMAWT + TArr.LMDWT;
		WS = TArr.SIVBWT;
	}

	AttGiven = false;

	if (TArr.MANOP < 0)
	{
		AttGiven = true;
		TArr.MANOP = -TArr.MANOP;
		if (TArr.MANOP != 2)
		{
			IATT = 1;
		}
	}

	if (TArr.MANOP > 4)
	{
		TArr.MANOP = 4;
	}

	double Thrust, isp;
	pRTCC->EngineParametersTable(TArr.ThrusterCode, Thrust, isp);

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSPLUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSPLUS4 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		THPS[0] = Thrust;
		WDOTPS[0] = Thrust / isp;

		THPS[6] = Thrust;
		THPS[9] = Thrust;

		WDOTPS[6] = Thrust / isp;
		WDOTPS[9] = Thrust / isp;

		DTSPAN[3] = 6.0;
		DTSPAN[5] = 4.0;
	}
	else if (TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSPLUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSPLUS4 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		THPS[0] = Thrust;
		WDOTPS[0] = Thrust / isp;

		THPS[6] = Thrust;
		THPS[9] = Thrust;

		WDOTPS[6] = Thrust / isp;
		WDOTPS[9] = Thrust / isp;

		DTSPAN[3] = 6.0;
		DTSPAN[5] = 4.0;
	}
	else if (TArr.ThrusterCode == RTCC_ENGINETYPE_LOX_DUMP)
	{
		THPS[0] = Thrust;
		WDOTPS[0] = Thrust / isp;

		THPS[6] = Thrust;
		THPS[9] = Thrust;

		WDOTPS[6] = Thrust / isp;
		WDOTPS[9] = Thrust / isp;

		DTSPAN[3] = TLARGE;
		DTSPAN[5] = TLARGE;
	}
	else if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS)
	{
		THPS[1] = pRTCC->MCTST2;
		THPS[2] = pRTCC->MCTST4;
		THPS[6] = Thrust;
		THPS[7] = pRTCC->MCTST4;
		THPS[9] = pRTCC->MCTST4;

		WDOTPS[1] = pRTCC->MCTSW2;
		WDOTPS[2] = pRTCC->MCTSW4;
		WDOTPS[6] = Thrust / isp;
		WDOTPS[7] = pRTCC->MCTSW4;
		WDOTPS[9] = pRTCC->MCTSW4;

		DTSPAN[1] = pRTCC->MCTSD2;
		DTSPAN[2] = pRTCC->MCTSD3;
		DTSPAN[3] = 6.0;
		DTSPAN[4] = 0.0;
		DTSPAN[5] = 4.0;
		DTSPAN[6] = 1.0;
		DTSPAN[7] = 0.0;
		DTSPAN[8] = pRTCC->MCTSD9;

		XK[0] = Thrust;
		XK[1] = 0.0;
		XK[2] = Thrust;
		XK[3] = 6975.34;
		if (TArr.UllageOption)
		{
			XK[3] *= 2.0;
		}
		if (TArr.DTU == 0.0)
		{
			XK[3] = 0.0;
		}
	}
	else if (TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS)
	{
		THPS[1] = pRTCC->MCTAT2;
		THPS[2] = pRTCC->MCTAT4;
		THPS[6] = Thrust;
		THPS[7] = pRTCC->MCTAT4;
		THPS[9] = pRTCC->MCTAT4;

		WDOTPS[1] = pRTCC->MCTAW2;
		WDOTPS[2] = pRTCC->MCTAW4;
		WDOTPS[6] = Thrust / isp;
		WDOTPS[7] = pRTCC->MCTAW4;
		WDOTPS[9] = pRTCC->MCTAW4;

		DTSPAN[1] = pRTCC->MCTAD2;
		DTSPAN[2] = pRTCC->MCTAD3;
		DTSPAN[3] = 6.0;
		DTSPAN[4] = 0.0;
		DTSPAN[5] = 4.0;
		DTSPAN[6] = 1.0;
		DTSPAN[7] = 0.1765;
		DTSPAN[8] = 0.5;

		XK[0] = pRTCC->MCTAK1;
		XK[1] = pRTCC->MCTAK2;
		XK[2] = pRTCC->MCTAK3;
		XK[3] = pRTCC->MCTAK4;

		if (TArr.DTU == 0.0)
		{
			XK[3] = 0.0;
		}
	}
	else if (TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
		THPS[1] = pRTCC->MCTDT2;
		THPS[2] = pRTCC->MCTDT3;
		THPS[3] = pRTCC->MCTDT4;
		THPS[4] = pRTCC->MCTDT5 * TArr.DPSScale;
		THPS[6] = Thrust;
		THPS[7] = pRTCC->MCTDT6 * TArr.DPSScale;
		THPS[9] = pRTCC->MCTDT6 * TArr.DPSScale;

		WDOTPS[1] = pRTCC->MCTDW2;
		WDOTPS[2] = pRTCC->MCTDW3;
		WDOTPS[3] = pRTCC->MCTDW4;
		WDOTPS[4] = pRTCC->MCTDW5 * TArr.DPSScale;
		WDOTPS[6] = THPS[6] / isp;
		WDOTPS[7] = pRTCC->MCTDW6 * TArr.DPSScale;
		WDOTPS[9] = pRTCC->MCTDW6 * TArr.DPSScale;

		DTSPAN[1] = pRTCC->MCTDD2;
		DTSPAN[2] = pRTCC->MCTDD3;
		DTSPAN[3] = 6.0;//TArr.DTPS10 - DTSPAN[2] - DTSPAN[1];
		DTSPAN[4] = pRTCC->MCTDD5;
		DTSPAN[5] = 4.0;
		DTSPAN[6] = pRTCC->MCTDD6;
		DTSPAN[7] = 0.38;
		DTSPAN[8] = 0.5;

		XK[3] = 6975.34;
		if (TArr.UllageOption)
		{
			XK[3] *= 2.0;
		}
		if (TArr.DTU == 0.0)
		{
			XK[3] = 0.0;
		}
	}

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
		if (TArr.UllageOption)
		{
			pRTCC->EngineParametersTable(RTCC_ENGINETYPE_LMRCSPLUS4, Thrust, isp);
		}
		else
		{
			pRTCC->EngineParametersTable(RTCC_ENGINETYPE_LMRCSPLUS2, Thrust, isp);
		}
		THPS[0] = Thrust;
		WDOTPS[0] = Thrust / isp;
	}
	else if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS)
	{
		if (TArr.UllageOption)
		{
			pRTCC->EngineParametersTable(RTCC_ENGINETYPE_CSMRCSPLUS4, Thrust, isp);
		}
		else
		{
			pRTCC->EngineParametersTable(RTCC_ENGINETYPE_CSMRCSPLUS2, Thrust, isp);
		}
		THPS[0] = Thrust;
		WDOTPS[0] = Thrust / isp;
	}

	if (TArr.DTU > 0)
	{
		THPS[1] += THPS[0];
		WDOTPS[1] += WDOTPS[0];
		if (TArr.ThrusterCode != RTCC_ENGINETYPE_LMDPS)
		{
			THPS[2] += THPS[0];
			WDOTPS[2] += WDOTPS[0];
		}
	}

	DTTOC = DTSPAN[7];
}

void CSMLMPoweredFlightIntegration::PCRUNG(EphemerisDataTable *E, std::vector<double> &W)
{
	VECTOR3 RDDP1, RDDP2, RDDP3;
	if (DT != DTPREV)
	{
		DT2 = DT / 2.0;
		DT4 = DT / 4.0;
		DT6 = DT / 6.0;
		DTPREV = DT;
	}

	KCODE = 1;
	PCRDD();
	if (KGN == 4)
	{
		return;
	}

	if (T == TBI)
	{
		A_T_out = A_T;
		DV_ul = DV;
		WTENGON = WT;
		if (TArr.MANOP >= 3)
		{
			VGN = VG;
		}
	}
	if (DT == 0.0)
	{
		goto PCRUNG_LABEL_4B;
	}

	//PCRUNG_LABEL_2B:
	TPREV = T;
	//ALTPR = ALT;
	T = T + DT2;

	RDDP1 = RDD;
	//RPREV = RP;
	//VPREV = VP;
	RP = R + (V + RDDP1 * DT4)*DT2;
	VP = V + RDDP1 * DT2;
	KCODE = 2;
	PCRDD();
	RDDP2 = RDD;
	VP = V + RDDP2 * DT2;
	KCODE = 3;
	PCRDD();
	T = TAU;
	RDDP3 = RDD;
	RP = R + (V + RDDP3 * DT2)*DT;
	VP = V + RDDP3 * DT;
	KCODE = 4;
	PCRDD();

	R = R + (V + (RDDP1 + RDDP2 + RDDP3)*DT6)*DT;
	RP = R;
	V = V + (RDDP1 + (RDDP2 + RDDP3)*2.0 + RDD)*DT6;
	VP = V;

PCRUNG_LABEL_4B:
	//Time to store some data
	if (T == TNEXT)
	{
		EphemerisData sv;

		sv.R = R;
		sv.V = V;
		sv.RBI = TArr.sv0.RBI;
		sv.GMT = TArr.sv0.GMT + T;
		E->table.push_back(sv);
		TLOP = TNEXT;
		TNEXT = TNEXT + TArr.DTOUT;
		if (TArr.KEPHOP == 2)
		{
			W.push_back(WT);
		}
	}

	if (TArr.MANOP != 0)
	{
		if (WT <= WTLIM)
		{
			IERR = 2;
		}
	}
}

void CSMLMPoweredFlightIntegration::PCRDD()
{
	double TL, WDOT;

	//Compute gravitational acceleration
	VECTOR3 r_p_ddot = OrbMech::gravityroutine(RP, pRTCC->GetGravref(TArr.sv0.RBI), pRTCC->GetGMTBase() + (TArr.sv0.GMT + T) / 24.0 / 3600.0);
	//Compute drag acceleration
	VECTOR3 r_d_ddot = _V(0, 0, 0);
	//Sum drag and gravity termins
	RDDP = r_p_ddot + r_d_ddot;
	if (KTHSWT == 0)
	{
		RDDT = _V(0, 0, 0);
		RDD = RDDP + RDDT;
		return;
	}
	if (KCODE == 3)
	{
		RDD = RDDP + RDDT;
		return;
	}

	if (MPHASE == 1)
	{
		if (KTHSWT < 0)
		{
			goto PCRDD_LABEL_9A;
		}
	}
	else
	{
		pRTCC->GIMGBL(WC, WL, P_G, Y_G, TL, WDOT, TArr.ThrusterCode, TArr.IC, IA, IJ, TArr.DOCKANG);
		goto PCRDD_LABEL_6A;
	}

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS)
	{
		goto PCRDD_LABEL_3B;
	}
	if (TArr.MANOP == 4 || TArr.MANOP == 5)
	{
		if (IATT == 1)
		{
			goto PCRDD_LABEL_3B;
		}
		else
		{
			goto PCRDD_LABEL_7B;
		}
	}
	else if (TArr.MANOP == 1)
	{
		A_T = A_T_in;
	}
	else
	{
	PCRDD_LABEL_3B:
		A_T = X_B * SIGN;
	}

PCRDD_LABEL_3C:
	if (T != TPREV)
	{
		double DW = WTLRT * (T - TPREV);
		double A = log(1.0 - DW / WT) / WTLRT;
		DV = DV - A * abs(THRUST);
		WT = WT - DW;
		if (TArr.TVC == 1)
		{
			WC = WC - DW;
		}
		else if (TArr.TVC == 2)
		{
			WS = WS - DW;
		}
		else
		{
			WL = WL - DW;
		}
		if (MPHASE == 1)
		{
			RCSFUELUSED += DW;
		}
		else
		{
			MAINFUELUSED += DW;
		}
		if (MPHASE == 8)
		{
			DVTO = DVTO - A * abs(THRUST);
			DVTOX = DVTOX - A * abs(THX);
			AOM = CD / WT;
			TPREV = T;
		}
		else
		{
			DVX = DVX - A * abs(THX);
			if (DTMAN <= 0)
			{
				if (TArr.DVMAN > 0)
				{
					DVGO = abs(TArr.DVMAN) - DV;
				}
				else if (TArr.DVMAN < 0)
				{
					DVGO = abs(TArr.DVMAN) - DVX;
				}
			}
			AOM = CD / WT;
			TPREV = T;
		}
	}
	double a_T = THRUST / WT;
	RDDT = A_T * a_T;
	if (KTHSWT > 0)
	{
		KTHSWT = -KTHSWT;
	}

	RDD = RDDP + RDDT;
	return;

PCRDD_LABEL_6A:
	if (MPHASE == 7)
	{
		//THRUST = TL;
		//WTLRT = WDOT;
	}
	THX = THRUST * cos(P_G)*cos(Y_G);
	if (AttGiven)
	{
		if (MPHASE != 2)
		{
			goto PCRDD_LABEL_3C;
		}
		if (KTHSWT > 0)
		{
			goto PCRDD_LABEL_7A;
		}
		goto PCRDD_LABEL_3C;
	}
	if (TArr.MANOP == 1)
	{
		goto PCRDD_LABEL_3C;
	}
	else if (TArr.MANOP == 2)
	{
		goto PCRDD_LABEL_7A;
	}
	if (MPHASE == 8)
	{
		goto PCRDD_LABEL_3C;
	}
	goto PCRDD_LABEL_7B;
PCRDD_LABEL_7A:
	MATRIX3 MATTEMP = mul(OrbMech::_MRz(Y_G), mul(OrbMech::_MRy(P_G), _M(X_B.x, X_B.y, X_B.z, Y_B.x, Y_B.y, Y_B.z, Z_B.x, Z_B.y, Z_B.z)));
	A_T = _V(MATTEMP.m11, MATTEMP.m12, MATTEMP.m13);
	goto PCRDD_LABEL_3C;
PCRDD_LABEL_7B:
	if (TCO != 0.0)
	{
	PCRDD_LABEL_7C:
		A_T = A_TR;
		goto PCRDD_LABEL_3C;
	}
	if (KCODE == 2 || KCODE == 3)
	{
		A_T = A_TM;
		goto PCRDD_LABEL_3C;
	}
	else if (KCODE == 4)
	{
		goto PCRDD_LABEL_7C;
	}
	if (Kg == 0)
	{
		if (T >= Tg)
		{
			Kg = 1;
			Tg = TLARGE;
			PCGUID();
		}
	}
	else
	{
		PCGUID();
	}
	A_T = A_TL;
	goto PCRDD_LABEL_3C;
PCRDD_LABEL_9A:
	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
		goto PCRDD_LABEL_3C;
	}
	if (TArr.MANOP == 1 || TArr.MANOP == 2)
	{
		goto PCRDD_LABEL_3C;
	}
	if (AttGiven)
	{
		goto PCRDD_LABEL_3C;
	}
	goto PCRDD_LABEL_7B;
}

void CSMLMPoweredFlightIntegration::PCGUID()
{
	double DTCO_apo, F, TGO, A, B, Vg, Vgo, V1, V2;

	if (T != TBI)
	{
		goto PCGUID_8_A;
	}

	double WTIG, TS, FAV, WDOTAV;

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
		DTCO_apo = WDOTPS[7] * DTSPAN[7] / WDOTPS[9];
	}
	else
	{
		DTSPAN[6] = 0.0;
		DTCO_apo = 0.0;
	}
	WTIG = WT - DTSPAN[0] * WDOTPS[0];

	if (TArr.ThrusterCode == RTCC_ENGINETYPE_LMDPS)
	{
		//10% thrust?
		F = THPS[3];
	}
	else
	{
		F = THPS[6];
	}
	TS = DTSPAN[3];
	FAV = THPS[6];
	WDOTAV = WDOTPS[6];
	if (TArr.ThrusterCode != RTCC_ENGINETYPE_LMDPS || TArr.MANOP > 2)
	{
		goto PCGUID_4_A;
	}
	if (DTMAN > 0)
	{
		TGO = DTMAN;
		goto PCGUID_5_C;
	}
	if (DV == 0.0)
	{
		return;
	}
	Vg = abs(DV);
	goto PCGUID_4_B;
PCGUID_3_B:
	V1 = Vgo - XK[0] / WTIG;
	if (V1 > 0)
	{
		goto PCGUID_3_C;
	}
	TGO = (WTIG*Vgo + XK[1]) / XK[2];
	goto PCGUID_5_E;
PCGUID_3_C:
	A = (DTSPAN[3] + DTSPAN[6]) / 2.0;
	B = DTSPAN[3] - DTSPAN[6];
	V2 = B * FAV / (WTIG - A * WDOTPS[6]);
	if (V1 > V2)
	{
		goto PCGUID_6_B;
	}
	TGO = DTSPAN[6] + B * V1 / V2;
	goto PCGUID_5_E;
PCGUID_4_A:
	Vg = length(VG);
	if (Vg > 0.0)
	{
		A_TR = VG / Vg;
		A_TM = A_TR;
		A_TL = A_TR;
		DTN = Tg - T;
	}
PCGUID_4_B:
	Vgo = Vg - XK[3] / WT;
	if (Vgo <= 0)
	{
		goto PCGUID_7_A;
	}
	if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMSPS || TArr.ThrusterCode == RTCC_ENGINETYPE_LMAPS)
	{
		goto PCGUID_3_B;
	}
	TGO = Vgo * WT / F;
PCGUID_5_C:
	if (TGO > TS)
	{
		if (TArr.DTPS10 > 0)
		{
			goto PCGUID_6_A;
		}
	PCGUID_5_B:
		DTSPAN[3] = abs(TArr.DTPS10) - DTSPAN[2] - DTSPAN[1];
		goto PCGUID_6_B;
	}
PCGUID_5_E:
	//Short burn logic, no guidance steering
	Tg = TLARGE;
	if (DTMAN > TGO)
	{
	PCGUID_5_D:
		DTMAN = TGO;
		goto PCGUID_5_B;
	}
	if (DTMAN > 0)
	{
		goto PCGUID_5_B;
	}
	goto PCGUID_5_D;
PCGUID_6_A:
	if (TGO >= DTSPAN[6])
	{
		goto PCGUID_5_B;
	}
	DTSPAN[3] = TLARGE;

PCGUID_6_B:
	Vn_apo = V;
	g_apo = RDDP;
	DTP = DTN;
	Vg_apo = Vg;
	return;
PCGUID_7_A:
	KGN = 5;
	if (AttGiven == false)
	{
		TArr.MANOP = 1;
	}
	return;
PCGUID_8_A:
	VECTOR3 g_av = (RDDP + g_apo) / 2.0;
	DTN = DT;
	VECTOR3 DVV = V - Vn_apo - g_av * DTP;
	VG = VG - DVV;
	Vg = length(VG);
	A = -WTLRT * Vg / THRUST;
	TGO = WT / WTLRT * (1.0 - exp(A)) - DTSPAN[7];
	if (TGO > DTSPAN[5])
	{
	PCGUID_8_B:
		A_T = VG / Vg;
		A_TL = A_TR;
		A_TM = A_TR + A_T;
		A_TR = A_T;
		A_TM = unit(A_TM);
		goto PCGUID_6_B;
	}
	TCO = T + TGO;
	goto PCGUID_8_B;
}

void CSMLMPoweredFlightIntegration::CalcBodyAttitude()
{
	VECTOR3 Y_T;
	double TTT = pRTCC->GetOnboardComputerThrust(TArr.ThrusterCode);

	if (AttGiven == false)
	{
		if (TArr.ExtDVCoordInd)
		{
			VG = pRTCC->PIEXDV(sv_ff.R, sv_ff.V, TArr.CAPWT, TTT, TArr.VG, true);
		}
		else
		{
			VG = TArr.VG;
		}
		A_T = unit(VG);

		Y_T = unit(crossp(A_T, sv_ff.R));

		if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSPLUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSPLUS4 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSPLUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSPLUS4)
		{
			X_B = A_T;
			if (TArr.HeadsUpDownInd)
			{
				Y_B = Y_T;
			}
			else
			{
				Y_B = -Y_T;
			}
		}
		else if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS4 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS4)
		{
			X_B = -A_T;
			if (TArr.HeadsUpDownInd)
			{
				Y_B = -Y_T;
			}
			else
			{
				Y_B = Y_T;
			}
		}
		else
		{
			if (TArr.HeadsUpDownInd == false)
			{
				Y_T = -Y_T;
			}
			VECTOR3 Z_T = crossp(A_T, Y_T);

			double Thr, WDOT;
			if (TArr.KTRIMOP == -1)
			{
				pRTCC->GIMGBL(TArr.CSMWT, TArr.LMAWT + TArr.LMDWT, P_G, Y_G, Thr, WDOT, TArr.ThrusterCode, TArr.IC, 1, IJ, TArr.DOCKANG);
			}
			else
			{
				pRTCC->GetSystemGimbalAngles(TArr.ThrusterCode, P_G, Y_G);
			}

			X_B = A_T * cos(P_G)*cos(Y_G) - Y_T * cos(P_G)*sin(Y_G) + Z_T * sin(P_G);
			Y_B = A_T * sin(Y_G) + Y_T * cos(Y_G);
		}

		Z_B = crossp(X_B, Y_B);
	}
	else
	{
		X_B = TArr.XB;
		Y_B = TArr.YB;
		Z_B = TArr.ZB;

		if (length(A_T_in) == 0.0)
		{
			if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSPLUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSPLUS4 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSPLUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSPLUS4)
			{
				A_T_in = X_B;
			}
			else if (TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_CSMRCSMINUS4 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS2 || TArr.ThrusterCode == RTCC_ENGINETYPE_LMRCSMINUS4)
			{
				A_T_in = -X_B;
			}
			else
			{
				//Fix this
				A_T_in = X_B;
			}
		}
	}
}