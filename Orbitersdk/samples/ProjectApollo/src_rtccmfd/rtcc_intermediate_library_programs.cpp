/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC) Intermediate Library Programs

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

#include "rtcc.h"

//Gimbal, Thrust, and Weight Loss Rate Subroutine
void RTCC::GIMGBL(double CSMWT, double LMWT, double &RY, double &RZ, double &T, double &WDOT, int ITC, unsigned &IC, int IA, int IJ, double D)
{
	//INPUTS:
	//CSMWT and LMWT, CSM and LM weights
	//ITC: 33 = SPS, 34 = APS, 35 = DPS, 36 = J2
	//IC: 1 = CSM, 12 = LM, 13 = CSM and LM (docked)
	//IA: -1: trim angles, thrust and weight loss rate desired outputs, 0: thrust and weight loss rate desired outputs, 1: trim angles desired outputs
	//IJ: LM descent stage included in configuration at beginning of this maneuver (only applicable if ITC=33 and IC=2). 0 = included, 1 = not included

	if (ITC < 33)
	{
		RY = 0.0;
		RZ = 0.0;
		return;
	}

	//DPS engine gimbal plane
	static const double K1 = 154.0*0.0254;
	//SPS engine gimbal plane
	static const double K2 = 833.2*0.0254;
	//Distance between SPS and DPS gimbal planes
	static const double K3 = 13.2;//435.55*0.0254;
	//Yaw
	static const double K4 = -0.0829031395;
	//Pitch
	static const double K5 = 0.1876228946;

	/*x1 = LMmass / (CSMmass + LMmass)*6.2;
	p_T = atan2(-2.15 * RAD * 5.0, 5.0 + x1);
	y_T = atan2(0.95 * RAD * 5.0, 5.0 + x1);*/

	int IND;
	double R, W[3];
	VECTOR3 XCG[2], XI, K;

	//CSM only
	if (IC == 1)
	{
		W[0] = CSMWT;
		W[1] = 0.0;
	}
	//LM only
	else if (IC == 12)
	{
		W[0] = 0.0;
		W[1] = LMWT;
	}
	//Both
	else
	{
		W[0] = CSMWT;
		W[1] = LMWT;
	}
	W[2] = W[0] + W[1];

	if (ITC == 34)
	{
		goto RTCC_GIMGBL_LABEL_3_3;
	}
	else if (ITC > 34)
	{
		if (IA == 0)
		{
			goto RTCC_GIMGBL_LABEL_3_5;
		}
		IND = 2;
		K = _V(K1, 0, 0);
		//Use LM DSC CG Table
		XI = GIMGB2(LMDSCCGTAB.Weight, LMDSCCGTAB.CG, LMDSCCGTAB.N, W[1]);
	}
	else
	{
		if (IA == 0)
		{
			goto RTCC_GIMGBL_LABEL_3_4;
		}
		if (IC == 1)
		{
			IND = 2;
		}
		else
		{
			IND = 1;
		}
		K = _V(K2, K4, K5);
		//Use CSM CG Table
		//GIMGB2();
		XI = _V(26.16328, 0.0, 0.0);
	}

	XCG[IND - 1] = XI - K;

	//CSM or LM, but not docked?
	if (IC <= 12)
	{
		goto RTCC_GIMGBL_LABEL_3_2;
	}
	if (IND > 1)
	{
		//Use CSM CG Table
		//GIMGB2();
		IND = 1;
		K = _V(K2 + K3, 0, 0);
		XI = _V(26.16328, 0.0, 0.0);
	}
	else
	{
		if (IJ != 0)
		{
			//Use LM w/o descent CG table
			//GIMGB2();
			XI = _V(7.6616, 0, 0);
		}
		else
		{
			//Use LM w/ descent CG table
			XI = GIMGB2(LMDSCCGTAB.Weight, LMDSCCGTAB.CG, LMDSCCGTAB.N, W[1]);
		}
		IND = 2;
		K = _V(K1 + K3, 0, 0);
	}

	XI = XI - K;

	XCG[IND - 1] = mul(_M(-1.0, 0.0, 0.0, 0.0, -cos(240.0*RAD - D), -sin(240.0*RAD - D), 0.0, -sin(240.0*RAD - D), cos(240.0*RAD - D)), XI);
	//Remove this when SPS is centered again
	if (ITC == 33)
	{
		XCG[IND - 1] -= _V(0, K4, K5);
	}
	XCG[1] = (XCG[0] * W[0] + XCG[1] * W[1]) / W[2];

RTCC_GIMGBL_LABEL_3_2:
	R = length(XCG[1]);
	if (R > 10e-6)
	{
		RZ = asin(XCG[1].y / R);
		if (XCG[1].x > 10e-6)
		{
			RY = atan(XCG[1].z / XCG[1].x);
		}
		else
		{
			RY = 0.0;
		}
	}
	else
	{
		RZ = 0.0;
		RY = 0.0;
	}
	if (IA == 1)
	{
		return;
	}
	if (ITC == 34)
	{
	RTCC_GIMGBL_LABEL_3_3:;
		//GIMGB2();
	}
	else if (ITC < 34)
	{
	RTCC_GIMGBL_LABEL_3_4:;
		//GIMGB2();
	}
	else
	{
	RTCC_GIMGBL_LABEL_3_5:;
		//GIMGB2();
	}
	//T = XI.x;
	//WDOT = XI.y;
}

VECTOR3 RTCC::GIMGB2(const double *WArr, const VECTOR3 *VecArr, int N, double W)
{
	VECTOR3 XI;
	int I;
	if (W <= WArr[0])
	{
		I = 1;
	RTCC_GIMGB2_4:
		XI = VecArr[I - 1];
		goto RTCC_GIMGB2_2;
	}
	if (W >= WArr[N - 1])
	{
		goto RTCC_GIMGB2_1;
	}
	I = 2;
RTCC_GIMGB2_3:
	if (W < WArr[I - 1])
	{
		//Linearly interpolate
		XI = VecArr[I - 2] + (VecArr[I - 1] - VecArr[I - 2]) / (WArr[I - 1] - WArr[I - 2])*(W - WArr[I - 2]);
		goto RTCC_GIMGB2_2;
	}
	if (W == WArr[I - 1])
	{
		goto RTCC_GIMGB2_4;
	}
	if (I < N)
	{
		I++;
		goto RTCC_GIMGB2_3;
	}
RTCC_GIMGB2_1:
	I = N;
	goto RTCC_GIMGB2_4;
RTCC_GIMGB2_2:
	return XI;
}

//LM AGS External DV Coordinate Transformation Subroutine
VECTOR3 RTCC::PIAEDV(VECTOR3 DV, VECTOR3 R_CSM, VECTOR3 V_CSM, VECTOR3 R_LM, bool i)
{
	//INPUTS:
	// i = 0: inertial to LVLH, 1: LVLH to inertial 

	VECTOR3 H, Z_PA, P, X_PA, Y_PA, DV_out;

	H = crossp(R_CSM, V_CSM);
	Z_PA = -unit(R_LM);
	P = crossp(Z_PA, H);
	X_PA = unit(P);
	Y_PA = crossp(Z_PA, X_PA);
	if (i)
	{
		DV_out = X_PA * DV.x + Y_PA * DV.y + Z_PA * DV.z;
	}
	else
	{
		DV_out = _V(dotp(DV, X_PA), dotp(DV, Y_PA), dotp(DV, Z_PA));
	}

	return DV_out;
}

//Right Ascension of Greenwich at Time T
double RTCC::PIAIES(double hour)
{
	return 0.0;
}

int RTCC::PIATSU(AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double &isg, double &gsg, double &hsg)
{
	PMMLAEG aeg;
	AEGHeader header;
	MATRIX3 Rot;
	VECTOR3 P, W, P_apo, W_apo;
	double eps_i, eps_t, usg, du, theta_dot, dt;
	int KE, K;

	eps_i = 1e-4;
	eps_t = 0.01;
	header.AEGInd = 1;
	header.ErrorInd = 0;
	AEGIN.TIMA = 0;

	AEGOUT = AEGIN;
	KE = 0;
	K = 1;
RTCC_PIATSU_1A:
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, GMTBASE + AEGOUT.TS / 24.0 / 3600.0);
	OrbMech::PIVECT(AEGOUT.coe_osc.i, AEGOUT.coe_osc.g, AEGOUT.coe_osc.h, P, W);
	P_apo = rhtmul(Rot, P);
	W_apo = rhtmul(Rot, W);
	OrbMech::PIVECT(P_apo, W_apo, isg, gsg, hsg);
	if (isg < eps_i || isg > PI - eps_i)
	{
		KE = 2;
		return KE;
	}
	usg = AEGOUT.f + gsg;
	if (usg >= PI2)
	{
		usg = usg - PI2;
	}
	if (K > 1)
	{
		du = AEGIN.Item8 - usg;
		theta_dot = sqrt(OrbMech::mu_Moon*AEGOUT.coe_osc.a*(1.0 - pow(AEGOUT.coe_osc.e, 2))) / pow(AEGOUT.R, 2) + AEGOUT.g_dot;
		if (abs(du) > PI)
		{
			if (du > 0)
			{
				du = du - PI2;
			}
			else if (du <= 0)
			{
				du = du + PI2;
			}
		}
	}
	else
	{
		du = AEGIN.Item8 - usg + AEGIN.Item10*PI2;
		theta_dot = sqrt(OrbMech::mu_Moon / pow(AEGIN.coe_osc.a, 3));
	}
	dt = du / theta_dot;
	if (abs(dt) <= eps_t)
	{
		return KE;
	}
	if (K >= 7)
	{
		KE = 1;
		return KE;
	}
	AEGIN.TE = AEGOUT.TS + dt;
	K++;
	aeg.CALL(header, AEGIN, AEGOUT);
	if (header.ErrorInd)
	{
		KE = -2;
		return KE;
	}
	goto RTCC_PIATSU_1A;
}

void RTCC::PIBETA(double BETA, double ONOVA, double &F1, double &F2, double &F3, double &F4)
{
	double a = ONOVA * BETA*BETA;
	F1 = OrbMech::stumpS(a);
	F2 = OrbMech::stumpC(a);

	F3 = 1.0 - a * F1;
	F4 = 1.0 - a * F2;
}

double RTCC::PIBSHA(double hour)
{
	double GMT = hour - MCCBES;
	return PIGMHA(GMT);
}

void RTCC::PIBURN(VECTOR3 R, VECTOR3 V, double T, double *B, VECTOR3 &ROUT, VECTOR3 &VOUT, double &TOUT)
{
	//B[0]: DT
	//B[1]: DR
	//B[2]: DV
	//B[3]: DS (arc length)
	//B[4]: dgamma
	//B[5]: dpsi
	//B[6]: isp
	//B[7]: characteristic DV
	//B[8]: mass ratio

	VECTOR3 H, R1, V1, R2, V2, R3, V3;
	double D;

	TOUT = T + B[0];
	H = crossp(R, V);
	D = dotp(R, V);
	if (B[3] == 0.0)
	{
		R1 = R;
		V1 = V;
	}
	else
	{
		R1 = R * cos(B[3]) + (V*pow(length(R), 2) - R * D)*sin(B[3]) / length(H);
		V1 = V * cos(B[3]) + (V*D - R * pow(length(R), 2))*sin(B[3]) / length(H);
	}
	if (B[4] == 0.0)
	{
		R2 = R1;
		V2 = V1;
	}
	else
	{
		R2 = R1;
		V2 = V1 * cos(-B[4]) + (V1*D - R1 * pow(length(V), 2))*sin(-B[4]) / length(H);
	}
	if (B[5] == 0.0)
	{
		R3 = R2;
		V3 = V2;
	}
	else
	{
		R3 = R2;
		V3 = R * 2.0*(dotp(R2, V)*pow(sin(-B[5] / 2.0), 2)) / pow(length(R), 2) + V2 * cos(-B[5]) + crossp(R2, V2)*sin(-B[5]) / length(R);
	}
	if (B[1] == 0.0)
	{
		ROUT = R3;
	}
	else
	{
		ROUT = R3 * (length(R) + B[1]) / length(R);
	}
	if (B[2] == 0.0)
	{
		VOUT = V3;
	}
	else
	{
		VOUT = V3 * (length(V) + B[2]) / length(V);
	}
	B[8] = sqrt(B[2] * B[2] + 4.0*length(V)*(length(V) + B[2])*pow(sin(B[4] / 2.0), 2) + cos(asin(dotp(unit(R), unit(V))))*cos(asin(dotp(unit(R), unit(V))) + B[4])*pow(sin(B[5] / 2.0), 2));
	B[7] = exp(-abs(B[8] / B[6]));
}

void RTCC::PICSSC(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &lat, double &lng, double &gamma, double &azi)
{
	if (vecinp)
	{
		r = length(R);
		lat = asin(R.z / r);
		lng = atan2(R.y, R.x);
		if (lng < 0)
		{
			lng += PI2;
		}
		VECTOR3 TEMP = mul(_M(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat), -sin(lng), cos(lng), 0, -sin(lat)*cos(lng), -sin(lat)*sin(lng), cos(lat)), V);
		v = length(TEMP);
		gamma = asin(TEMP.x / v);
		azi = atan2(TEMP.y, TEMP.z);
		if (azi < 0)
		{
			azi += PI2;
		}
	}
	else
	{
		R = _V(cos(lat)*cos(lng), cos(lat)*sin(lng), sin(lat))*r;
		V = mul(_M(cos(lat)*cos(lng), -sin(lng), -sin(lat)*cos(lng), cos(lat)*sin(lng), cos(lng), -sin(lat)*sin(lng), sin(lat), 0, cos(lat)), _V(sin(gamma), cos(gamma)*sin(azi), cos(gamma)*cos(azi))*v);
	}
}

MATRIX3 RTCC::PIDREF(VECTOR3 AT, VECTOR3 R, VECTOR3 V, double PG, double YG, bool K)
{
	//K: 0 for heads-down, 1 for heads-up
	VECTOR3 X_T, Y_T, Z_T, X_P, Y_P, Z_P;
	double r, y_T;

	X_T = AT;
	r = length(R);
	Y_T = crossp(R, X_T);
	y_T = length(Y_T);

	if (K)
	{
		Y_T = -Y_T;
	}
	if (y_T / r < 0.0017)
	{
		Y_T = crossp(R, V);
		y_T = length(Y_T);
	}
	Y_T = Y_T / y_T;
	Z_T = crossp(X_T, Y_T);

	X_P = X_T * cos(PG)*cos(YG) - Y_T * cos(PG)*sin(YG) + Z_T * sin(PG);
	Y_P = X_T * sin(YG) + Y_T * cos(YG);
	Z_P = -X_T * sin(PG)*cos(YG) + Y_T * sin(PG)*sin(YG) + Z_T * cos(PG);
	return _M(X_P.x, X_P.y, X_P.z, Y_P.x, Y_P.y, Y_P.z, Z_P.x, Z_P.y, Z_P.z);
}

VECTOR3 RTCC::PIEXDV(VECTOR3 R_ig, VECTOR3 V_ig, double WT, double T, VECTOR3 DV, bool i)
{
	//INPUTS:
	// i = 0: inertial to LVLH, 1: LVLH to inertial 
	VECTOR3 H, Y_PH, Z_PH, X_PH, DV_out;
	double h, rr, r, dv, theta, V_F, V_D;

	H = crossp(R_ig, V_ig);
	h = length(H);
	rr = dotp(R_ig, R_ig);
	r = sqrt(rr);
	Y_PH = -H / h;
	Z_PH = -R_ig / r;
	X_PH = crossp(Y_PH, Z_PH);
	dv = length(DV);
	theta = h * dv*WT / (2.0*rr*T);
	if (i)
	{
		double V_S;
		V_F = DV.x*cos(theta) - DV.z*sin(theta);
		V_S = DV.y;
		V_D = DV.x*sin(theta) + DV.z*cos(theta);
		DV_out = X_PH * V_F + Y_PH * V_S + Z_PH * V_D;
	}
	else
	{
		V_F = dotp(DV, X_PH);
		DV_out.y = dotp(DV, Y_PH);
		V_D = dotp(DV, Z_PH);
		DV_out.x = V_F * cos(theta) + V_D * sin(theta);
		DV_out.z = -V_F * sin(theta) + V_D * cos(theta);
	}

	return DV_out;
}

void RTCC::PIFAAP(double a, double e, double i, double f, double u, double r, double &r_apo, double &r_peri)
{
	double a_ref, e_ref, p_ref, p, K1, K2, df, r1, r2;

	a_ref = r + OrbMech::J2_Earth / OrbMech::R_Earth*(1.0 - 3.0 / 2.0*pow(sin(i), 2) + 5.0 / 6.0*pow(sin(i), 2)*cos(2.0*u));
	e_ref = 1.0 - r / a_ref;
	p_ref = a_ref * (1.0 - e_ref * e_ref);
	p = a * (1.0 - e * e);
	K1 = e / sqrt(p);
	K2 = e_ref / sqrt(p_ref);
	df = atan2(K1*sin(f), K2 - K1 * cos(f));
	r1 = p / (1.0 + e * cos(f + df)) - p_ref / (1.0 + e_ref * cos(df)) + r;
	r2 = p / (1.0 - e * cos(f + df)) - p_ref / (1.0 - e_ref * cos(df)) + r;
	if (r1 >= r2)
	{
		r_apo = r1;
		r_peri = r2;
	}
	else
	{
		r_peri = r1;
		r_apo = r2;
	}
}

double RTCC::PIGBHA()
{
	int Y, E, D, XN;
	static const double A = 0.0929;
	static const double B = 8640184.542;
	static const double W1 = 1.720217954160054e-2;
	double C, T, DE, BHA, DI, DELTA;

	E = Y = GZGENCSN.Year;
	//July and later uses epoch of next year
	if (GZGENCSN.MonthofLiftoff >= 7)
	{
		E = E + 1;
	}
	D = GZGENCSN.RefDayOfYear;
	XN = (E - 1901) / 4;
	C = -86400.0*(double)(E - 1900) - 74.164;
	T = 2 * C / (-B - sqrt(B*B - 4 * A*C));
	DE = 36525.0*T - 365.0*(double)(E - 1900) + 0.5 - (double)XN;
	if (Y == E)
	{
		DI = D;
	}
	else
	{
		int X = Y % 4;
		if (X == 0)
		{
			DI = D - 366.0;
		}
		else
		{
			DI = D - 365.0;
		}
	}
	DELTA = DI - DE;
	BHA = PI2 / 3.6 + W1 * DELTA;
	return BHA;
}

double RTCC::PIGMHA(double hour)
{
	return MCLAMD + hour * MCERTS;
}

void RTCC::PIMCKC(VECTOR3 R, VECTOR3 V, int body, double &a, double &e, double &i, double &l, double &g, double &h)
{
	double mu;

	if (body == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	double v = length(V);
	double r = length(R);
	double eps = v * v / 2.0 - mu / r;
	VECTOR3 H = crossp(R, V);

	a = -mu / (2.0*eps);
	VECTOR3 E = crossp(V, H) / mu - R / r;
	e = length(E);
	i = acos(H.z / length(H));
	double theta = acos2(dotp(unit(E), unit(R)));
	if (dotp(R, V) < 0)
	{
		theta = PI2 - theta;
	}
	double EE = atan2(sqrt(1.0 - e * e)*sin(theta), e + cos(theta));
	if (EE < 0)
	{
		EE += PI2;
	}
	l = EE - e * sin(EE);
	VECTOR3 K = _V(0, 0, 1);
	VECTOR3 N = crossp(K, H);
	g = acos2(dotp(unit(N), unit(E)));
	if (E.z < 0)
	{
		g = PI2 - g;
	}
	h = acos2(N.z / length(N));
	if (N.y < 0)
	{
		h = PI2 - h;
	}
}

void RTCC::PITFPC(double MU, int K, double AORP, double ECC, double rad, double &TIME, double &P)
{
	//INPUT:
	//MU: gravitational constant (er^3/hr^2)
	//K: outward leg (0.) and return lef (1.) flag. k is input as a floating point number
	//AORP: semimajor axis of elliptic or hyperbolic conic or semilatus rectum of parabolic conic (er)
	//e: eccentricity
	//r: radial distance from focus (er)

	double XP = 0.0, E, TP, X, XAORP = AORP;

	//Parabolic case?
	if (abs(ECC - 1.0) < 1.e-5) goto RTCC_PITFPC_2;
RTCC_PITFPC_5:
	//Eccentric or hyperbolic?
	if (ECC < 1.0) goto RTCC_PITFPC_3;
	//Hyperbolic
	X = 1.0 / ECC * (1.0 - rad / XAORP);
	E = log(X + sqrt(X*X - 1.0));
	TP = XAORP * sqrt(abs(XAORP) / MU)*(E - ECC * (0.5*(exp(E) - exp(-E))));
	goto RTCC_PITFPC_4;
RTCC_PITFPC_2:
	//Check if C3 is sufficiently large to use the non-parabolic calculations
	double C3;
	C3 = MU * (ECC*ECC - 1.0) / XAORP;
	if (abs(C3) >= 10.e-5)
	{
		XAORP = XAORP / (1.0 - ECC * ECC);
		goto RTCC_PITFPC_5;
	}
	//Parabolic
	double ETAP = acos(XAORP / rad - 1.0);
	double TEMP1 = sin(ETAP / 2.0) / cos(ETAP / 2.0);
	TP = XAORP / 2.0*sqrt(XAORP / MU)*(TEMP1 + 1.0 / 3.0*pow(TEMP1, 3.0));
	goto RTCC_PITFPC_4;
RTCC_PITFPC_3:
	//Eccentric
	E = acos(1.0 / ECC * (1.0 - rad / XAORP));
	XP = PI2 * XAORP*sqrt(XAORP / MU);
	TP = XAORP * sqrt(XAORP / MU)*(E - ECC * sin(E));
RTCC_PITFPC_4:
	P = XP;
	TIME = TP;
	//Outward or return leg?
	if (K)
	{
		TIME = -TP;
	}
}