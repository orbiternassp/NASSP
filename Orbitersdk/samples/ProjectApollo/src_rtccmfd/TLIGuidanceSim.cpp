/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC S-IVB TLI Numerical Integrator and IGM Guidance

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

#include "TLIGuidanceSim.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "saturn.h"
#include "rtcc.h"

TLIGuidanceSim::TLIGuidanceSim(RTCC *rtcc, RTCCNIInputTable tablin, int &iretn, EphemerisDataTable *ephem, RTCCNIAuxOutputTable *aux, std::vector<double> *wtabl) :
	TABLIN(tablin), IRETN(iretn), EPHEM(ephem), AUX(aux), WTABL(wtabl)
{
	T = 0.0;
	TPR = 0.0;
	WT = 0.0;
	WTLRT = 0.0;
	PITCHG = 0.0;
	YAWG = 0.0;
	MPHASE = 0;
	DT = 0.0;
	DTPREV = 0.0;
	DVMAG = 0.0;
	DVTO = 0.0;
	DVX = 0.0;
	DVXTO = 0.0;
	for (int i = 0;i < 7;i++)
	{
		DTPHASE[i] = 0.0;
		FORCE[i] = 0.0;
		WTFLO[i] = 0.0;
	}

	hEarth = oapiGetObjectByName("Earth");
	EMU = OrbMech::mu_Earth;

	E = TABLIN.Params[0];
	C3 = TABLIN.Params[1];
	ALPHD = TABLIN.Params[2];
	F = TABLIN.Params[3];
	P = TABLIN.Params[4];
	XK5 = TABLIN.Params[5];
	RT = TABLIN.Params[6];
	VT = TABLIN.Params[7];
	GAMT = TABLIN.Params[8];
	GT = TABLIN.Params[9];
	PRP = TABLIN.Params[10];
	YRP = TABLIN.Params[11];
	T3 = TABLIN.Params[13];
	TAU3 = TABLIN.Params[14];
	T2 = TABLIN.Params2[0];
	VEX2 = TABLIN.Params2[1];
	WDOT2 = TABLIN.Params2[2];
	DVBIAS = TABLIN.Params2[3];
	TAU2N = TABLIN.Params2[4];

	VEX3 = 0.0;
	WDOT3 = 0.0;
	TB2 = 0.0;

	this->rtcc = rtcc;
}

void TLIGuidanceSim::PCMTRL()
{
	DTPHASE[0] = rtcc->MCTJD1;
	DTPHASE[2] = rtcc->MCTJD3;

	FORCE[0] = rtcc->MCTJT1;
	FORCE[1] = rtcc->MCTJT2;
	FORCE[2] = rtcc->MCTJT3;
	FORCE[3] = rtcc->MCTJT4;
	FORCE[5] = rtcc->MCTJT5;
	FORCE[6] = rtcc->MCTJT6;
	WTFLO[0] = rtcc->MCTJW1;
	WTFLO[1] = rtcc->MCTJW2;
	WTFLO[2] = rtcc->MCTJW3;
	WTFLO[3] = rtcc->MCTJW4;
	WTFLO[5] = rtcc->MCTJW5;
	WTFLO[6] = rtcc->MCTJW6;

	EPSL1 = rtcc->MCVEP1;
	EPSL2 = rtcc->MCVEP2;
	EPSL3 = rtcc->MCVEP3;
	EPSL4 = rtcc->MCVEP4;
	VEX3 = rtcc->MCVVX3;
	WDOT3 = rtcc->MCVWD3;
	TB2 = rtcc->MCVTB2;

	G = rtcc->PZTLIMAT.G;
	GG = rtcc->PZTLIMAT.GG;
	PLMB = rtcc->PZTLIMAT.EPH;

	IGMSKP = 0;
	IERR = 0;
	IEND = 0;
	TENDF = 0.0;
	IFIRST = 0;
	INITL = 0;
	IDOUBL = 0;
	ITUP = 0;
	IERRE = 0;
	TLOP = 0.0;
	TCOF = 0.0;
	PC = 0.0;
	CPR = 0.0;
	PITO = 0.0;
	YAWO = 0.0;
	TGO = 0.0;
	TCO = 0.0;
	ICO = 0;
	VSB1 = 0.0;
	VSB2 = 0.0;
	DT2P = 0.0;
	PITCHG = 0.0;
	YAWG = 0.0;

	MPHASE = 1;
	IMRS = -1;
	INPASS = 1;
	EPS = 10e-14*3600.0;
	TE = TABLIN.GMTI;
	WBM = TABLIN.CAPWT;
	WT = TABLIN.CAPWT;
	STEP = TABLIN.DTOUT;
	KTHSWT = 1;
	IPUTIG = 2;
	IPHFUL = 5;
	IPHTOF = 7;
	T = TABLIN.GMTI;
	TPR = TABLIN.GMTI;
	TLAST = TABLIN.GMTI;
	CDFACT = 0.0 * TABLIN.DENSMULT *TABLIN.Area;
	AOM = CDFACT / WT;
	DTPHASE[1] = TABLIN.Params2[5];
	DTPHASE[3] = T2 + (rtcc->MCVIGM - (DTPHASE[0] + DTPHASE[1] + DTPHASE[2]));
	TLARGE = T + 4096.0*3600.0;
	DTPHASE[4] = rtcc->MCTJDS;
	DTPHASE[5] = rtcc->MCTJD5;
	DTPHASE[6] = rtcc->MCTJD6;
	PITN = PRP;
	YAWN = YRP;
	TIG = TABLIN.GMTI + DTPHASE[0] + DTPHASE[1];
	TI = TABLIN.GMTI + DTPHASE[0];
	TIGM = TABLIN.GMTI + rtcc->MCVIGM;
	T = TABLIN.GMTI;
	KEHOP = abs(TABLIN.IEPHOP);
	RE = TABLIN.R;
	VE = TABLIN.V;
	RV = TABLIN.R_frozen;
	VV = TABLIN.V_frozen;
	TEND = TLARGE;
	if (TABLIN.Word68i[0] != 0)
	{
		ITUP = 1;
	}

	if (TABLIN.MANOP == 1)
	{
		//Initialize for the inertial attitude control mode
		IGMSKP = 1;
		TCO = TLARGE;
		//UT=
	}
	if (TABLIN.DTINP < 0)
	{
		IEND = 1;
		IGMSKP = 1;
		TEND = TABLIN.GMTI;
		TENDF = TABLIN.GMTI;
		TIG = TABLIN.GMTI;
	}

	if (rtcc->MCTIND == 1)
	{
		//Low thrust
		FORCE[4] = rtcc->MCTJTL;
		WTFLO[4] = rtcc->MCTJWL;
	}
	else
	{
		//High thrust
		FORCE[4] = rtcc->MCTJTH;
		WTFLO[4] = rtcc->MCTJWH;
	}

	//Ephemeris storage initialization
	if (KEHOP != 0)
	{
		JT = 1;
		KWT = 1;
		NITEMS = 7;
		IEPHCT = 0;
		TNEXT = TABLIN.GMTI + TABLIN.DTOUT;
	}
	//Double integration required
	if (TABLIN.IFROZN == 1)
	{
		IDOUBL = 1;
	}
	//Zero out auxiliary table
	if (TABLIN.KAUXOP != 0)
	{
		AUX->R_1 = TABLIN.R;
		AUX->V_1 = TABLIN.V;
		AUX->GMT_1 = TABLIN.GMTI;
		AUX->WTINIT = WBM;
	}
	if (TABLIN.DTINP > 0)
	{
		TEND = TIG + TABLIN.DTINP;
	}
	//Loop
	do
	{
		if (TABLIN.IEPHOP < 0)
		{
			if (TNEXT - TE < 0)
			{
				TE = TNEXT;
			}
		}
		if (IGMSKP == 0)
		{
			if (TIGM - TE <= 0)
			{
				TE = TIGM;
			}
		}
		if (TEND - TE <= 0)
		{
			if (MPHASE != IPHTOF)
			{
				TI = TEND;
			}
		}
		if (TI - TE <= 0)
		{
			TE = TI;
			if (TABLIN.IEPHOP > 0)
			{
				TNEXT = TI;
			}
		}
		if (TCO == 0.0)
		{
			PCMGN();
			if (IERR != 0)
			{
				goto PMMSIU_999;
			}
			if (TCO != 0.0)
			{
				goto PMMSIU_XFER1;
			}
		}
		DT = TE - T;
		TAU = TE;
		if (WT <= rtcc->MCVWMN)
		{
			IERR = 2;
			goto PMMSIU_999;
		}
		if (TABLIN.IEPHOP > 0)
		{
			if (T + DT > TNEXT)
			{
				if (TNEXT != TLOP)
				{
					TNEXT = T;
					PCMEP();
				}
			}
		}
		PCMRK();
		if (KEHOP != 0)
		{
			if (TNEXT == TLOP)
			{
				TNEXT = TLOP + TABLIN.DTOUT;
			}
			else
			{
				if (abs(TNEXT - T) <= EPS)
				{
					PCMEP();
				}
			}
		}
		if (abs(TIGM - T) <= EPS)
		{
			IGMSKP = 1;
		}
		if (TABLIN.KAUXOP != 0)
		{
			if (abs(TIG - T) <= EPS)
			{
				//Store auxiliary quantities for ignition
				AUX->R_BI = RE;
				AUX->V_BI = VE;
				AUX->GMT_BI = T;
				AUX->A_T = UT;
				AUX->X_B = UT;
				AUX->Y_B = unit(crossp(UT, -RE));
				AUX->Z_B = unit(crossp(UT, AUX->Y_B));
				AUX->WTENGON = WT;
			}
		}
		if (IEND != 0)
		{
			if (TENDF == T)
			{
				break;
			}
		}
		else
		{
			/*if (abs(TEND - TI) <= EPS)
			{
				goto PMMSIU_XCUT;
			}*/
			if (abs(TI - T) <= EPS)
			{
				if (abs(TEND - T) <= EPS)
				{
					MPHASE = IPHTOF;
					IEND = 1;
					TENDF = T + DTPHASE[MPHASE - 1];
					if (TCO == 0.0)
					{
						TCO = 4096.0*3600.0;
					}
				}
				else
				{
					MPHASE++;
				}
				KTHSWT = 1;
				TI = T + DTPHASE[MPHASE - 1];
			}
		}
	PMMSIU_XFER1:
		TE = TE + STEP;
		continue;
	} while (1 == 1);
	if (TABLIN.KAUXOP != 0)
	{
		//Store final auxiliary quantities
		AUX->R_BO = RE;
		AUX->V_BO = VE;
		AUX->GMT_BO = T;
		AUX->DT_TO = DTPHASE[6];
		double DTU = TIG - TABLIN.GMTI;
		AUX->DT_B = T - TABLIN.GMTI - DTU;
		AUX->P_G = PITCHG;
		AUX->Y_G = YAWG;
		AUX->DV_TO = DVTO;
		AUX->DV = DVMAG;
		AUX->DV_C = DVX;
		AUX->DV_cTO = DVXTO;
		AUX->DV_U = DVULL;
		AUX->W_CSM = TABLIN.CSMWT;
		AUX->W_LMA = TABLIN.LMAWT;
		AUX->W_LMD = TABLIN.LMDWT;
		AUX->W_SIVB = TABLIN.SIVBWT;
		AUX->WTEND = WT;
		AUX->MainFuelUsed = AUX->WTINIT - AUX->WTEND;
		AUX->RBI = BODY_EARTH;
		AUX->CSI = 0;
		AUX->Word60 = E;
		AUX->Word61 = C3;
		AUX->Word62 = ALPHD;
		AUX->Word63 = F;
		AUX->Word64 = P;
		AUX->Word65 = XK5;
		AUX->Word66 = RT;
		AUX->Word67 = VT;
		AUX->Word68 = GAMT;
		AUX->Word69 = GT;
	}
	if (KEHOP != 0)
	{
		//Store ephemeris and header information
	}
PMMSIU_999:
	IRETN = IERR;
	return;

}

void TLIGuidanceSim::PCMTH()
{
	double DW, ALOGC, AMAG, CPITG, CYAWG;

	if (KTHSWT > 0)
	{
		THRUST = FORCE[MPHASE - 1];
		WTLRT = WTFLO[MPHASE - 1];
		KTHSWT = -1;
		WTLRT = WTLRT * TABLIN.WDMULT;
	}
	if (T != TPR)
	{
		DW = WTLRT * (T - TPR);
		CPITG = cos(PITCHG);
		CYAWG = cos(YAWG);
		ALOGC = log(1.0 - DW / WT) / WTLRT;
		if (MPHASE >= IPHTOF)
		{
			DVTO = DVTO - THRUST * ALOGC;
			DVXTO = DVXTO - THRUST * CPITG*CYAWG*ALOGC;
		}
		else
		{
			DVX = DVX - THRUST * CPITG*CYAWG*ALOGC;
			if (MPHASE <= IPUTIG)
			{
				DVULL = DVULL - THRUST * ALOGC;
			}
		}
		DVMAG = DVMAG - THRUST * ALOGC;
		WT = WT - WTLRT * (T - TPR);
		AOM = CDFACT / WT;
		TPR = T;
	}
	AMAG = (THRUST / WT);
	RDDTH = UT * AMAG;
}

void TLIGuidanceSim::PCMEP()
{
	if (IEPHCT > TABLIN.MAXSTO)
	{
		IERRE = 4;
		TNEXT = TNEXT + TLARGE;
	}
	else
	{
		EphemerisData sv;

		sv.GMT = T;
		sv.R = RE;
		sv.V = VE;
		sv.RBI = BODY_EARTH;
		EPHEM->table.push_back(sv);

		if (KEHOP >= 2 && WTABL)
		{
			WTABL->push_back(T);
			WTABL->push_back(WT);
			KWT = KWT + 2;
		}
		JT = JT + NITEMS;
		TLOP = TNEXT;
		TNEXT = TNEXT + TABLIN.DTOUT;
		IEPHCT++;
	}
}

void TLIGuidanceSim::PCMRK()
{
	VECTOR3 RDD1, RDD2, RDD3, RDD4, TEMP1, RDDTH1, RDDTH2, RDDTH4;

	if (DT == 0.0)
	{
		return;
	}
	if (DT != DTPREV)
	{
		H = DT;
		H2 = 0.5*H;
		H4 = 0.5*H2;
		H6 = H / 6.0;
		DTPREV = DT;
	}
	TPREV = T;
	K3SWT = 0;
	if (IDOUBL == 1)
	{
		R = RV;
		V = VV;
	}
	else
	{
	PCMRK_LOOP:
		R = RE;
		V = VE;
	}
	RP = R;
	VP = V;
	RDD1 = PCMDC();
	if (IDOUBL != 1)
	{
		UT = THRSTM;
	}
	else
	{
		if (INPASS == 1)
		{
			RDDTH1 = RDDTH;
			UT = THRSTM;
		}
		else
		{
			RDDTH = RDDTH2;
		}
	}
	RP = R + (V + RDD1 * H4)*H2;
	VP = V + RDD1 * H2;
	T = T + H2;
	RDD2 = PCMDC();
	if (IDOUBL == 1)
	{
		if (INPASS == 1)
		{
			RDDTH2 = RDDTH;
		}
	}
	K3SWT = 1;
	VP = V + RDD2 * H2;
	RDD3 = PCMDC();
	K3SWT = 0;
	RP = R + (V + RDD3 * H2)*H;
	VP = V + RDD3 * H;
	T = TAU;
	if (IDOUBL != 1 || INPASS == 1)
	{
		UT = THRSTR;
	}
	else
	{
		RDDTH = RDDTH4;
	}
	RDD4 = PCMDC();
	TEMP1 = RDD2 + RDD3;
	R = R + (V + (RDD1 + TEMP1)*H6)*H;
	V = V + (RDD1 + TEMP1 * 2.0 + RDD4)*H6;
	if (IDOUBL == 1 && INPASS == 1)
	{
		RDDTH4 = RDDTH;
		RV = R;
		VV = V;
		INPASS = 2;
		RDDTH = RDDTH1;
		T = TPREV;
		goto PCMRK_LOOP;
	}
	RE = R;
	VE = V;
	INPASS = 1;
}

VECTOR3 TLIGuidanceSim::PCMDC()
{
	VECTOR3 RDD;
	if (KTHSWT == 0)
	{
		RDDTH = _V(0, 0, 0);
	}
	if (K3SWT == 0)
	{
		PVEC = OrbMech::gravityroutine(RP, hEarth, TABLIN.GMTBASE + T / 24.0 / 3600.0);
		if (KTHSWT != 0)
		{
			if (IDOUBL != 0 || INPASS == 1)
			{
				PCMTH();
			}
		}
	}

	RDD = PVEC;
	RDD = RDD + RDDTH;
	return RDD;
}

void TLIGuidanceSim::PCMGN()
{
	MATRIX3 MX_phi_T, MX_EKX;
	VECTOR3 RINP, VINP, X4, GS0D, GS1D, GS2D, TEMPY, AGS, UTA, THRSTL, AVG;
	double TEMP1, TEMP2, TEMP4, RMAG, VMAG, FOM, DDLT, CPP, SPP, T3P, TTOTP, XL3P, XJ3P, XLYP, PHIT, SGAMT, CGAMT, DELL2;
	double SGAM, CGAM, XIT, XITD, ZETATD, ZETATG, PHIDI, PHIDT, XITG, DXIDP, DETADP, DZETDP, DELL3, DELT3, XL3, XJ3, U3, P3, S3, Q3;
	double DXID, DETAD, DZETD, XLY, XBRY, XBRP, XJY, SY, QY, XKY, DY, DETA, XLP, C2, C4, XJP, QP, XKP, DP, DXI, XPRY, XPRP;
	double ZCP, ZSP, ZCY, ZSY, TTOT, SP;
	int IFLOP;

	if (T == TLAST)
	{
		if (T != TABLIN.GMTI)
		{
			if (T != TIGM)
			{
				return;
			}
		}
	}
	if (IDOUBL == 1)
	{
		RINP = RV;
		VINP = VV;
	}
	else
	{
		RINP = RE;
		VINP = VE;
	}
	RMAG = length(RINP);
	VMAG = length(VINP);
	if (MPHASE != IPHTOF && (abs(T - TIGM) <= EPS || T >= TIGM))
	{
		goto PMMSIU_PCMGN_1A;
	}
	if (!(T == TIG && ITUP == 1))
	{
		goto PMMSIU_PCMGN_20;
	}
	RN = P;
	E = RMAG * (E - 1.0) / RN + 1.0;
	TEMP1 = E * E;
	P = EMU * (TEMP1 - 1.0) / C3;
	XK5 = sqrt(EMU / P);
	TEMP4 = (1.0 - P / RT) / E;
	if (TEMP4 > 1.0)
	{
		//Error
	}
	goto PMMSIU_PCMGN_1B;
PMMSIU_PCMGN_1A:
	FOM = THRUST / WT;
	if (TCOF != 0.0)
	{
		return;
	}
	if (IFIRST == 0)
	{
		IFIRST = 1;
		TLAST = T;
	}
	DDLT = T - TLAST;
	goto PMMSIU_PCMGN_2A;
PMMSIU_PCMGN_1B:
	ALPHD = ALPHD - acos(TEMP4);
	TEMP2 = E * cos(F);
	RT = P / (1.0 + TEMP2);
	VT = XK5 * sqrt(1.0 + 2.0*TEMP2 + TEMP1);
	GAMT = E * sin(F);
	ITUP = 0;
	GT = -EMU / (RT*RT);
	if (TABLIN.KAUXOP != 0)
	{
		//Store target values in auxiliary table
	}
PMMSIU_PCMGN_20:
	PLPS = mul(PLMB, RINP);
	X4 = mul(GG, PLPS);
	double R4;
	R4 = sqrt(X4.x*X4.x + X4.z*X4.z);
	SPP = (-X4.x*1.0 - X4.z*0.0) / R4;
	CPP = (-X4.z*1.0 + X4.x*0.0) / R4;
	SYP = 0.0;
	CYP = 1.0;
	DDLT = T - TLAST;
	TLAST = T;
	AGS.x = CPP * CYP;
	AGS.y = SYP;
	AGS.z = -SPP * CYP;
	UTA = mul(OrbMech::tmat(GG), AGS);
	goto PMMSIU_PCMGN_330;
PMMSIU_PCMGN_2A:
	if (IMRS == 0)
	{
		goto PMMSIU_PCMGN_120;
	}
	else if (IMRS == -1)
	{
		T2 = T2 - DDLT;
		if (T2 > 0.0)
		{
			if (CPR >= rtcc->MCVCPQ)
			{
				TAU2 = VEX2 / FOM;
			}
			else
			{
				TAU2 = (VEX2 / FOM - DDLT * 0.5 - TAU2N)*pow(CPR / rtcc->MCVCPQ, 4);
				TAU2 = TAU2N + TAU2;
				TAU2N = TAU2N - DDLT;
				CPR = CPR + DDLT;
			}
		}
		else
		{
			PC = PC + DT;
			if (PC > rtcc->MCVKPC)
			{
				IMRS = 1;
				TB4 = 0.0;
			}
			TEMP1 = T2 * WDOT2 / WDOT3;
			T3 = T3 + TEMP1;
			TAU3 = TAU3 + TEMP1;
			T2 = 0.0;
		}
	}
	else
	{
		T2 = (WDOT2*(TB4 - TB2) - WDOT3 * TB4);
		TEMP1 = WDOT2 * TB2;
		T2 = T2 / TEMP1;
		if (TB2 >= TB4)
		{
			TB4 = TB4 + DT;
			TEMP1 = T2 * WDOT2 / WDOT3;
			T3 = T3 + TEMP1;
			TAU3 = TAU3 + TEMP1;
			T2 = 0.0;
		}
		else
		{
			IMRS = 0;
		PMMSIU_PCMGN_120:
			T3 = T3 - DDLT;
			TAU3 = VEX3 / FOM;
			T2 = 0.0;
		}
	}
	T3P = T3;
	TLAST = T;
	if (T2 != 0.0)
	{
		if (TAU2 < T2)
		{
			//Error exit
			goto PMMSIU_PCMGN_700;
		}
		TEMP1 = VEX2 * T2;
		TEMP2 = 0.5*TEMP1*T2;
		XL2 = VEX2 * log(TAU2 / (TAU2 - T2));
		XJ2 = XL2 * TAU2 - TEMP1;
		S2 = XL2 * T2 - XJ2;
		Q2 = S2 * TAU2 - TEMP2;
		P2 = XJ2 * TAU2 - TEMP2;
		U2 = Q2 * TAU2 - TEMP2 * T2 / 3.0;
	}
	else
	{
		if (XL2 != 0.0)
		{
			XL2 = 0.0;
			XJ2 = 0.0;
			S2 = 0.0;
			Q2 = 0.0;
			P2 = 0.0;
			U2 = 0.0;
		}
	}
	if (TAU3 < T3P)
	{
		//Error exit
		goto PMMSIU_PCMGN_700;
	}
	TTOTP = T2 + T3P;
	XL3P = VEX3 * log(TAU3 / (TAU3 - T3P));
	XJ3P = XL3P * TAU3 - VEX3 * T3P;
	XLYP = XL2 + XL3P;
	PLPS = mul(PLMB, RINP);
	X4 = mul(G, PLPS);
	CGAMT = cos(GAMT);
	IFLOP = 230;
PMMSIU_PCMGN_5A:
	PHIT = atan2(X4.z, X4.x);
	if (TTOTP > EPSL1)
	{
		DELL2 = (VMAG * TTOTP) - XJ3P + (XLYP * T3P) - (rtcc->MCVRQV / VEX3) *((TAU2 - T2) * XL2 + (TAU3 - T3P) * XL3P)*(XLYP + VMAG - VT);
		TEMP2 = (S2 + DELL2)*GAMT / RT;
	}
	else
	{
		SGAM = dotp(RINP, VINP) / (RMAG*VMAG);
		CGAM = sqrt(1.0 - SGAM * SGAM);
		PHIDI = VMAG * CGAM / RMAG;
		PHIDT = VT * CGAMT / RT;
		TEMP2 = 0.5*(PHIDI + PHIDT)*TTOTP;
	}
	PHIT = PHIT + TEMP2;
	if (TTOTP > EPSL3)
	{
		F = PHIT + ALPHD;
		TEMP1 = E * cos(F);
		TEMP2 = 1.0 + TEMP1;
		RT = P / TEMP2;
		VT = XK5 * sqrt(1.0 + 2.0*TEMP1 + E * E);
		TEMP1 = E * sin(F);
		GAMT = atan2(TEMP1, TEMP2);
		GT = -EMU / (RT*RT);
	}
	SGAMT = sin(GAMT);
	CGAMT = cos(GAMT);
	if (rtcc->MCVRQT == 1.0)
	{
		XIT = RT * CGAMT;
		ZETATD = VT;
		XITD = 0.0;
		ZETATG = GT * SGAMT;
		XITG = GT * CGAMT;
		PHIT = PHIT - GAMT;
	}
	else
	{
		XIT = RT;
		ZETATD = VT * CGAMT;
		XITD = VT * SGAMT;
		ZETATG = 0.0;
		XITG = GT;
	}
	MX_phi_T.m11 = (cos(PHIT));    MX_phi_T.m12 = 0; MX_phi_T.m13 = ((sin(PHIT)));
	MX_phi_T.m21 = 0;               MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
	MX_phi_T.m31 = (-sin(PHIT)); MX_phi_T.m32 = 0; MX_phi_T.m33 = (cos(PHIT));
	MX_K = mul(MX_phi_T, G);
	MX_EKX = mul(MX_K, PLMB);

	AVG = OrbMech::gravityroutine(RINP, hEarth, TABLIN.GMTBASE + T / 24.0 / 3600.0);

	GS0D = mul(MX_EKX, RINP);
	GS1D = mul(MX_EKX, VINP);
	TEMPY = mul(MX_EKX, AVG);
	GS2D.x = 0.5*(XITG + TEMPY.x);
	GS2D.y = 0.5*TEMPY.y;
	GS2D.z = 0.5*(ZETATG + TEMPY.z);
	DXIDP = XITD - GS1D.x - (GS2D.x*TTOTP);
	DETADP = -GS1D.y - GS2D.y*TTOTP;
	DZETDP = ZETATD - GS1D.z - GS2D.z*TTOTP;
	DELL3 = 0.5*((DXIDP*DXIDP + DETADP * DETADP + DZETDP * DZETDP) / XLYP - XLYP);
	DELT3 = DELL3 * (TAU3 - T3P) / VEX3;
	T3 = T3P + DELT3;
	TTOT = TTOTP + DELT3;

	if (IFLOP == 230)
	{
		T3P = T3;
		TTOTP = TTOT;
		XL3P = XL3P + DELL3;
		XLYP = XLYP + DELL3;
		XJ3P = XJ3P + DELL3 * T3;
		IFLOP = 240;
		goto PMMSIU_PCMGN_5A;
	}
	XL3 = XL3P + DELL3;
	XJ3 = XJ3P + DELL3 * T3;
	S3 = XL3 * T3 - XJ3;
	TEMP1 = VEX3 * T3*T3 / 2.0;
	TEMP2 = TAU3 + 2.0*T2;
	Q3 = S3 * TAU3 - TEMP1;
	P3 = XJ3 * TEMP2 - TEMP1;
	U3 = Q3 * TEMP2 - TEMP1 * T3 / 3.0;
	DXID = DXIDP - GS2D.x*DELT3;
	DETAD = DETADP - GS2D.y*DELT3;
	DZETD = DZETDP - GS2D.z*DELT3;
	XLY = XL2 + XL3;
	TEMP1 = DETAD / sqrt(DXID*DXID + DZETD * DZETD);
	XBRY = atan2(DETAD, sqrt(DXID*DXID + DZETD * DZETD));
	XBRP = atan2(DXID, DZETD);
	if (TTOTP <= EPSL2)
	{
		goto PMMSIU_PCMGN_2B;
	}
	TEMP2 = cos(XBRY);
	XJY = XJ2 + XJ3 + XL3 * T2;
	SY = S2 - XJ3 + XLY * T3;
	QY = Q2 + Q3 + S3 * T2 + XJ2 * T3;
	XKY = XLY / XJY;
	DY = SY - XKY * QY;
	DETA = GS0D.y + GS1D.y*TTOT + GS2D.y*TTOT * TTOT / 2.0 + SY * sin(XBRY);
	XK3 = DETA / (DY*TEMP2);
	XK4 = XKY * XK3;
	XLP = XLY * TEMP2;
	C2 = TEMP2 + XK3 * TEMP1;
	C4 = XK4 * TEMP1;
	TEMP2 = T2 * T2;
	XJP = XJY * C2 - C4 * (P2 + P3 + TEMP2 * XL3);
	SP = SY * C2 - C4 * QY;
	QP = QY * C2 - C4 * (U2 + U3 + TEMP2 * S3 + T3 * P2);
	XKP = XLP / XJP;
	DP = SP - XKP * QP;
	DXI = GS0D.x - XIT + GS1D.x*TTOT + GS2D.x*TTOT*TTOT / 2.0 + SP * sin(XBRP);
	XK1 = DXI / (DP*cos(XBRP));
	XK2 = XKP * XK1;
PMMSIU_PCMGN_300:
	XPRY = XBRY - XK3 + XK4 * DDLT;
	XPRP = XBRP - XK1 + XK2 * DDLT;
	CYP = cos(XPRY);
	SYP = sin(XPRY);
	CPP = cos(XPRP);
	SPP = sin(XPRP);
	//PMMSIU_PCMGN_310:
	AGS.x = SPP * CYP;
	AGS.y = SYP;
	AGS.z = CPP * CYP;
	UTA = mul(OrbMech::tmat(MX_K), AGS);
PMMSIU_PCMGN_330:
	PITN = atan2(-UTA.z, UTA.x);
	YAWN = asin(UTA.y);
	if (abs(YAWN) > rtcc->MCVYMX)
	{
		YAWN = rtcc->MCVYMX*(YAWN / abs(YAWN));
	}
	if (T == TABLIN.GMTI)
	{
		PITO = PITN;
		YAWO = YAWN;
	}
	if (DDLT == 0.0 || abs(YAWN - YAWO) / DDLT > rtcc->MCVYDL)
	{
		if (YAWN > YAWO)
		{
			YAWN = YAWO + rtcc->MCVYDL*DDLT;
		}
		else
		{
			YAWN = YAWO - rtcc->MCVYDL*DDLT;
		}
	}
	if (DDLT == 0.0 || abs(PITN - PITO) / DDLT > rtcc->MCVPDL)
	{
		if (PITN > PITO)
		{
			PITN = PITO + rtcc->MCVPDL*DDLT;
		}
		else
		{
			PITN = PITO - rtcc->MCVPDL*DDLT;
		}
	}
	ZCP = cos(PITN);
	ZSP = sin(PITN);
	ZCY = cos(YAWN);
	ZSY = sin(YAWN);
	UTA.x = ZCP * ZCY;
	UTA.y = ZSY;
	UTA.z = -ZSP * ZCY;
	PITO = PITN;
	YAWO = YAWN;
	THRSTM = unit(mul(OrbMech::tmat(PLMB), UTA));
	if (INITL == 0)
	{
		UT = THRSTL = THRSTR = THRSTM;
		INITL = 1;
	}
	else
	{
		THRSTL = THRSTR;
		THRSTR = THRSTM;
		THRSTM = THRSTL + THRSTR;
		THRSTM = unit(THRSTM);
		UT = THRSTL;
	}
	return;
PMMSIU_PCMGN_2B:
	if (XK1 != 0.0)
	{
		XK1 = 0.0;
		XK2 = 0.0;
		XK3 = 0.0;
		XK4 = 0.0;
		VTEST = VMAG;
		STEP = 2.0;
	}
	VTEST = 0.5*(VTEST + VMAG * VMAG / VTEST);
	VSB0 = VSB1;
	VSB1 = VSB2;
	VSB2 = VTEST;
	DT1P = DT2P;
	DT2P = DDLT;

	if (!(TTOTP <= EPSL4 && (VTEST + rtcc->MCVTGQ) >= VT))
	{
		goto PMMSIU_PCMGN_300;
	}
	if (TGO == 0.0)
	{
		TGO = T3;
	}
	else
	{
		TGO = TGO - DDLT;
	}
	double RTCO, VTCO;
	RTCO = RMAG + TGO * dotp(RINP, VINP) / RMAG;
	VTCO = sqrt(C3 + 2.0*EMU / RTCO);
	TEMP1 = VSB2 - VSB1;

	double A1, A2;
	A2 = DT2P * DT1P*(DT2P + DT1P);
	A2 = (TEMP1*DT1P - (VSB1 - VSB0)*DT2P) / A2;
	A1 = TEMP1 / DT2P + A2 * DT2P;
	TGO = ((VTCO - DVBIAS) - VSB2) / (A1 + A2 * TGO);
	UT = THRSTR;
	THRSTM = THRSTR;
	if (TGO > STEP)
	{
		goto PMMSIU_PCMGN_999;
	}
	if (TGO < 0.0)
	{
		TCO = T;
		ICO = 1;
	}
	else
	{
		TCO = T + TGO;
		ICO = 1;
	}
	if (TEND == TLARGE)
	{
		TEND = TCO;
	}
	return;
PMMSIU_PCMGN_700:
	IERR = 3;
PMMSIU_PCMGN_999:
	return;
}