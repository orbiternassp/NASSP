/****************************************************************************
  This file is part of Shuttle FDO MFD for Orbiter Space Flight Simulator
  Copyright (C) 2019 Niklas Beug

  Launch Window Processor

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
  **************************************************************************/

#include "Orbitersdk.h"
#include "AnalyticEphemerisGenerator.h"
#include "rtcc.h"
#include "LWP.h"

LWPInputTable::LWPInputTable()
{
	LAZCOE[0] = 490.931*RAD;
	LAZCOE[1] = -20.131;
	LAZCOE[2] = -1.2501;
	LAZCOE[3] = 0.0975*DEG;

	NS = 0;
	DTOPT = 6.0*60.0;
	WRAP = 0;
	NEGTIV = 0;
	GAMINS = 0.0;
	PFT = 10.0*60.0 + 15.0;
	PFA = 18.8*RAD;
	RINS = 6528178.0; //EDD
	VINS = 7835.13; //EDD
	YSMAX = 14.0*RAD;
	LW = 2;
	STABLE = 2;
	LPT = 0;
	INSCO = 1;
	LOT = 1;
	DELNOF = true;
	TRANS = 0.0;
	DTGRR = 17.0;
	BIAS = 0.0;
	GMTLOR = 0.0;
	Pad = 1;
}

LaunchWindowProcessor::LaunchWindowProcessor(RTCC *r) : RTCCModule(r)
{
	TPLANE = 0.0;
	TYAW = 0.0;
	GSTAR = 0.0;
	DELNOD = 0.0;
	TSTART = 0.0;
	TEND = 0.0;
	TSTEP = 0.0;
	PAO = 0.0;
	PAC = 0.0;
	OPEN = 0.0;
	CLOSE = 0.0;
	TINS = 0.0;
	WEDGE = 0.0;
	IIGM = 0.0;
	AZL = 0.0;
	TIGM = 0.0;
	TDIGM = 0.0;
	DN = 0.0;
	PA = 0.0;
	DH = 0.0;
	error = 0;
}

void LaunchWindowProcessor::SetGlobalConstants(const LWPGlobalConstants &gl)
{
	GLOCON = gl;
}

void LaunchWindowProcessor::LWP(const LWPInputTable &in)
{
	inp = in;

	if (inp.Pad == 1)
	{
		LATLS = pRTCC->SystemParameters.MCLLTP[0];
		LONGLS = pRTCC->SystemParameters.MCLGRA;
	}
	else
	{
		LATLS = pRTCC->SystemParameters.MCLLLP[0];
		LONGLS = pRTCC->SystemParameters.MCLLPL;
	}

	RT = inp.RT;
	VT = inp.VT;
	TT = inp.TT;

	UPDAT(RT, VT, TT, TT);
	svtab.sv_T0 = aegdata;

	//Call LWPIN
	if (inp.LW != 1)
	{
		LWT();
		if (error)
		{
			lwsum.LWPERROR = error;
			return;
		}
		LWDSP();
		if (inp.LPT > 0)
		{
			LWPT();
		}
	}
	if (inp.LW != 0)
	{
		RLOT();
		if (error)
		{
			lwsum.LWPERROR = error;
			return;
		}
		RLOTD();
		LWPOT();
	}
}

void LaunchWindowProcessor::UPDAT(VECTOR3 &R, VECTOR3 &V, double &T, double T_des)
{
	PMMAEG pmmaeg;
	AEGBlock aeg;

	aeg.Data.coe_osc = OrbMech::GIMIKC(R, V, GLOCON.mu);
	aeg.Data.TS = T;
	aeg.Data.TE = T_des;
	aeg.Data.TIMA = 0;

	pmmaeg.CALL(aeg.Header, aeg.Data, aegdata);

	OrbMech::GIMKIC(aegdata.coe_osc, GLOCON.mu, R, V);
	T = aegdata.TE;
}

void LaunchWindowProcessor::LENSR(double GMTLO)
{
	OELEMENTS coe;
	MATRIX3 MATR1, MATR2, MATR3;
	VECTOR3 URLS, H, K, J;
	double psi, Gamma, DYAW, LONG;

	TP = GMTLO + inp.PFT;
	UPDAT(RT, VT, TT, TP);

	LONG = LONGLS + GLOCON.w_E*GMTLO;
	URLS = _V(cos(LATLS)*cos(LONG), cos(LATLS)*sin(LONG), sin(LATLS));

	H = unit(crossp(RT, VT));
	K = unit(crossp(H, URLS));
	J = crossp(K, H);

	psi = asin(dotp(H, URLS));
	WEDGE = asin(sin(psi) / cos(inp.PFA));
	Gamma = asin(sin(inp.PFA) / cos(psi));
	DYAW = abs(WEDGE) - inp.YSMAX;
	if (inp.YSMAX > 0.)
	{
		if (DYAW <= 0.)
		{
			Gamma = acos(cos(inp.PFA) / cos(psi));
			WEDGE = 0.;
		}
		else
		{
			double cosx, x, y, sinG;

			cosx = cos(inp.PFA) / cos(inp.YSMAX);
			x = acos(cosx);
			y = asin(sin(psi) / cosx);
			WEDGE = abs(y) - inp.YSMAX;
			sinG = sin(x) / cos(psi);
			Gamma = acos(sqrt(1.0 - sinG * sinG));
		}
	}
	MATR1 = _M(J.x, J.y, J.z, K.x, K.y, K.z, H.x, H.y, H.z);
	MATR2 = mul(_M(cos(Gamma), sin(Gamma), 0., -sin(Gamma), cos(Gamma), 0., 0., 0., 1.), MATR1);
	if (DYAW > 0.)
	{
		double phi;
		phi = psi / abs(psi)*WEDGE;
		MATR2 = mul(_M(cos(phi), 0., sin(phi), 0., 1., 0., -sin(phi), 0., cos(phi)), MATR2);
	}
	MATR3 = mul(_M(cos(inp.GAMINS), -sin(inp.GAMINS), 0., sin(inp.GAMINS), cos(inp.GAMINS), 0., 0., 0., 1.), MATR2);
	RP = _V(MATR2.m11, MATR2.m12, MATR2.m13)*inp.RINS;
	VP = _V(MATR3.m21, MATR3.m22, MATR3.m23)*inp.VINS;
}

void LaunchWindowProcessor::NPLAN()
{
	OELEMENTS coe;
	double UT, ETA, LAMBDA, LONG, DLON, dt;
	int ITER;

	ITER = 1;

	do
	{
		UPDAT(RT, VT, TT, TIP);

		UT = asin2(sin(LATLS) / sin(aegdata.coe_mean.i));
		if (inp.NS == 1)
		{
			UT = PI - UT;
		}

		ETA = acos(cos(UT) / cos(LATLS));
		if (aegdata.coe_mean.i > PI05)
		{
			ETA = -ETA;
		}

		LAMBDA = aegdata.coe_osc.h - GLOCON.w_E*TIP;
		LONG = LAMBDA + ETA;

		if (LONG < 0)
		{
			LONG += PI2;
		}
		DLON = LONG - LONGLS;
		while (DLON > PI) DLON -= PI2;
		while (DLON <= -PI) DLON += PI2;
		if (DLON < 0 && ITER == 1)
		{
			DLON += PI2;
		}

		dt = DLON / (GLOCON.w_E - aegdata.h_dot);
		TIP += dt;
		ITER++;
		if (ITER > PROCON.CMAX)
		{
			error = -1;
			return;
		}
	} while (abs(DLON) >= PROCON.DOS);
}

void LaunchWindowProcessor::LWT()
{
	double V, DV, TIP0;
	ITERSTATE iter;

	//Initial guess for liftoff time, current target state vector time
	TIP = TT;
	
LWT1:
	iter.dv = 0.0;
	//Find analytical inplane time
	NPLAN();
	//Store as base time
	TIP0 = TIP;
	do
	{
		TIP = TIP0 + iter.dv;
		LENSR(TIP);
		V = length(VP);
		DV = 2.0*V*sin(WEDGE / 2.0);

		iter.err = DV;

		if (abs(iter.err) > PROCON.DVTOL)
		{
			OrbMech::ITER(iter.c_I, iter.s_F, iter.err, iter.p_H, iter.dv, iter.erro, iter.dvo, PROCON.DX1);
			if (iter.s_F)
			{
				error = 3;
				return;
			}
		}
	} while (abs(iter.err) > PROCON.DVTOL);

	if (inp.NS != 1)
	{
		OPEN = TIP - inp.DTOPT;
		TINS = OPEN + inp.PFT;
		TPLANE = OPEN;
		LENSR(TPLANE);

		PAO = PHANG();
	}
	if (inp.NS == 2)
	{
		if (aegdata.coe_osc.i > LATLS)
		{
			inp.NS = 1;
			goto LWT1;
		}
	}
	if (inp.NS != 0)
	{
		CLOSE = TIP - inp.DTOPT;
		TINS = CLOSE + inp.PFT;
		TPLANE = CLOSE;
		LENSR(TPLANE);

		PAC = PHANG();
	}
	if (inp.STABLE == 1)
	{
		double TI, TF;
		if (inp.NS != 1)
		{
			TI = OPEN - inp.STARS;
			TF = OPEN + inp.STARE;
			GMTLS(TI, TF);
		}
		if (inp.NS != 0)
		{
			TI = CLOSE - inp.STARS;
			TF = CLOSE + inp.STARE;
			GMTLS(TI, TF);
		}
	}
}

double LaunchWindowProcessor::TTHET(double phase)
{
	return phase / aegdata.l_dot;
}

double LaunchWindowProcessor::PHANG()
{
	VECTOR3 u, R_A1, U_L;
	double phase;

	u = unit(crossp(RT, VT));
	U_L = unit(crossp(u, RT));

	R_A1 = unit(RP - u * dotp(RP, u))*length(RP);

	phase = acos(dotp(unit(R_A1), unit(RT)));
	if (dotp(U_L, R_A1) > 0)
	{
		phase = -phase;
	}

	return phase;
}

void LaunchWindowProcessor::GMTLS(double TI, double TF)
{
	double TLO, DT, phase;
	int ITER;

	K25 = 0;
	TLO = TI;

	while (TLO <= TF)
	{
		K25++;
		if (K25 > 10)
		{
			error = -1;
			return;
		}
		ITER = 0;
		do
		{
			LENSR(TLO);
			phase = PHANG();
			DT = TTHET(phase);
			ITER++;
			if (ITER > PROCON.CMAX)
			{
				error = -1;
				return;
			}
			TLO = TLO - DT;
		} while (abs(DT) > PROCON.DET);

		STAR[K25 - 1] = TLO;
		TLO = TLO + PI2 / aegdata.l_dot;
		if (inp.STABLE == 2)
		{
			TLO = TF + 60.0;
		}
	}

	GSTAR = STAR[K25 - 1];
}

void LaunchWindowProcessor::RLOT()
{
	double DALT, DTYAW, UINS, ANGLE, ALT, DHA, UOC;
	int ITER, ITINS, LAST;

	ITER = ITINS = 0;
	LAST = 1;
	DALT = DTYAW = 0.0;

	if (inp.WRAP != 0.0)
	{
		inp.OFFSET += PI2 * (double)inp.WRAP;
	}
	if (inp.INSCO != 1)
	{
		inp.VINS = sqrt(GLOCON.mu*(2.0 / inp.RINS - 1.0 / inp.ANOM)) + 10.0*0.3048;
	}
	switch (inp.LOT)
	{
	case 1:
	case 2:
	case 3:
		GMTLO = inp.GMTLOR;
		break;
	case 4:
	case 6:
		GMTLO = TPLANE;
		break;
	case 5:
		GMTLO = TPLANE + inp.TRANS;
		break;
	}
	GMTLS(GMTLO, GMTLO + 1.0); //The 1.0 doesn't matter as STABLE is set to 2
	do
	{
		do
		{
			ITINS = 0;
			do
			{
				switch (inp.LOT)
				{
				case 1:
				case 5:
				case 6:
					NSERT(RT, VT, TT, UINS);
					break;
				case 2:
					ITER = 0;
					do
					{
						NSERT(RT, VT, TT, UINS);
						ANGLE = PA - inp.OFFSET;
						GMTLO = GMTLO - ANGLE / aegdata.l_dot;
						ITER++;
						if (ITER >= PROCON.CMAX)
						{
							error = -1;
							return;
						}
					} while (abs(ANGLE) < PROCON.DOS);
					inp.LOT = 1;
					break;
				case 3:
				case 4:
					GMTLO = GSTAR + inp.BIAS;
					NSERT(RT, VT, TT, UINS);
					inp.LOT = 1;
					break;
				}

				switch (inp.INSCO)
				{
				case 1:
					break;
				case 2:
				case 3:
					UOC = UINS + inp.DU;
					UPDAT(RP, VP, TP, UOC);
					if (inp.INSCO == 2)
					{
						UPDAT(RT, VT, TT, UOC);
						DHA = length(RT) - length(RP);
						DALT = DHA - inp.DHW;
					}
					else
					{
						ALT = length(RP) - GLOCON.RREF;
						DALT = inp.DHW - ALT;
					}
					inp.VINS = inp.VINS + DALT / 3420.0;
					break;
				}
				ITINS++;
				if (ITINS >= PROCON.CMAX)
				{
					error = -1;
					return;
				}
			} while (abs(DALT) > PROCON.DELH);
			inp.BIAS = GMTLO - GSTAR;
			TARGT();
			if (LAST == 1)
			{
				TYAW = TPLANE + inp.DELNO / GLOCON.w_E;
			}
			else
			{
				LAST = 1;
			}
			if (inp.LOT == 6)
			{
				DTYAW = TYAW - GMTLO;
				GMTLO = TYAW;
			}
			ITER++;
			if (ITER >= PROCON.CMAX)
			{
				error = -1;
				return;
			}
		} while (abs(DTYAW) > PROCON.DET);
		if (inp.LOT == 6)
		{
			LAST = 0;
			inp.LOT = 1;
			GMTLO = TYAW + inp.TRANS;
		}
	} while (LAST != 1);
}


void LaunchWindowProcessor::TARGT()
{
	double lat_star, TINS, TRS, TS, TGRR, WT;

	TINS = GMTLO + inp.PFT;

	//Update target to insertion
	UPDAT(RT, VT, TT, TINS);
	AEGDataBlock sv_T = aegdata;
	//Update chaser to insertion
	UPDAT(RP, VP, TP, TINS);

	TGRR = GMTLO - inp.DTGRR;
	WT = GLOCON.w_E*TGRR;
	while (WT >= PI2)
	{
		WT -= PI2;
	}

	//H is now defined relative to GRR, not midnight
	sv_T.coe_mean.h = sv_T.coe_mean.h - WT;
	if (sv_T.coe_mean.h < 0)
	{
		sv_T.coe_mean.h += PI2;
	}

	//aegdata.h_dot = aegdata.h_dot*cos(aegdata.coe_mean.i) / cos(sv_T.coe_mean.i);

	DN = sv_T.coe_mean.h - PI;
	if (DN < 0)
	{
		DN = sv_T.coe_mean.h + PI;
	}

	DN = DN + 1.5*OrbMech::J2_Earth*pow(OrbMech::R_Earth, 2) / (2.0*pow(aegdata.coe_osc.a, 2)*pow(1.0 - pow(aegdata.coe_osc.e, 2), 2))*cos(sv_T.coe_mean.i)*sin(2.0*aegdata.U);

	if (inp.DELNOF)
	{
		inp.DELNO = -((sv_T.h_dot - aegdata.h_dot)*PA) / (sv_T.l_dot - aegdata.l_dot);
		if (abs(inp.BIAS) > 0.0)
		{
			DELNOD = inp.DELNO / inp.BIAS;
		}
	}

	//Compute IIGM
	IIGM = sv_T.coe_mean.i + 1.5*OrbMech::J2_Earth*pow(OrbMech::R_Earth, 2) / (4.0*pow(aegdata.coe_osc.a, 2)*pow(1.0 - pow(aegdata.coe_osc.e, 2), 2))*sin(2.0*sv_T.coe_mean.i)*cos(2.0*aegdata.U);
	
	TIGM = DN - LONGLS + inp.DELNO;// +GLOCON.w_E*(inp.PFT + inp.DTGRR);
	if (TIGM < 0)
	{
		TIGM += PI2;
	}
	else if (TIGM > PI2)
	{
		TIGM -= PI2;
	}

	TDIGM = sv_T.h_dot - GLOCON.w_E + DELNOD;

	//Compute AZL
	AZL = inp.LAZCOE[0] + inp.LAZCOE[1] * IIGM + inp.LAZCOE[2] * TIGM + inp.LAZCOE[3] * IIGM*TIGM;

	aegdata.coe_osc.i = IIGM;
	aegdata.coe_osc.h = DN + PI + inp.DELNO + GLOCON.w_E*TGRR;
	OrbMech::GIMKIC(aegdata.coe_osc, GLOCON.mu, RP, VP);
	//Compute GPAZ
	TRS = DN / (GLOCON.w_E + sv_T.h_dot) + TINS;
	TS = GMTLO - TRS;
	lat_star = LONGLS - GLOCON.w_E*TS;
	GPAZ = atan2(-cos(IIGM)*cos(LATLS) + sin(IIGM)*sin(lat_star)*sin(LATLS), sin(IIGM)*cos(lat_star));
}

void LaunchWindowProcessor::NSERT(VECTOR3 RB, VECTOR3 VB, double GMTB, double &UINS)
{
	UPDAT(RB, VB, GMTB, GMTLO);
	RT = RB;
	VT = VB;
	TT = GMTB;

	LENSR(GMTLO);
	TINS = TP;

	PA = PHANG();

	if (inp.NEGTIV == 0)
	{
		while (PA >= PI2) PA -= PI2;
		while (PA < 0) PA += PI2;
	}
	else if (inp.NEGTIV == 1)
	{
		while (PA >= 0) PA -= PI2;
		while (PA < -PI2) PA += PI2;
	}
	else
	{
		while (PA >= PI) PA -= PI2;
		while (PA < -PI) PA += PI2;
	}
	PA += PI2 * (double)inp.WRAP;

	DH = length(RT) - length(RP); //TBD
	//TBD: Calculate UINS
	UINS = 0.0;
}

void LaunchWindowProcessor::LWPT()
{
	if (inp.LPT == 1)
	{
		do
		{
			TIP = OPEN - inp.TSTART;
			LENSR(TIP);

			//TBD: Calculate stuff
		} while (TIP < OPEN + inp.TEND);
	}
}

void LaunchWindowProcessor::LWDSP()
{
	lwtt.DAY = 0;
	lwtt.DATE[0] = lwtt.DATE[1] = lwtt.DATE[2] = 0;
	if (inp.NS != 1)
	{
		lwtt.GMT_OPEN = OPEN;
		lwtt.EST_OPEN = lwtt.GMT_OPEN - 5.0*3600.0;
		lwtt.PST_OPEN = lwtt.EST_OPEN - 3.0*3600.0;
		lwtt.PAO = PAO;
	}
	else
	{
		lwtt.GMT_OPEN = -1.0;
		lwtt.EST_OPEN = -1.0;
		lwtt.PST_OPEN = -1.0;
		lwtt.PAO = -1.0;
	}
	if (inp.NS != 0)
	{
		lwtt.GMT_CLOSE = CLOSE;
		lwtt.EST_CLOSE = lwtt.GMT_CLOSE - 5.0*3600.0;
		lwtt.PST_CLOSE = lwtt.EST_CLOSE - 3.0*3600.0;
		lwtt.PAC = PAC;
	}
	else
	{
		lwtt.GMT_CLOSE = -1.0;
		lwtt.PAC = -1.0;
	}
	double RA, RP;
	OrbMech::periapo(RT, VT, GLOCON.mu, RA, RP);
	lwtt.HA = (RA - GLOCON.RREF) / 1852.0;
	lwtt.HP = (RP - GLOCON.RREF) / 1852.0;
	lwtt.LATLS = LATLS*DEG;
	lwtt.LONGLS = LONGLS*DEG;

	if (inp.STABLE == 1)
	{
		for (int i = 0;i < 10;i++)
		{
			gmtt.STAR[i] = STAR[i];
			gmtt.DELTA[i] = STAR[i] - TPLANE;
		}
	}
}


//Recommended Lift-off-Time (RLOT) display routine
void LaunchWindowProcessor::RLOTD()
{
	rlott.DELNOD = DELNOD;
	rlott.GSTAR = GSTAR;
	rlott.TYAW = TYAW;
	rlott.DH = DH;
}

//Launch Window Processor Output Tables routine
void LaunchWindowProcessor::LWPOT()
{
	lwsum.LWPERROR = error;
	lwsum.GMTLO = GMTLO;
	lwsum.TINS = TINS;
	lwsum.AZL = AZL;
	lwsum.VIGM = inp.VINS;
	lwsum.RIGM = inp.RINS;
	lwsum.GIGM = inp.GAMINS;
	lwsum.IIGM = IIGM;
	lwsum.TIGM = TIGM;
	lwsum.TDIGM = TDIGM;
	lwsum.DN = DN;
	lwsum.DELNO = inp.DELNO;
	lwsum.PA = PA;
	lwsum.TPLANE = TPLANE;
	lwsum.LATLS = LATLS;
	lwsum.LONGLS = LONGLS;

	UPDAT(RT, VT, TT, TINS);
	svtab.sv_T1 = aegdata;
	UPDAT(RP, VP, TP, TINS);
	svtab.sv_C = aegdata;
}