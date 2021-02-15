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
#include "LWP.h"

LaunchWindowProcessor::LaunchWindowProcessor()
{
	SVPROP = 0;
	NS = 0;
	mu = 0.0;
	w_E = 0.0;
	DTOPT = 0.0;
	WRAP = 0;
	NEGTIV = 0;
	GAMINS = 0.0;
	LATLS = 0.0;
	LONGLS = 0.0;
	PFT = 0.0;
	PFA = 0.0;
	RINS = 0.0;
	VINS = 0.0;
	YSMAX = 0.0;
	CMAX = 20;
	INSCO = 1;
	ANOM = 0.0;
	LOT = 0;
	TRANS = 0.0;
	MJDPLANE = 0.0;
	MJDLO = 0.0;
	MJDLOR = 0.0;
	DET = 1.0;
	DELH = 250.0*0.3048;
	DELNOF = true;
	DELNO = 0.0;
	DTGRR = 0.0;
	BIAS = 0.0;
	DELNOD = 0.0;
	LW = 0;
	TSTART = 0.0;
	TEND = 0.0;
	TSTEP = 0.0;
	DX1 = 25.0;
	DVTOL = 0.1*0.3048;
	PAO = 0.0;
	PAC = 0.0;
	MJDOPEN = 0.0;
	MJDCLOSE = 0.0;
	MJDINS = 0.0;
	WEDGE = 0.0;
	LPT = 1;
	OFFSET = 0.0;
	LAZCOE[0] = 490.931;
	LAZCOE[1] = -20.131;
	LAZCOE[2] = -1.2501;
	LAZCOE[3] = 0.0975;
	IIGM = 0.0;
	AZL = 0.0;
	TIGM = 0.0;
	TDIGM = 0.0;
	DN = 0.0;
	PA = 0.0;
	error = 0;
	hEarth = oapiGetObjectByName("Earth");
}

void LaunchWindowProcessor::SetGlobalConstants(double mu, double w_E, double R_E)
{
	this->mu = mu;
	this->w_E = w_E;
	this->RREF = R_E;
}

void LaunchWindowProcessor::Init(LWPSettings &set)
{
	SVPROP = set.SVPROP;
	NS = set.NS;
	DTOPT = set.DTOPT;
	WRAP = set.WRAP;
	NEGTIV = set.NEGTIV;
	GAMINS = set.GAMINS;
	LATLS = set.LATLS;
	LONGLS = set.LONGLS;
	PFT = set.PFT;
	PFA = set.PFA;
	RINS = set.RINS;
	VINS = set.VINS;
	YSMAX = set.YSMAX;
	MJDPLANE = set.MJDPLANE;
	LW = set.LW;
	TSTART = set.TSTART;
	TEND = set.TEND;
	//For now
	TSTEP = (TEND - TSTART)/2.0;
	sv_T = set.TRGVEC;
	DELNO = set.DELNO;
	LOT = set.LOT;
	lwp_param_table = set.lwp_param_table;
	error = 0;
}

void LaunchWindowProcessor::LWP()
{
	//Call LWPIN
	if (LW != 1)
	{
		LWT();
		if (error) return;
		LWDSP();
		if (LPT > 0)
		{
			LWPT();
		}
	}
	if (LW != 0)
	{
		RLOT();
		if (error) return;
		RLOTD();
		LWPOT();
	}
}

SV LaunchWindowProcessor::UPDATE(SV sv0, double dt)
{
	if (SVPROP == 1)
	{
		return coast(sv0, dt);
	}
	else
	{
		return coast_osc(sv0, dt, mu);
	}
}

void LaunchWindowProcessor::LENSR(double mjdlo, double ysmax, SV &sv_P, double &delta)
{
	SV sv_T1;
	OELEMENTS coe;
	MATRIX3 MATR1, MATR2, MATR3;
	VECTOR3 URLS, H, K, J;
	double psi, Gamma, DYAW, MJDINS;

	MJDINS = mjdlo + PFT / 24.0 / 3600.0;
	sv_T = UPDATE(sv_T, (MJDINS - sv_T.MJD)*24.0*3600.0);
	sv_T1 = sv_T;

	URLS = _V(cos(LATLS)*cos(LONGLS), cos(LATLS)*sin(LONGLS), sin(LATLS));
	URLS = OrbMech::ECEFToEcliptic(URLS, mjdlo);
	H = unit(crossp(sv_T1.R, sv_T1.V));
	K = unit(crossp(H, URLS));
	J = crossp(K, H);

	psi = asin(dotp(H, URLS));
	delta = asin(sin(psi) / cos(PFA));
	Gamma = asin(sin(PFA) / cos(psi));
	DYAW = abs(delta) - ysmax;
	if (ysmax > 0.)
	{
		if (DYAW <= 0.)
		{
			Gamma = acos(cos(PFA) / cos(psi));
			delta = 0.;
		}
		else
		{
			double cosx, x, y, sinG;

			cosx = cos(PFA) / cos(ysmax);
			x = acos(cosx);
			y = asin(sin(psi) / cosx);
			delta = abs(y) - ysmax;
			sinG = sin(x) / cos(psi);
			Gamma = acos(sqrt(1.0 - sinG * sinG));
		}
	}
	MATR1 = _M(J.x, J.y, J.z, K.x, K.y, K.z, H.x, H.y, H.z);
	MATR2 = mul(_M(cos(Gamma), sin(Gamma), 0., -sin(Gamma), cos(Gamma), 0., 0., 0., 1.), MATR1);
	if (DYAW > 0.)
	{
		double phi;
		phi = psi / abs(psi)*delta;
		MATR2 = mul(_M(cos(phi), 0., sin(phi), 0., 1., 0., -sin(phi), 0., cos(phi)), MATR2);
	}
	MATR3 = mul(_M(cos(GAMINS), -sin(GAMINS), 0., sin(GAMINS), cos(GAMINS), 0., 0., 0., 1.), MATR2);
	sv_P.R = _V(MATR2.m11, MATR2.m12, MATR2.m13)*RINS;
	sv_P.V = _V(MATR3.m21, MATR3.m22, MATR3.m23)*VINS;
	sv_P.MJD = MJDINS;

	//Test
	OELEMENTS coetest = coe_from_sv(sv_P.R, sv_P.V, mu);
	double atest2 = 1;
}

void LaunchWindowProcessor::NPLAN(double &MJDIP)
{
	OELEMENTS coe;
	VECTOR3 R_ECEF, V_ECEF;
	double I_m, U_T, eta, lambda_N, lambda, DLON, DOS, dt;
	int ITER;

	ITER = 1;
	DOS = 2.0e-4;

	do
	{
		sv_T = UPDATE(sv_T, (MJDIP - sv_T.MJD)*24.0*3600.0);
		OrbMech::EclipticToECEF(sv_T.R, sv_T.V, sv_T.MJD, R_ECEF, V_ECEF);
		coe = OrbMech::coe_from_sv(R_ECEF, V_ECEF, mu);
		I_m = coe.i;
		lambda_N = coe.RA;
		U_T = asin2(sin(LATLS) / sin(I_m));
		if (NS == 1) U_T = PI - U_T;
		eta = acos(cos(U_T) / cos(LATLS));
		if (I_m > PI05) eta = -eta;
		lambda = lambda_N + eta;
		if (lambda < 0) lambda += PI2;
		DLON = lambda - LONGLS;
		while (DLON > PI) DLON -= PI2;
		while (DLON <= -PI) DLON += PI2;
		if (DLON < 0 && ITER == 1) DLON += PI2;
		dt = DLON / w_E;
		MJDIP += dt / 24.0 / 3600.0;
		ITER++;
	} while (abs(DLON) >= DOS && ITER <= CMAX);
}

void LaunchWindowProcessor::LWT()
{
	SV sv_P;
	double MJDIP, V, delta, DV, MJDIP0;
	ITERSTATE iter;

	MJDIP = sv_T.MJD;
	
LWT1:

	iter.dv = 0.0;
	NPLAN(MJDIP);
	MJDIP0 = MJDIP;
	do
	{
		MJDIP = MJDIP0 + iter.dv / 24.0 / 3600.0;
		LENSR(MJDIP, -1, sv_P, delta);
		V = length(sv_P.V);
		DV = 2.0*V*sin(delta / 2.0);

		iter.err = DV;

		if (abs(iter.err) > DVTOL)
		{
			OrbMech::ITER(iter.c_I, iter.s_F, iter.err, iter.p_H, iter.dv, iter.erro, iter.dvo, DX1);
			if (iter.s_F)
			{
				error = 3;
				return;
			}
		}
	} while (abs(iter.err) > DVTOL);

	if (NS != 1)
	{
		MJDOPEN = MJDIP + DTOPT / 24.0 / 3600.0;
		MJDPLANE = MJDOPEN;
		LENSR(MJDPLANE, YSMAX, sv_P, delta);
		PAO = PHANG(sv_P, NEGTIV, WRAP);
	}
	if (NS == 2)
	{
		NS = 1;
		goto LWT1;
	}
	if (NS != 0)
	{
		MJDCLOSE = MJDIP + DTOPT / 24.0 / 3600.0;
		MJDINS = MJDCLOSE + PFT / 24.0 / 3600.0;
		MJDPLANE = MJDCLOSE;
		LENSR(MJDPLANE, YSMAX, sv_P, delta);
		PAC = PHANG(sv_P, NEGTIV, WRAP);
	}

	sv_P1 = sv_P;
}

double LaunchWindowProcessor::PHANG(SV sv_P, int phcont, int wrp)
{
	VECTOR3 u, R_A1, U_L;
	double phase;

	u = unit(crossp(sv_T.R, sv_T.V));
	U_L = unit(crossp(u, sv_T.R));

	R_A1 = unit(sv_P.R - u * dotp(sv_P.R, u))*length(sv_P.R);
	phase = acos(dotp(unit(R_A1), unit(sv_T.R)));
	if (dotp(U_L, R_A1) > 0)
	{
		phase = -phase;
	}
	if (phcont == 0)
	{
		while (phase >= PI2) phase -= PI2;
		while (phase < 0) phase += PI2;
	}
	else if (phcont == 1)
	{
		while (phase >= 0) phase -= PI2;
		while (phase < -PI2) phase += PI2;
	}
	else
	{
		while (phase >= PI) phase -= PI2;
		while (phase < -PI) phase += PI2;
	}
	phase += PI2 * (double)wrp;
	return phase;
}

double LaunchWindowProcessor::GMTLS(double MJDI)
{
	SV sv_P;
	double DT, STAR, phase, n, delta;

	DT = 1000.0;
	STAR = MJDI;
	n = OrbMech::GetMeanMotion(sv_T.R, sv_T.V, mu);

	do
	{
		LENSR(STAR, YSMAX, sv_P, delta);
		phase = PHANG(sv_P, 2, 0);
		DT = -phase / n;
		STAR += DT / 24.0 / 3600.0;
	} while (abs(DT) > DET);
	return STAR;
}

void LaunchWindowProcessor::RLOT()
{
	SV sv_P;
	double DALT, DTYAW, GSTAR, UINS, DH, MJDYAW;
	int ITER, ITINS, LAST;

	ITER = 0;
	ITINS = 0;

	LAST = 1;

	DALT = 0.0;
	DTYAW = 0.0;

	if (WRAP != 0) OFFSET = OFFSET + PI2 * (double)WRAP;

	if (INSCO != 1)
	{
		VINS = sqrt(mu*(2.0 / RINS - 1.0 / ANOM)) + 10.0*0.3048;
	}

	switch (LOT)
	{
	case 1:
	case 2:
	case 3:
		MJDLO = MJDLOR;
		break;
	case 4:
	case 6:
		MJDLO = MJDPLANE;
		break;
	case 5:
		MJDLO = MJDPLANE + TRANS;
		break;
	}

	GSTAR = GMTLS(MJDLO);

	do
	{
		do
		{
			do
			{
				switch (LOT)
				{
				case 1:
				case 5:
				case 6:
					NSERT(MJDLO, UINS, sv_P, DH);
					break;
				case 2:
					break;
				case 3:
				case 4:
					break;
				}

				switch (INSCO)
				{
				case 1:
					DALT = 0.0;
					break;
				case 2:
				case 3:
					//TBD
					break;
				}
			} while (abs(DALT) >= DELH);

			BIAS = (MJDLO - GSTAR)*24.0*3600.0;
			TARGT(sv_P);
			if (LAST == 1)
			{
				MJDYAW = MJDPLANE + (DELNO / w_E) / 24.0 / 3600.0;
			}
			else
			{
				LAST = 1;
			}
			if (LOT == 6)
			{
				DTYAW = (MJDYAW - MJDLO)*24.0*3600.0;
				MJDLO = MJDYAW;
			}

			ITER++;

			if (ITER >= CMAX) return;
		} while (abs(DTYAW) >= DET);

		if (LOT == 6)
		{
			LAST = 0;
			LOT = 1;
			MJDLO = MJDYAW + TRANS / 24.0 / 3600.0;
		}

	} while (LAST != 1);
}

void LaunchWindowProcessor::TARGT(SV &sv_P)
{
	//Position matched target state vector
	SV sv_T1;
	VECTOR3 R_TINS, V_TINS;
	OELEMENTS coe;
	//nodal precession rate of target
	double HDOTT;
	//nodal precession rate of chaser
	double HDOTC;
	double NT, NC;

	NC = 0.0;
	NT = 1.0;
	HDOTT = 0.0;
	HDOTC = 0.0;

	//Update target to insertion
	//Update chaser to insertion

	//Compute descending node
	DN = 0.0;

	if (DELNOF == 1)
	{
		/*DELNO = -((HDOTT - HDOTC)*PA) / (NT - NC);
		if (abs(BIAS) > 0)
		{
			DELNOD = DELNO / BIAS;
		}*/

	}

	//Calculate IIGM
	sv_T1 = PositionMatch(sv_T, sv_P);
	OrbMech::EclipticToECI(sv_T1.R, sv_T1.V, sv_T1.MJD, R_TINS, V_TINS);
	coe = coe_from_sv(R_TINS, V_TINS, mu);
	IIGM = coe.i;

	TIGM = DN - LONGLS + DELNO + w_E * (PFT + DTGRR);
	//TDIGM = HDOTT - w_E + DELNOD;
	//Compute AZL
	AZL = LAZCOE[0] + LAZCOE[1] * IIGM*DEG + LAZCOE[2] * TIGM*DEG + LAZCOE[3] * IIGM*DEG*TIGM*DEG;
	AZL *= RAD;
	//I_C = IIGM;
	//H_C = DN + PI + DELNO + w_E * T_Ins;

	if (DELNO != 0.0)
	{
		OrbMech::EclipticToECI(sv_P.R, sv_P.V, sv_P.MJD, sv_P1.R, sv_P1.V);
		coe = coe_from_sv(sv_P1.R, sv_P1.V, mu);
		coe.i = IIGM;
		coe.RA = coe.RA + DELNO;
		sv_from_coe(coe, mu, sv_P1.R, sv_P1.V);
		OrbMech::ECIToEcliptic(sv_P1.R, sv_P1.V, sv_P1.MJD, sv_P1.R, sv_P1.V);
	}
	else
	{
		sv_P1 = sv_P;
	}
}

SV LaunchWindowProcessor::PositionMatch(SV sv_A, SV sv_P)
{
	SV sv_A1, sv_P1;
	VECTOR3 u, R_A1, U_L;
	double phase, n, dt, ddt;

	dt = 0.0;

	u = unit(crossp(sv_P.R, sv_P.V));
	U_L = unit(crossp(u, sv_P.R));
	sv_A1 = UPDATE(sv_A, (sv_P.MJD - sv_A.MJD)*24.0*3600.0);

	do
	{
		R_A1 = unit(sv_A1.R - u * dotp(sv_A1.R, u))*length(sv_A1.R);
		phase = acos(dotp(unit(R_A1), unit(sv_P.R)));
		if (dotp(U_L, R_A1) > 0)
		{
			phase = -phase;
		}
		n = OrbMech::GetMeanMotion(sv_A1.R, sv_A1.V, mu);
		ddt = phase / n;
		sv_A1 = UPDATE(sv_A1, ddt);
		dt += ddt;
	} while (abs(ddt) > 0.01);

	return sv_A1;
}

void LaunchWindowProcessor::NSERT(double MJDLO, double &UINS, SV &sv_P, double &DH)
{
	double delta;

	LENSR(MJDLO, YSMAX, sv_P, delta);

	DH = length(sv_T.R) - length(sv_P.R);
	PA = PHANG(sv_P, NEGTIV, WRAP);
	UINS = 0.0;
}

void LaunchWindowProcessor::LWPT()
{

}

void LaunchWindowProcessor::LWDSP()
{

}

//Recommended Lift-off-Time (RLOT) display routine
void LaunchWindowProcessor::RLOTD()
{

}

//Launch Window Processor Output Tables routine
void LaunchWindowProcessor::LWPOT()
{
	SV sv_P;
	double delta;
	bool last = false;

	lwp_param_table->MJDLO[0] = MJDLO + TSTART / 24.0 / 3600.0;
	LENSR(lwp_param_table->MJDLO[0], YSMAX, sv_P, delta);
	lwp_param_table->PHASE[0] = PHANG(sv_P, NEGTIV, WRAP);

	lwp_param_table->MJDLO[1] = MJDLO + TEND / 24.0 / 3600.0;
	LENSR(lwp_param_table->MJDLO[1], YSMAX, sv_P, delta);
	lwp_param_table->PHASE[1] = PHANG(sv_P, NEGTIV, WRAP);
}

void LaunchWindowProcessor::GetOutput(LWPSummary &out)
{
	out.LWPERROR = error;
	out.MJDLO = MJDLO;
	out.MJDINS = MJDINS;
	out.AZL = AZL;
	out.VIGM = VINS;
	out.RIGM = RINS;
	out.IIGM = IIGM;
	out.TIGM = TIGM;
	out.TDIGM = TDIGM;
	out.DN = DN;
	out.DELNO = DELNO;
	out.PA = PA;
	out.MJDPLANE = MJDPLANE;
	out.LATLS = LATLS;
	out.LONGLS = LONGLS;
	out.sv_P = sv_P1;
}

SV LaunchWindowProcessor::coast(SV sv0, double dt)
{
	SV sv1;

	OrbMech::oneclickcoast(sv0.R, sv0.V, sv0.MJD, dt, sv1.R, sv1.V, hEarth, hEarth);
	sv1.mass = sv0.mass;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}

SV LaunchWindowProcessor::coast_osc(SV sv0, double dt, double mu)
{
	SV sv1;

	OrbMech::rv_from_r0v0(sv0.R, sv0.V, dt, sv1.R, sv1.V, mu);
	sv1.mass = sv0.mass;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}