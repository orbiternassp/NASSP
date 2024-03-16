/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2014-2018

RTCC Entry Calculations

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

#include "Orbitersdk.h"
#include "nasspdefs.h"
#include "OrbMech.h"
#include "EntryCalculations.h"
#include "CSMLMGuidanceSim.h"
#include "rtcc.h"

namespace EntryCalculations
{

	double ReentryTargetLine(double vel, bool msfn)
	{
		double ang;

		if (msfn)
		{
			ang = INTER(RTE_VEL_LINE, RTE_MSFN_LINE, 26, vel / 0.3048 / 1000.0);
		}
		else
		{
			ang = INTER(RTE_VEL_LINE, RTE_CONT_LINE, 26, vel / 0.3048 / 1000.0);
		}

		return ang * RAD;
	}

	double ReentryTargetLineTan(double vel, bool msfn)
	{
		return tan(PI05 - ReentryTargetLine(vel, msfn));
	}

	void augekugel(double ve, double gammae, double &phie, double &Te)
	{
		double vefps, gammaedeg, K1, K2;

		vefps = ve / 0.3048;
		gammaedeg = gammae * DEG;
		if (vefps <= 21000)
		{
			K1 = 5500.0;
		}
		else
		{
			if (vefps <= 28000.0)
			{
				K1 = 2400.0 + 0.443*(28000.0 - vefps);
			}
			else
			{
				K1 = 2400.0;
			}
		}
		if (vefps <= 24000.0)
		{
			K2 = -3.2 - 0.001222*(24000.0 - vefps);
		}
		else
		{
			if (vefps <= 28400.0)
			{
				K2 = 1.0 - 0.00105*(28000.0 - vefps);
			}
			else
			{
				K2 = 2.4 + 0.000285*(vefps - 33625.0);
			}
		}
		phie = min(2000.0, K1 / (abs(abs(gammaedeg) - K2)));
		if (vefps < 26000.0)
		{
			Te = 8660.0 * phie / vefps;
		}
		else
		{
			Te = phie / 3.0;
		}
	}

	void landingsite(MATRIX3 Rot_J_B, VECTOR3 REI, VECTOR3 VEI, double MJD_EI, double &lambda, double &phi)
	{
		double t32, v3, S_FPA, gammaE, phie, te, Sphie, Cphie, tLSMJD, l, m, n;
		VECTOR3 R3, V3, UR3, U_H3, U_LS, LSEF;
		MATRIX3 R;

		t32 = OrbMech::time_radius(REI, VEI, length(REI) - 30480.0, -1, OrbMech::mu_Earth);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, OrbMech::mu_Earth);
		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		augekugel(v3, gammaE, phie, te);

		tLSMJD = MJD_EI + (t32 + te) / 24.0 / 3600.0;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		R = OrbMech::GetRotationMatrix(BODY_EARTH, tLSMJD);
		LSEF = rhtmul(R, tmul(Rot_J_B, U_LS));

		l = LSEF.x;
		m = LSEF.y;
		n = LSEF.z;

		phi = asin(n);
		if (m > 0)
		{
			lambda = acos(l / cos(phi));
		}
		else
		{
			lambda = PI2 - acos(l / cos(phi));
		}
		if (lambda > PI) { lambda -= PI2; }
	}

	void Reentry(MATRIX3 Rot_J_B, VECTOR3 REI, VECTOR3 VEI, double mjd0, bool highspeed, double &EntryLatPred, double &EntryLngPred, double &EntryRTGO, double &EntryVIO, double &EntryRET)
	{
		double t32, v3, S_FPA, gammaE, phie, te, t_LS, Sphie, Cphie, tLSMJD, l, m, n, phi, lambda2, EntryInterface, RCON, EMSAlt;
		VECTOR3 R3, V3, UR3, U_H3, U_LS, LSEF;
		MATRIX3 R;

		EntryInterface = 400000.0 * 0.3048;

		RCON = OrbMech::R_Earth + EntryInterface;

		if (highspeed)
		{
			EMSAlt = 297431.0*0.3048;
		}
		else
		{
			EMSAlt = 284643.0*0.3048;
		}

		t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, OrbMech::mu_Earth);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, OrbMech::mu_Earth);
		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		augekugel(v3, gammaE, phie, te);

		t_LS = t32 + te;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		tLSMJD = mjd0 + t_LS / 24.0 / 3600.0;

		R = OrbMech::GetRotationMatrix(BODY_EARTH, tLSMJD);
		LSEF = rhtmul(R, tmul(Rot_J_B, U_LS));

		l = LSEF.x;
		m = LSEF.y;
		n = LSEF.z;

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

		EntryLatPred = phi;
		EntryLngPred = lambda2;

		VECTOR3 R05G, V05G;
		double dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, OrbMech::mu_Earth);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, OrbMech::mu_Earth);

		EntryRTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		EntryVIO = length(V05G);
		EntryRET = t32 + dt22;
	}

	void REENTRYNew(double LD, int ICRNGG, double v_i, double i_r, double A_Z, double mu, double r_rbias, double &eta_rz1, double &theta_cr, double &T)
	{
		// INPUT:
		// LD = lift to drag ratio
		// ICRNGG = 0: indicates a guided reentry, 1: indicates a manual reentry to the MSFN target line, 2 = indicates a manual reentry to the contingency target line
		// v_i = inertial speed
		// i_r = inertial reentry inclination
		// A_Z = inertial reentry azimuth
		// mu = reentry latitude
		// r_rbias = relative range override
		// OUTPUT:
		// eta_rz1 = downrange angle from reentry to landing
		// theta_cr = crossrange angle from reentry to landing
		// T = time from reentry to landing

		double u_r, r_r, r, CR, NMPER;
		NMPER = 3443.93359;
		r_r = 0.0;

		//u_r in feet per second
		u_r = v_i / 0.3048;

		if (i_r > PI05)
		{
			A_Z = PI;
			if (i_r > 3.0*PI05)
			{
				A_Z = 0.0;
			}
			i_r = PI05;
		}
		if (r_rbias == 0)
		{
			if (ICRNGG == 0)
			{
				for (int k = 0;k < 6;k++)
				{
					r_r += RTE_b[k] * pow(u_r, k);
				}
			}
			else
			{
				//B/2
				int i = ICRNGG - 1;
				r = (RTE_q[i][1] * u_r + RTE_q[i][2])*RTE_q[i][3] * (LD - 0.3);
				for (int k = 0;k < 6;k++)
				{
					r += RTE_jj[i][k] * pow(u_r, k);
				}
				T = RTE_pp[i][0] + RTE_pp[i][1] * r;
				CR = RTE_cc * T*cos(mu)*cos(A_Z + 0.3*mu) + RTE_q[i][4] * LD + RTE_q[i][5];
				//NM to radians
				eta_rz1 = abs(r) / NMPER;
				theta_cr = CR / NMPER;
				return;
			}
		}
		else
		{
			r_r = r_rbias;
		}

		r = r_r + (RTE_ff[0] + RTE_ff[1] * r_r + RTE_ff[2] * u_r)*cos(i_r);
		T = RTE_ee[0] + RTE_ee[1] * r + RTE_ee[2] * u_r;
		CR = RTE_cc * T*cos(mu)*cos(A_Z + 0.3*mu);
		//NM to radians
		eta_rz1 = abs(r) / NMPER;
		theta_cr = CR / NMPER;
	}

	VECTOR3 TVECT(VECTOR3 a, VECTOR3 b, double alpha, double gamma)
	{
		VECTOR3 Y_apo = b - a * cos(alpha);
		double y_apo = length(Y_apo);
		alpha = fmod(alpha, PI2);
		if (alpha < 0)
		{
			alpha += PI2;
		}
		VECTOR3 y_unit = Y_apo / y_apo *sign(PI - alpha);
		return a * cos(gamma) + y_unit * sin(gamma);
	}

	void EGTR(VECTOR3 R_geoc, VECTOR3 V_geoc, double GMT, double alpha_SID0, VECTOR3 &R_geogr, VECTOR3 &V_geogr)
	{
		MATRIX3 Rot;
		VECTOR3 V_e;
		double alpha_SID;

		alpha_SID = alpha_SID0 + GMT * OrbMech::w_Earth;
		Rot = _M(cos(alpha_SID), sin(alpha_SID), 0, -sin(alpha_SID), cos(alpha_SID), 0, 0, 0, 1);

		V_e = _V(-OrbMech::w_Earth*R_geoc.y, OrbMech::w_Earth*R_geoc.x, 0);
		R_geogr = mul(Rot, R_geoc);
		V_geogr = mul(Rot, V_geoc - V_e);
	}

	double INTER(const double *X, const double *Y, int IMAX, double x)
	{
		double y[2] = { 0.0, 0.0 };
		double a[3], b[3], yy;
		int i = 0;
		while (x > X[i])
		{
			i++;
			if (i > IMAX - 1)
			{
				//error
				return Y[IMAX - 1];
			}
		}
		if (x == X[i])
		{
			yy = Y[i];
			return yy;
		}
		if (i == 0)
		{
			//error
			return Y[0];
		}
		if (i != 1)
		{
			a[0] = X[i - 2];
			a[1] = X[i - 1];
			a[2] = X[i];
			b[0] = Y[i - 2];
			b[1] = Y[i - 1];
			b[2] = Y[i];
			y[1] = (x - a[1])*(x - a[2]) / ((a[0] - a[1])*(a[0] - a[2]))*b[0] + (x - a[0])*(x - a[2]) / ((a[1] - a[0])*(a[1] - a[2]))*b[1] + 
				(x - a[0])*(x - a[1]) / ((a[2] - a[0])*(a[2] - a[1]))*b[2];
		}
		if (i != IMAX - 1)
		{
			a[0] = X[i - 1];
			a[1] = X[i];
			a[2] = X[i + 1];
			b[0] = Y[i - 1];
			b[1] = Y[i];
			b[2] = Y[i + 1];
			y[0] = (x - a[1])*(x - a[2]) / ((a[0] - a[1])*(a[0] - a[2]))*b[0] + (x - a[0])*(x - a[2]) / ((a[1] - a[0])*(a[1] - a[2]))*b[1] +
				(x - a[0])*(x - a[1]) / ((a[2] - a[0])*(a[2] - a[1]))*b[2];
		}
		yy = (y[0] + y[1]) / 2.0;
		if (y[0] == 0.0 || y[1] == 0.0)
		{
			yy = y[0] + y[1];
		}

		return yy;
	}

	double URF(double T, double x)
	{
		// INPUT:
		// T = flight time (hr)
		// x = radial distance (er)
		// OUTPUT:
		// u_r = reentry velocity (er/hr)

		double t, r, CV, u_r_apo, u_r;
		double B[21];

		t = 1.0 / T;
		r = log10(x);
		CV = 20925738.2 / 3600.0;

		if (t >= 0.005)
		{
			B[0] = 468.472477; 
			B[1] = 7888927872.0;
			B[2] = -61855.51562;
			B[3] = 110537.274;
			B[4] = -171799868.0;
			B[5] = 4620848192.0;
			B[6] = -906.54586;
			B[7] = -7956138368.0;
			B[8] = 245869.516;
			B[9] = 9781565.88;
			B[10] = 233791598.0;
			B[11] = -3296.07199;
			B[12] = 68248515.0;
			B[13] = 17869.88452;
			B[14] = -20069842.5;
			B[15] = 8065.55524;
			B[16] = 8235085.44;
			B[17] = -1037248.34;
			B[18] = -419.062894; 
			B[19] = 966684.227; 
			B[20] = 30762.3813;
		}
		else
		{
			B[0] = 714.692459;
			B[1] = -0.39039204e15;
			B[2] = -59994.8452;
			B[3] = 67463803.0;
			B[4] = -0.7353992e11;
			B[5] = -0.82126629e12;
			B[6] = -4242.845153;
			B[7] = 0.69973117e13;
			B[8] = 349795.461;
			B[9] = 225454802.0;
			B[10] = 0.26584838e12;
			B[11] = 8898.09949;
			B[12] = -0.26926284e12;
			B[13] = -1822582.34;
			B[14] = -1428488260.0;
			B[15] = -6715.65247;
			B[16] = 1523663950.0;
			B[17] = 4487404.25;
			B[18] = -828.241081;
			B[19] = -3735268.44;
			B[20] = 38712.8818;
		}

		u_r_apo = B[0] * pow(r, 5) + B[1] * pow(t, 5) + B[2] * pow(r, 4)*t + B[3] * pow(r, 3)*pow(t, 2) + B[4] * pow(r, 2)*pow(t, 3) + B[5] * r*pow(t, 4) +
			B[6] * pow(r, 4) + B[7] * pow(t, 4) + B[8] * pow(r, 3)*t + B[9] * pow(r, 2)*pow(t, 2) + B[10] * r*pow(t, 3) + B[11] * pow(r, 3) + B[12] * pow(t, 3) + 
			B[13] * pow(r, 2)*t + B[14] * r*pow(t, 2) + B[15] * pow(r, 2) + B[16] * pow(t, 2) + B[17] * r*t + B[18] * r + B[19] * t + B[20];
		u_r = u_r_apo / CV;
		return u_r;
	}

	void AESR(double r1, double r2, double beta1, double T, double R, double mu, double eps, double &a, double &e, bool &k2, int &info, double &V1)
	{
		//INPUT:
		//r1: first radial distance
		//r2: second radial distance
		//beta1: flight-path angle for first radius
		//T: flight time
		//R: radius of the central body
		//mu: gravitational constant
		//eps: tolerance on time
		//OUTPUT:
		//a: semimajor axis; or p, the semilatus rectum
		//e: eccentricity
		//k2: 0 = no apogee passage, 1 = apogee passage
		//INFO: 0 = no solution found, 1 = solution found
		//V1: reentry velocity

		double tan_eta1_2, M_1, q, p, tan_eta2_2, M_2, T_P, DT, T_m, a_m, T_1, DDT_m, esinE_1, ecosE_1, E_1, DT_m, Q, k_1_apo, DT_1;
		double esinE_2, ecosE_2, E_2, T_12, k_apo, k, B, sec2_eta12;
		int iter, IH, C_2;

		C_2 = 1;
		k2 = false;
		//On parabolic trajectories the flight path angle (measured from local horizontal!) is always one-half the true anomaly
		tan_eta1_2 = OrbMech::cot(beta1);
		//M_1 is twice of parabolic mean anomaly
		M_1 = tan_eta1_2 + 1.0 / 3.0*pow(tan_eta1_2, 3);
		sec2_eta12 = 1.0 + tan_eta1_2 * tan_eta1_2;
		//This seems to be the periapsis radius
		q = r1 / sec2_eta12;
		p = 2.0*q;
		//MSC memo had no minus here. But we are returning home on a parabolic trajectory, so we better have a minus here.
		tan_eta2_2 = sqrt(r2 / q - 1.0);
		//MSC memo had square root here. MSC memos is clearly wrong.
		M_2 = tan_eta2_2 + 1.0 / 3.0*pow(tan_eta2_2, 3);
		//M_1 and M_2 exchanged from what MSC memo had.
		T_P = q * sqrt(p / mu)*(M_2 - M_1);
		//Difference between parabolic and desired flight time
		DT = T_P - T;
		//We have no error... yet
		info = 0;
		//If we are very nearly parabolic, assume we are parabolic
		if (abs(DT) <= eps)
		{
			//Semi-major axis is undefined, just set it to 0
			a = 0.0;
			//Eccentricity is 1, as we are hyperbolic
			e = 1.0;
			//Parabolic velocity is easily found
			V1 = 2.0*mu / r1;
			return;
		}
		//If parabolic flight time is greater than desired flight time then we are hyperbolic
		if (T_P > T)
		{
			//Set up hyberbolic iteration
			iter = 2;
			k_1_apo = 0.0;
			T_m = 0.0;
			a_m = 0.0;
			IH = 2;
			a = -2.0*R;
			T_1 = T_P;
			DT_m = T;
		}
		else
		{
			IH = 1;
			//Assumes r2 is at apogee to distingush between no apogee passage vs. apogee passage?
			a_m = 0.5*(pow(r2, 2) - pow(r1, 2)*pow(sin(beta1), 2)) / (r2 - r1 * pow(sin(beta1), 2));
			//Velocity at r1 with semi-major axis a_m
			V1 = sqrt(mu*(2.0 / r1 - 1.0 / a_m));
			//Terms for eccentric anomaly
			esinE_1 = r1 * V1*cos(beta1) / sqrt(a_m*mu);
			ecosE_1 = 1.0 - r1 / a_m;
			//Calculate eccentric anomaly
			E_1 = atan2(esinE_1, ecosE_1);
			//E_1 between 0° and 360°
			if (E_1 < 0)
			{
				E_1 += PI2;
			}
			//Calculate mean anomaly
			M_1 = E_1 - esinE_1;
			T_m = a_m * sqrt(a_m / mu)*(PI - M_1);
			DT_m = T_m - T;
			//r2 was at apogee
			if (abs(T_m) < eps)
			{
				e = esinE_1 / sin(E_1);
				a = a_m;
				return;
			}
			//Check if T is greater than the time it takes to return from apogee
			if (T_m < T)
			{
				//We have an apogee passage
				Q = -1.0;
				k2 = true;
				iter = 2;
				k_1_apo = 0.0;
				T_1 = T_P;
			}
			else
			{
				//We have no apogee passage
				Q = 1.0;
				k2 = false;
				if (T <= 1.2*T_P)
				{
					iter = 1;
					DT_1 = T - T_P;
				}
				else
				{
					iter = 2;
					k_1_apo = 0.0;
					T_1 = T_P;
				}
			}
			a = a_m + 2.0*R;
		}

		DDT_m = DT_m * DT_m;

		do
		{
			V1 = sqrt(mu*(2.0 / r1 - 1.0 / a));
			esinE_1 = r1 * V1*cos(beta1) / sqrt(abs(a)*mu);
			ecosE_1 = 1.0 - r1 / a;
			if (a > 0.0)
			{
				e = sqrt(ecosE_1 * ecosE_1 + esinE_1 * esinE_1); //Pythagorean identity for ellipses
				E_1 = atan2(esinE_1, ecosE_1);
				if (E_1 < 0)
				{
					E_1 += PI2;
				}
				ecosE_2 = 1.0 - r2 / a;
				esinE_2 = Q * sqrt(e*e - ecosE_2 * ecosE_2);
				E_2 = atan2(esinE_2, ecosE_2);
				if (E_2 < 0)
				{
					E_2 += PI2;
				}
			}
			else
			{
				e = sqrt(ecosE_1 * ecosE_1 - esinE_1 * esinE_1); //Pythagorean identity for hyperbolas
				E_1 = atanh(esinE_1 / ecosE_1);
				E_2 = acosh(1.0 / e * (1.0 - r2 / a));
				esinE_2 = e * sinh(E_2);
			}
			//Mean anomalies
			M_1 = a / abs(a)*(E_1 - esinE_1);
			M_2 = a / abs(a)*(E_2 - esinE_2);
			//Flight time between the two mean anomalies
			T_12 = sqrt(pow(abs(a), 3) / mu)*(M_2 - M_1);
			//Time difference between desired and calculated flight time
			DT = T_12 - T;
			//Calculation has converged, return
			if (abs(DT) < eps) return;
			if (C_2 > 20)
			{
				info = 2;
				return;
			}
			C_2++;
			k_apo = pow(T_12 - T_m, 2) / (a - a_m);
			if (iter == 1)
			{
				B = k_apo / (T_12 - T_P);
				k = B * DT_1;
			}
			else
			{
				B = (k_apo - k_1_apo) / (T_12 - T_1);
				k = k_apo + B * (T - T_12);
				k_1_apo = k_apo;
				T_1 = T_12;
			}
			a = DT_m * DT_m / k + a_m;
		} while (abs(DT) >= eps);
	}

	int MINMIZ(VECTOR3 &X, VECTOR3 &Y, VECTOR3 &Z, bool opt, VECTOR3 CUR, double TOL, double &XMIN, double &YMIN)
	{
		int ISUB = 0;

		if (opt == false)
		{
			int J;

			if (CUR.x - X.y != 0)
			{
				if (CUR.x - X.y < 0)
				{
					if (CUR.y < Y.y)
					{
						X.z = X.y;
						Y.z = Y.y;
						Z.z = Z.y;
						J = 2;
					}
					else
					{
						J = 1;
					}
				}
				else
				{
					if (CUR.y < Y.y)
					{
						X.x = X.y;
						Y.x = Y.y;
						Z.x = Z.y;
						J = 2;
					}
					else
					{
						J = 3;
					}
				}
				X.data[J - 1] = CUR.x;
				Y.data[J - 1] = CUR.y;
				Z.data[J - 1] = CUR.z;
			}
		}

		if ((Y.x <= Y.y) || (Y.z <= Y.y))
		{
			if (Y.x < Y.z)
			{
				ISUB = 1;
				XMIN = X.x + TOL;
			}
			else
			{
				ISUB = 3;
				XMIN = X.z - TOL;
			}
			YMIN = pow(10, 10);
			return ISUB;
		}

		double X1, X2, Y1, Y2;
		Y1 = (Y.x - Y.y) / (X.x - X.y);
		Y2 = (Y.y - Y.z) / (X.y - X.z);
		X1 = (X.x + X.y) / 2.0;
		X2 = (X.y + X.z) / 2.0;
		XMIN = X1 - Y1 * (X2 - X1) / (Y2 - Y1);
		YMIN = Y.y - (pow(X.y - XMIN, 2)*(Y.x - Y.z)) / (pow(X.x - XMIN, 2) - pow(X.z - XMIN, 2));

		return 0;
	}

	double LNDING(VECTOR3 REI, VECTOR3 VEI, double GMT_EI, double alpha_SIDO0, double LD, int ICRNGG, double r_rbias, double &lambda, double &phi, double &GMT_L)
	{
		VECTOR3 H, R_EI_u, V_EI_u, R_1_u, N_u, R_1_apo, R_geo, V_geo;
		double i_r, rmag, vmag, rtasc, decl, fpav, az, eta_rzI, theta_cr, T, NMPER;

		NMPER = 3443.93359;
		
		R_EI_u = unit(REI);
		V_EI_u = unit(VEI);

		OrbMech::rv_from_adbar(REI, VEI, rmag, vmag, rtasc, decl, fpav, az);
		H = crossp(REI, VEI);
		i_r = acos(H.z / length(H));

		REENTRYNew(LD, ICRNGG, vmag, i_r, az, decl, r_rbias, eta_rzI, theta_cr, T);

		R_1_u = TVECT(R_EI_u, V_EI_u, fpav, eta_rzI);
		N_u = unit(crossp(REI, VEI));
		R_1_apo = R_1_u * cos(theta_cr) + N_u * sin(theta_cr);
		GMT_L = GMT_EI + T;
		if (theta_cr*(R_1_u.z - R_1_apo.z) < 0)
		{
			R_1_apo = R_1_u * cos(theta_cr) - N_u * sin(theta_cr);
		}

		EGTR(R_1_apo, _V(0, 0, 0), GMT_L, alpha_SIDO0, R_geo, V_geo);
		OrbMech::latlong_from_r(R_geo, phi, lambda);

		return eta_rzI * NMPER;
	}

	void SIDCOM(double JD0, double DT, double N, double &alpha_go, double &T)
	{
		double JDt, JDf, JD, J1, J12;
		int JDI, J11;

		JDt = JD0 - 2.43e6 + DT / 86.4;
		JDI = (int)JDt;
		JDf = JDt - (double)JDI;

		if (JDf < 0.5)
		{
			if (JDf != 0.5)
			{
				JDf = 0.5;
			}
			JD = (double)JDI + JDf;
		}
		else
		{
			JDf = 0.5;
			JDI = JDI - 1;
		}
		J1 = (JD + 14980.0) / 365.25;
		J11 = (int)J1;
		J12 = J1 - (double)J11;
		alpha_go = PI / 43200.0*(23925.836 + 1.84542*(double)J11 + 9.29e-6*pow((double)J11, 2) + N) + PI2 * J12;
		T = fmod(alpha_go, PI2);
	}

	double XDOTX2(VECTOR3 a, VECTOR3 b, double DIFG)
	{
		//Calculate alpha given a, b, and DIFG

		double alpha, k;

		alpha = acos2(dotp(unit(a), unit(b)));
		k = DIFG * (a.x*b.y - a.y*b.x);
		if (k < 0)
		{
			alpha = PI2 - alpha;
		}
		return alpha;
	}

	void RA2XYZ4(VECTOR3 R, VECTOR3 V, double &beta, double &A_Z)
	{
		//Calculate beta, A_Z given R and V
		double c, r;

		r = length(R);
		beta = XDOTX2(unit(R), unit(V), 0.0);
		c = dotp(R, V) / r;
		A_Z = atan2(R.x*V.y - R.y*V.x, r*V.z - c * R.z);
		if (A_Z < 0)
		{
			A_Z += PI2;
		}
	}

	double landingzonelong(int zone, double lat)
	{
		if (zone == 0)
		{
			return MPL(lat);
		}
		else if (zone == 1)
		{
			return EPL(lat);
		}
		else if (zone == 2)
		{
			return AOL(lat);
		}
		else if (zone == 3)
		{
			return IOL(lat);
		}
		else
		{
			return WPL(lat);
		}
	}

	double MPL(double lat) //Calculate the splashdown longitude from the latitude for the Mid-Pacific landing area
	{
		return -165.0*RAD;
	}

	double MPL2(double lat) //Calculate the splashdown longitude from the latitude for the Mid-Pacific landing area (Apollo 11 and later)
	{
		if (lat > 15.0*RAD)
		{
			return -175.0*RAD;
		}
		else if (lat > 0.0)
		{
			return -10.0 / 15.0*lat - 165.0*RAD;
		}
		else
		{
			return -165.0*RAD;
		}
	}

	double EPL(double lat) //Calculate the splashdown longitude from the latitude for the East-Pacific landing area
	{
		if (lat > 21.0*RAD)
		{
			return (-135.0*RAD + 122.0*RAD) / (40.0*RAD - 21.0*RAD)*(lat - 21.0*RAD) - 122.0*RAD;
		}
		else if (lat > -11.0*RAD)
		{
			return (-122.0*RAD + 89.0*RAD) / (21.0*RAD + 11.0*RAD)*(lat + 11.0*RAD) - 89.0*RAD;
		}
		else
		{
			return (-89.0*RAD + 83.0*RAD) / (-11.0*RAD + 40.0*RAD)*(lat + 40.0*RAD) - 83.0*RAD;
		}
	}

	double AOL(double lat)
	{
		if (lat > 10.0*RAD)
		{
			return -30.0*RAD;
		}
		else if (lat > -5.0*RAD)
		{
			return (-30.0*RAD + 25.0*RAD) / (10.0*RAD + 5.0*RAD)*(lat + 5.0*RAD) - 25.0*RAD;
		}
		else
		{
			return -25.0*RAD;
		}
	}

	double IOL(double lat)
	{
		return 65.0*RAD;
	}

	double WPL(double lat)
	{
		if (lat > 10.0*RAD)
		{
			return 150.0*RAD;
		}
		else if (lat > -15.0*RAD)
		{
			return (150.0*RAD - 170.0*RAD) / (10.0*RAD + 15.0*RAD)*(lat + 15.0*RAD) + 170.0*RAD;
		}
		else
		{
			return 170.0*RAD;
		}
	}

	bool TBLOOK(double *LINE, double lat, double &lng)
	{
		double XR[5], YR[5];
		bool LF = false;
		int N, J = 2;

		while (J - 1 < 10 && LINE[J - 1] != 1e10)
		{
			N = J / 2;
			YR[N - 1] = LINE[J - 2];
			XR[N - 1] = LINE[J - 1];
			J = J + 2;
		}

		if (lat > YR[0])
		{
			lng = XR[0];
			LF = true;
		}
		else
		{
			J = 2;

			while (lat <= YR[J - 1])
			{
				J++;
				if (J > N)
				{
					lng = XR[N - 1];
					LF = true;
					break;
				}
			}

			double TEST = XR[J - 1] - XR[J - 2];
			if (abs(TEST) >= PI)
			{
				if (TEST < 0)
				{
					TEST = TEST + PI2;
				}
				else
				{
					TEST = TEST - PI2;
				}
			}
			if (TEST == 0.0)
			{
				lng = XR[J - 1];
			}
			else
			{
				lng = TEST * (lat - YR[J - 2]) / (YR[J - 1] - YR[J - 2]) + XR[J - 2];
			}
		}
		if (lng > PI2)
		{
			lng = lng - PI2;
		}
		if (lng < 0)
		{
			lng = lng + PI2;
		}
		return LF;
	}
}

RetrofirePlanning::RetrofirePlanning(RTCC *r) : RTCCModule(r)
{
	SQMU = sqrt(OrbMech::mu_Earth);
}

bool RetrofirePlanning::RMSDBMP(EphemerisData sv, double GETI, double lat_T, double lng_T, double CSMmass, double Area)
{
	//Check if we are in lunar reference
	if (sv.RBI == BODY_MOON)
	{
		return true;
	}

	//Simulate burn
	PMMRKJInputArray integin;
	int PMMRKJerr;
	CSMLMPoweredFlightIntegration integ(pRTCC, integin, PMMRKJerr, NULL, &burnaux);
	ELVCTRInputTable in;
	ELVCTROutputTable2 out;
	ManeuverTimesTable mantimes;
	EphemerisData sv_ECT;
	RMMYNIInputTable reentryin;

	//Save data in class
	sv0 = sv;
	//If latitude less than -90° don't iterate on it
	if (lat_T < -PI05)
	{
		pRTCC->RZJCTTC.Type = 2;
	}
	else
	{
		pRTCC->RZJCTTC.Type = 1;
		this->lat_T = lat_T;
	}
	
	this->lng_T = lng_T;
	this->CSMmass = CSMmass;
	this->Area = Area;
	Thruster = pRTCC->RZJCTTC.R31_Thruster;
	BurnMode = pRTCC->RZJCTTC.R31_BurnMode;
	GimbalIndicator = pRTCC->RZJCTTC.R31_GimbalIndicator;
	refsid = pRTCC->RZJCTTC.R31_REFSMMAT;
	MD_max = pRTCC->RZJCTTC.R32_MD;
	if (Thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		dt_ullage = pRTCC->RZJCTTC.R31_UllageTime;
	}
	else
	{
		dt_ullage = 0.0;
	}

	if (pRTCC->RZJCTTC.R32_Code == 2)
	{
		if (pRTCC->RZJCTTC.R30_Thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			dt_ullage_sep = pRTCC->RZJCTTC.R30_Ullage_DT;
		}
		else
		{
			dt_ullage_sep = 0.0;
		}
	}

	//Set error indicator to zero
	ERR = 0;

	//Get REFSMMAT
	refsdata = pRTCC->EZJGMTX1.data[refsid - 1];

	//Get thruster thrust and mass flow
	double ullage_overlap = pRTCC->SystemParameters.MCTSD9;
	switch (Thruster)
	{
	case RTCC_ENGINETYPE_CSMRCSPLUS2:
		F = pRTCC->SystemParameters.MCTCT1;
		mdot = pRTCC->SystemParameters.MCTCW1;
		TCMC = pRTCC->SystemParameters.MCTCT5;
		break;
	case RTCC_ENGINETYPE_CSMRCSPLUS4:
		F = pRTCC->SystemParameters.MCTCT2;
		mdot = pRTCC->SystemParameters.MCTCW2;
		TCMC = pRTCC->SystemParameters.MCTCT6;
		break;
	case RTCC_ENGINETYPE_CSMRCSMINUS2:
		F = pRTCC->SystemParameters.MCTCT3;
		mdot = pRTCC->SystemParameters.MCTCW3;
		TCMC = pRTCC->SystemParameters.MCTCT5;
		break;
	case RTCC_ENGINETYPE_CSMRCSMINUS4:
		F = pRTCC->SystemParameters.MCTCT4;
		mdot = pRTCC->SystemParameters.MCTCW4;
		TCMC = pRTCC->SystemParameters.MCTCT6;
		break;
	case RTCC_ENGINETYPE_CSMSPS:
		F = pRTCC->SystemParameters.MCTST1;
		mdot = pRTCC->SystemParameters.MCTSW1;
		TCMC = pRTCC->SystemParameters.MCTST9;
		break;
	}

	//Convert GETs to GMTs
	if (GETI < 0)
	{
		WasGETIInput = false;
	}
	else
	{
		WasGETIInput = true;
		GMTI = pRTCC->GMTfromGET(GETI);
	}

	if (pRTCC->RZJCTTC.R30_GETI_SH > 0)
	{
		GMTI_SH = pRTCC->GMTfromGET(pRTCC->RZJCTTC.R30_GETI_SH);
	}
	else
	{
		GMTI_SH = 0.0;
	}
	
	//Determine maneuver type
	if (pRTCC->RZJCTTC.R32_Code == 1)
	{
		ManeuverType = 0;
	}
	else
	{
		if (GMTI_SH > 0)
		{
			ManeuverType = 2;
		}
		else
		{
			ManeuverType = 1;
		}
	}

	//Calculate bounds for ephemeris
	RMMDBF();

	if (ERR)
	{
		return true;
	}

	EphemerisData sv_L;
	EMMENIInputTable coastin;

	//Shaping maneuver
	if (ManeuverType == 2)
	{
		//Calculate time of shaping burn initiation
		double GMT_BI;
		if (dt_ullage_sep != 0.0)
		{
			GMT_BI = GMTI_SH - dt_ullage_sep + ullage_overlap;
		}
		else
		{
			GMT_BI = GMTI_SH;
		}

		coastin.AnchorVector = sv0;
		coastin.Area = Area;
		coastin.CutoffIndicator = 1;
		if (GMT_BI > sv0.GMT)
		{
			coastin.IsForwardIntegration = 1.0;
		}
		else
		{
			coastin.IsForwardIntegration = -1.0;
		}
		coastin.MaxIntegTime = abs(GMT_BI - sv0.GMT);
		coastin.EphemerisBuildIndicator = false;
		coastin.ECIEphemerisIndicator = false;
		coastin.Weight = CSMmass;

		pRTCC->EMMENI(coastin);

		sv_BI_SEP = coastin.sv_cutoff;

		//Calculate attitude
		VECTOR3 U_T_SEP, IMUAtt_SEP;
		RMMATT(1, 1, false, pRTCC->RZJCTTC.R30_Att, refsdata.REFSMMAT, pRTCC->RZJCTTC.R30_Thruster, sv_BI_SEP.R, sv_BI_SEP.V, pRTCC->RZJCTTC.R30_GimbalIndicator, CSMmass, U_T_SEP, IMUAtt_SEP);

		//Maneuver simulation
		integin.sv0 = sv_BI_SEP;
		integin.A = Area;
		integin.CAPWT = CSMmass;
		integin.CSMWT = CSMmass;
		integin.TVC = 1;
		integin.KEPHOP = 0;
		integin.KAUXOP = 1;
		integin.MANOP = -1;
		integin.ThrusterCode = pRTCC->RZJCTTC.R30_Thruster;
		integin.UllageOption = pRTCC->RZJCTTC.R30_Use4UllageThrusters;
		integin.IC = 1;
		integin.DTU = dt_ullage_sep;
		integin.KTRIMOP = pRTCC->RZJCTTC.R30_GimbalIndicator;
		integin.DTMAN = pRTCC->RZJCTTC.R30_DeltaT;
		integin.DVMAN = pRTCC->RZJCTTC.R30_DeltaV;
		integin.AT = U_T_SEP;
		integin.XB = X_B;
		integin.YB = Y_B;
		integin.ZB = Z_B;

		integ.PMMRKJ();

		if (PMMRKJerr)
		{
			RMGTTF("RMSDBMP", 36);
			return true;
		}

		//Save
		burnaux_sep = burnaux;

		sv_TIG_SEP = burnaux.sv_FF;

		sv_BO_SEP.R = burnaux.R_BO;
		sv_BO_SEP.V = burnaux.V_BO;
		sv_BO_SEP.GMT = burnaux.GMT_BO;
		sv_BO_SEP.RBI = BODY_EARTH;
		CSMmass_Sep = burnaux.WTEND;

		sv0_apo = sv_BO_SEP;
	}
	else
	{
		sv0_apo = sv0;
	}

	//Coast to TL
	if (TL == sv0_apo.GMT)
	{
		sv_L = sv0_apo;
	}
	else
	{
		coastin.AnchorVector = sv0_apo;
		coastin.Area = Area;
		coastin.CutoffIndicator = 1;
		if (TL > sv0_apo.GMT)
		{
			coastin.IsForwardIntegration = 1.0;
		}
		else
		{
			coastin.IsForwardIntegration = -1.0;
		}
		coastin.MaxIntegTime = abs(TL- sv0_apo.GMT);
		coastin.Weight = CSMmass;

		pRTCC->EMMENI(coastin);

		sv_L = coastin.sv_cutoff;
	}

	//Generate ephemeris
	coastin.AnchorVector = sv_L;
	coastin.Area = Area;
	coastin.EphemerisBuildIndicator = true;
	coastin.ECIEphemerisIndicator = true;
	coastin.ECIEphemTableIndicator = &ephem;
	coastin.IsForwardIntegration = 1.0;
	coastin.MaxIntegTime = TR - TL;
	coastin.Weight = CSMmass;

	pRTCC->EMMENI(coastin);

	//Coverge two body solution
	RMMDBM();

	if (ERR)
	{
		return true;
	}

	//Predict finite burn ignition time
	double dt_man = DVBURN / (F / CSMmass);
	GMTI = GMTI - dt_man / 2.0;

	if (ManeuverType == 1)
	{
		GMT_TI = GMTI - pRTCC->RZJCTTC.R30_DeltaT_Sep;
	}
	else
	{
		GMT_TI = GMTI;
	}

	//Get state vector at burn begin (ullage on)
	if (ManeuverType == 1)
	{
		if (dt_ullage_sep != 0.0)
		{
			in.GMT = GMT_TI - dt_ullage_sep + ullage_overlap;
		}
		else
		{
			in.GMT = GMT_TI;
		}
	}
	else
	{
		if (dt_ullage != 0.0)
		{
			in.GMT = GMTI - dt_ullage + ullage_overlap;
		}
		else
		{
			in.GMT = GMTI;
		}
	}
	
	pRTCC->ELVCTR(in, out, ephem, mantimes);

	if (out.ErrorCode == 2)
	{
		RMGTTF("RMSDBMP", 27);
	}
	if (out.ErrorCode > 2)
	{
		RMGTTF("RMSDBMP", 32);
		return true;
	}

	//Simulate sep maneuver
	if (ManeuverType != 1)
	{
		sv_BI = pRTCC->RotateSVToSOI(out.SV);
		if (ManeuverType == 0)
		{
			CSMmass_Sep = CSMmass;
		}
	}
	else
	{
		sv_BI_SEP = pRTCC->RotateSVToSOI(out.SV);

		//Calculate attitude
		VECTOR3 U_T_SEP, IMUAtt_SEP;
		RMMATT(1, 1, false, pRTCC->RZJCTTC.R30_Att, refsdata.REFSMMAT, pRTCC->RZJCTTC.R30_Thruster, sv_BI_SEP.R, sv_BI_SEP.V, pRTCC->RZJCTTC.R30_GimbalIndicator, CSMmass, U_T_SEP, IMUAtt_SEP);

		//Maneuver simulation
		integin.sv0 = sv_BI_SEP;
		integin.A = Area;
		integin.AT = U_T_SEP;
		integin.CAPWT = CSMmass;
		integin.CSMWT = CSMmass;
		integin.TVC = 1;
		integin.KEPHOP = 0;
		integin.KAUXOP = 1;
		integin.MANOP = -1;
		integin.ThrusterCode = pRTCC->RZJCTTC.R30_Thruster;
		integin.UllageOption = pRTCC->RZJCTTC.R30_Use4UllageThrusters;
		integin.IC = 1;
		integin.DTU = dt_ullage_sep;
		integin.KTRIMOP = pRTCC->RZJCTTC.R30_GimbalIndicator;
		integin.DTMAN = pRTCC->RZJCTTC.R30_DeltaT;
		integin.DVMAN = pRTCC->RZJCTTC.R30_DeltaV;
		integin.XB = X_B;
		integin.YB = Y_B;
		integin.ZB = Z_B;

		integ.PMMRKJ();

		if (PMMRKJerr)
		{
			RMGTTF("RMSDBMP", 36);
			return true;
		}

		//Save
		burnaux_sep = burnaux;

		sv_TIG_SEP = burnaux.sv_FF;

		sv_BO_SEP.R = burnaux.R_BO;
		sv_BO_SEP.V = burnaux.V_BO;
		sv_BO_SEP.GMT = burnaux.GMT_BO;
		sv_BO_SEP.RBI = BODY_EARTH;
		CSMmass_Sep = burnaux.WTEND;

		//Integrate to deorbit burn
		coastin.AnchorVector = sv_BO_SEP;
		coastin.CutoffIndicator = 1;
		coastin.IsForwardIntegration = 1.0;
		coastin.MaxIntegTime = (GMTI - dt_ullage + ullage_overlap) - sv_BO_SEP.GMT;
		coastin.EphemerisBuildIndicator = false;
		coastin.ECIEphemerisIndicator = false;
		coastin.Weight = CSMmass_Sep;

		pRTCC->EMMENI(coastin);

		sv_BI = coastin.sv_cutoff;
	}

	//Calculate thrust direction
	if (pRTCC->RZJCTTC.R31_AttitudeMode == 1)
	{
		LVLHAtt = pRTCC->RZJCTTC.R31_LVLHAttitude;
	}
	else
	{
		LVLHAtt = _V(0.0, -(31.7*RAD + 2.15*RAD + acos(OrbMech::R_Earth / length(sv_BI.R))), PI);
	}
	RMMATT(1, 1, false, LVLHAtt, refsdata.REFSMMAT, Thruster, sv_BI.R, sv_BI.V, GimbalIndicator, CSMmass_Sep, U_T, IMUAtt);

	//Maneuver simulation
	integin.sv0 = sv_BI;
	integin.A = Area;
	integin.CAPWT = CSMmass_Sep;
	integin.CSMWT = CSMmass_Sep;
	integin.TVC = 1;
	integin.KEPHOP = 0;
	integin.KAUXOP = 1;
	integin.MANOP = pRTCC->RZJCTTC.R31_GuidanceMode;
	integin.ThrusterCode = Thruster;
	integin.UllageOption = pRTCC->RZJCTTC.R31_Use4UllageThrusters;
	integin.IC = 1;
	integin.DTU = dt_ullage;
	integin.KTRIMOP = GimbalIndicator;
	integin.VG = U_T * DVBURN;
	integin.ExtDVCoordInd = false;
	integin.DTMAN = 0.0;
	integin.DVMAN = 0.0;
	integin.XB = X_B;
	integin.YB = Y_B;
	integin.ZB = Z_B;
	integin.HeadsUpDownInd = true; //TBD

	integ.PMMRKJ();

	if (PMMRKJerr)
	{
		RMGTTF("RMSDBMP", 36);
		return true;
	}

	sv_TIG = burnaux.sv_FF;

	double r_apo, r_peri;
	OrbMech::periapo(burnaux.R_BO, burnaux.V_BO, OrbMech::mu_Earth, r_apo, r_peri);

	//Integrate to EI. TBD: Simulate sep maneuver and use CM area and weight for integration to EI
	sv_BO.R = burnaux.R_BO;
	sv_BO.V = burnaux.V_BO;
	sv_BO.GMT = burnaux.GMT_BO;
	sv_BO.RBI = BODY_EARTH;

	coastin.AnchorVector = sv_BO;
	coastin.CutoffIndicator = 3;
	coastin.IsForwardIntegration = 1.0;
	coastin.MaxIntegTime = 4.0*3600.0;
	coastin.StopParamRefFrame = 0;
	coastin.EarthRelStopParam = 400000.0*0.3048;
	coastin.EphemerisBuildIndicator = false;
	coastin.ECIEphemerisIndicator = false;
	coastin.Weight = burnaux.WTEND;

	pRTCC->EMMENI(coastin);

	if (coastin.TerminationCode != 3)
	{
		RMGTTF("RMSDBMP", 28);
		return true;
	}

	sv_EI = coastin.sv_cutoff;

	PARTSTAT = 2;

	if (pRTCC->RZJCTTC.Type == 1)
	{
		//Simulate reentry
		pRTCC->ELVCNV(sv_EI, 1, sv_ECT);
		reentryin.g_c_BU = pRTCC->RZJCTTC.R31_GLevel;
		reentryin.K1 = pRTCC->RZJCTTC.R31_InitialBankAngle;
		reentryin.K2 = pRTCC->RZJCTTC.R31_FinalBankAngle;
		reentryin.KSWCH = 2;
		reentryin.R0 = sv_ECT.R;
		reentryin.V0 = sv_ECT.V;
		reentryin.GMT0 = sv_ECT.GMT;
		reentryin.RLDIR = 1.0;

		//Integrate max lift to impact
		pRTCC->RMMYNI(reentryin, reentryout);

		//Did we impact?
		if (BurnMode != 3 && reentryout.IEND != 2)
		{
			RMGTTF("RMSDBMP", 29);
			return true;
		}

		lat_ML = reentryout.lat_IP;
		lng_ML = reentryout.lng_IP;

		//Integrate min lift to impact
		reentryin.KSWCH = 1;
		pRTCC->RMMYNI(reentryin, reentryout);

		//Did we impact?
		if (BurnMode != 3 && reentryout.IEND != 2)
		{
			RMGTTF("RMSDBMP", 29);
			return true;
		}

		lat_ZL = reentryout.lat_IP;
		lng_ZL = reentryout.lng_IP;

		RMMDBN(1);
	}

	MAINITER = 0;
	HASCONVERGED = false;

	do
	{
		//Simulate reentry
		pRTCC->ELVCNV(sv_EI, 1, sv_ECT);
		reentryin.g_c_BU = pRTCC->RZJCTTC.R31_GLevel;
		reentryin.K1 = pRTCC->RZJCTTC.R31_InitialBankAngle;
		reentryin.K2 = pRTCC->RZJCTTC.R31_FinalBankAngle;
		if (pRTCC->RZJCTTC.Type == 1)
		{
			reentryin.KSWCH = 8;
			reentryin.t_RB = t_RB;
		}
		else
		{
			reentryin.KSWCH = 9;
		}
		reentryin.R0 = sv_ECT.R;
		reentryin.V0 = sv_ECT.V;
		reentryin.GMT0 = sv_ECT.GMT;
		reentryin.RLDIR = 1.0;

		pRTCC->RMMYNI(reentryin, reentryout);
		lat_IP = reentryout.lat_IP;
		lng_IP = reentryout.lng_IP;

		//Did we impact?
		if (BurnMode != 3 && reentryout.IEND != 2)
		{
			RMGTTF("RMSDBMP", 29);
			return true;
		}

		dlng = lng_T - lng_IP;
		while (dlng > PI)
		{
			dlng -= PI2;
		}
		while (dlng < -PI)
		{
			dlng += PI2;
		}

		if (pRTCC->RZJCTTC.Type == 1)
		{
			dlat = lat_T - lat_IP;
		}
		else
		{
			dlat = 0.0;
		}

		//Compare target to impact, adjust retrofire maneuver quantities
		RMMDBN(2);

		//Continue loop?
		if (HASCONVERGED || ERR) break;

		//Get state vector at ullage on or ignition of first maneuver
		if (ManeuverType == 1)
		{
			GMT_TI = GMTI - pRTCC->RZJCTTC.R30_DeltaT_Sep;

			if (dt_ullage_sep != 0.0)
			{
				in.GMT = GMT_TI - dt_ullage_sep + ullage_overlap;
			}
			else
			{
				in.GMT = GMT_TI;
			}
		}
		else
		{
			GMT_TI = GMTI;

			if (dt_ullage != 0.0)
			{
				in.GMT = GMTI - dt_ullage + ullage_overlap;
			}
			else
			{
				in.GMT = GMTI;
			}
		}

		//Get state vector at burn begin (ullage on)
		pRTCC->ELVCTR(in, out, ephem, mantimes);

		if (out.ErrorCode == 2)
		{
			RMGTTF("RMSDBMP", 27);
		}
		if (out.ErrorCode > 2)
		{
			RMGTTF("RMSDBMP", 32);
			return true;
		}

		//Simulate sep maneuver
		if (ManeuverType != 1)
		{
			sv_BI = pRTCC->RotateSVToSOI(out.SV);
			if (ManeuverType == 0)
			{
				CSMmass_Sep = CSMmass;
			}
		}
		else
		{
			sv_BI_SEP = pRTCC->RotateSVToSOI(out.SV);

			//Calculate attitude
			VECTOR3 U_T_SEP, IMUAtt_SEP;
			RMMATT(1, 1, false, pRTCC->RZJCTTC.R30_Att, refsdata.REFSMMAT, pRTCC->RZJCTTC.R30_Thruster, sv_BI_SEP.R, sv_BI_SEP.V, pRTCC->RZJCTTC.R30_GimbalIndicator, CSMmass, U_T_SEP, IMUAtt_SEP);

			//Maneuver simulation
			integin.sv0 = sv_BI_SEP;
			integin.A = Area;
			integin.AT = U_T_SEP;
			integin.CAPWT = CSMmass;
			integin.CSMWT = CSMmass;
			integin.TVC = 1;
			integin.KEPHOP = 0;
			integin.KAUXOP = 1;
			integin.MANOP = -1;
			integin.ThrusterCode = pRTCC->RZJCTTC.R30_Thruster;
			integin.UllageOption = pRTCC->RZJCTTC.R30_Use4UllageThrusters;
			integin.IC = 1;
			integin.DTU = dt_ullage_sep;
			integin.KTRIMOP = pRTCC->RZJCTTC.R30_GimbalIndicator;
			integin.DTMAN = pRTCC->RZJCTTC.R30_DeltaT;
			integin.DVMAN = pRTCC->RZJCTTC.R30_DeltaV;
			integin.XB = X_B;
			integin.YB = Y_B;
			integin.ZB = Z_B;

			integ.PMMRKJ();

			if (PMMRKJerr)
			{
				RMGTTF("RMSDBMP", 36);
				return true;
			}

			//Save
			burnaux_sep = burnaux;
			sv_BO_SEP.R = burnaux.R_BO;
			sv_BO_SEP.V = burnaux.V_BO;
			sv_BO_SEP.GMT = burnaux.GMT_BO;
			sv_BO_SEP.RBI = BODY_EARTH;
			CSMmass_Sep = burnaux.WTEND;

			//Integrate to deorbit burn
			coastin.AnchorVector = sv_BO_SEP;
			coastin.CutoffIndicator = 1;
			coastin.IsForwardIntegration = 1.0;
			coastin.MaxIntegTime = (GMTI - dt_ullage + ullage_overlap) - sv_BO_SEP.GMT;
			coastin.Weight = CSMmass_Sep;

			pRTCC->EMMENI(coastin);

			sv_BI = coastin.sv_cutoff;
		}

		//Calculate thrust direction
		if (pRTCC->RZJCTTC.R31_AttitudeMode == 1)
		{
			LVLHAtt = pRTCC->RZJCTTC.R31_LVLHAttitude;
		}
		else
		{
			LVLHAtt = _V(0.0, -(31.7*RAD + 2.15*RAD + acos(OrbMech::R_Earth / length(sv_BI.R))), PI);
		}
		RMMATT(1, 1, false, LVLHAtt, refsdata.REFSMMAT, Thruster, sv_BI.R, sv_BI.V, GimbalIndicator, CSMmass_Sep, U_T, IMUAtt);

		integin.sv0 = sv_BI;
		integin.A = Area;
		integin.CAPWT = CSMmass_Sep;
		integin.CSMWT = CSMmass_Sep;
		integin.TVC = 1;
		integin.KEPHOP = 0;
		integin.KAUXOP = 1;
		integin.MANOP = pRTCC->RZJCTTC.R31_GuidanceMode;
		integin.ThrusterCode = Thruster;
		integin.UllageOption = pRTCC->RZJCTTC.R31_Use4UllageThrusters;
		integin.IC = 1;
		integin.DTU = dt_ullage;
		integin.KTRIMOP = GimbalIndicator;
		integin.VG = U_T * DVBURN;
		integin.ExtDVCoordInd = false;
		integin.DTMAN = 0.0;
		integin.DVMAN = 0.0;
		integin.XB = X_B;
		integin.YB = Y_B;
		integin.ZB = Z_B;
		integin.HeadsUpDownInd = true; //TBD

		integ.PMMRKJ();

		if (PMMRKJerr)
		{
			RMGTTF("RMSDBMP", 36);
			return true;
		}

		sv_TIG = burnaux.sv_FF;

		//Integrate to EI. TBD: Simulate sep maneuver and use CM area and weight for integration to EI
		sv_BO.R = burnaux.R_BO;
		sv_BO.V = burnaux.V_BO;
		sv_BO.GMT = burnaux.GMT_BO;
		sv_BO.RBI = BODY_EARTH;

		coastin.AnchorVector = sv_BO;
		coastin.CutoffIndicator = 3;
		coastin.IsForwardIntegration = 1.0;
		coastin.MaxIntegTime = 4.0*3600.0;
		coastin.StopParamRefFrame = 0;
		coastin.EarthRelStopParam = 400000.0*0.3048;
		coastin.EphemerisBuildIndicator = false;
		coastin.ECIEphemerisIndicator = false;
		coastin.Weight = burnaux.WTEND;

		pRTCC->EMMENI(coastin);

		if (coastin.TerminationCode != 3)
		{
			RMGTTF("RMSDBMP", 28);
			return true;
		}

		sv_EI = coastin.sv_cutoff;

	} while (HASCONVERGED == false && ERR == 0);

	if (ERR)
	{
		//Didn't converge
		RMGTTF("RMSDBMP", 31);
		return true;
	}

	//Store parameters from last reentry run
	gmax = reentryout.gmax;
	gmt_gmax = reentryout.t_gmax;

	//Integrate max lift to impact for display
	reentryin.KSWCH = 2;
	pRTCC->RMMYNI(reentryin, reentryout);
	if (reentryout.IEND == 2)
	{
		lat_ML = reentryout.lat_IP;
		lng_ML = reentryout.lng_IP;
	}
	else
	{
		lat_ML = lng_ML = 0.0;
	}
	//Integrate zero lift to impact for display
	reentryin.KSWCH = 1;
	pRTCC->RMMYNI(reentryin, reentryout);
	if (reentryout.IEND == 2)
	{
	lat_ZL = reentryout.lat_IP;
	lng_ZL = reentryout.lng_IP;
	}
	else
	{
		lat_ZL = lng_ZL = 0.0;
	}
	//Output display information
	RMSTTF();

	return false;
}

void RetrofirePlanning::RMMDBF()
{
	if (WasGETIInput)
	{
		TL = GMTI - 1.9*3600.0;
		TR = GMTI + 20.0*60.0;
	}
	else
	{
		//Calculate target longitude crossing

		//Convert vector to ECT
		EphemerisData sv0_ECT, sv_ECT;
		double GMT_sv, lng_v, dlambda;
		pRTCC->ELVCNV(sv0, 1, sv0_ECT);
		GMT_sv = sv0_ECT.GMT;

		lng_v = atan2(sv0_ECT.R.y, sv0_ECT.R.x) - OrbMech::w_Earth*(sv0_ECT.GMT - GMT_sv);

		dlambda = lng_T - lng_v;
		while (dlambda >= PI2)
		{
			dlambda -= PI2;
		}
		while (dlambda < 0)
		{
			dlambda += PI2;
		}

		double w_C, ddt, dt;
		int i = 0;

		dt = 0.0;
		w_C = PI2 / OrbMech::period(sv0_ECT.R, sv0_ECT.V, OrbMech::mu_Earth) - OrbMech::w_Earth;

		do
		{
			ddt = dlambda / w_C;
			dt += ddt;

			LLBRTDERR = pRTCC->LLBRTD(sv0_ECT, 0, SQMU, sv0_ECT.GMT + dt, sv_ECT);
			if (LLBRTDERR == 4)
			{
				RMGTTF("RMMDBF", 20);
				ERR = 1;
				return;
			}

			lng_v = atan2(sv_ECT.R.y, sv_ECT.R.x) - OrbMech::w_Earth*(sv_ECT.GMT - GMT_sv);

			dlambda = lng_T - lng_v;
			while (dlambda >= PI)
			{
				dlambda -= PI2;
			}
			while (dlambda < -PI)
			{
				dlambda += PI2;
			}

			if (abs(dlambda) < 0.001) break;

			w_C = length(sv_ECT.V) / length(sv_ECT.R) - OrbMech::w_Earth;

			i++;
		} while (i < 30);

		if (i == 30)
		{
			RMGTTF("RMMDBF", 24);
			ERR = 1;
			return;
		}
		TL = sv_ECT.GMT - 1.9*3600.0;
		TR = sv_ECT.GMT;
		if (Thruster == 33)
		{
			GMTI = TR - 20.0*60.0;
		}
		else
		{
			GMTI = TR - 30.0*60.0;
		}
	}
	//Additional checks if it's a type II maneuver (sep or shaping)
	if (ManeuverType == 0)
	{
		GMT_TI = GMTI;
	}
	else if (ManeuverType == 1)
	{
		GMT_TI = GMTI - pRTCC->RZJCTTC.R30_DeltaT_Sep;
	}
	else
	{
		if (GMTI < GMTI_SH)
		{
			RMGTTF("RMMDBF", 22);
			ERR = 1;
			return;
		}
		GMT_TI = GMTI;
	}
}

void RetrofirePlanning::RMMDBM()
{
	//State vector at deorbit burn time of ignition
	EphemerisData sv_TIG;
	//State vector at cutoff of deorbit burn
	EphemerisData sv_apo;
	VECTOR3 Att, Y;
	double R_E, theta_T;//, MJD_L;
	int iter;
	ELVCTRInputTable in;
	ELVCTROutputTable2 out;
	ManeuverTimesTable mantimes;

	//Calculate fixed DV for BurnMode 1 and 2
	if (BurnMode == 1)
	{
		//Fixed DV
		DVBURN = pRTCC->RZJCTTC.R31_dv;
	}
	else if (BurnMode == 2)
	{
		DVBURN = F / mdot * log(CSMmass / (CSMmass - mdot * pRTCC->RZJCTTC.R31_dt));
	}
	else
	{
		DVBURN = 0.0;
	}

	iter = 0;
	do
	{
		//Interpolate for vector
		in.GMT = GMT_TI;
		pRTCC->ELVCTR(in, out, ephem, mantimes);

		if (out.ErrorCode > 2)
		{
			//Error
			RMGTTF("RMMDBM", 51);
			ERR = 1;
			return;
		}

		//Path for sep maneuver
		if (ManeuverType != 1)
		{
			sv_TIG = sv_apo = pRTCC->RotateSVToSOI(out.SV);
		}
		else
		{
			EphemerisData sv_TIG_SEP, sv_apo_SEP;
			VECTOR3 U_T_SEP, IMUAtt_SEP;
			double DVBURN_SEP;

			sv_TIG_SEP = sv_apo_SEP = pRTCC->RotateSVToSOI(out.SV);

			//Calculate thrust direction
			RMMATT(1, 1, false, pRTCC->RZJCTTC.R30_Att, refsdata.REFSMMAT, pRTCC->RZJCTTC.R30_Thruster, sv_TIG_SEP.R, sv_TIG_SEP.V, pRTCC->RZJCTTC.R30_GimbalIndicator, CSMmass, U_T_SEP, IMUAtt_SEP);

			if (pRTCC->RZJCTTC.R30_DeltaV > 0)
			{
				DVBURN_SEP = pRTCC->RZJCTTC.R30_DeltaV;
			}
			else
			{
				DVBURN_SEP = pRTCC->SystemParameters.MCTCT2 / pRTCC->SystemParameters.MCTCW2 * log(CSMmass / (CSMmass - pRTCC->SystemParameters.MCTCW2 * pRTCC->RZJCTTC.R30_DeltaT)); //TBD: Assumes RCS right now
			}

			//Burn simulation
			sv_apo_SEP.V = sv_TIG_SEP.V + U_T_SEP * DVBURN_SEP;

			//Coast to TIG
			pRTCC->LLBRTD(sv_apo_SEP, 0, SQMU, GMTI, sv_TIG);
			sv_apo = sv_TIG;
		}

		if (pRTCC->RZJCTTC.R31_AttitudeMode == 1)
		{
			Att = pRTCC->RZJCTTC.R31_LVLHAttitude;
		}
		else
		{
			Att = _V(0.0, -(31.7*RAD + 2.15*RAD + acos(OrbMech::R_Earth / length(sv_TIG.R))), PI);
		}

		//Calculate thrust direction
		RMMATT(1, 1, false, Att, refsdata.REFSMMAT, Thruster, sv_TIG.R, sv_TIG.V, GimbalIndicator, CSMmass, U_T, IMUAtt);

		//Adjust thrust direction for impulsive maneuver
		Y = unit(crossp(sv_TIG.V, sv_TIG.R));
		theta_T = -length(crossp(sv_TIG.R, sv_TIG.V))*DVBURN*CSMmass / pow(length(sv_TIG.R), 2) / F;
		U_T = U_T * cos(theta_T / 2.0) + unit(crossp(U_T, Y))*sin(theta_T / 2.0);

		//Calculate DV fo V, gamma targeting
		if (BurnMode == 3)
		{
			DVBURN = 0.0;

			double r_a, r_p = 0.0, r_p_last;
			int i = 0;
			//Add DV until perigee radius is below 40 NM
			do
			{
				sv_apo.V = sv_TIG.V + U_T * DVBURN;
				r_p_last = r_p;
				OrbMech::periapo(sv_TIG.R, sv_apo.V, OrbMech::mu_Earth, r_a, r_p);
				if (i > 0 && r_p > r_p_last)
				{
					RMGTTF("RMMDBM", 53);
					ERR = 1;
					return;
				}
				DVBURN += 10.0;
				i++;
			} while (r_p > OrbMech::R_Earth + 40.0*1852.0);

			//Now iterate on V, gamma
			double v_EI, gamma_EI, gamma_EI_des, dgamma, dgammao, dvo;
			double c_I;
			int s_F;

			s_F = 0;
			c_I = 0.0;
			p_gam = 2000.0;

			do
			{
				sv_apo.V = sv_TIG.V + U_T * DVBURN;

				pRTCC->LLBRTD(sv_apo, -1, SQMU, OrbMech::R_Earth, 400000.0*0.3048, 1.0, sv_EI, R_E);
				v_EI = length(sv_EI.V);
				gamma_EI = acos(dotp(unit(sv_EI.R), unit(sv_EI.V)));
				gamma_EI_des = EntryCalculations::ReentryTargetLine(v_EI, true);

				dgamma = gamma_EI - gamma_EI_des;
				if (abs(dgamma) >= 0.001*RAD)
				{
					OrbMech::ITER(c_I, s_F, dgamma, p_gam, DVBURN, dgammao, dvo);
					if (s_F == 1)
					{
						break;
					}
				}
			} while (abs(dgamma) >= 0.001*RAD);
		}

		//Predict state vector at EI
		sv_apo.V = sv_TIG.V + U_T * DVBURN;
		LLBRTDERR = pRTCC->LLBRTD(sv_apo, -1, SQMU, OrbMech::R_Earth, 400000.0*0.3048, 1.0, sv_EI, R_E);

		if (LLBRTDERR)
		{
			if (LLBRTDERR == 4)
			{
				RMGTTF("RMMDBM", 56);
			}
			else
			{
				RMGTTF("RMMDBM", 53);
			}
			ERR = 1;
			return;
		}

		//Calculate landing point
		EntryCalculations::landingsite(pRTCC->SystemParameters.MAT_J2000_BRCS, sv_EI.R, sv_EI.V, pRTCC->GetGMTBase() + sv_EI.GMT / 24.0 / 3600.0, lng_IP, lat_IP);
		//EntryCalculations::LNDING(sv_EI.R, sv_EI.V, pRTCC->GetGMTBase() + sv_EI.GMT / 24.0 / 3600.0, 0.3, 1, 0.0, lng_IP, lat_IP, MJD_L);
		
		dlng = lng_T - lng_IP;
		while (dlng > PI)
		{
			dlng -= PI2;
		}
		while (dlng <= -PI)
		{
			dlng += PI2;
		}

		if (iter == 0)
		{
			GMTI_old = GMTI;
			lng_old = lng_IP;

			double w_C = PI2 / OrbMech::period(sv_TIG.R, sv_TIG.V, OrbMech::mu_Earth);
			p_dlng_dtf = w_C - OrbMech::w_Earth;
		}
		else
		{
			dlng2 = lng_IP - lng_old;

			while (dlng2 > PI)
			{
				dlng2 -= PI2;
			}
			while (dlng2 <= -PI)
			{
				dlng2 += PI2;
			}

			ddt = GMTI - GMTI_old;
			p_dlng_dtf = dlng2 / ddt;

			GMTI_old = GMTI;
			lng_old = lng_IP;
		}
		GMTI = GMTI + dlng / p_dlng_dtf;
		if (ManeuverType != 1)
		{
			GMT_TI = GMTI;
		}
		else
		{
			GMT_TI = GMTI - pRTCC->RZJCTTC.R30_DeltaT_Sep;
		}
		iter++;

	} while (abs(dlng) > 0.0072591411 && iter < 15); //25 NM tolerance

	if (iter >= 15)
	{
		//No convergence
		pRTCC->RTCCONLINEMON.DoubleBuffer[0] = dlng * 3443.93359;
		RMGTTF("RMMDBM", 52);
		ERR = 1;
		return;
	}
}

void RetrofirePlanning::RMMDBN(int entry)
{
	if (entry == 1)
	{
		//Estimate time to reverse bank angle
		t_RB = 350.0;

		//Reverse bank time has to be greater than the time of initial bank angle plus margin
		double t_GC = reentryout.t_gc - sv_EI.GMT;
		if (t_RB < t_GC + 120.0)
		{
			t_RB = t_GC + 120.0;
		}

		//Footprint calculations
		/*A = cos(lat_ZL)*sin(lat_ML) - cos(lat_ML)*sin(lat_ZL)*cos(lng_ML - lng_ZL);
		B = cos(lat_ML)*sin(lng_ML - lng_ZL);
		BI = cos(lat_T)*sin(lng_T - lng_ZL);
		AI = sin(lat_T)*cos(lat_ZL) - cos(lat_T)*cos(lng_T - lng_ZL)*sin(lat_ZL);
		SITHT = sqrt(AI*AI + BI * BI);
		SITHR = sqrt(A*A + B * B);
		PSI = atan2(B, A);
		PSIT = atan2(BI, AI);
		THETR = asin(SITHR);
		CSTR = cos(THETR);
		THET = asin(SITHT);
		SPMT = sin(PSI - PSIT);
		THTAC = asin(SPMT*SITHT);
		SINTHETL = SITHT * cos(PSI - PSIT) / cos(THTAC);*/
		PARTSTAT = 0;
	}
	else if (entry == 2)
	{
		if (BurnMode == 3)
		{
			double gamma_EI, gamma_EI_des, v_EI, dgamma;

			v_EI = length(sv_EI.V);
			gamma_EI = acos(dotp(unit(sv_EI.R), unit(sv_EI.V)));
			gamma_EI_des = EntryCalculations::ReentryTargetLine(v_EI, true);
			dgamma = gamma_EI - gamma_EI_des;
			if (abs(dgamma) > 0.001*RAD)
			{
				DVBURN -= dgamma / p_gam;
				return;
			}
		}
		//Calculate landing error
		MD_lat = dlat * 3443.93359;
		MD_lng = dlng * 3443.93359;
		MD_total = sqrt(MD_lat*MD_lat + MD_lng * MD_lng);

		//if (MAINITER == 0)
		//{
		//	sprintf(oapiDebugString(), "%lf %lf", MD_lat, MD_lng);
		//}

		//Check on convergence
		if (MD_total < MD_max || (abs(MD_lat) < 0.3 && abs(MD_lng) < 0.4))
		{
			pRTCC->RTCCONLINEMON.DoubleBuffer[0] = MD_lng;
			pRTCC->RTCCONLINEMON.DoubleBuffer[1] = MD_lat;
			RMGTTF("RMMDBN", 41);
			HASCONVERGED = true;
			return;
		}

		//Check on iteration limit
		MAINITER++;
		if (MAINITER >= 10)
		{
			pRTCC->RTCCONLINEMON.DoubleBuffer[0] = MD_lng;
			pRTCC->RTCCONLINEMON.DoubleBuffer[1] = MD_lat;
			RMGTTF("RMMDBN", 43);
			ERR = 1;
			return;
		}

		//Adjust GMTI
		if (pRTCC->RZJCTTC.Type == 1)
		{
			if (PARTSTAT == 0)
			{
				if (abs(MD_lng) > 25.0)
				{
					GMTI = GMTI + dlng / p_dlng_dtf;
				}
				else
				{
					//Start partials calculation
					dlat_0 = dlat;
					dlng_0 = dlng;
					t_RB += DT_TRB;
					
					PARTSTAT++;
					MAINITER--;
				}
			}
			else if (PARTSTAT == 1)
			{
				//Store landing point with perturbed time of reverse bank angle
				dlat_TRB = dlat;
				dlng_TRB = dlng;
				//Back to initial guess
				t_RB -= DT_TRB;
				//Perturb time-to-fire
				GMTI += DT_TTF;
				PARTSTAT++;
				MAINITER--;
			}
			else
			{
				if (PARTSTAT == 2)
				{
					//Calculate partials
					p_dlat_dtf = -(dlat - dlat_0) / DT_TTF;
					p_dlng_dtf = -(dlng - dlng_0) / DT_TTF;
					p_dlat_dtRB = -(dlat_TRB - dlat_0) / DT_TRB;
					p_dlng_dtRB = -(dlng_TRB - dlng_0) / DT_TRB;
					partialprod = (p_dlat_dtf*p_dlng_dtRB - p_dlat_dtRB * p_dlng_dtf);

					if (partialprod == 0.0)
					{
						pRTCC->RTCCONLINEMON.DoubleBuffer[0] = MD_lng;
						pRTCC->RTCCONLINEMON.DoubleBuffer[1] = MD_lat;
						RMGTTF("RMMDBN", 43);
						ERR = 1;
						return;
					}

					PARTSTAT++;
					MAINITER--;
				}

				GMTI += (p_dlng_dtRB * dlat - p_dlat_dtRB * dlng) / partialprod;
				t_RB += (-p_dlng_dtf * dlat + p_dlat_dtf * dlng) / partialprod;

				if (t_RB < 0)
				{
					t_RB = 0.0;
				}
			}
		}
		else
		{
			//Logic for converging on a longitude only
			if (MAINITER > 1)
			{
				//Update partial for dlng/dtf
				dlng2 = lng_IP - lng_old;

				while (dlng2 > PI)
				{
					dlng2 -= PI2;
				}
				while (dlng2 <= -PI)
				{
					dlng2 += PI2;
				}

				ddt = GMTI - GMTI_old;
				p_dlng_dtf = dlng2 / ddt;
			}

			GMTI_old = GMTI;
			lng_old = lng_IP;

			GMTI = GMTI + dlng / p_dlng_dtf;
		}
	}
}

void RetrofirePlanning::RMMATT(int entry, int opt, bool calcDesired, VECTOR3 Att, MATRIX3 REFSMMAT, int thruster, VECTOR3 R, VECTOR3 V, int TrimIndicator, double mass, VECTOR3 &U_T, VECTOR3 &OtherAtt)
{
	//entry: 1 = calculate unit vector. 2 = calculate attitude in the other coordinate system and the deorbit REFSMMAT
	//opt: 1 = Att is LVLH, 2 = Att is IMU

	VECTOR3 X_P, Y_P, Z_P;
	double SINP, SINY, SINR, COSP, COSY, COSR;
	double AL, BE, a1, a2, a3, b1, b2, b3, c1, c2, c3;
	double P_G = 0.0, Y_G = 0.0;

	SINP = sin(Att.y);
	SINY = sin(Att.z);
	SINR = sin(Att.x);
	COSP = cos(Att.y);
	COSY = cos(Att.z);
	COSR = cos(Att.x);

	if (thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		if (TrimIndicator == -1)
		{
			double T, WDOT;
			unsigned int IC = 1;
			pRTCC->GIMGBL(mass, 0.0, P_G, Y_G, T, WDOT, RTCC_ENGINETYPE_CSMSPS, IC, 1, 1, 0.0);
		}
		else
		{
			pRTCC->GetSystemGimbalAngles(RTCC_ENGINETYPE_CSMSPS, P_G, Y_G);
		}
	}

	if (opt == 1)
	{
		VECTOR3 Y_T, Z_T;

		Z_P = -unit(R);
		Y_P = -unit(crossp(R, V));
		X_P = crossp(Y_P, Z_P);

		AL = SINP * SINR;
		BE = SINP * COSR;
		a1 = COSY * COSP;
		a2 = SINY * COSP;
		a3 = -SINP;
		b1 = AL * COSY - SINY * COSR;
		b2 = AL * SINY + COSY * COSR;
		b3 = COSP * SINR;
		c1 = BE * COSY + SINY * SINR;
		c2 = BE * SINY - COSY * SINR;
		c3 = COSP * COSR;

		U_T = X_P * a1 + Y_P * a2 + Z_P * a3;
		Y_T = X_P * b1 + Y_P * b2 + Z_P * b3;
		Z_T = X_P * c1 + Y_P * c2 + Z_P * c3;

		X_B = U_T * cos(P_G)*cos(Y_G) - Y_T * cos(P_G)*sin(Y_G) + Z_T * sin(P_G);
		Y_B = U_T * sin(Y_G) + Y_T * cos(Y_G);
		Z_B = crossp(X_B, Y_B);
	}
	else
	{
		X_P = _V(REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13);
		Y_P = _V(REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23);
		Z_P = _V(REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		AL = COSP * SINY;
		BE = SINP * SINY;
		a1 = COSP * COSY;
		a2 = SINY;
		a3 = -SINP * COSY;
		b1 = SINP * SINR - AL * COSR;
		b2 = COSY * COSR;
		b3 = COSP * SINR + BE * COSR;
		c1 = SINP * COSR + AL * SINR;
		c2 = -COSY * SINR;
		c3 = COSP * COSR - BE * SINR;

		X_B = X_P * a1 + Y_P * a2 + Z_P * a3;
		Y_B = X_P * b1 + Y_P * b2 + Z_P * b3;
		Z_B = X_P * c1 + Y_P * c2 + Z_P * c3;

		if (thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			MATRIX3 MTEMP = _M(X_B.x, X_B.y, X_B.z, Y_B.x, Y_B.y, Y_B.z, Z_B.x, Z_B.y, Z_B.z);
			MATRIX3 MTEMP2 = pRTCC->GLMRTM(MTEMP, P_G, 2, Y_G, 3);
			U_T = _V(MTEMP2.m11, MTEMP2.m12, MTEMP2.m13);
		}
		else if (thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
		{
			U_T = X_B;
		}
		else
		{
			U_T = -X_B;
		}
	}

	if (entry == 1)
	{
		return;
	}

	//Attitude in other coordinates
	if (opt == 1)
	{
		//Calculate IMU angles
		X_P = _V(REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13);
		Y_P = _V(REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23);
		Z_P = _V(REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		OtherAtt.z = asin(dotp(Y_P, X_B));
		if (abs(abs(OtherAtt.z) - PI05) < 1e-8)
		{
			OtherAtt.x = 0.0;
			OtherAtt.y = atan2(dotp(X_P, Z_B), dotp(Z_P, Z_B));
		}
		else
		{
			OtherAtt.x = atan2(-dotp(Y_P, Z_B), dotp(Y_P, Y_B));
			OtherAtt.y = atan2(-dotp(Z_P, X_B), dotp(X_P, X_B));
		}

		if (OtherAtt.x < 0)
		{
			OtherAtt.x += PI2;
		}
		if (OtherAtt.y < 0)
		{
			OtherAtt.y += PI2;
		}
		if (OtherAtt.z < 0)
		{
			OtherAtt.z += PI2;
		}
	}
	else
	{
		//Calculate LVLH angles
		Z_P = unit(-R);
		Y_P = unit(crossp(V, R));
		X_P = unit(crossp(Y_P, Z_P));
		OtherAtt.y = asin(-dotp(Z_P, X_B));
		if (abs(abs(OtherAtt.y) - PI05) < 0.0017)
		{
			OtherAtt.x = 0.0;
			OtherAtt.z = atan2(-dotp(X_P, Y_B), dotp(Y_P, Y_B));
		}
		else
		{
			OtherAtt.z = atan2(dotp(Y_P, X_B), dotp(X_P, X_B));
			OtherAtt.x = atan2(dotp(Z_P, Y_B), dotp(Z_P, Z_B));
		}
	}

	if (calcDesired)
	{
		//Retrofire preferred alignment
		VECTOR3 X_SM, Y_SM, Z_SM;
		X_SM = -X_B;
		if (thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			Y_SM = -Y_B;
			Z_SM = Z_B;
		}
		else
		{
			Y_SM = Y_B;
			Z_SM = -Z_B;
		}
		DesREFSMMAT = _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);
	}
}

void RetrofirePlanning::RMSTTF()
{
	//Just for convenience, later we should have primary and contingency tables, too
	RetrofireDisplayParametersTableData *tab = &pRTCC->RZRFDP.data[2];

	TimeConstraintsTable elem;
	double r_apo, r_peri;

	tab->Indicator = 0;

	if (pRTCC->RZJCTTC.R32_Code == 2)
	{
		tab->Indicator_Sep = 0;

		EphemerisData sv_BO_SEP_ECT;
		pRTCC->ELVCNV(sv_BO_SEP, 1, sv_BO_SEP_ECT);
		sv_BO_SEP_ECT.RBI = BODY_EARTH;
		pRTCC->EMMDYNEL(sv_BO_SEP_ECT, elem);
		
		pRTCC->PIFAAP(elem.a, elem.e, elem.i, elem.TA, elem.TA + elem.AoP, length(sv_BO_SEP_ECT.R), r_apo, r_peri);

		tab->CSMWeightSep = burnaux_sep.WTENGON *LBS*1000.0;
		tab->H_apo = (r_apo - OrbMech::R_Earth) / 1852.0;
		tab->H_peri = (r_peri - OrbMech::R_Earth) / 1852.0;
		tab->TrueAnomalySep = elem.TA*DEG;
		tab->H_Sep = elem.h / 1852.0;

		if (pRTCC->RZJCTTC.R30_Thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			if (pRTCC->RZJCTTC.R30_Use4UllageThrusters)
			{
				tab->UllageQuads_Sep = 4;
			}
			else
			{
				tab->UllageQuads_Sep = 2;
			}
		}
		else
		{
			tab->UllageQuads_Sep = 0;
		}

		RMMATT(2, 1, true, pRTCC->RZJCTTC.R30_Att, refsdata.REFSMMAT, pRTCC->RZJCTTC.R30_Thruster, sv_TIG_SEP.R, sv_TIG_SEP.V, pRTCC->RZJCTTC.R30_GimbalIndicator, CSMmass, U_T, IMUAtt);
		tab->Att_IMU_Sep = IMUAtt * DEG;
		RMMATT(2, 2, false, IMUAtt, refsdata.REFSMMAT, pRTCC->RZJCTTC.R30_Thruster, sv_TIG_SEP.R, sv_TIG_SEP.V, pRTCC->RZJCTTC.R30_GimbalIndicator, CSMmass, U_T, BodyAtt);
		tab->Att_LVLH_Sep = BodyAtt * DEG;
		for (int i = 0;i < 3;i++)
		{
			if (tab->Att_IMU_Sep.data[i] < 0)
			{
				tab->Att_IMU_Sep.data[i] += 360.0;
			}
		}
		tab->DVC_Sep = burnaux_sep.DV_C / 0.3048;
		tab->BurnTime_Sep = burnaux_sep.DT_B;
		tab->DVT_Sep = burnaux_sep.DV / 0.3048;
		tab->UllageDT_Sep = dt_ullage_sep;
		tab->GMTI_Sep = burnaux_sep.GMT_BI;
		tab->GETI_Sep = pRTCC->GETfromGMT(tab->GMTI_Sep);

		if (pRTCC->RZJCTTC.R30_Thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			tab->P_G_Sep = (burnaux_sep.P_G - pRTCC->SystemParameters.MCTSPP)*DEG;
			tab->Y_G_Sep = (burnaux_sep.Y_G - pRTCC->SystemParameters.MCTSYP)*DEG;
		}
		else
		{
			tab->P_G_Sep = 0.0;
			tab->Y_G_Sep = 0.0;
		}
	}
	else
	{
		tab->Indicator_Sep = 1;
	}

	if (pRTCC->RZJCTTC.R31_Use4UllageThrusters)
	{
		tab->UllageQuads = 4;
	}
	else
	{
		tab->UllageQuads = 2;
	}
	//TBD: Burn code
	//TBD: Area
	char Buffer[16];
	pRTCC->EMGSTGENName(pRTCC->RZJCTTC.R31_REFSMMAT, Buffer);
	tab->RefsID.assign(Buffer);
	tab->CSMWeightRetro = burnaux.WTENGON *LBS*1000.0;

	pRTCC->EMMDYNEL(sv_TIG, elem);

	tab->TrueAnomalyRetro = elem.TA*DEG;
	RMMATT(2, 1, true, LVLHAtt, refsdata.REFSMMAT, Thruster, sv_TIG.R, sv_TIG.V, GimbalIndicator, CSMmass_Sep, U_T, IMUAtt);
	tab->Att_IMU = IMUAtt * DEG;
	RMMATT(2, 2, false, IMUAtt, refsdata.REFSMMAT, Thruster, sv_TIG.R, sv_TIG.V, GimbalIndicator, CSMmass_Sep, U_T, BodyAtt);
	tab->Att_LVLH = BodyAtt * DEG;
	for (int i = 0;i < 3;i++)
	{
		if (tab->Att_IMU.data[i] < 0)
		{
			tab->Att_IMU.data[i] += 360.0;
		}
	}
	tab->DVC = burnaux.DV_C / 0.3048;
	tab->BurnTime = burnaux.DT_B;
	tab->DVT = burnaux.DV / 0.3048;
	tab->UllageDT = dt_ullage;
	tab->GMTI = burnaux.GMT_BI;
	tab->GETI = pRTCC->GETfromGMT(tab->GMTI);
	tab->RET400k = sv_EI.GMT - tab->GMTI;

	double v_EI = length(sv_EI.V);
	double gamma_EI = acos(dotp(unit(sv_EI.R), unit(sv_EI.V)));

	tab->V400k = v_EI / 0.3048;
	tab->Gamma400k = (PI05 - gamma_EI) * DEG;
	tab->BankAngle = pRTCC->RZJCTTC.R31_FinalBankAngle*DEG;
	if (pRTCC->RZJCTTC.Type == 1)
	{
		tab->RETRB = (sv_EI.GMT + t_RB) - sv_TIG.GMT;
	}
	else
	{
		tab->RETRB = 0.0;
	}
	
	tab->lat_ML = lat_ML * DEG;
	tab->lng_ML = lng_ML * DEG;
	if (pRTCC->RZJCTTC.Type == 2)
	{
		//Save predicted impact as target
		tab->lat_T = lat_IP*DEG;
		tab->lng_T = lng_IP * DEG;
	}
	else
	{
		//Save actual target as target
		tab->lat_T = lat_T * DEG;
		tab->lng_T = lng_T * DEG;
	}
	tab->lat_IP = lat_IP * DEG;
	tab->lng_IP = lng_IP * DEG;
	tab->lat_ZL = lat_ZL * DEG;
	tab->lng_ZL = lng_ZL * DEG;
	tab->dlat_NM = MD_lat;
	tab->dlng_NM = MD_lng;
	tab->H_Retro = elem.h / 1852.0;
	if (Thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		tab->P_G = (burnaux.P_G - pRTCC->SystemParameters.MCTSPP)*DEG;
		tab->Y_G = (burnaux.Y_G - pRTCC->SystemParameters.MCTSYP)*DEG;
	}
	else
	{
		tab->P_G = 0.0;
		tab->Y_G = 0.0;
	}
	tab->REFSMMAT = DesREFSMMAT;
	tab->DV_TO = burnaux.DV_TO / 0.3048;
	tab->DT_TO = burnaux.DT_TO;
	
	VECTOR3 DV_EXDV = pRTCC->PIEXDV(sv_TIG.R, sv_TIG.V, CSMmass_Sep, TCMC, U_T*DVBURN, 0);

	tab->VG_XDX = DV_EXDV / 0.3048;
	//TBD: VG THR
	EphemerisData sv_BO_ECT;
	pRTCC->ELVCNV(sv_BO, 1, sv_BO_ECT);
	sv_BO_ECT.RBI = BODY_EARTH;
	pRTCC->EMMDYNEL(sv_BO_ECT, elem);

	pRTCC->PIFAAP(elem.a, elem.e, elem.i, elem.TA, elem.TA + elem.AoP, length(sv_BO_ECT.R), r_apo, r_peri);
	tab->H_apo = (r_apo - OrbMech::R_Earth) / 1852.0;
	tab->H_peri = (r_peri - OrbMech::R_Earth) / 1852.0;

	//Transfer table
	RetrofireTransferTableEntry *tab2 = &pRTCC->RZRFTT.Manual;

	tab2->GMTI = sv_TIG.GMT;
	tab2->DeltaV = DV_EXDV;
	tab2->Thruster = Thruster;
	tab2->UllageThrusterOption = pRTCC->RZJCTTC.R31_Use4UllageThrusters;
	tab2->dt_ullage = dt_ullage;

	if (pRTCC->RZJCTTC.Type == 1)
	{
		tab2->entry.lat_T = lat_T;
		tab2->entry.lng_T = lng_T;
	}
	else
	{
		tab2->entry.lat_T = lat_IP;
		tab2->entry.lng_T = lng_IP;
	}
	tab2->entry.LiftMode = 3;
	tab2->entry.GNInitialBank = tab2->entry.InitialBankAngle = pRTCC->RZJCTTC.R31_InitialBankAngle;
	tab2->entry.FinalBankAngle = pRTCC->RZJCTTC.R31_FinalBankAngle;
	tab2->entry.GMTReverseBank = 0.0;
	tab2->entry.GLevel = pRTCC->RZJCTTC.R31_GLevel;
	tab2->entry.BackupLiftMode = 0;
	tab2->entry.RollDirection = 1.0;
	tab2->entry.ConstantGLevel = 0.0;

	//Print
	RMGTTF("RMGTTF", 100);
}

void RetrofirePlanning::RMGTTF(std::string source, int i)
{
	std::vector<std::string> message;
	char Buffer[128];

	switch (i)
	{
	case 1:
		message.push_back("UVT VECTOR REQUEST INVALID");
		break;
	case 2:
		message.push_back("MSK 356 RECEIVED AN INVALID VEHICLE ID");
		break;
	case 3:
		sprintf_s(Buffer, "LONGITUDE CONVERGENCE FAILURE - LONGITUDE DIFFERENCE = %08.3lf (RAD)", pRTCC->RTCCONLINEMON.DoubleBuffer[0]);
		message.push_back(Buffer);
		break;
	case 4:
		message.push_back("MSK 355 RECEIVED AN INVALID VEHICLE ID");
		break;
	case 5:
		message.push_back("MSK 329 RECEIVED AN INVALID VEHICLE ID");
		break;
	case 6:
		message.push_back("MSK 335 RECEIVED AN INVALID VEHICLE ID");
		break;
	case 7:
		message.push_back("MPT MANEUVER TOO NEAR RETRO");
		break;
	case 8:
		message.push_back("SEPARATION MNVR VIOLATES PRESENT TIME CONSTRAINT");
		break;
	case 9:
		message.push_back("BURNOUT REV IS AFTER TARGET REV");
		break;
	case 10:
		message.push_back("LESS THAN 2 REVS IN CAPE CROSSING TABLE");
		break;
	case 11:
		message.push_back("NOT 18 MIN BETWEEN TL/TR AFTER CONSTRAINTS");
		break;
	case 12:
		message.push_back("SEPARATION MNVR NOT WITHIN FINAL TL/TR BOUNDS");
		break;
	case 13:
		message.push_back("RETRO VIOLATES PRESENT TIME +10 MIN CONSTRAINT");
		break;
	case 14:
		message.push_back("MPT MANEUVER STARTS BETWEEN TSEP AND TRETRO");
		break;
	case 15:
		message.push_back("MPT MANEUVER TOO NEAR SEPARATION");
		break;
	case 16:
		message.push_back("MPT MANEUVER OVERLAPS RETRO AND/OR SEPARATION");
		break;
	case 17:
		message.push_back("INSUFFICIENT EPHEMERIS AVAILABLE");
		break;
	case 18:
		message.push_back("TARGET REV NOT IN CAPE CROSSING TABLE");
		break;
	case 19:
		message.push_back("UNABLE TO FETCH INITIAL VECTOR FOR 2-BODY");
		break;
	case 20:
		message.push_back("HYPERBOLIC ORBIT VECTOR IN 2-BODY ROUTINE");
		break;
	case 21:
		message.push_back("HYPERBOLIC ORBIT VECTOR IN 2-BODY ROUTINE");
		break;
	case 22:
		message.push_back("INPUT RETRO TIME LESS THAN SHAPING TIME");
		break;
	case 23:
		message.push_back("RETRO NOT WITHIN TL/TR BOUNDARIES");
		break;
	case 24:
		message.push_back("2-BODY TARGET PASSAGE DID NOT CONVERGE");
		break;
	case 25:
		message.push_back("NO CONVERGENCE ON CONTINGENCY TARGET in THREE REVS");
		break;
	case 26:
		message.push_back("NO CONVERGENCE ON PRIMARY TARGET in THREE REVS");
		break;
	case 27:
		message.push_back("LESS THAN 8TH ORDER INTERPOLATION - CONTINUING");
		break;
	case 28:
		message.push_back("EMMENI - TIME RIGHT REACHED BEFORE 412K");
		break;
	case 29:
		message.push_back("RMMYNI - TIME RIGHT REACHED BEFORE IMPACT");
		break;
	case 30:
		message.push_back("EMMENI - SUN - MOON EPHEMERIS ERROR");
		break;
	case 31:
		message.push_back("NO CONVERGENCE ON MANUAL TARGET in THREE REVS");
		break;
	case 32:
		message.push_back("COMPUTATION TERMINATED - INSUFFICIENT EPHEMERIS");
		break;
	case 33:
		message.push_back("NO PRIMARY TARGET - UPDATE TARGET TABLE");
		break;
	case 34:
		message.push_back("NO CONTINGENCY TARGET - UPDATE TARGET TABLE");
		break;
	case 35:
		message.push_back("CM/SM SEP ADJUSTED TO 412K");
		break;
	case 36:
		message.push_back("PMMRKJ - EXCEEDED AVAILABLE FUEL");
		break;
	case 37:
		message.push_back("PMMRKJ - IMPACTED BEFORE REACHING END CONDITIONS");
		break;
	case 38:
		message.push_back("SHAPING TIME IS IN MPT MNVR - ADJUST SHAPING TIME");
		break;
	case 39:
		message.push_back("RECEIVED AN INVALID VEHICLE ID. MED NOT PROCESSED");
		break;
	case 40:
		message.push_back("RECEIVED AN INVALID VEHICLE ID");
		break;
	case 41:
		sprintf(Buffer, "CONVERGED. E-W MISS = %.2lf N-S MISS = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 42:
		message.push_back("LESS THAN 5 MIN. SEPARATING MANEUVERS");
		sprintf(Buffer, "E-W MISS = %.2lf N-S MISS = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 43:
		message.push_back("NO DOWNRANGE CONVERGENCE");
		sprintf(Buffer, "E-W MISS = %.2lf N-S MISS = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 44:
		message.push_back("NO CROSSRANGE CONVERGENCE");
		sprintf(Buffer, "E-W MISS = %.2lf N-S MISS = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 45:
		message.push_back("TL OR TR BOUNDS STOPPED CONVERGENCE");
		sprintf(Buffer, "E-W MISS = %.2lf N-S MISS = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 46:
		message.push_back("V, GAMMA EI CONDITIONS VIOLATED");
		sprintf(Buffer, "E-W MISS = %.2lf N-S MISS = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 47:
		message.push_back("NO DELTA-V POSSIBLE");
		break;
	case 48:
		message.push_back("CANNOT INTERPOLATE FOR VECTOR");
		break;
	case 49:
		message.push_back("EI TARGET CURVE NOT HIT");
		sprintf(Buffer, "VEL = %.1lf GAMMA = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 50:
		message.push_back("(RCMDBM) ENTERED");
		break;
	case 51:
		message.push_back("CANNOT INTERPOLATE FOR VECTOR");
		break;
	case 52:
		message.push_back("NO DOWRANGE CONVERGENCE IN 15 ITERATIONS");
		sprintf(Buffer, "MISS DISTANCE = %.1lf NM", pRTCC->RTCCONLINEMON.DoubleBuffer[0]);
		message.push_back(Buffer);
		break;
	case 53:
		message.push_back("NO INTERSECTION WITH EI");
		break;
	case 54:
		message.push_back("NO IMPULSIVE DELTA-V POSSIBLE");
		break;
	case 55:
		message.push_back("VEL, GAMMA BEYOND CURVE FIT");
		sprintf(Buffer, "VEL = %.1lf GAMMA = %.2lf", pRTCC->RTCCONLINEMON.DoubleBuffer[0], pRTCC->RTCCONLINEMON.DoubleBuffer[1]);
		message.push_back(Buffer);
		break;
	case 56:
		message.push_back("HYPERBOLIC ORBIT FROM 2-BODY ROUTINE");
		break;
	case 100: //The big one
		std::string Buffer2, Buffer3;

		RetrofireDisplayParametersTableData *tab = &pRTCC->RZRFDP.data[2];

		//Line 1
		sprintf_s(Buffer, "MANUAL TIME-TO-FIRE PARAMETERS TYPE %d COMPUTATION", pRTCC->RZJCTTC.R32_Code);
		message.push_back(Buffer);
		if (pRTCC->RZJCTTC.R32_Code == 2)
		{
			if (ManeuverType == 1)
			{
				Buffer2.assign("-SEPARATION MNVR");
			}
			else
			{
				Buffer2.assign("SHAPING MANEUVER");
			}
			Buffer2.append(" PARAMETERS VEH=CSM AREA=");
			sprintf_s(Buffer, "%06.2lf", Area / 0.3048 / 0.3048);
			Buffer2.append(Buffer);
			Buffer2 += " WT=";
			sprintf_s(Buffer, "%08.2lf", tab->CSMWeightSep);
			Buffer2.append(Buffer);
			Buffer2 += " GETI=";
			pRTCC->OnlinePrintTimeHHHMMSS(tab->GETI_Sep, Buffer3);
			Buffer2 += Buffer3;
			Buffer2 += " THR=";

			switch (pRTCC->RZJCTTC.R30_Thruster)
			{
			case RTCC_ENGINETYPE_CSMSPS:
				Buffer3 = "SPS";
				break;
			case RTCC_ENGINETYPE_CSMRCSMINUS2:
				Buffer3 = "-R2";
				break;
			case RTCC_ENGINETYPE_CSMRCSMINUS4:
				Buffer3 = "-R4";
				break;
			case RTCC_ENGINETYPE_CSMRCSPLUS2:
				Buffer3 = "+R2";
				break;
			case RTCC_ENGINETYPE_CSMRCSPLUS4:
				Buffer3 = "+R4";
				break;
			}
			Buffer2 += Buffer3;
			message.push_back(Buffer2);

			Buffer2.assign("CONSTRAINT=");
			if (pRTCC->RZJCTTC.R30_DeltaV > 0)
			{
				Buffer3 = "DV";
			}
			else
			{
				Buffer3 = "DT";
			}
			Buffer2 += Buffer3;
			Buffer2 += " DV=";
			sprintf_s(Buffer, "%08.2lf", tab->DVT_Sep);
			Buffer2.append(Buffer);
			Buffer2 += " DT=";
			sprintf_s(Buffer, "%06.2lf", tab->BurnTime_Sep);
			Buffer2.append(Buffer);
			Buffer2 += " DT ULL=";
			sprintf_s(Buffer, "%05.2lf", tab->UllageDT_Sep);
			Buffer2.append(Buffer);
			Buffer2 += " ";
			sprintf_s(Buffer, "%d", tab->UllageQuads_Sep);
			Buffer2.append(Buffer);
			Buffer2 += " QUADS";
			message.push_back(Buffer2);

			sprintf_s(Buffer, "INPUT ATT-ROLL=%+07.2lf PITCH=%+07.2lf YAW=%+07.2lf TRIM ANGLE IND=", pRTCC->RZJCTTC.R30_Att.x*DEG, pRTCC->RZJCTTC.R30_Att.y*DEG, pRTCC->RZJCTTC.R30_Att.z*DEG);
			Buffer2.assign(Buffer);
			if (pRTCC->RZJCTTC.R30_GimbalIndicator == -1)
			{
				Buffer2.append("C");
			}
			else
			{
				Buffer2.append("I");
			}
			message.push_back(Buffer2);
			//Line 5
			sprintf_s(Buffer, "LVLH BODY-ROLL=%+07.2lf PITCH=%+07.2lf YAW=%+07.2lf TRIM ANGLES--PITCH=%+07.2lf YAW=%+07.2lf", tab->Att_LVLH_Sep.x, tab->Att_LVLH_Sep.y, tab->Att_LVLH_Sep.z, tab->P_G_Sep, tab->Y_G_Sep);
			Buffer2.assign(Buffer);
			message.push_back(Buffer2);
			//Line 6
			sprintf_s(Buffer, "IMU-------ROLL=%+07.2lf PITCH=%+07.2lf YAW=%+07.2lf DT SEP=", tab->Att_IMU_Sep.x, tab->Att_IMU_Sep.y, tab->Att_IMU_Sep.z);
			Buffer2.assign(Buffer);
			pRTCC->OnlinePrintTimeHHHMMSS(pRTCC->RZJCTTC.R30_DeltaT_Sep, Buffer3);
			Buffer2 += Buffer3;
			sprintf_s(Buffer, " TRUE ANOMALY AT GETI(SEP)=%+07.2lf", tab->TrueAnomalySep);
			Buffer2.append(Buffer);
			message.push_back(Buffer2);
		}
		//Line 2
		Buffer2.assign("--MANEUVER PARAMETERS VEH=CSM AREA="); //RETRO
		sprintf_s(Buffer, "%06.2lf", Area / 0.3048 / 0.3048);
		Buffer2.append(Buffer);
		Buffer2 += " WT=";
		sprintf_s(Buffer, "%08.2lf", tab->CSMWeightRetro);
		Buffer2.append(Buffer);
		Buffer2 += " GETI=";
		pRTCC->OnlinePrintTimeHHHMMSS(tab->GETI, Buffer3);
		Buffer2 += Buffer3;
		Buffer2 += " THR=";

		switch (Thruster)
		{
		case RTCC_ENGINETYPE_CSMSPS:
			Buffer3 = "SPS";
			break;
		case RTCC_ENGINETYPE_CSMRCSMINUS2:
			Buffer3 = "-R2";
			break;
		case RTCC_ENGINETYPE_CSMRCSMINUS4:
			Buffer3 = "-R4";
			break;
		case RTCC_ENGINETYPE_CSMRCSPLUS2:
			Buffer3 = "+R2";
			break;
		case RTCC_ENGINETYPE_CSMRCSPLUS4:
			Buffer3 = "+R4";
			break;
		}
		Buffer2 += Buffer3;
		message.push_back(Buffer2);
		//Line 3
		Buffer2.assign("CONSTRAINT=");
		switch (BurnMode)
		{
		case 1:
			Buffer3 = "DV";
			break;
		case 2:
			Buffer3 = "DT";
			break;
		case 3:
			Buffer3 = "V,GAM";
			break;
		}
		Buffer2 += Buffer3;
		Buffer2 += " DV=";
		sprintf_s(Buffer, "%08.2lf", tab->DVT);
		Buffer2.append(Buffer);
		Buffer2 += " DT=";
		sprintf_s(Buffer, "%06.2lf", tab->BurnTime);
		Buffer2.append(Buffer);
		Buffer2 += " DT ULL=";
		sprintf_s(Buffer, "%05.2lf", tab->UllageDT);
		Buffer2.append(Buffer);
		Buffer2 += " ";
		sprintf_s(Buffer, "%d", tab->UllageQuads);
		Buffer2.append(Buffer);
		Buffer2 += " QUADS POSTBURN WT=";
		sprintf_s(Buffer, "%08.2lf", burnaux.WTEND*LBS*1000.0);
		Buffer2.append(Buffer);
		message.push_back(Buffer2);
		//Line 4
		sprintf_s(Buffer, "INPUT ATT-ROLL=%+07.2lf PITCH=%+07.2lf YAW=%+07.2lf TRIM ANGLE IND=", LVLHAtt.x*DEG, LVLHAtt.y*DEG, LVLHAtt.z*DEG);
		Buffer2.assign(Buffer);
		if (GimbalIndicator == -1)
		{
			Buffer2.append("C");
		}
		else
		{
			Buffer2.append("I");
		}
		message.push_back(Buffer2);
		//Line 5
		sprintf_s(Buffer, "LVLH BODY-ROLL=%+07.2lf PITCH=%+07.2lf YAW=%+07.2lf TRIM ANGLES--PITCH=%+07.2lf YAW=%+07.2lf", BodyAtt.x*DEG, BodyAtt.y*DEG, BodyAtt.z*DEG, tab->P_G, tab->Y_G);
		Buffer2.assign(Buffer);
		message.push_back(Buffer2);
		//Line 6
		sprintf_s(Buffer, "IMU-------ROLL=%+07.2lf PITCH=%+07.2lf YAW=%+07.2lf HT AT RETRO=%+07.2lf TRUE ANOMALY AT GETI(RET)=%+07.2lf", IMUAtt.x*DEG, IMUAtt.y*DEG, IMUAtt.z*DEG, tab->H_Retro, tab->TrueAnomalyRetro);
		Buffer2.assign(Buffer);
		message.push_back(Buffer2);
		//Line 7
		Buffer2.assign("REFSMMAT ID=");
		pRTCC->FormatREFSMMATCode(refsid, refsdata.ID, Buffer);
		Buffer2.append(Buffer);
		sprintf_s(Buffer, " XX=%+.8lf XY=%+.8lf XZ=%+.8lf", refsdata.REFSMMAT.m11, refsdata.REFSMMAT.m12, refsdata.REFSMMAT.m13);
		Buffer2.append(Buffer);
		message.push_back(Buffer2);
		//Line 8
		sprintf_s(Buffer, "                   YX=%+.8lf YY=%+.8lf YZ=%+.8lf", refsdata.REFSMMAT.m21, refsdata.REFSMMAT.m22, refsdata.REFSMMAT.m23);
		Buffer2.assign(Buffer);
		message.push_back(Buffer2);
		//Line 9
		sprintf_s(Buffer, "                   ZX=%+.8lf ZY=%+.8lf ZZ=%+.8lf", refsdata.REFSMMAT.m31, refsdata.REFSMMAT.m32, refsdata.REFSMMAT.m33);
		Buffer2.assign(Buffer);
		message.push_back(Buffer2);
		//Line 10
		sprintf_s(Buffer, "LVLH XDV(TRUE) VGX=%+09.2lf VGY=%+09.2lf VGZ=%+09.2lf", tab->VG_XDX.x, tab->VG_XDX.y, tab->VG_XDX.z);
		Buffer2.assign(Buffer);
		message.push_back(Buffer2);
		//Line 11
		VECTOR3 vtemp = sv_TIG.R / OrbMech::R_Earth;
		VECTOR3 vtemp2 = sv_TIG.V / OrbMech::R_Earth*3600.0;
		sprintf_s(Buffer, "VECTOR AT GETI(RETRO) X=%+.6lf Y=%+.6lf Z=%+.6lf XV=%+.6lf YV=%+.6lf ZV=%+.6lf", vtemp.x, vtemp.y, vtemp.z, vtemp2.x, vtemp2.y, vtemp2.z);
		Buffer2.assign(Buffer);
		message.push_back(Buffer2);
		//Line 12
		sprintf_s(Buffer, "ENTRY PARAMETERS");
		Buffer2.assign(Buffer);
		message.push_back(Buffer2);
		//Line 13
		Buffer2.assign("ENTRY PROFILE=() INITIAL BANK=");
		sprintf_s(Buffer, "%+07.2lf", pRTCC->RZJCTTC.R31_InitialBankAngle*DEG);
		Buffer2.append(Buffer);
		sprintf_s(Buffer, " G-LEVEL=%.2lf", pRTCC->RZJCTTC.R31_GLevel);
		Buffer2.append(Buffer);
		Buffer2 += " FINAL-BANK=";
		if (pRTCC->RZJCTTC.R31_FinalBankAngle > 0)
		{
			sprintf_s(Buffer, "RL%03.0lf", pRTCC->RZJCTTC.R31_FinalBankAngle*DEG);
		}
		else
		{
			sprintf_s(Buffer, "RR%03.0lf", abs(pRTCC->RZJCTTC.R31_FinalBankAngle)*DEG);
		}
		Buffer2.append(Buffer);
		Buffer2 += " GETRB=";
		pRTCC->OnlinePrintTimeHHHMMSS(pRTCC->GETfromGMT(sv_EI.GMT + tab->RETRB), Buffer3);
		Buffer2.append(Buffer3);
		message.push_back(Buffer2);
		//Line 14
		Buffer2.assign("TARGET LAT=");
		if (tab->lat_T >= 0.0)
		{
			sprintf_s(Buffer, "%07.4lfN", tab->lat_T);
		}
		else
		{
			sprintf_s(Buffer, "%07.4lfS", abs(tab->lat_T));
		}
		Buffer2.append(Buffer);
		Buffer2.append(" LONG=");
		if (tab->lng_T >= 0.0)
		{
			sprintf_s(Buffer, "%08.4lfE", tab->lng_T);
		}
		else
		{
			sprintf_s(Buffer, "%08.4lfW", abs(tab->lng_T));
		}
		Buffer2.append(Buffer);
		message.push_back(Buffer2);
		//Line 15
		Buffer2.assign("IMPACT LAT=");
		if (tab->lat_IP >= 0.0)
		{
			sprintf_s(Buffer, "%07.4lfN", tab->lat_IP);
		}
		else
		{
			sprintf_s(Buffer, "%07.4lfS", abs(tab->lat_IP));
		}
		Buffer2.append(Buffer);
		Buffer2.append(" LONG=");
		if (tab->lng_IP >= 0.0)
		{
			sprintf_s(Buffer, "%08.4lfE", tab->lng_IP);
		}
		else
		{
			sprintf_s(Buffer, "%08.4lfW", abs(tab->lng_IP));
		}
		Buffer2.append(Buffer);
		Buffer2.append(" GETEI=");
		pRTCC->OnlinePrintTimeHHHMMSS(pRTCC->GETfromGMT(sv_EI.GMT), Buffer3);
		Buffer2.append(Buffer3);
		sprintf_s(Buffer, " VEI=%08.2lf GEI= %+06.2lf", tab->V400k, tab->Gamma400k);
		Buffer2.append(Buffer);
		message.push_back(Buffer2);
		//Line 16
		Buffer2.assign("MAX IP LAT=");
		if (tab->lat_ML >= 0.0)
		{
			sprintf_s(Buffer, "%07.4lfN", tab->lat_ML);
		}
		else
		{
			sprintf_s(Buffer, "%07.4lfS", abs(tab->lat_ML));
		}
		Buffer2.append(Buffer);
		Buffer2.append(" LONG=");
		if (tab->lng_ML >= 0.0)
		{
			sprintf_s(Buffer, "%08.4lfE", tab->lng_ML);
		}
		else
		{
			sprintf_s(Buffer, "%08.4lfW", abs(tab->lng_ML));
		}
		Buffer2.append(Buffer);
		sprintf_s(Buffer, " MAX G=%05.2lf GET OF MAX G=", gmax);
		Buffer2.append(Buffer);
		pRTCC->OnlinePrintTimeHHHMMSS(pRTCC->GETfromGMT(gmt_gmax), Buffer3);
		Buffer2.append(Buffer3);
		message.push_back(Buffer2);
		//Line 17
		Buffer2.assign("MIN IP LAT=");
		if (tab->lat_ZL >= 0.0)
		{
			sprintf_s(Buffer, "%07.4lfN", tab->lat_ZL);
		}
		else
		{
			sprintf_s(Buffer, "%07.4lfS", abs(tab->lat_ZL));
		}
		Buffer2.append(Buffer);
		Buffer2.append(" LONG=");
		if (tab->lng_ZL >= 0.0)
		{
			sprintf_s(Buffer, "%08.4lfE", tab->lng_ZL);
		}
		else
		{
			sprintf_s(Buffer, "%08.4lfW", abs(tab->lng_ZL));
		}
		Buffer2.append(Buffer);
		message.push_back(Buffer2);
		break;
	}
	pRTCC->OnlinePrint(source, message);
}

EarthEntry::EarthEntry(RTCC *r, VECTOR3 R0B, VECTOR3 V0B, double mjd, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, bool entrynominal, bool entrylongmanual)
	: RTCCModule(r)
{
	MA1 = -6.986643e7;//8e8;
	C0 = 1.81000432e8;
	C1 = 1.5078514;
	C2 = -6.49993054e-9;
	C3 = 9.769389245e-18;
	k1 = 7.0e6;
	k2 = 6.495e6;
	k3 = -0.06105;//-0.043661;
	k4 = -0.10453;

	this->entrylongmanual = entrylongmanual;

	this->GETbase = GETbase;
	this->EntryAng = EntryAng;

	if (entrylongmanual)
	{
		this->EntryLng = EntryLng;
	}
	else
	{
		landingzone = (int)EntryLng;
		this->EntryLng = EntryCalculations::landingzonelong(landingzone, 0);
	}

	this->gravref = gravref;

	this->R0B = R0B;
	this->V0B = V0B;
	this->mjd = mjd;
	get = (mjd - GETbase)*24.0*3600.0;

	EntryInterface = 400000.0 * 0.3048;

	hEarth = oapiGetObjectByName("Earth");

	RCON = oapiGetSize(hEarth) + EntryInterface;
	RD = RCON;
	mu = GGRAV*oapiGetMass(hEarth);

	EntryTIGcor = EntryTIG;

	Tguess = PI2 / sqrt(mu)*OrbMech::power(length(R0B), 1.5);

	tigslip = 100.0;
	ii = 0;

	entryphase = 0;

	dt0 = EntryTIGcor - get;

	OrbMech::oneclickcoast(pRTCC->SystemParameters.AGCEpoch, R0B, V0B, mjd, dt0, R11B, V11B, gravref, hEarth);

	x2 = OrbMech::cot(PI05 - EntryAng);

	EMSAlt = 284643.0*0.3048;

	R_E = oapiGetSize(hEarth);
	earthorbitangle = (-31.7 - 2.15)*RAD;

	this->entrynominal = entrynominal;

	precision = 1;
	errorstate = 0;
}

void EarthEntry::xdviterator3(VECTOR3 R1B, VECTOR3 V1B, double xmin, double xmax, double &x)
{
	double xdes, fac1, fac2, sing, cosg, R0, R, VT0, VR0, h0, x_apo, p, dv;
	int i;
	VECTOR3 N, V;

	x_apo = 100000;
	i = 0;
	dv = 0.0;

	xdes = tan(earthorbitangle - acos(R_E / length(R1B)));
	fac1 = 1.0 / sqrt(xdes*xdes + 1.0);
	fac2 = -xdes / sqrt(xdes*xdes + 1.0);
	h0 = length(crossp(R1B, V1B));

	N = crossp(unit(R1B), unit(V1B));
	sing = length(N);
	cosg = dotp(unit(R1B), unit(V1B));
	x = cosg / sing;
	R0 = length(R1B);
	R = R0 / RCON;
	VT0 = h0 / R0;
	VR0 = dotp(R1B, V1B) / R0;
	while (abs(x_apo - x) > OrbMech::power(2.0, -20.0) && i <= 100)
	{
		p = 2.0*R0*(R - 1.0) / (R*R*(1.0 + x2*x2) - (1.0 + x*x));
		V = (unit(R1B)*x + unit(crossp(crossp(R1B, V1B), R1B)))*sqrt(mu*p) / R0;
		if (dv - length(V - V1B)>100.0)
		{
			dv -= 100.0;
		}
		else if (dv - length(V - V1B) < -100.0)
		{
			dv += 100.0;
		}
		else
		{
			dv = length(V - V1B);
		}
		x_apo = x;
		x = (VR0 - dv*fac2) / (VT0 - dv*fac1);
		if (x > xmax)
		{
			x = xmax;
		}
		else if (x < xmin)
		{
			x = xmin;
		}
		i++;
	}
}

void EarthEntry::xdviterator2(int f1, VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x)
{
	double xdes, xact, x_err, x_apo, x_err_apo, epsilon, p_CON, beta9;
	VECTOR3 Entry_DV, DV, V2;
	int i;
	i = 0;
	epsilon = pow(2.0, -20.0);

	xdes = tan(earthorbitangle - acos(R_E / length(R1B)));

	x_err_apo = 1000;

	while (abs(dx) > epsilon && i < 135)
	{
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);

		Entry_DV = _V(dotp(DV, U_H), 0.0, -dotp(DV, U_R1));

		xact = Entry_DV.z / Entry_DV.x;
		x_err = xdes - xact;

		//if (i == 0)
		//{
		//	dx = -0.01*(xdes - xact);
		//}
		//else
		//{
		//dx = x_err*((x_apo - x) / (x_err - x_err_apo));
		//}
		if (abs(x_err) > abs(x_err_apo))
		{
			dx = -dx*0.5;
		}
		beta9 = x + 1.1*dx;
		if (beta9 > xmax)
		{
			dx = (xmax - x) / 2.0;
		}
		else if (beta9 < xmin)
		{
			dx = (xmin - x) / 2.0;
		}
		x_err_apo = x_err;
		x_apo = x;
		x += dx;
		i++;
	}
}

void EarthEntry::xdviterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x)
{
	int i;
	double epsilon, p_CON, dvapo, beta8, beta9;
	VECTOR3 DV, V2;

	i = 0;
	epsilon = OrbMech::power(2.0, -20.0);

	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);

	beta8 = xmax - xmin;
	if (beta8 <= dxmax)
	{
		dx = 0.5*beta8*OrbMech::sign(dx);
	}

	while (abs(dx) > epsilon && i < 135)
	{
		dvapo = length(DV);
		xapo = x;
		x += dx;
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
		if (length(DV) > dvapo)
		{
			dx = -dx*0.5;
		}
		beta9 = x + 1.1*dx;
		if (beta9 > xmax)
		{
			dx = (xmax - x) / 2.0;
		}
		else if (beta9 < xmin)
		{
			dx = (xmin - x) / 2.0;
		}
		i++;
	}
}

void EarthEntry::dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON)
{
	p_CON = theta2 / (theta1 - x*x);
	V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
	DV = V2 - V1B;
}

void EarthEntry::reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 REI, VECTOR3 VEI)
{
	if (n1 == 0)
	{
		if (EntryAng == 0)
		{
			if (length(R1B) > k1)
			{
				x2 = k4;
			}
			else
			{
				x2 = k3;
			}
		}
		//n1 = 1;
	}
	else
	{
		if (EntryAng == 0)
		{
			double v2;
			v2 = length(VEI);
			x2 = EntryCalculations::ReentryTargetLineTan(v2, v2 < 30000.0*0.3048);
		}
		else
		{
			x2 = x2;
		}
	}
}

void EarthEntry::coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21)
{
	VECTOR3 U_R1, U_H, REI, VEI;
	double MA2, x2_err, C_FPA;
	int n1;

	x2_err = 1.0;
	precomputations(1, R1B, V1B, U_R1, U_H, MA2, C_FPA);
	n1 = 1;
	while (abs(x2_err) > 0.00001 && n1 <= 10)
	{
		conicreturn(0, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
		t21 = OrbMech::time_radius(R1B, V2, RCON, -1, mu);
		OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
		x2_apo = x2;
		reentryconstraints(n1, R1B, REI, VEI);
		x2_err = x2_apo - x2;
		n1++;
	}
	t2 = t1 + t21;
	OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
	EntryCalculations::landingsite(pRTCC->SystemParameters.MAT_J2000_BRCS, REI, VEI, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
}

void EarthEntry::precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2)
{
	double RD, R_ERR, dRCON, rPRE_apo, r1b, lambda, beta1, beta5, theta1, theta2, p_CON, C_FPA, MA2, x2_err;
	VECTOR3 U_R1, U_V1, RPRE, VPRE, U_H, eta;
	int n1, n2;

	n1 = 0;
	n2 = 0;
	RCON = oapiGetSize(hEarth) + EntryInterface;
	RD = RCON;
	R_ERR = 1000.0;
	x2_err = 1.0;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	p_CON = theta2 / (theta1 - x*x);

	if (RCON - p_CON*beta1 >= 0)
	{
		phi2 = 1.0;
	}
	else
	{
		phi2 = -1.0;
	}

	finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
	//reentryconstraints(n1 + 1, R1B, VPRE);
	//x2 = x2_apo;
	//beta1 = 1.0 + x2*x2;
	R_ERR = length(RPRE) - RD;
	while (abs(x2_err) > 0.00001 && n2 <= 10)
	{
		n1 = 0;
		while (n1 == 0 || (abs(R_ERR) > 100.0 && n1 <= 20))
		{
			//finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
			//R_ERR = length(RPRE) - RD;
			newrcon(n1, RD, length(RPRE), R_ERR, dRCON, rPRE_apo);
			conicreturn(1, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
			finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
			R_ERR = length(RPRE) - RD;
			n1++;
		}
		x2_apo = x2;
		x2_err = x2_apo - x2;
		beta1 = 1.0 + x2*x2;
		n2++;
	}
	t2 = t1 + t21;
	EntryCalculations::landingsite(pRTCC->SystemParameters.MAT_J2000_BRCS, RPRE, VPRE, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
	if (n1 == 21)
	{
		errorstate = 1;
	}
	if (n2 == 11)
	{
		errorstate = 2;
	}
}

void EarthEntry::precisionperi(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2)
{
	double R0, R1, R, x_apo, p, dv, h0, sing, cosg, VT0, VR0, xdes, fac1, fac2;
	VECTOR3 N, RPRE, VPRE;
	int i;

	x_apo = 100000;
	i = 0;

	xdes = tan(earthorbitangle - acos(R_E / length(R1B)));
	fac1 = 1.0 / sqrt(xdes*xdes + 1.0);
	fac2 = -xdes / sqrt(xdes*xdes + 1.0);
	h0 = length(crossp(R1B, V1B));
	N = crossp(unit(R1B), unit(V1B));
	sing = length(N);
	cosg = dotp(unit(R1B), unit(V1B));
	x = cosg / sing;
	R0 = length(R1B);
	R1 = R_E - 30.0*1852.0;
	R = R0 / R1;
	VT0 = h0 / R0;
	VR0 = dotp(R1B, V1B) / R0;
	while (abs(x_apo - x) > OrbMech::power(2.0, -20.0))
	{
		p = 2.0*R0*(R - 1.0) / (R*R - (1.0 + x*x));
		V2 = (unit(R1B)*x + unit(crossp(crossp(R1B, V1B), R1B)))*sqrt(mu*p) / R0;
		dv = length(V2 - V1B);
		x_apo = x;
		x = (VR0 - dv*fac2) / (VT0 - dv*fac1);
		i++;
	}
	t21 = OrbMech::time_radius_integ(pRTCC->SystemParameters.AGCEpoch, R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, RD, -1, hEarth, hEarth, RPRE, VPRE);
	N = crossp(unit(RPRE), unit(VPRE));
	sing = length(N);
	cosg = dotp(unit(RPRE), unit(VPRE));
	x2 = cosg / sing;
	t2 = t1 + t21;
	EntryCalculations::landingsite(pRTCC->SystemParameters.MAT_J2000_BRCS, RPRE, VPRE, GETbase + t2 / 24.0 / 3600.0, theta_long, theta_lat);
}

void EarthEntry::newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo)
{
	double S;

	if (n1 == 0)
	{
		this->RCON = RD*RD / rPRE;
		dRCON = RCON - RD;
	}
	else
	{
		S = dRCON / (rPRE_apo - rPRE);
		if (abs(S + 2.0) > 2.0)
		{
			S = -4.0;
		}
		dRCON = S*R_ERR;
		this->RCON = RCON + dRCON;
		if (RCON < 1e6)
		{
			RCON = 1e6;
		}
	}
	rPRE_apo = rPRE;
}

void EarthEntry::finalstatevector(VECTOR3 R1B, VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE)
{
	VECTOR3 N;
	double beta12, x2PRE, c3, alpha_N, sing, cosg, p_N, beta2, beta3, beta4, RF, phi4, dt21, beta13, dt21apo, beta14;

	OrbMech::oneclickcoast(pRTCC->SystemParameters.AGCEpoch, R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, t21, RPRE, VPRE, hEarth, hEarth);

	beta12 = 100.0;
	x2PRE = 1000000;
	dt21apo = 100000000.0;

	while (abs(beta12) > 0.000007 && abs(x2 - x2PRE) > 0.00001)
	{
		c3 = length(RPRE)*pow(length(VPRE), 2.0) / mu;
		alpha_N = 2.0 - c3;
		N = crossp(unit(RPRE), unit(VPRE));
		sing = length(N);
		cosg = dotp(unit(RPRE), unit(VPRE));
		x2PRE = cosg / sing;
		p_N = c3*sing*sing;
		beta2 = p_N*beta1;
		beta3 = 1.0 - alpha_N*beta2;
		if (beta3 < 0)
		{
			beta4 = 1.0 / alpha_N;
		}
		else
		{
			beta4 = beta2 / (1.0 - phi2*sqrt(beta3));
		}
		beta12 = beta4 - 1.0;
		//if (abs(beta12) > 0.000007)
		//{
		RF = beta4*length(RPRE);
		if (beta12 > 0)
		{
			phi4 = -1.0;
		}
		else if (x2PRE > 0)
		{
			phi4 = -1.0;
		}
		else
		{
			phi4 = 1.0;
		}
		dt21 = OrbMech::time_radius(RPRE, VPRE*phi4, RF, -phi4, mu);
		dt21 = phi4*dt21;
		beta13 = dt21 / dt21apo;
		if (beta13 > 0)
		{
			beta14 = 1.0;
		}
		else
		{
			beta14 = -0.6;
		}
		if (beta13 / beta14 > 1.0)
		{
			dt21 = beta14*dt21apo;
		}
		dt21apo = dt21;
		OrbMech::oneclickcoast(pRTCC->SystemParameters.AGCEpoch, RPRE, VPRE, mjd + (dt0 + dt1 + t21) / 24.0 / 3600.0, dt21, RPRE, VPRE, hEarth, hEarth);
		t21 += dt21;
		//}
	}
}

void EarthEntry::conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1)
{
	double theta1, theta2, theta3, xmin, xmax, p_CON, beta6, dx;
	VECTOR3 DV;
	conicinit(R1B, MA2, xmin, xmax, theta1, theta2, theta3);
	if (f1 == 0)
	{
		if (C_FPA >= 0)
		{
			x = xmax;
			dx = -dxmax;
		}
		else
		{
			x = xmin;
			dx = dxmax;
		}

		if (ii == 0)
		{
			if (entrynominal)
			{
				xdviterator3(R1B, V1B, xmin, xmax, x);
			}
			else
			{
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
		}
		else
		{
			if (entrynominal)
			{
				xdviterator3(R1B, V1B, xmin, xmax, x);
			}
			else
			{
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
		}
	}
	else
	{
		double r1b, lambda, beta1, beta5;
		r1b = length(R1B);
		lambda = r1b / RCON;
		beta1 = 1.0 + x2*x2;
		beta5 = lambda*beta1;
		beta6 = beta5*(2.0 - lambda) - 1.0;
		if (beta6 > 0)
		{
			xlim = sqrt(beta6);
			if (phi2 == 1)
			{
				xmax = xlim;
				xmin = -xlim;
				if (x > 0)
				{
					x = xmax;
					dx = -dxmax;
				}
				else
				{
					x = xmin;
					dx = dxmax;
				}
			}
			else
			{
				if (x > 0)
				{
					xmin = xlim;
					x = xmax;
					dx = -dxmax;
				}
				else
				{
					xmax = -xlim;
					x = xmin;
					dx = dxmax;
				}
			}
		}
		else
		{
			if (phi2 == 1)
			{
				phi2 = -1.0;
				n1 = -1;
			}
			if (x > 0)
			{
				x = xmax;
				dx = -dxmax;
			}
			else
			{
				x = xmin;
				dx = dxmax;
			}
		}

		if (entrynominal)
		{
			xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			xdviterator2(f1, R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
		}
		else
		{
			xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
		}
	}
	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
}

void EarthEntry::conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3)
{
	double r1b, lambda, beta1, beta5, beta10;
	r1b = length(R1B);

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	theta3 = sqrt(mu) / r1b;
	beta10 = beta5*(MA1 - r1b) / (MA1 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmin = 0.0;
	}
	else
	{
		xmin = -sqrt(beta10);
	}
	dxmax = -xmin / 16.0;
	beta10 = beta5*(MA2 - r1b) / (MA2 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmax = 0.0;
	}
	else
	{
		xmax = sqrt(beta10);
	}
}

void EarthEntry::precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2, double &C_FPA)
{
	VECTOR3 U_V1, eta;
	double r1b;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;
	if (x2set)
	{
		reentryconstraints(0, R1B, _V(0, 0, 0), _V(0, 0, 0));
	}
}

bool EarthEntry::EntryIter()
{
	double theta_long, theta_lat, dlng;
	VECTOR3 R1B, V1B, V2;

	errorstate = 0;

	dt1 = EntryTIGcor - get - dt0;
	OrbMech::oneclickcoast(pRTCC->SystemParameters.AGCEpoch, R11B, V11B, mjd + dt0 / 24.0 / 3600.0, dt1, R1B, V1B, hEarth, hEarth);
	if (precision == 2)
	{
		precisionperi(R1B, V1B, EntryTIGcor, t21, x, theta_long, theta_lat, V2);
	}
	else
	{
		coniciter(R1B, V1B, EntryTIGcor, theta_long, theta_lat, V2, x, dx, t21);

		if (entryphase == 1 && precision == 1)
		{
			precisioniter(R1B, V1B, EntryTIGcor, t21, x, theta_long, theta_lat, V2);
		}
	}

	if (!entrylongmanual)
	{
		EntryLng = EntryCalculations::landingzonelong(landingzone, theta_lat);
	}

	dlng = EntryLng - theta_long;
	if (abs(dlng) > PI)
	{
		dlng = dlng - OrbMech::sign(dlng)*PI2;
	}
	
	if (ii == 0)
	{
		tigslip = Tguess*dlng / PI2;
		dlng_old = dlng;
		EntryTIGcor_old = EntryTIGcor;
	}
	else
	{
		tigslip = -dlng*(EntryTIGcor - EntryTIGcor_old) / (dlng - dlng_old);
		EntryTIGcor_old = EntryTIGcor;
		dlng_old = dlng;
	}
	if (abs(tigslip) > 100.0)
	{
		tigslip = 100.0*OrbMech::sign(tigslip);
	}
	EntryTIGcor += tigslip;

	ii++;

	if (((abs(dlng) > 0.005*RAD && ii < 60) || entryphase == 0))
	{
		if (abs(tigslip) < 10.0)// && abs(dx)<0.1)
		{
			if (entryphase == 0)
			{
				entryphase = 1;
				ii = 0;
			}
		}
		return false;
	}
	else
	{
		if (precision == 1 && entrynominal)
		{
			if (abs(x - xlim) < OrbMech::power(2.0, -20.0) || abs(x + xlim) < OrbMech::power(2.0, -20.0) || ii == 60)
				//if (ii == 40)
			{
				ii = 0;
				precision = 2;
				return false;
			}
		}
		VECTOR3 R05G, V05G, REI, VEI, R3, V3, UR3, DV;
		double t32, dt22, v3, S_FPA;

		t2 = EntryTIGcor + t21;
		OrbMech::time_radius_integ(pRTCC->SystemParameters.AGCEpoch, R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, RD, -1, hEarth, hEarth, REI, VEI);//Maneuver to Entry Interface (400k ft)

		t32 = OrbMech::time_radius(REI, VEI, length(REI) - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu); //Entry Interface to 300k ft

		dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu); //300k ft to 0.05g

		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;

		VECTOR3 Rsph, Vsph;

		OrbMech::oneclickcoast(pRTCC->SystemParameters.AGCEpoch, R1B, V1B, mjd + (dt0 + dt1) / 24.0 / 3600.0, 0.0, Rsph, Vsph, hEarth, hEarth);
		DV = V2 - V1B;
		VECTOR3 i, j, k;
		MATRIX3 Q_Xx;
		j = unit(crossp(Vsph, Rsph));
		k = unit(-Rsph);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		Entry_DV = mul(Q_Xx, DV);

		EntryRTGO = OrbMech::CMCEMSRangeToGo(pRTCC->SystemParameters.MAT_J2000_BRCS, R05G, OrbMech::MJDfromGET(t2 + t32 + dt22, GETbase), theta_lat, theta_long);
		EntryVIO = length(V05G);
		EntryRET = t2 + t32 + dt22;
		EntryAng = atan(x2);//asin(dotp(unit(REI), VEI) / length(VEI));

		EntryLngcor = theta_long;
		EntryLatcor = theta_lat;

		if (errorstate != 0)
		{
			precision = 9;
		}

		return true;
	}
}

RTEEarth::RTEEarth(RTCC *r, EphemerisData sv0, double GMTbase, double alpha_SID0, double EntryTIG, double t_Z, int critical) : RTCCModule(r)
{
	MA1 = -6.986643e7;//8e8;
	C0 = 1.81000432e8;
	C1 = 1.5078514;
	C2 = -6.49993054e-9;
	C3 = 9.769389245e-18;
	k1 = 7.0e6;
	k2 = 6.495e6;
	k3 = -0.06105;//-0.043661;
	k4 = -0.10453;

	this->GMTbase = GMTbase;
	this->critical = critical;
	this->alpha_SID0 = alpha_SID0;

	RCON = OrbMech::R_Earth + 400000.0 * 0.3048;
	RD = RCON;
	mu = OrbMech::mu_Earth;

	ii = 0;

	//Propagate state vector to TIG
	pRTCC->PMMCEN(sv0, 0.0, 0.0, 1, EntryTIG - sv0.GMT, 1.0, sv_ig, ITS);

	//State vector at TIG needs to be in Earth SOI
	if (sv_ig.RBI != BODY_EARTH)
	{
		errorstate = 3;
		return;
	}

	sv_ig_apo = sv_ig;

	EMSAlt = 297431.0*0.3048;
	revcor = -5;

	precision = 1;
	errorstate = 0;
	
	dt_z = t_Z - sv_ig.GMT;
}

void RTEEarth::READ(double RRBI, double DVMAXI, int EPI, double URMAXI)
{
	r_rbias = RRBI;
	dv_max = DVMAXI;
	ICRNGG = EPI;
	u_rmax = URMAXI;

	MA1 = 2.0 / (2.0 / RD - u_rmax * u_rmax / mu);
}

void RTEEarth::ATP(double *line)
{
	for (int i = 0;i < 10;i++)
	{
		LINE[i] = line[i];
	}
}

void RTEEarth::newxt2(int n1, double xt2err, double &xt2_apo, double &xt2, double &xt2err_apo)
{
	double Dxt2;

	if (n1 == 1)
	{
		Dxt2 = xt2err;
	}
	else
	{
		Dxt2 = xt2err;//*((xt2_apo - xt2) / (xt2err - xt2err_apo));
	}
	xt2err_apo = xt2err;
	xt2_apo = xt2;
	xt2 = xt2 + Dxt2;
}

void RTEEarth::xdviterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x)
{
	int i;
	double epsilon, p_CON, dvapo, beta8, beta9;
	VECTOR3 DV, V2;

	i = 0;
	epsilon = OrbMech::power(2.0, -20.0);

	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);

	beta8 = xmax - xmin;
	if (beta8 <= dxmax)
	{
		dx = 0.5*beta8*OrbMech::sign(dx);
	}

	while (abs(dx) > epsilon && i < 135)
	{
		dvapo = length(DV);
		xapo = x;
		x += dx;
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
		if (length(DV) > dvapo)
		{
			dx = -dx*0.5;
		}
		beta9 = x + 1.1*dx;
		if (beta9 > xmax)
		{
			dx = (xmax - x) / 2.0;
		}
		else if (beta9 < xmin)
		{
			dx = (xmin - x) / 2.0;
		}
		i++;
	}
}

void RTEEarth::dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON)
{
	p_CON = theta2 / (theta1 - x*x);
	V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
	DV = V2 - V1B;
}

void RTEEarth::limitxchange(double theta1, double theta2, double theta3, VECTOR3 V1B, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double xmax, double &x)
{
	double beta7, p_CON;
	VECTOR3 V2, DV;

	beta7 = abs(x - xapo);
	if (beta7>2.0*dxmax)
	{
		x = xapo;
		if (x > xmax)
		{
			x = xmax;
		}
		else
		{
			if (x < xmin)
			{
				x = xmin;
			}
		}
		dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
	}
}

void RTEEarth::reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 REI, VECTOR3 VEI)
{
	if (n1 == 0)
	{
		if (length(R1B) > k1)
		{
			x2 = k4;
		}
		else
		{
			x2 = k3;
		}
	}
	else
	{
		double v2;
		v2 = length(VEI);
		x2 = EntryCalculations::ReentryTargetLineTan(v2, ICRNGG != 2);
	}
}

void RTEEarth::coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21)
{
	VECTOR3 U_R1, U_H, REI, VEI;
	double MA2, x2_err, GMT_L;
	int n1;

	x2_err = 1.0;
	precomputations(1, R1B, V1B, U_R1, U_H, MA2);
	n1 = 1;
	while (abs(x2_err) > 0.00001 && n1 <= 10)
	{
		conicreturn(0, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
		t21 = OrbMech::time_radius(R1B, V2, RCON, -1, mu);
		OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
		x2_apo = x2;
		reentryconstraints(n1, R1B, REI, VEI);
		x2_err = x2_apo - x2;
		//newxt2(n1, x2_err, x2_apo, x2, x2_err_apo);
		n1++;
	}
	t2 = t1 + t21;
	OrbMech::rv_from_r0v0(R1B, V2, t21, REI, VEI, mu);
	EntryCalculations::LNDING(REI, VEI, t2, alpha_SID0, 0.3, ICRNGG, r_rbias, theta_long, theta_lat, GMT_L);
}

void RTEEarth::precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2)
{
	double R_ERR, dRCON, rPRE_apo, r1b, lambda, beta1, beta5, theta1, theta2, p_CON, C_FPA, MA2, x2_err, GMT_L;
	VECTOR3 U_R1, U_V1, RPRE, VPRE, U_H, eta;
	int n1, n2;

	n1 = 0;
	n2 = 0;
	RCON = RD;
	R_ERR = 1000.0;
	x2_err = 1.0;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	p_CON = theta2 / (theta1 - x*x);

	if (RCON - p_CON*beta1 >= 0)
	{
		phi2 = 1.0;
	}
	else
	{
		phi2 = -1.0;
	}

	finalstatevector(V2, beta1, t21, RPRE, VPRE);
	//reentryconstraints(n1 + 1, R1B, VPRE);
	//x2 = x2_apo;
	//beta1 = 1.0 + x2*x2;
	R_ERR = length(RPRE) - RD;
	while (abs(x2_err) > 0.00001 && n2 <= 10)
	{
		n1 = 0;
		while (n1 == 0 || (abs(R_ERR) > 100.0 && n1 <= 20))
		{
			//finalstatevector(R1B, V2, beta1, t21, RPRE, VPRE);
			//R_ERR = length(RPRE) - RD;
			newrcon(n1, RD, length(RPRE), R_ERR, dRCON, rPRE_apo);
			conicreturn(1, R1B, V1B, MA2, C_FPA, U_R1, U_H, V2, x, n1);
			finalstatevector(V2, beta1, t21, RPRE, VPRE);
			R_ERR = length(RPRE) - RD;
			n1++;
		}
		x2_apo = x2;
		reentryconstraints(n2, R1B, RPRE, VPRE);
		x2_err = x2_apo - x2;
		beta1 = 1.0 + x2*x2;
		n2++;
	}
	t2 = t1 + t21;
	EntryCalculations::LNDING(RPRE, VPRE, t2, alpha_SID0, 0.3, ICRNGG, r_rbias, theta_long, theta_lat, GMT_L);
	if (n1 == 21)
	{
		errorstate = 1;
	}
	if (n2 == 11)
	{
		errorstate = 2;
	}
}

void RTEEarth::newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo)
{
	double S;

	if (n1 == 0)
	{
		this->RCON = RD*RD / rPRE;
		dRCON = RCON - RD;
	}
	else
	{
		S = dRCON / (rPRE_apo - rPRE);
		if (abs(S + 2.0) > 2.0)
		{
			S = -4.0;
		}
		dRCON = S*R_ERR;
		this->RCON = RCON + dRCON;
		if (RCON < 1e6)
		{
			RCON = 1e6;
		}
	}
	rPRE_apo = rPRE;
}

void RTEEarth::finalstatevector(VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE)
{
	EphemerisData sv_PRE, sv_PRE2;
	VECTOR3 N;
	double beta12, x2PRE, c3, alpha_N, sing, cosg, p_N, beta2, beta3, beta4, RF, phi4, dt21, beta13, dt21apo, beta14;

	sv_ig_apo.V = V2;
	pRTCC->PMMCEN(sv_ig_apo, 0.0, 0.0, 1, t21, 1.0, sv_PRE, ITS);

	beta12 = 100.0;
	x2PRE = 1000000;
	dt21apo = 100000000.0;

	while (abs(beta12) > 0.000007 && abs(x2 - x2PRE) > 0.00001)
	{
		c3 = length(sv_PRE.R)*pow(length(sv_PRE.V), 2.0) / mu;
		alpha_N = 2.0 - c3;
		N = crossp(unit(sv_PRE.R), unit(sv_PRE.V));
		sing = length(N);
		cosg = dotp(unit(sv_PRE.R), unit(sv_PRE.V));
		x2PRE = cosg / sing;
		p_N = c3*sing*sing;
		beta2 = p_N*beta1;
		beta3 = 1.0 - alpha_N*beta2;
		if (beta3 < 0)
		{
			beta4 = 1.0 / alpha_N;
		}
		else
		{
			beta4 = beta2 / (1.0 - phi2*sqrt(beta3));
		}
		beta12 = beta4 - 1.0;
		//if (abs(beta12) > 0.000007)
		//{
			RF = beta4*length(sv_PRE.R);
			if (beta12 > 0)
			{
				phi4 = -1.0;
			}
			else if (x2PRE > 0)
			{
				phi4 = -1.0;
			}
			else
			{
				phi4 = 1.0;
			}
			dt21 = OrbMech::time_radius(sv_PRE.R, sv_PRE.V*phi4, RF, -phi4, mu);
			dt21 = phi4*dt21;
			beta13 = dt21 / dt21apo;
			if (beta13 > 0)
			{
				beta14 = 1.0;
			}
			else
			{
				beta14 = -0.6;
			}
			if (beta13 / beta14 > 1.0)
			{
				dt21 = beta14*dt21apo;
			}
			dt21apo = dt21;

			pRTCC->PMMCEN(sv_PRE, 0.0, 0.0, 1, dt21, 1.0, sv_PRE2, ITS);
			sv_PRE = sv_PRE2;

			//OrbMech::oneclickcoast(RPRE, VPRE, mjd_ig + t21 / 24.0 / 3600.0, dt21, RPRE, VPRE, hEarth, hEarth);
			t21 += dt21;
		//}
	}

	RPRE = sv_PRE.R;
	VPRE = sv_PRE.V;
}

double RTEEarth::dtiterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double xmax, double dxmax, double dt_des)
{
	double R0, R, x_apo, p, xx, dxx, dt, dt_apo, dt_err;
	int i;
	VECTOR3 V;

	x_apo = 100000;
	xx = xmin;
	dxx = dxmax;
	i = 0;

	R0 = length(R1B);
	R = R0 / RCON;

	while (abs(x_apo - xx) > OrbMech::power(2.0, -20.0) && i <= 100)
	{
		p = 2.0*R0*(R - 1.0) / (R*R*(1.0 + x2 * x2) - (1.0 + xx * xx));
		V = (unit(R1B)*xx + unit(crossp(crossp(R1B, V1B), R1B)))*sqrt(mu*p) / R0;
		
		dt = OrbMech::time_radius(sv_ig.R, V, RD, -1.0, mu);
		dt_err = dt_des - dt;

		if (i > 0)
		{
			dxx = (xx - x_apo) / (dt - dt_apo)*dt_err;
		}
		if (dxx > dxmax)
		{
			dxx = dxmax;
		}
		else if (dxx < -dxmax)
		{
			dxx = -dxmax;
		}
		x_apo = xx;
		dt_apo = dt;
		xx += dxx;

		if (xx < xmin)
		{
			xx = xmin;
		}
		if (xx > xmax)
		{
			xx = xmax;
		}
		i++;
	}
	return xx;
}

double RTEEarth::dvmaxiterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double dxmax, double dv_des)
{
	double R0, R, x_apo, p, xx, dxx, dv, dv_apo, dv_err;
	int i;
	VECTOR3 V;

	x_apo = 100000;
	xx = xmin;
	dxx = dxmax;
	i = 0;

	R0 = length(R1B);
	R = R0 / RCON;

	while (abs(x_apo - xx) > OrbMech::power(2.0, -20.0) && i <= 100)
	{
		p = 2.0*R0*(R - 1.0) / (R*R*(1.0 + x2*x2) - (1.0 + xx*xx));
		V = (unit(R1B)*xx + unit(crossp(crossp(R1B, V1B), R1B)))*sqrt(mu*p) / R0;

		dv = length(V - V1B);
		dv_err = dv_des - dv;

		if (i > 0)
		{
			dxx = (xx - x_apo) / (dv - dv_apo)*dv_err;
		}
		x_apo = xx;
		dv_apo = dv;
		xx += dxx;

		if (xx < xmin)
		{
			xx = xmin;
		}
		i++;
	}
	return xx;
}

void RTEEarth::conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1)
{
	double theta1, theta2, theta3, xmin, xmax, p_CON, beta6;
	VECTOR3 DV;
	conicinit(R1B, MA2, xmin, xmax, theta1, theta2, theta3);
	if (f1 == 0)
	{
		if (ii == 0)
		{
			//First iteration
			if (critical == 1)
			{
				//ATP
				dx = dxmax;
				x = dtiterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, xmin, xmax, dxmax, dt_z);
			}
			else if (critical == 2)
			{
				//time critical
				dx = dxmax;
				x = dvmaxiterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, xmin, dxmax, dv_max);
			}
			else
			{
				//Fuel critical
				if (C_FPA >= 0)
				{
					x = xmax;
					dx = -dxmax;
				}
				else
				{
					x = xmin;
					dx = dxmax;
				}
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
		}
		else
		{
			//Is this needed?
			if (critical == 3)
			{
				xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);
			}
		}
	}
	else
	{
		double r1b, lambda, beta1, beta5;
		r1b = length(R1B);
		lambda = r1b / RCON;
		beta1 = 1.0 + x2*x2;
		beta5 = lambda*beta1;
		beta6 = beta5*(2.0 - lambda) - 1.0;
		if (critical != 1)
		{
			if (beta6 > 0)
			{
				xlim = sqrt(beta6);
				if (phi2 == 1)
				{
					xmax = xlim;
					xmin = -xlim;
					if (x > 0)
					{
						x = xmax;
						dx = -dxmax;
					}
					else
					{
						x = xmin;
						dx = dxmax;
					}
				}
				else
				{
					if (x > 0)
					{
						xmin = xlim;
						x = xmax;
						dx = -dxmax;
					}
					else
					{

						xmax = -xlim;
						x = xmin;
						dx = dxmax;
					}
				}
			}
			else
			{
				if (phi2 == 1)
				{
					phi2 = -1.0;
					n1 = -1;
				}
				if (x > 0)
				{
					x = xmax;
					dx = -dxmax;
				}
				else
				{
					x = xmin;
					dx = dxmax;
				}
			}

			xdviterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, dx, xmin, xmax, x);

			if (critical == 2)
			{
				x = dvmaxiterator(R1B, V1B, theta1, theta2, theta3, U_R1, U_H, xmin, dxmax, dv_max);
			}
		}
	}
	dvcalc(V1B, theta1, theta2, theta3, x, U_R1, U_H, V2, DV, p_CON);
}

void RTEEarth::conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3)
{
	double r1b, lambda, beta1, beta5, beta10;
	r1b = length(R1B);

	lambda = r1b / RCON;
	beta1 = 1.0 + x2*x2;
	beta5 = lambda*beta1;
	theta1 = beta5*lambda - 1.0;
	theta2 = 2.0*r1b*(lambda - 1.0);
	theta3 = sqrt(mu) / r1b;
	beta10 = beta5*(MA1 - r1b) / (MA1 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmin = 0.0;
	}
	else
	{
		xmin = -sqrt(beta10);
	}
	dxmax = -xmin / 16.0;
	beta10 = beta5*(MA2 - r1b) / (MA2 - RCON) - 1.0;
	if (beta10 < 0.0)
	{
		xmax = 0.0;
	}
	else
	{
		xmax = sqrt(beta10);
	}
}

void RTEEarth::precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2)
{
	VECTOR3 U_V1, eta;
	double r1b;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;
	if (x2set)
	{
		reentryconstraints(0, R1B, _V(0, 0, 0), _V(0, 0, 0));
	}
}

bool RTEEarth::EntryIter()
{
	double theta_long, theta_lat, dlng;
	VECTOR3 V2;

	errorstate = 0;
	
	if (ii == 0)
	{
		coniciter(sv_ig.R, sv_ig.V, sv_ig.GMT, theta_long, theta_lat, V2, x, dx, t21);
	}
	else
	{
		precisioniter(sv_ig.R, sv_ig.V, sv_ig.GMT, t21, x, theta_long, theta_lat, V2);
	}

	if (critical == 1)
	{
		EntryCalculations::TBLOOK(LINE, theta_lat, EntryLng); //TBD: Error return
		dlng = EntryLng - theta_long;
		if (abs(dlng) > PI)
		{
			dlng = dlng - OrbMech::sign(dlng)*PI2;
		}

		if (ii == 0)
		{
			dx = -dlng * RAD;
			xapo = x;
			dlngapo = theta_long;
			x += dx;
		}
		else
		{
			dx = (x - xapo) / (theta_long - dlngapo)*dlng;
			if (length(V2 - sv_ig.V) > dv_max && dx < 0)
			{
				dx = 0.5*max(1.0, revcor);
				revcor++;
			}
			else if (abs(dx) > dxmax)
			{
				dx = OrbMech::sign(dx)*dxmax;
			}
			xapo = x;
			dlngapo = theta_long;
			x += dx;

		}
	}

	ii++;

	//Always go to precision phase on second iteration
	if (ii == 1)
	{
		return false;
	}
	//If ATP hasn't converge and it's not a fuel critical return
	else if (critical == 1 && (abs(dlng) > 0.005*RAD && ii < 60))
	{
		return false;
	}
	else
	{
		EphemerisData sv_EI;
		VECTOR3 R05G, V05G, R3, V3, UR3;
		double t32, dt22, v3, S_FPA;

		sv_ig_apo.V = V2;
		pRTCC->PMMCEN(sv_ig_apo, 0.0, 10.0*24.0*3600.0, 3, RD, 1.0, sv_EI, ITS);
		//t21 = OrbMech::time_radius_integ(R_ig, V2, mjd_ig, RD, -1, hEarth, hEarth, REI, VEI);//Maneuver to Entry Interface (400k ft)
		
		R_r = sv_EI.R;
		V_r = sv_EI.V;
		t2 = sv_EI.GMT;

		EntryCalculations::LNDING(sv_EI.R, sv_EI.V, t2, alpha_SID0, 0.3, ICRNGG, r_rbias, theta_long, theta_lat, t_Z);

		t32 = OrbMech::time_radius(sv_EI.R, sv_EI.V, length(sv_EI.R) - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(sv_EI.R, sv_EI.V, t32, R3, V3, mu); //Entry Interface to 300k ft

		dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu); //300k ft to 0.05g

		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;

		DV = V2 - sv_ig.V;
		VECTOR3 i, j, k;
		MATRIX3 Q_Xx;
		j = unit(crossp(sv_ig.V, sv_ig.R));
		k = unit(-sv_ig.R);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		Entry_DV = mul(Q_Xx, DV);

		EntryRTGO = OrbMech::CMCEMSRangeToGo(pRTCC->SystemParameters.MAT_J2000_BRCS, R05G, OrbMech::MJDfromGET(t2 + t32 + dt22, GMTbase), theta_lat, theta_long);
		EntryVIO = length(V05G);
		EntryRET = t2 + t32 + dt22;
		EntryAng = atan(x2);//asin(dotp(unit(REI), VEI) / length(VEI));

		EntryLngcor = theta_long;
		EntryLatcor = theta_lat;

		if (errorstate != 0)
		{
			precision = 9;
		}

		return true;
	}
}

ConicRTEEarthNew::ConicRTEEarthNew(RTCC *r, std::vector<EphemerisData2> &SVArray) : RTCCModule(r),
	XArray(SVArray)
{
	mu = OrbMech::mu_Earth*pow(SCPHR, 2) / pow(KMPER*1000.0, 3);
	RR = (OrbMech::R_Earth + 400000.0 * 0.3048) / (KMPER*1000.0);
	R_E = 1.0;
	w_E = OrbMech::w_Earth*SCPHR;
	T_rz_avg = 0.14;
	eta_rz_avg = 0.5;
	hEarth = oapiGetObjectByName("Earth");
}

void ConicRTEEarthNew::Init(double dvm, int icrngg, double irmax, double urmax, double rrbi, int imsfn)
{
	double CV, CD;

	CV = FTPER / SCPHR;
	CD = NMPER;

	DVMAX = dvm / CV;
	ICRNGG = icrngg;
	I_rmax = irmax / DPR;
	U_rmax = urmax / CV;
	RRBI = rrbi;
	IMSFN = imsfn;
}

void ConicRTEEarthNew::READ(int mode, double gmtbase, double tzmin, double tzmax)
{
	Mode = mode;
	GMTbase = gmtbase;
	T_zmax = tzmax;
	T_zmin = tzmin;
}

void ConicRTEEarthNew::ATP(std::vector<ATPData> line)
{
	mm = line.size();

	for (unsigned i = 0;i < line.size();i++)
	{
		delta_apo[i] = line[i].lat;
		lambda_apo[i] = line[i].lng;
	}
}

void ConicRTEEarthNew::MAIN()
{
	//Pre-initialization
	VECTOR3 DV, V_a_uncal, V_a_cal;
	double beta_r, dv, U_r, DVC, T, VT_a, VR_a, v_a, beta_a, T_z, alpha, delta, lambda, p, eta_ar, phi, phi_z, theta_z, TP;
	int J, J_m, FLAG;
	bool QA;
	EphemerisData2 sv;

	J = 0;
	J_m = XArray.size();

	do
	{
		sv = XArray[J];

		X0 = sv.R / KMPER / 1000.0;
		U0 = sv.V*SCPHR / KMPER / 1000.0;
		T0 = sv.GMT / SCPHR;
		INITAL();
		if (NOSOLN == 1)
		{
			goto ConicRTEEarth_MAIN_E;
		}
		if (Mode == 1)
		{
			beta_r = EntryCalculations::ReentryTargetLine(U_rmax*KMPER*1000.0 / SCPHR, false);
			FCUA(0, X0, beta_r, dv, U_r, v_a, beta_a);
			if (NOSOLN == 1)
			{
				goto ConicRTEEarth_MAIN_E;
			}
			VT_a = v_a * sin(beta_a);
			//Radial velocity
			VR_a = v_a * cos(beta_a);
		ConicRTEEarth_MAIN_A:
			VACOMP(VR_a, VT_a, beta_r, theta_0, DV, T_z, V_a_uncal, alpha, delta, lambda);
			if (Mode != 1)
			{
				VUP2(X0, V_a_uncal, T, beta_r, V_a_cal);
			}
			else
			{
				V_a_cal = V_a_uncal;
			}
			//Store solutions
			//T_ar_stored = T;
			//OrbMech::rv_from_r0v0(X0, V_a, T_ar_stored, RR_vec, VV_vec, mu);
			T_ar_stored = OrbMech::time_radius(X0, V_a_uncal, RR, -1.0, mu);
			OrbMech::rv_from_r0v0(X0, V_a_uncal, T_ar_stored, RR_vec, VV_vec, mu);
			StoreSolution(V_a_cal - U0, delta, T0, T, T_z);
		}
		else
		{
			TMIN(dv, FLAG, T, U_r, VT_a, VR_a, beta_r);
			if (NOSOLN != 0)
			{
				goto ConicRTEEarth_MAIN_E;
			}
			if (Mode == 0)
			{
				DVC = dv;
				goto ConicRTEEarth_MAIN_A;
			}
			T_mt = T;
		ConicRTEEarth_MAIN_B:
			VELCOM(T, r0, beta_r, DT, p, QA, SW6, U_r, VR_a, VT_a, beta_a, eta_ar, dv);
			MSDS(VR_a, VT_a, beta_r, theta_0, delta, phi, phi_z, lambda, theta_z);
			if (SW2 == 0 || Mode >= 4)
			{
				if (T > T_mt && abs(MD) < 1e-4 && (Mode == 2 || Mode == 3))
				{
					SW2 = 1;
					//MD1 = 50;
					//TARSP?
					goto ConicRTEEarth_MAIN_C;
				}
				else
				{
					TCOMP(dv, delta, T, TP);
					if (STORE)
					{
						VACOMP(VR_a, VT_a, beta_r, theta_0, DV, T_z, V_a_uncal, alpha, delta, lambda);

						//OrbMech::rv_from_r0v0(X0, V_a_uncal, T_ar_stored, RR_vec, VV_vec, mu);
						T_ar_stored = OrbMech::time_radius(X0, V_a_uncal, RR, -1.0, mu);
						OrbMech::rv_from_r0v0(X0, V_a_uncal, T_ar_stored, RR_vec, VV_vec, mu);

						VUP2(X0, V_a_uncal, T_ar_stored, beta_r, V_a_cal);
						StoreSolution(V_a_cal - U0, delta, T0, T, T_z);
						STORE = false;
					}
				ConicRTEEarth_MAIN_C:

					if (SOL && (Mode == 2 || Mode == 3) && MDM != 0)
					{
						//SCAN();
					}

					if (END == 1)
					{
						if (NOSOLN == 2)
						{
							goto ConicRTEEarth_MAIN_A;
						}
						else
						{
							goto ConicRTEEarth_MAIN_E;
						}
					}
					else
					{
						goto ConicRTEEarth_MAIN_B;
					}
				}
			}
		}
	ConicRTEEarth_MAIN_E:
		J++;
		if (J >= J_m)
		{
			return;
		}
	} while (J < J_m);
}

void ConicRTEEarthNew::StoreSolution(VECTOR3 dv, double lat, double t0, double t, double tz)
{
	if (Mode == 2 || Mode == 4)
	{
		//Tradeoff
		TradeoffData data;
		data.DV = length(dv) * FTPER / SCPHR;
		data.lat = lat*DEG;
		data.T0 = t0;
		data.T_Z = tz;
		TOData.push_back(data);
	}
	else
	{
		//Discrete
		SolData.delta_z = lat * DEG;
		SolData.T_r = (T0 + T_ar_stored) * SCPHR;
		SolData.RR = RR_vec * KMPER * 1000.0;
		SolData.U_r = VV_vec /SCPHR * KMPER * 1000.0;
		SolData.DV = dv / SCPHR * KMPER * 1000.0;
		SolData.NOSOLN = NOSOLN;
		END = true;
		NOSOLN = 1;
	}
}

void ConicRTEEarthNew::INITAL()
{
	VECTOR3 R4, R5, R_p, U0_apo;
	double SAZ, CAZ, T1, T2, A_m, beta_r, p, R_a, A, DV, e, V_a, beta_a, T_1i, T_s, delta_0, Am1, Am2, beta_r_apo, A_Z;
	double theta_mu, theta_md, K1, K2, U_rmin, T_apo, DNDT, I_0, T, delta, U_r, VR_a, VT_a, eta_ar;
	int FLAG;
	bool QA;

	NOSOLN = 0;
	STORE = false;
	//Values from initial state vector
	beta_0 = PI05 - asin(dotp(unit(X0), unit(U0)));
	R0 = unit(X0);
	r0 = length(X0);
	u0 = length(U0);

	//angular momentum vector
	R1 = crossp(X0, U0);
	//unit angular momentum vector
	R1 = unit(R1);
	//Local horizon vector, pointing forward
	R2 = crossp(R1, X0);
	//Unit vector of above
	R2 = unit(R2);
	//Vector pointing east?
	R4 = crossp(_V(0, 0, 1), X0);
	R4 = unit(R4);
	//Vector pointing north?
	R5 = crossp(X0, R4);
	R5 = unit(R5);
	//Sine of azimuth
	SAZ = dotp(R2, R4);
	//Cosine of azimuth
	CAZ = dotp(R2, R5);

	T_min = T_zmin - T0 - T_rz_avg;
	T_max = T_zmax - T0 - T_rz_avg;
	//Landing time limit (looks like upper limit)
	T1 = 77.0 + 6.2 * length(X0) - 0.103333 * pow(length(X0), 2);
	//Reciproke of semi-major axis, memo had 4 instead of mu
	A_m = 2.0 / RR - pow(U_rmax, 2) / mu;
	beta_r = EntryCalculations::ReentryTargetLine(U_rmax*KMPER*1000.0 / SCPHR, false);
	//Orbit parameter
	p = RR * RR*U_rmax*U_rmax*pow(sin(beta_r), 2) / mu;
	//Apoapsis radius with max reentry speed
	R_a = 1.0 / A_m * (1.0 + sqrt(1.0 - p * A_m));
	//If max apoapsis radius is greater than abort position radius, we have an error. Hyperbolic is ok?
	if (R_a > 0.0 && R_a < length(X0) && Mode != 1)
	{
		NOSOLN = 1;
		return;
	}
	//Generate return flight time for max reentry speed and apogee passage (also prograde)
	if (RUBR(1, 0, length(X0), length(U0), U_rmax, beta_r, A, DV, e, T_1i, V_a, beta_a))
	{
		T_1i = 10000000.0;
	}
	//Generate return flight time for max reentry speed and no apogee passage (also prograde)
	RUBR(0, 0, length(X0), length(U0), U_rmax, beta_r, A, DV, e, T_s, V_a, beta_a);
	
	if (T_1i < T1)
	{
		T1 = T_1i;
	}
	//Specified maximum time is too high, use T1 (upper limit?) instead
	if (T1 < T_max)
	{
		T_max = T1;
	}
	//If T_min is returning too quickly, use min return time (max speed) in its place
	if (T_s > T_min)
	{
		T_min = T_s;
	}
	//Latitude
	delta_0 = asin(dotp(X0, _V(0, 0, 1)) / length(X0));
	//Azimuth
	A_Z = atan2(SAZ, CAZ);
	if (A_Z < 0)
	{
		A_Z += PI2;
	}
	//Preabort motion direct flag (chose based on azimuth)
	// 0 = prograde, 1 = retrograde
	Q0 = 1;
	if (A_Z < PI)
	{
		Q0 = 0;
	}
	//Azimuth limit for max inclination
	Am1 = asin(cos(I_rmax) / cos(delta_0));
	//Other azimuth limit
	Am2 = PI - Am1;
	//Upper (north) azimuth change constraint
	theta_mu = A_Z - Am1;
	//Down (south) azimuth change constraint
	theta_md = A_Z - Am2;
	double alpha_g0 = OrbMech::GetPlanetCurrentRotation(BODY_EARTH, GMTbase);
	alpha_g = alpha_g0 + w_E * T0;
	//Radial speed
	VR_0 = dotp(X0 / length(X0), U0);
	//Tangential speed
	VT_0 = sqrt(pow(length(U0), 2) - VR_0 * VR_0);
RTEEarth_INITAL_B:
	//Coefficients for calculating slowest reentry speed. Calculates speed assuming apogee?
	K1 = pow(r0 / (RR * sin(beta_r)), 2);
	K2 = 2.0*mu*(1.0 / RR - 1.0 / r0);
	U_rmin = sqrt(K2*K1 / (K1 - 1.0));
	beta_r_apo = EntryCalculations::ReentryTargetLine(U_rmin*KMPER*1000.0 / SCPHR, false);
	if (abs(beta_r - beta_r_apo) >= 1e-4)
	{
		beta_r = beta_r_apo;
		goto RTEEarth_INITAL_B;
	}
	//Return time with minimum reentry speed
	RUBR(0, 0, length(X0), length(U0), U_rmin, beta_r, A, DV, e, T_apo, V_a, beta_a);
	//Azimuth change nearest to inclination constraint
	if (Am1 <= A_Z && A_Z <= Am2)
	{
		//If inside limit, set to 0
		theta_0 = 0.0;
	}
	else if (A_Z > 3.0*PI05 || A_Z < PI05)
	{
		//If outside limit and direction is northerly, use northerly constraint
		theta_0 = theta_mu;
	}
	else
	{
		//If outside limit and direction is southerly, use southherly constraint
		theta_0 = theta_md;
	}
	DVM = pow(DVMAX, 2) - pow(VT_0*sin(theta_0), 2);
	if (DVM < 0)
	{
		NOSOLN = 1;
		return;
	}
	DVM = sqrt(DVM);
	R_p = unit(X0);
	U0_apo = R_p * VR_0 + R2 * VT_0*pow(cos(theta_0), 2) + R1 * VT_0*cos(theta_0)*sin(theta_0);
	if (theta_0 == 0.0)
	{
		I_0 = acos(dotp(R1, _V(0, 0, 1)));
	}
	else
	{
		I_0 = I_rmax;
	}
	if (T_min > 4.0)
	{
		TSW6 = T_min - 0.01;
		goto RTEEarth_INITAL_End;
	}

	TSW6 = max(2.0, T_min);
	T = TSW6;
	VELCOM(TSW6, r0, beta_r, DT, p, QA, SW6, U_r, VR_a, VT_a, beta_a, eta_ar, DV);
	if (T > T_apo)
	{
		FLAG = 1;
	}
	else
	{
		FLAG = -1;
	}
	DNDT = PRTIAL(FLAG, r0, U_rmax);
	delta = asin(cos(eta_ar + eta_rz_avg))*dotp(R0, _V(0, 0, 1)) + sin(eta_ar + eta_rz_avg)*dotp(R2, _V(0, 0, 1));
	if (DNDT <= w_E*pow(cos(delta), 2) / cos(I_0))
	{
		TSW6 = T_min - 0.01;
		goto RTEEarth_INITAL_End;
	}

	T1 = TSW6;
	T2 = 4.0*3600.0;
RTEEarth_INITAL_C:
	TSW6 = (T1 + T2) / 2.0;
	if (abs(T1 - T2) < 0.1)
	{
		goto RTEEarth_INITAL_End;
	}

	T = TSW6;
	VELCOM(TSW6, r0, beta_r, DT, p, QA, SW6, U_r, VR_a, VT_a, beta_a, eta_ar, DV);
	if (T > T_apo)
	{
		FLAG = 1;
	}
	else
	{
		FLAG = -1;
	}

	DNDT = PRTIAL(FLAG, r0, U_rmax);
	delta = asin(cos(eta_ar + eta_rz_avg))*dotp(R0, _V(0, 0, 1)) + sin(eta_ar + eta_rz_avg)*dotp(R2, _V(0, 0, 1));
	double eps = 0.005; //TBD
	if (DNDT > w_E*pow(cos(delta), 2) / cos(I_0) + eps)
	{
		T1 = TSW6;
		goto RTEEarth_INITAL_C;
	}
	if (DNDT <= w_E*pow(cos(delta), 2) / cos(I_0) - eps)
	{
		T2 = TSW6;
		goto RTEEarth_INITAL_C;
	}

RTEEarth_INITAL_End:
	//KK = 1;
	MDP = 10e10;
	XK = 1;
	SW2 = 0;
	//DVSP[0] = DVSP[1] = DVSP[2] = DVSP[3] = DVSP[4] = 10.0;
	SW6 = 0;
	TEST = 0;
	DDT = 1.0;
	DT = 0.1;
	//PARP = 0;
}

bool ConicRTEEarthNew::RUBR(int QA, int QE, double R_a, double U_0, double U_r, double beta_r, double &A, double &DV, double &e, double &T, double &V_a, double &beta_a)
{
	//INPUTS:
	//QA: Apogee passage flag. 0 = no apogee passage, 1 = apogee passage
	//QE: Postabort motion flag. 0 = direct, 1 = retrograde

	double E, T_ap, T_rp, Period, sin_beta_a;

	//Specific orbital energy
	E = U_r * U_r / mu - 2.0 / RR;

	//E>0 means hyperbolic orbit. If apogee passage is desired and orbit is hyperbolic, then you aren't coming home
	if (E > 0 && QA == 1)
	{
		//No solution
		return true;
	}
	//Check for elliptic vs. hyperbolic. If nearly hyperbolic use hyperbolic
	if (abs(E) - 0.0001 > 0)
	{
		//Elliptical or hyperbolic orbit
		//Semi-major axis
		A = -1.0 / E;
		//Orbit parameter (semi-latus rectum)
		double P = pow(RR*U_r*sin(beta_r), 2) / mu;
		//Eccentricity
		e = sqrt(1.0 - P / A);
	}
	else
	{
		//Parabolic orbit
		A = pow(RR*U_r*sin(beta_r), 2) / mu;
		e = 1.0;
	}
	//Postabort speed
	V_a = sqrt(mu*(U_r*U_r / mu + 2.0 / R_a - 2.0 / RR));
	//Sine of postabort flight-path angle
	sin_beta_a = U_r * RR / V_a / R_a * sin(beta_r);
	//Postabort flight-path angle (-90° to 90°)
	beta_a = asin(sin_beta_a);
	//If no apogee passage is desired, make angle 90° to 270°. MSC memo was wrong, probably.
	if (QA == 0)
	{
		beta_a = PI - beta_a;
	}
	//If postabort motion is retrograde, invert angle
	if (QE == 1)
	{
		beta_a = PI2 - beta_a;
	}
	//Change in velocity
	DV = sqrt(U_0*U_0 + V_a * V_a - 2.0*U_0*V_a*cos(beta_a - beta_0));
	//Time from abort to perigee
	pRTCC->PITFPC(mu, QA, A, e, R_a, T_ap, Period);
	//Time from reentry to perigee
	pRTCC->PITFPC(mu, QA, A, e, RR, T_rp, Period);
	//Time from abort to reentry
	T = T_ap - T_rp;
	if (T < 0)
	{
		T = T + Period;
	}
	return false;
}

void ConicRTEEarthNew::VARMIN()
{
	SOL = false;

	double ERR;
	//Miss distance larger than 0.001, we have not found a solution yet
	if (abs(MD) >= 0.001)
	{
		//Difference to previous miss distance
		ERR = abs(MD) - abs(MDP);
		//Miss distance has changed to before, no solution yet
		if (abs(ERR) >= 0.0001)
		{
			//DDT has gotten quite small, don't continue
			if (!(abs(DDT) < 0.025 && MD > 0.5))
			{
				//Is miss distance changing in the right direction?
				if (ERR*(double)XK < 0)
				{
					//Yes, just continue on
					MDP = MD;
					return;
				}
				else
				{
					//No, it's going in the wrong direction
					//Did we want to maximize MD?
					if (XK < 0)
					{
						//Extremum has been reached, now do minimization
						XK = 1;
						MDP = MD;
						return;
					}
					else
					{
						//Change direction for minimization
						DDT = -DDT / 2.0;
						if (abs(DDT) >= 0.001)
						{
							MDP = MD;
							return;
						}
					}
				}
			}
		}
	}

	MDP = pow(10, 10);
	SOL = true;
	DDT = 1.0;
	return;
}

void ConicRTEEarthNew::VELCOM(double T, double R_a, double &beta_r, double &dt, double &p, bool &QA, int &sw6, double &U_r, double &VR_a, double &VT_a, double &beta_a, double &eta_ar, double &DV)
{
	double V_a, C0, C1, C2, S1, DVR, DVT, a, e, beta_rp;
	int info, TEMP;

	do
	{
		//Set current flight-path angle at reentry into past value
		beta_rp = beta_r;
		//Calculate new trajectory
		EntryCalculations::AESR(RR, R_a, PI - beta_r, T, R_E, mu, 0.01 / SCPHR, a, e, QA, info, U_r);
		//Calculate new flight path angle
		beta_r = EntryCalculations::ReentryTargetLine(U_r*KMPER*1000.0 / SCPHR, false);
		//Check if flight path angle has converged
	} while (abs(beta_r - beta_rp) >= 1e-5);
	//Calculate velocity after abort
	V_a = sqrt(U_r*U_r + 2.0*mu * (1.0 / R_a - 1.0 / RR));
	//Flight path angle after abort (0 to 90°)
	beta_a = asin(RR*U_r*sin(beta_r) / (R_a*V_a));
	//Full range (0° to 180°)
	beta_a = abs(beta_a + PI * ((double)QA - 1.0));
	C0 = R_a / RR * OrbMech::cot(beta_r);
	C1 = (OrbMech::cot(beta_a) + C0) / (1.0 - R_a / RR);
	C2 = (C1*C1 - 1.0) / (C1*C1 + 1.0);
	S1 = (1.0 - C2)*C1;
	//Transfer angle from abort to reentry
	eta_ar = atan2(S1, C2);
	if (eta_ar < 0)
	{
		eta_ar += PI2;
	}
	//Tangential velocity
	VT_a = V_a * sin(beta_a);
	//Radial velocity
	VR_a = V_a * cos(beta_a);
	//Radial DV
	DVR = VR_a - VR_0;
	//Tangential DV
	DVT = VT_a - VT_0;
	//Total DV
	DV = sqrt(DVR*DVR + DVT * DVT);
	//Is return time smaller than motion switch time?
	if (T <= TSW6)
	{
		//Yes, motion is easterly
		sw6 = 0;
		dt = 0.5;
	}
	else
	{
		//No, motion is westerly (we don't want this)
		sw6 = 1;
		if (Mode >= 4)
		{
			dt = 3.0;
		}
		else
		{
			double T_rz, eta_rz, theta_cr;
			dt = 0.5;
			TEMP = ICRNGG;
			ICRNGG = 10;
			RENTRY(0.3, U_r, eta_ar, 0.0, T_rz, eta_rz, theta_cr);
			ICRNGG = TEMP;
			eta_rz_avg = eta_rz;
		}
	}
}

void ConicRTEEarthNew::FCUA(int FLAG, VECTOR3 R_a, double &beta_r, double &DV, double &U_r, double &V_a, double &beta_a)
{
	double r_a, beta_r_apo, dbeta, ER, ERR, BS, U_rmax_apo, A, e, T;
	int QA;

	r_a = length(R_a);
	U_rmax_apo = U_rmax;
ConicRTE_FCUA_A:

	DVMINQ(FLAG, 0, Q0, beta_r, DV, QA, V_a, beta_a);
	if (DV > 10e8)
	{
		return;
	}
	U_r = sqrt(V_a*V_a + 2.0*mu*(1.0 / RR - 1.0 / r_a));
	beta_r_apo = EntryCalculations::ReentryTargetLine(U_r*KMPER*1000.0 / SCPHR, false);
	if (SW2 == 0)
	{
		if (abs(beta_r - beta_r_apo) < 1e-4)
		{
			if (DV > DVM)
			{
				dbeta = 2.0*RAD;
				SW2 = 1;
				beta_r = PI05;
				goto ConicRTE_FCUA_A;
			}
			else
			{
				goto ConicRTE_FCUA_B;
			}
		}
		else
		{
			beta_r = beta_r_apo;
			goto ConicRTE_FCUA_A;
		}
	}
	else
	{
		if (beta_r == PI05)
		{
		ConicRTE_FCUA_C:
			ER = DVM - DV;
			BS = beta_r;
			beta_r += dbeta;
			if (beta_r > 110.0*RAD)
			{
				NOSOLN = 1;
				return;
			}
			goto ConicRTE_FCUA_A;
		}
		ERR = DVM - DV;
		if (ERR < 1e-4)
		{
			goto ConicRTE_FCUA_B;
		}
		if (ERR*ER < 0 || SW2 == 2)
		{
			dbeta = (beta_r - BS) / (ERR - ER)*(DVM - DV);
		}
		goto ConicRTE_FCUA_C;
	}

ConicRTE_FCUA_B:
	if (U_r > U_rmax)
	{
		if (SW2 == 0)
		{
			QA = 1;
			if (beta_0 > PI05)
			{
				QA = 0;
			}
			else
			{
				U_rmax_apo = min(U_rmax, 36323.0*0.3048);
			}
			beta_r = EntryCalculations::ReentryTargetLine(U_rmax_apo*KMPER*1000.0 / SCPHR, false);
			RUBR(QA, 0, r_a, u0, U_r, beta_r, A, DV, e, T, V_a, beta_a);
			if (DV > DVM)
			{
				NOSOLN = 1;
			}
			return;
		}
		else
		{
			NOSOLN = 1;
			return;
		}
	}
}

void ConicRTEEarthNew::DVMINQ(int FLAG, int QE, int Q0, double beta_r, double &DV, int &QA, double &V_a, double &beta_a)
{
	double K1, K2, A0, B0, X[4], Y[4], A, B, C, D, E, DVV[4], TEST;
	int k, km, K, J, a, I;

	K1 = pow(r0 / (RR*sin(beta_r)), 2);
	K2 = 2.0*mu*(1.0 / RR - 1.0 / r0);
	//By default no apogee passage
	QA = 0;
	//Tangential velocity
	A0 = u0 * sin(beta_0);
	//Radial velocity
	B0 = u0 * cos(beta_0);
	//If radial velocity is small, use this path
	if (abs(B0) < 0.005)
	{
		X[0] = A0 / K1;
		X[1] = X[0];
		X[2] = sqrt(K2 / (K1 - 1.0));
		X[3] = -X[2];
		if (X[0] < X[2])
		{
			X[0] = X[2];
			X[1] = X[3];
			Y[0] = 0.0;
			Y[1] = 0.0;
			k = 2;
		}
		else
		{
			Y[0] = sqrt(X[0] * X[0] * (K1 - 1.0) - K2);
			Y[1] = -Y[0];
			Y[2] = 0.0;
			Y[3] = 0.0;
			k = 4;
		}
	RTEEarth_DVQMIN_AA:
		km = k;
		k = 1;
		do
		{
			DVV[k - 1] = sqrt(pow(X[k - 1] - A0, 2) + pow(Y[k - 1] - B0, 2));
			k++;
		} while (k <= km);

		goto RTEEarth_DVQMIN_D;
	}
	else
	{
		A = K1 * K1 * (K1 - 1.0);
		B = -2.0*A0*K1*(K1 - 1.0);
		C = -K1 * K1*K2 + A0 * A0*(K1 - 1.0) - B0 * B0*pow(K1 - 1.0, 2);
		D = 2.0*A0*K1*K2;
		E = -A0 * A0*K2;
		OrbMech::DROOTS(A, B, C, D, E, 4, X, K, I);
		km = K;
		J = 1;
		k = 1;
	RTEEarth_DVQMIN_C:
		TEST = pow(X[k - 1], 2)*(K1 - 1.0) - K2;
		if (TEST < 0)
		{
			goto RTEEarth_DVQMIN_A;
		}
		else
		{
			goto RTEEarth_DVQMIN_B;
		}
	}

RTEEarth_DVQMIN_B:
	Y[J - 1] = B0 * X[k - 1] * (K1 - 1.0) / ((X[k - 1] * K1 - A0));
	X[J - 1] = X[k - 1];
	J++;
RTEEarth_DVQMIN_A:
	k++;
	if (k > km)
	{
		k = J - 1;
		goto RTEEarth_DVQMIN_AA;
	}
	else
	{
		goto RTEEarth_DVQMIN_C;
	}

RTEEarth_DVQMIN_D:
	if (FLAG <= 0)
	{
		a = 2;
	RTEEarth_DVQMIN_BB:
		J = a - 1;
		DV = DVV[a - 2];
		k = a;
	RTEEarth_DVQMIN_DD:
		if ((Q0 == QE && DVV[k - 1] < DV) || (Q0 != QE && DVV[k - 1] > DV))
		{
			if (FLAG == 1 && Y[k - 1] > 0)
			{
			}
			else
			{
				DV = DVV[k - 1];
				J = k;
			}
		}
		if (k == km)
		{
			beta_a = atan2(X[J - 1], Y[J - 1]);
			if (beta_a < 0)
			{
				beta_a += PI2;
			}
			V_a = sqrt(pow(X[J - 1], 2) + pow(Y[J - 1], 2));
			if ((QE == 0 && beta_a < PI05) || (QE == 1 && beta_a > 3.0*PI05))
			{
				QA = 1;
			}
		}
		else
		{
			k++;
			goto RTEEarth_DVQMIN_DD;
		}
	}
	else
	{
		k = 1;
	RTEEarth_DVQMIN_CC:
		if (X[k - 1] >= 0 && Y[k - 1] < 0)
		{
			a = k + 1;
			k = km;
			goto RTEEarth_DVQMIN_BB;
		}
		else
		{
			if (k == km)
			{
				DV = pow(10, 10);
				return;
			}
			else
			{
				k++;
				goto RTEEarth_DVQMIN_CC;
			}
		}
	}
}

void ConicRTEEarthNew::MSDS(double VR_a, double VT_a, double beta_r, double theta, double &delta, double &phi, double &phi_z, double &lambda, double &theta_z)
{
	if (Mode >= 4)
	{
		VECTOR3 DV, V_a;
		double L, T_z, alpha, DL;
		int m;

		VACOMP(VR_a, VT_a, beta_r, theta, DV, T_z, V_a, alpha, delta, lambda);
		m = 1;
		if (delta >= delta_apo[m - 1])
		{
			L = lambda_apo[m - 1];
		}
		else if (delta <= delta_apo[mm - 1])
		{
			L = lambda_apo[mm - 1];
		}
		else
		{
			while (delta < delta_apo[m] && m < mm - 1)
			{
				m++;
			}
			DL = lambda_apo[m] - lambda_apo[m - 1];
			if (abs(DL) > PI)
			{
				DL = -DL / abs(DL)*(PI2 - abs(DL));
			}
			L = lambda_apo[m - 1] + DL / (delta_apo[m] - delta_apo[m - 1])*(delta - delta_apo[m - 1]);
		}

		MD = L - lambda;
		if (abs(MD) > PI)
		{
			MD = -MD / abs(MD)*(PI2 - abs(MD));
		}
		return;
	}

	/*double phi;

	//Angle inplane from abort to impact point
	phi = eta_ar + eta_rz;
	cos_theta_m = (DVR*DVR + VT_a * VT_a + VT_0 * VT_0 - DVM * DVM) / (2.0*VT_0*VT_a);
	if (cos_theta_m < -1 || cos_theta_m > 1)
	{
		cos_theta_m = cos_theta_m / abs(cos_theta_m);
	}
	theta_m = acos(cos_theta_m);
	Azz1 = A_z - theta_m;
	Azz2 = A_z + theta_m;
	if (Azz1 < -PI)
	{
		Azz1 += PI2;
	}
	else if (Azz1 > PI)
	{
		Azz1 -= PI2;
	}
	if (Azz2 < 0)
	{
		Azz2 += PI2;
	}
	else if (Azz2 > PI2)
	{
		Azz2 -= PI2;
	}
	if (Azz1 < 0)
	{
		Azz1 = 0.0;
	}
	if (Azz2 <= Azz1)
	{
		DUM = Azz1;
		Azz1 = Azz2;
		Azz2 = DUM;
	}
	theta_mu = theta_mu_apo;
	theta_md = theta_md_apo;
	A_zp = A_z;
	if (theta_m > PI)
	{

	}
	if (A_zp<)*/
}

double ConicRTEEarthNew::PRTIAL(int FLAG, double R_a, double U_r)
{
	double beta_r, a, p, e, PER, DFDA[2], DTDA[2], X, SF, CE, E, T, DNDT;
	int J;

	//Flight path angle
	beta_r = EntryCalculations::ReentryTargetLine(U_r*KMPER*1000.0 / SCPHR, false);
	//Semi-major axis
	a = 1.0 / (2.0 / RR - U_r * U_r / mu);
	//Semi-latus rectum
	p = pow(U_r*RR*sin(beta_r), 2) / mu;
	e = sqrt(1.0 - p / a);
	//Half period?
	PER = PI * sqrt(pow(abs(a), 3) / mu);
	if (e <= 1)
	{
		if (R_a >= a * (1.0 + e))
		{
			goto RTEEarth_PRTIAL_A;
		}
	}

	J = 1;
	X = R_a;
RTEEarth_PRTIAL_E:
	SF = (double)(FLAG)*sqrt(1.0 - pow((p / X - 1.0) / e, 2));
	CE = (1.0 - X / a) / e;
	if (U_r > 36333.0*SCPHR/FTPER)
	{
		E = log(CE + sqrt(CE*CE - 1.0));
		T = PER / PI * (e*sinh(E) - E);
		DTDA[J - 1] = 1.5*T / abs(a) + PER / PI * (CE + e * (CE - 1.0) - 1.0 - e * e*pow(sinh(E), 2)) / (a*e*sinh(E));

		goto RTEEarth_PRTIAL_C;
	}
	E = atan2((double)(FLAG)*sqrt(1.0 - CE * CE), CE);
	if (E < 0)
	{
		E += PI2;
	}
	if (abs(E - PI) < 1e-5)
	{
		goto RTEEarth_PRTIAL_A;
	}
	T = PER * (E - e * sin(E)) / PI;
	DTDA[J - 1] = 1.5*(2.0*PER - T) / a - PER / PI * ((X*(CE - 1.0)) / (a*a*e*sin(E)) - (1.0 - e)*sin(E) / a);
RTEEarth_PRTIAL_C:
	DFDA[J - 1] = (1.0 - e)*(p - X - a * e*(1.0 - e)) / (a*e*e*X*SF);
	if (J == 2)
	{
		goto RTEEarth_PRTIAL_D;
	}
	J = 2;
	X = RR;
	FLAG = -1;
	goto RTEEarth_PRTIAL_E;
RTEEarth_PRTIAL_D:
	DNDT = (DFDA[1] - DFDA[0]) / (DTDA[0] - DTDA[1]);
	return DNDT;
RTEEarth_PRTIAL_A:
	DNDT = PI / PER * sqrt((1.0 - e) / (1.0 + e))*1.0 / (1.0 + e);
	return DNDT;
}

void ConicRTEEarthNew::RENTRY(double LD, double U_r, double eta_ar, double theta, double &T_rz, double &eta_rz, double &theta_cr)
{
	VECTOR3 P, PP, Z;
	double delta_r, I_r, A_z, RO, DR, CR;
	int i;

	U_r = U_r * FTPER / SCPHR;
	Z = _V(0, 0, 1);
	//Vector pointing at reentry position
	RR_vec = R0 * cos(eta_ar) + R1 * sin(eta_ar)*sin(theta) + R2 * sin(eta_ar)*cos(theta);
	//Latitude of above
	delta_r = asin(dotp(RR_vec, Z));
	//Orbital plane at reentry
	P = unit(crossp(R0, RR_vec))*sin(eta_ar) / abs(sin(eta_ar));
	//Inclination at reentry
	I_r = acos(dotp(P, Z));
	//Azimuth at reentry
	A_z = asin(cos(I_r) / cos(delta_r));
	//Local horizon vector?
	PP = unit(crossp(P, RR_vec));
	if (dotp(P, Z) - dotp(RR_vec, Z)*dotp(RR_vec, PP) < 0)
	{
		A_z = PI - A_z;
	}
	if (RRBI > 0)
	{
		RO = RRBI;
	}
	else
	{
		if (ICRNGG == 1)
		{
			DR = 0.0;
			for (i = 0;i <= 5;i++)
			{
				DR += EntryCalculations::RTE_jj[IMSFN - 1][i] * pow(U_r, i);
			}
			DR += (EntryCalculations::RTE_q[IMSFN - 1][1] * U_r + EntryCalculations::RTE_q[IMSFN - 1][2])*EntryCalculations::RTE_q[IMSFN - 1][3] * (LD - 0.3);
			T_rz = EntryCalculations::RTE_ee[0] + EntryCalculations::RTE_ee[1] * DR;
			CR = EntryCalculations::RTE_cc*T_rz*cos(delta_r)*cos(A_z + 0.3*delta_r) + EntryCalculations::RTE_q[IMSFN - 1][4] * LD + EntryCalculations::RTE_q[IMSFN - 1][5];
			eta_rz = DR / NMPER;
			theta_cr = CR / NMPER;
			T_rz = T_rz / 3600.0;
			return;
		}
		else
		{
			RO = 0.0;
			for (i = 0;i <= 5;i++)
			{
				RO += EntryCalculations::RTE_b[i] * pow(U_r, i);
			}
		}
	}

	if (ICRNGG < 9)
	{
		DR = RO + (EntryCalculations::RTE_ff[0] + EntryCalculations::RTE_ff[1] * RO + EntryCalculations::RTE_ff[2] * U_r)*cos(I_r);
		T_rz = EntryCalculations::RTE_ee[0] + EntryCalculations::RTE_ee[1] * DR + EntryCalculations::RTE_ee[2] * U_r;
		CR = EntryCalculations::RTE_cc*T_rz*cos(delta_r)*cos(A_z + 0.3*delta_r);

		eta_rz = DR / NMPER;
		theta_cr = CR / NMPER;
		T_rz = T_rz / 3600.0;
	}
}

void ConicRTEEarthNew::SCAN()
{
	/*VACOMP();
	if ((DVS > DVMAX && T > T_mt&&SW2 == 0) || T > T_max)
	{
		END = 1;
		return;
	}
	MDMM = MD + MDSP[0];
	DVSPP = DVS;
	DVS = length(DV);
	if (SEEK != 1)
	{
		if (SW2 == 2 && MD > 0)
		{

		}
	}*/
}

void ConicRTEEarthNew::TCOMP(double dv, double delta, double &T, double &TP)
{
	END = false;

	if (Mode >= 4)
	{
		goto ConicRTEEarth_TCOMP_A2;
	}

	/*if (T > T_max || DVS > T_max)
	{
		SOL = 1;
		return;
	}

	P_ar = phi - phi_z;
	if (MDM != 0)
	{
		P_ar = MD;
	}
	if (abs(P_ar) < pow(10, -4))
	{
		SOL = 1;
		P_arp = 0;
		TEST = 0;
		DDT = 1.0;
	}
	else
	{
		if (TEST == 0)
		{
			if (theta != theta_z || P_ar * P_arp > 0 || P_arp == 0.0)
			{
				P_arp = P_ar;
				SOL = 0;
				VARMIN();
			}
			else
			{

			}
		}
	}*/

ConicRTEEarth_TCOMP_A2:

	if (T != T_mt)
	{
		if (SW2 == 1 || (MD * MDP < 0 && abs(MD - MDP) < PI))
		{
			goto ConicRTEEarth_TCOMP_C2;
		}
	}
	
	VARMIN();
	if (SOL == true)
	{
		if (abs(MD) > 0.1)
		{
		ConicRTEEarth_TCOMP_E2:
			DDT = 1.0;
			XK = -1;
			MDP = 0.0;
			TP = T;
			T = T + DDT * DT;
			return;
		}
		else
		{
			goto ConicRTEEarth_TCOMP_D2;
		}
	}
	else
	{
		if (T <= T_mt && DDT < 0.0)
		{
			DDT = 1.0;
			XK = -1;
			MDP = 0.0;
		}
		
		TP = T;
		T = T + DDT * DT;
		return;
	}

ConicRTEEarth_TCOMP_C2:
	if (abs(MD) < 0.01)
	{
	ConicRTEEarth_TCOMP_D2:
		SW2 = 0;
		if (T > T_max || dv > DVMAX)
		{
			END = true;
			//if (Mode == 2 || Mode == 4)
			//if (Mode > 1)
			//{
			//	NOSOLN = 0;
			//}
			return;
		}
		if (delta <= delta_apo[0] && delta >= delta_apo[mm - 1])
		{
			//Store solution
			NOSOLN = 2;
			//if (Mode == 2 || Mode == 4)
			if (Mode > 1)
			{
				STORE = true;
				T_ar_stored = T;
			}
		}
		if (SW6)
		{
			T = T + 24.0;
			MDP = pow(10, 10);
			XK = 1;
			return;
		}

		goto ConicRTEEarth_TCOMP_E2;
	}
	else
	{
		DT = -(TP - T) / (MDP - MD)*MD / DDT;
		MDP = MD;
		SW2 = 1;
		TP = T;
		T = T + DDT * DT;
		return;
	}
}

void ConicRTEEarthNew::TMIN(double &dv, int &FLAG, double &T, double &U_r, double &VT_a, double &VR_a, double &beta_r)
{
	VECTOR3 V_a;
	double A, e, v_a, beta_a, T1, p, eta_ar, eps1, eps2, T2;
	int SW;
	bool QA;

	//Tolerances
	eps1 = 1e-3;
	eps2 = 1e-5;

	//Set reentry speed to maximum allowed
	U_r = U_rmax;
	//No apogee passage
	QA = 0;
	//Reentry flight-path angle with maximum reentry speed
	beta_r = EntryCalculations::ReentryTargetLine(U_r*KMPER*1000.0 / SCPHR, false);
	//Calculate trajectory from abort to reentry with maximum speed
	RUBR(QA, 0, r0, u0, U_r, beta_r, A, dv, e, T, v_a, beta_a);
	//Trip time shorter than allowed?
	if (T < T_min)
	{
		//Use minimum
		T = T_min;
		T1 = T;
	}
	else
	{
		//Trip time with max reentry speed is acceptable.
		if (dv < DVM)
		{
			//DV is also acceptable. Solution found.
			return;
		}
		else
		{
			//DV too large. Use as initial guess.
			T1 = T;
			T_min = T;
		}
	}
	SW = 0;
ConicRTEEarth_TMIN_E:
	//Calculate DV using T as input
	VELCOM(T, r0, beta_r, DT, p, QA, SW6, U_r, VR_a, VT_a, beta_a, eta_ar, dv);
	V_a = R0 * VR_a + R2 * VT_a*cos(theta_0) + R1 * VT_a*sin(theta_0);
	dv = length(V_a - U0);

	if (abs(dv - DVM) < eps1)
	{
		if (U_r < U_rmax)
		{
			return;
		}
		else
		{
			NOSOLN = 1;
			return;
		}
	}
	else
	{
		if (SW == 0)
		{
			if (dv < DVM)
			{
				if (U_r < U_rmax)
				{
					return;
				}
			}
			goto ConicRTEEarth_TMIN_A;
		}
		else
		{
			goto ConicRTEEarth_TMIN_C;
		}
	}

ConicRTEEarth_TMIN_A:
	FLAG = 1;
ConicRTEEarth_TMIN_F:
	FCUA(FLAG, X0, beta_r, dv, U_r, v_a, beta_a);
	T = TripTime(v_a, beta_a);
ConicRTEEarth_TMIN_D:
	if (dv > DVM || T < T_min)
	{
		if (FLAG == 1)
		{
			FLAG = -1;
			goto ConicRTEEarth_TMIN_F;
		}
		else
		{
			NOSOLN = 1;
			return;
		}
	}
	if (T > T_max)
	{
		T2 = 2 * T_max - T_min;
		SW = 2;
	}
	else
	{
		T2 = T;
		SW = 1;
	}
	T1 = T_min;
ConicRTEEarth_TMIN_B:
	if (abs(T1 - T2) < eps2)
	{
		NOSOLN = 1;
		return;
	}
	T = (T1 + T2) / 2.0;
	goto ConicRTEEarth_TMIN_E;

ConicRTEEarth_TMIN_C:
	if (SW == 2)
	{
		if (dv > DVM)
		{
			goto ConicRTEEarth_TMIN_D;
		}
		else
		{
			T2 = T;
			SW = 1;
			goto ConicRTEEarth_TMIN_B;
		}
	}
	else
	{
		if (dv < DVM)
		{
			T2 = T;
			goto ConicRTEEarth_TMIN_B;
		}
		else
		{
			T1 = T;
			goto ConicRTEEarth_TMIN_B;
		}
	}
}

void ConicRTEEarthNew::VACOMP(double VR_a, double VT_a, double beta_r, double theta, VECTOR3 &DV, double &T_z, VECTOR3 &V_a, double &alpha, double &delta, double &lambda)
{
	VECTOR3 R_z, N, R_e;
	double T_rz, eta_rz, theta_cr, eta, A, e, E, P, beta_a, Period, T_ap, T_rp, T, eta_ar, U_r, C0, C1, C2, S1;
	int k;

	if (Mode == 2 || Mode == 3)
	{
		if (PI05 < abs(theta) && abs(theta) < C3PIO2)
		{

		}
		else
		{
			theta = theta + PI;
		}
		if (theta < 0)
		{
			theta += PI2;
		}
		else if (theta >= PI2)
		{
			theta -= PI2;
		}
		/*double cos_dtheta = (cos(MD + 1.0) - cos(phi_Z)*cos(phi)) / (sin(phi_z)*sin(phi));
		if (cos_dtheta > 1.0)
		{
			cos_dtheta = 1.0;
		}
		dtheta = acos(cos_dtheta);
		if (abs(dtheta) < abs(theta_z))
		{

		}
		else
		{

		}*/
	}

	theta = theta_0;
	V_a = R0 * VR_a + R2 * VT_a*cos(theta) + R1 * VT_a*sin(theta);

	E = pow(length(V_a), 2) / mu - 2.0 / r0;
	beta_a = atan2(VT_a, VR_a);
	if (abs(E) - 0.0001 > 0)
	{
		A = -1.0 / E;
		P = pow(r0*length(V_a)*sin(beta_a), 2) / mu;
		e = sqrt(1.0 - P / A);
	}
	else
	{
		A = pow(r0*length(V_a)*sin(beta_a), 2) / mu;
		e = 1.0;
	}

	if (beta_a > PI05)
	{
		k = 0;
	}
	else
	{
		k = 1;
	}

	pRTCC->PITFPC(mu, k, A, e, r0, T_ap, Period);
	pRTCC->PITFPC(mu, k, A, e, RR, T_rp, Period);
	T = T_ap - T_rp;
	if (T < 0)
	{
		T += Period;
	}
	U_r = sqrt(pow(length(V_a), 2) + 2.0*mu*(1.0 / RR - 1.0 / r0));
	C0 = r0 / RR * OrbMech::cot(beta_r);
	C1 = (OrbMech::cot(beta_a) + C0) / (1.0 - r0 / RR);
	C2 = (C1*C1 - 1.0) / (C1*C1 + 1.0);
	S1 = (1.0 - C2)*C1;
	//Transfer angle from abort to reentry
	eta_ar = atan2(S1, C2);
	if (eta_ar < 0)
	{
		eta_ar += PI2;
	}

	RENTRY(0.3, U_r, eta_ar, theta, T_rz, eta_rz, theta_cr);
	N = unit(crossp(V_a, X0));
	eta = eta_ar + eta_rz;
	R_z = R0 * cos(eta) + R2 * cos(theta)*sin(eta) - R1 * sin(theta)*sin(eta);
	R_z = R_z * cos(theta_cr) + N * sin(theta_cr);
	delta = dotp(R_z, _V(0, 0, 1));
	delta = asin(delta);
	R_e = R_z - _V(0, 0, 1)*sin(delta);
	alpha = atan2(R_e.y, R_e.x);
	lambda = alpha - alpha_g - w_E * (T + T_rz);
	while (lambda >= PI2)
	{
		lambda -= PI2;
	}
	while (lambda < 0)
	{
		lambda += PI2;
	}
	T_z = T0 + T + T_rz;
	DV = V_a - U0;
}

void ConicRTEEarthNew::VUP2(VECTOR3 R_a, VECTOR3 V_a, double T_ar, double beta_r, VECTOR3 &V_a2)
{
	VECTOR3 HANG, R_Moon, R_EM, V_EM, R_ES, R, RMAP, RAPOG, R_apo;
	double r_a, v_a, Z1, Z2, cos_beta_a, a, p, beta_a, e, cos_eta, sin_eta, eta, r_m, r_m_sq, r_cap, R_p, EAA, TAA, AMAA, TJ, theta, RCAPAP, FK, FKB;
	double deltat, deltat1, cos_PV, deltaT, Tr, T_arm, T_arm2, Z3, TERM, dt_dbetaa, dt_dva, dbetar_dbetaa, dbetar_dva, ALVA, D, dv, dbeta, T_art, dw;
	double DUM, DUM2, ES, ESS, PS, h, Sbeta_s, beta_s, CDB, DLBET, ZIT;

	pRTCC->PLEFEM(1, T0, 0, &R_EM, &V_EM, &R_ES, NULL);
	R_Moon = R_EM / (KMPER*1000.0);

	r_a = length(R_a);
	v_a = length(V_a);
	Z1 = r_a * v_a;
	Z2 = dotp(R_a, V_a);
	cos_beta_a = Z2 / Z1;
	beta_a = acos(cos_beta_a);
	a = mu * r_a / (2.0*mu - Z1 * v_a);
	p = pow(Z1*sin(beta_a), 2) / mu;
	e = sqrt(1.0 - p / a);
	cos_eta = (p / r_a - 1.0) / e;
	HANG = crossp(R_a, V_a);

	if (cos_eta > 1.0)
	{
		cos_eta = 1.0;
	}
	else if (cos_eta < -1.0)
	{
		cos_eta = -1.0;
	}
	sin_eta = sign(Z2)*sqrt(1.0 - pow(cos_eta, 2));
	eta = atan2(sin_eta, cos_eta);
	if (Z2 >= 0.0)
	{
		R = R_Moon - R_a;
		r_m = length(R_Moon);
		r_cap = length(R);
		r_m_sq = r_m * r_m;
		R_p = a * (1.0 + e);
		EAA = acos((1.0 - r_a / a) / e);
		AMAA = EAA - e * sin(EAA);
		TAA = a * sqrt(a / mu)*(PI - AMAA);
		TJ = T0 + TAA;
		pRTCC->PLEFEM(1, TJ, 0, &R_EM, &V_EM, &R_ES, NULL);
		RMAP = R_EM / (KMPER*1000.0);
		theta = PI - eta - beta_a;
		RAPOG = (V_a*cos(theta) / v_a + (V_a*cos(beta_a) / v_a - R_a / r_a)*sin(theta) / sin(beta_a))*R_p;
		R_apo = RMAP - RAPOG;
		ZIT = sign(dotp(crossp(RAPOG, RMAP), HANG));
		RCAPAP = length(R_apo);
		FK = r_a * (dotp(R_a, R) / pow(r_cap, 3) - dotp(R_a, R_Moon) / pow(r_m, 3));
		FKB = 1.0 - ((R_p*R_p + R_p * r_a + r_a * r_a) / (3.0*r_m_sq) - 2.0*r_m / r_cap + r_m_sq / (r_cap*RCAPAP));
	
		deltat = -0.012299896*(3.0 / sqrt(mu*p))*e*a*a*sqrt(1.0 - cos_eta * cos_eta)*(2.0*FKB - FK);
		deltat1 = 0.012299896*(sqrt(pow(a, 3) / mu)*acos((1.0 - r_a / a) / e))*(FK*(1.0 - 3.0*a / r_a) + 3.0*a*(1.0 / r_m - 1.0 / r_cap + r_a / r_m_sq));
		cos_PV = dotp(R, R_a) / r_cap / r_a;
		deltaT = cos_PV * (deltat + deltat1);
	}

	if (Z2 < 0)
	{
		if (r_a < 35.6)
		{
			Tr = 22.0 + 1.31*(35.6 - r_a);
		}
		else if (r_a < 41.7)
		{
			Tr = 14.0 + 1.31*(41.7 - r_a);
		}
		else
		{
			Tr = 1.555*(50.7 - r_a);
		}

		deltaT = -1.377229 + 0.093*(T_ar - Tr) - 0.00157*pow(T_ar - Tr, 2);
		T_arm = 59.0 - 1.456*(r_a - 30.0);
		if (T_ar <= T_arm)
		{
			T_arm2 = T_arm - 8.0;
			deltaT = -0.027778*(T_ar - T_arm2); //Set to zero in new version?
			if (T_ar < T_arm2)
			{
				deltaT = 0.0;
			}
		}
		if (e > 0.989)
		{
			deltaT = 0.0;
		}
	}
	Z3 = p * cos_eta / (e*e*sin_eta);
	TERM = 2.0*r_a / cos(beta_a) + r_a * sin(beta_a)*Z3*(1.0 / a - v_a * v_a / mu);
	if (abs(sin_eta) < 1e-3)
	{
		TERM = 0.0;
	}
	dt_dbetaa = r_a * r_a*v_a*sin(beta_a)*cos_eta / (e*mu);
	dt_dva = a / mu * (3.0*v_a*(T_ar - deltaT) + TERM);

	dbetar_dbetaa = tan(beta_r) / tan(beta_a);
	dbetar_dva = tan(beta_r) / v_a;
	ALVA = (1.9624867 - 1.0 / a)*mu;
	dbetar_dva = dbetar_dva * (1.0 - v_a * v_a / ALVA);
	D = dt_dva * dbetar_dbetaa - dt_dbetaa * dbetar_dva;
	dv = deltaT / D * dbetar_dbetaa;
	dbeta = (-deltaT / D)*dbetar_dva;
	T_art = 1.4272*r_a + 0.01784*r_a*r_a;

	if (r_a >= 2.5 && T_ar >= T_art)
	{
		deltaT = deltaT / cos_PV - deltat1;
		dw = deltaT * sqrt(mu*p) / (3.0*a*a*e*e);
		eta = atan2(sin_eta, cos_eta) - dw;
		DUM = a / r_a;
		DUM2 = cos_eta * cos_eta - 4.0*(DUM - DUM * DUM);
		if (DUM2 < 0)
		{
			ES = sqrt(r_a / a - 1.0);
		}
		else
		{
			ES = (-cos_eta + sqrt(DUM2)) / (2.0*DUM);
			ESS = (-cos_eta - sqrt(DUM2)) / (2.0*DUM);
			if (abs(ESS - e) < abs(ES - e))
			{
				ES = ESS;
			}
		}
		PS = a * (1.0 - ES * ES);
		h = sqrt(mu*PS);
		Sbeta_s = h / (r_a*v_a);
		beta_s = asin(Sbeta_s);
		DLBET = -abs(beta_s - beta_a)*ZIT;
		dbeta = dbeta + DLBET;
	}
	CDB = cos(dbeta) / v_a;
	Z1 = sin(dbeta) / sin(beta_a);
	Z2 = cos(beta_a) / v_a;
	v_a = v_a + dv;
	V_a2 = (V_a*CDB + (V_a*Z2 - R_a / r_a)*Z1)*v_a;
}

double ConicRTEEarthNew::TripTime(double v_a, double beta_a)
{
	VECTOR3 V_a;
	double VT_a, VR_a, E, A, P, e, T_ap, T_rp, Period, T;
	int k;

	VT_a = v_a * sin(beta_a);
	VR_a = v_a * cos(beta_a);
	V_a = R0 * VR_a + R2 * VT_a*cos(theta_0) + R1 * VT_a*sin(theta_0);

	E = pow(v_a, 2) / mu - 2.0 / r0;
	if (abs(E) - 0.0001 > 0)
	{
		A = -1.0 / E;
		P = pow(r0*v_a*sin(beta_a), 2) / mu;
		e = sqrt(1.0 - P / A);
	}
	else
	{
		A = pow(r0*v_a*sin(beta_a), 2) / mu;
		e = 1.0;
	}

	if (beta_a > PI05)
	{
		k = 0;
	}
	else
	{
		k = 1;
	}

	pRTCC->PITFPC(mu, k, A, e, r0, T_ap, Period);
	pRTCC->PITFPC(mu, k, A, e, RR, T_rp, Period);
	T = T_ap - T_rp;
	if (T < 0)
	{
		T += Period;
	}
	return T;
}

RTEMoon::RTEMoon(RTCC *r, EphemerisData2 sv0, double GMTBASE, double alpha_SID0) : RTCCModule(r)
{
	this->GMTBASE = GMTBASE;
	this->alpha_SID0 = alpha_SID0;
	this->sv0 = sv0;

	hMoon = BODY_MOON;
	mu_E = OrbMech::mu_Earth;
	mu_M = OrbMech::mu_Moon;
	w_E = OrbMech::w_Earth;
	R_E = OrbMech::R_Earth;
	R_M = OrbMech::R_Moon;
	r_r = R_E + 400000.0*0.3048;

	dTIG = 30.0;
	precision = 1;
	ReturnInclination = 0.0;
	EntryLng = 0.0;
	t_Landing = 0.0;
}

void RTEMoon::READ(int SMODEI, double IRMAXI, double URMAXI, double RRBI, int CIRI, double HMINI, int EPI, double L2DI, double DVMAXI, double MUZI, double IRKI, double MDMAXI, double TZMINI, double TZMAXI)
{
	double LETSGOF, CRITF;

	u_rmax = URMAXI * 0.3048;
	i_rmax = IRMAXI * RAD;
	CENT = 1;
	t_zmax = TZMAXI;
	h_min = HMINI * 1852.0;
	t_zmin = TZMINI;
	MD_max = MDMAXI;
	SMODE = SMODEI;
	CIRCUM = CIRI;
	LD = L2DI;
	DV_max = DVMAXI * 0.3048;
	lambda_z = 0.0;
	mu_z = MUZI;
	lambda_z1 = lambda_z;
	mu_z1 = mu_z;
	r_rbias = RRBI;
	ICRNGG = EPI;

	CRITF = modf(((double)SMODEI) / 10.0, &LETSGOF);
	LETSGO = (int)LETSGOF;
	CRIT = SMODE - LETSGO * 10;
	i_rk = IRKI;

	if (LETSGO == 2)
	{
		LFLAG = 1;
	}
	else
	{
		LFLAG = 2;
	}
	bRTCC = false;
	if (CIRI == 0 && CRIT != 6)
	{
		//If postmaneuver direction of motion is to be determined internally and mode is fuel critical, unspecified area
		bRTCC = true;
	}

	if (SMODE == 12 || SMODE == 14 || SMODE == 32 || SMODE == 34)
	{
		t_zmax = t_zmin + 12.0*3600.0;
		t_zmin = t_zmin - 12.0*3600.0;
	}
}

void RTEMoon::ATP(double *line)
{
	for (int i = 0;i < 10;i++)
	{
		LINE[i] = line[i];
	}
}

bool RTEMoon::MASTER()
{
	VECTOR3 DVARR, TIGARR;
	double i_r, theta_long, theta_lat, dlng, dt, INTER, dv, t_z;
	double r_0, v_0, delta_0, alpha_0, beta_0, A_0, z_0, e_0;
	int IPART, ii;
	bool ISOL, IOUT, q_0;
	//0 = noncircumlunar, 1 = circumlunar
	bool q_m;

	IOUT = false;
	IPART = 1;
	ii = 0;

	while (IOUT == false)
	{
		pRTCC->PICSSC(true, sv0.R, sv0.V, r_0, v_0, delta_0, alpha_0, beta_0, A_0);
		beta_0 = PI05 - beta_0;

		q_0 = false;
		z_0 = sv0.R.x*sv0.V.y - sv0.R.y*sv0.V.x;
		//Is motion prograde?
		if (z_0 >= 0)
		{
			q_0 = true;
		}
		e_0 = sqrt(1.0 - r_0 * v_0*v_0 / mu_M * (2.0 - r_0 * v_0*v_0 / mu_M)*pow(sin(beta_0), 2));

		if (pRTCC->PLEFEM(4, sv0.GMT / 3600.0, 0, &X_m0, &U_m0, NULL, NULL))
		{
			//TBD: Error
		}

		//Normally the pseudostate sphere is 24 Earth radii. Probably doesn't iterate very well if the spacecraft is close to that, so use a slightly larger radius then
		if (length(sv0.R) >= 23.0*R_E)
		{
			r_s = length(sv0.R) + R_E;
		}
		else
		{
			r_s = 24.0*R_E;
		}

		//Direction of motion logic
		QDFLG = false;
		if (bRTCC)
		{
			double EAI;

			//Is position on transearth side of earth-moon line?
			EAI = sv0.R.x*X_m0.y - sv0.R.y*X_m0.x;
			if (EAI >= 0 || q_0)
			{
				//Yes, only allow non-circumlunar motion
				CIRCUM = 1;
			}
			//Is trajectory elliptical?
			else if (e_0 < 1.0)
			{
				//Yes, only allow retrograde motion with respect to the Moon
				CIRCUM = 1;
				QDFLG = true;
			}
		}

		if (CRIT == 4)
		{
			ISOL = CLL(i_r, INTER, q_m, t_z, dv);
		}
		else
		{
			ISOL = MCUA(i_r, INTER, q_m, t_z, dv);
		}

		if (LETSGO == 1) break;

		dTIG = SEARCH(IPART, DVARR, TIGARR, sv0.GMT, dv, IOUT);

		if (IOUT == false)
		{
			OrbMech::oneclickcoast(pRTCC->SystemParameters.AGCEpoch, sv0.R, sv0.V, GMTBASE + sv0.GMT / 24.0 / 3600.0, dTIG, sv0.R, sv0.V, hMoon, hMoon);
			sv0.GMT += dTIG;
		}
	}

	if (ISOL == false) return false;

	//Precision Solution
	t_R = t_z;
	do
	{
		Vig_apo = ThreeBodyAbort(sv0.R, sv0.V, sv0.GMT, t_R, q_m, i_r, INTER, R_EI, V_EI);

		EntryCalculations::LNDING(R_EI, V_EI, t_R, alpha_SID0, LD, ICRNGG, r_rbias, theta_long, theta_lat, t_Landing);
		if (CRIT == 6) break;

		 EntryCalculations::TBLOOK(LINE, theta_lat, EntryLng);

		dlng = theta_long - EntryLng;
		if (abs(dlng) > PI)
		{
			dlng = dlng - OrbMech::sign(dlng)*PI2;
		}

		dt = dlng / w_E;
		t_R += dt;

		ii++;
	} while (abs(dt) > 0.2);

	// Final Calculations
	double sing, cosg, x2;
	VECTOR3 i, j, k, N, H_EI_equ, R_peri, V_peri;
	MATRIX3 Q_Xx;
	j = unit(crossp(sv0.V, sv0.R));
	k = unit(-sv0.R);
	i = crossp(j, k);
	Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

	DV = Vig_apo - sv0.V;
	Entry_DV = mul(Q_Xx, DV);
	EntryLatcor = theta_lat;
	EntryLngcor = theta_long;
	N = crossp(unit(R_EI), unit(V_EI));
	sing = length(N);
	cosg = dotp(unit(R_EI), unit(V_EI));
	x2 = cosg / sing;
	EntryAng = atan(x2);

	H_EI_equ = unit(N);
	ReturnInclination = -acos(H_EI_equ.z)*INTER;

	OrbMech::timetoperi_integ(pRTCC->SystemParameters.AGCEpoch, sv0.R, Vig_apo, OrbMech::MJDfromGET(sv0.GMT, GMTBASE), pRTCC->GetGravref(hMoon), pRTCC->GetGravref(hMoon), R_peri, V_peri);
	FlybyPeriAlt = length(R_peri) - R_M;

	return true;
}

void RTEMoon::MCSS()
{
	double lambda_z0, mu_z0, eps_SS, t_t, Dmu_z, u_r, T_xr, MD_maxs;
	int k_xx;
	bool PNT, REP, FRZ, KIP;

	lambda_z0 = lambda_z;
	mu_z0 = mu_z;
	PNT = 0;
	REP = 0;
	eps_SS = 0.002;
	t_t = pow(10, 10);
	Dmu_z = pow(10, 9);
	k_xx = 0;
	FRZ = 0;

	//B
	u_r = u_rmax;
	T_xr = 0.0;
	MD_maxs = MD_max;
	MD_max = 0.0;
	KIP = 0;
}

void RTEMoon::MCSSLM(bool &REP, double t_z_apo)
{
	VECTOR3 UZTAB1, LAMZTAB1, Vig_apo;
	double DV_maxs, h_mins, t_zmin_apo, Di_r, T_ar, i_r, INTER, i_r_apo, u_r, indvar, r_p, t_z, t_z1_apo, t_z1_aapo, mu_min, mu_max, GMT_L;
	int KK, XNRMSS, XX, n2, n1;
	bool MCSOL, SRFLG, STAYFL, REPP, NIR, IREP, KIP, q_d;

	if (MD_max == 0.0)
	{
		if (lambda_z < 0)
		{
			lambda_z += PI2;
		}
		MCSOL = 0;
		DV_maxs = DV_max;
		h_mins = h_min;
		SRFLG = 0;
		h_min = -0.3*R_E;
		DV_max = 100.0*20925738.2 / 3600.0;
		STAYFL = 0;
		t_zmin_apo = t_zmin;
		KK = 1;
		Di_r = 0.1745329;
		XNRMSS = -1;
		REPP = REP;
		T_ar = 0.0;
		XX = 0;

		//B
		NIR = 0;
		IREP = REP;
		n2 = 0;
		i_r = i_rmax;
		INTER = 1.0;
		i_r_apo = i_rmax;
		n1 = 0;

		//C
		if (t_z_apo < t_zmin)
		{
			KIP = 0;
			u_r = u_rmax;
		}

		if (KIP)
		{
			indvar = t_z_apo;
		}
		else
		{
			indvar = u_r;
		}
		REP = MCDRIV(sv0.R, sv0.V, sv0.GMT, indvar, false, i_r, INTER, KIP, t_zmin, Vig_apo, R_EI, V_EI, t_z, NIR, i_r_apo, r_p, q_d); //TBD
		EntryCalculations::LNDING(R_EI, V_EI, t_z, alpha_SID0, LD, ICRNGG, r_rbias, lambda_z1, mu_z1, GMT_L);

		//A
		UZTAB1.x = mu_z1;
		LAMZTAB1.x = lambda_z1;
		t_z1_apo = t_z;

		if (MD_max == 0.0)
		{
			INTER = 1.0;
			t_z_apo = t_z;
			if (KIP == 0)
			{
				t_zmin = t_z;
			}
			KIP = 1;

			//E
			mu_min = 1000.0;
			mu_max = 1000.0;
			KIP = 0;

			//F
			IREP = REP;
			if (KIP)
			{
				indvar = t_z_apo;
			}
			else
			{
				indvar = u_r;
			}
			REP = MCDRIV(sv0.R, sv0.V, sv0.GMT, indvar, false, i_r, INTER, KIP, t_zmin, Vig_apo, R_EI, V_EI, t_z, NIR, i_r_apo, r_p, q_d); //TBD
			EntryCalculations::LNDING(R_EI, V_EI, t_z, alpha_SID0, LD, ICRNGG, r_rbias, lambda_z1, mu_z1, GMT_L);

			UZTAB1.y = mu_z1;
			LAMZTAB1.y = lambda_z1;
			t_z1_aapo = t_z;
			INTER = 1.0;
		}
	}
}

bool RTEMoon::CLL(double &i_r, double &INTER, bool &q_m, double &t_z, double &dv)
{
	VECTOR3 IRTAB, DVTAB, ZTAB, Vig_apo;
	double theta_long, theta_lat, dlng, dt, i_r_apo, TOL, i_rmin, DV_min, delta_S, i_rc, h_p, r_p, D1, D2, DVSS, i_rs, INS, u_r;
	double t_z1, t_z_apo, indvar, eps, GMT_L, h_mins;
	int ISUB, KOUNT, ICNT, jj, ICONVG, ii, ITCNT, LOPCNT, REP;
	bool NIR, IOPT, NIRS, KIP, KFLG, q_d;

	q_m = false;
	h_mins = h_min;
	DV_min = pow(10, 10);
	ZTAB = _V(0, 0, 0);
	KFLG = false;
	solution.dv = pow(10, 10);
	t_z_apo = 0.0;

	if (CIRCUM == 0)
	{
		KFLG = true;
	}
	else if (CIRCUM == 2)
	{
	RTEMoon_CLL_1_AA:
		q_m = true;
		KFLG = false;
	}

	KIP = 0;
	u_r = u_rmax;
	INTER = 1.0;
	i_r = 0.5235988;
	eps = 0.05;
	KOUNT = 0;
	ITCNT = 0;

	if (i_rk != 0)
	{
		i_r = abs(i_rk);
		i_rmax = i_r;
		eps = 0.005;
		INTER = -i_rk / i_r;
	}

	//Minimum return time without further constraints
	if (KIP)
	{
		indvar = t_z_apo;
	}
	else
	{
		indvar = u_r;
	}
	REP = MCDRIV(sv0.R, sv0.V, sv0.GMT, indvar, q_m, i_r, INTER, KIP, t_zmin, Vig_apo, R_EI, V_EI, t_z, NIR, i_r_apo, r_p, q_d);
	h_p = r_p - R_M;
	EntryCalculations::LNDING(R_EI, V_EI, t_z, alpha_SID0, LD, ICRNGG, r_rbias, theta_long, theta_lat, GMT_L);

	t_z1 = t_z;
	KOUNT = 0;
	LOPCNT = 0;

	//This loop roughly converges on the desired landing site
	do
	{
		EntryCalculations::TBLOOK(LINE, theta_lat, EntryLng);
		dlng = theta_long - EntryLng;
		if (ITCNT > 0 && abs(dlng) < eps)
		{
			break;
		}
		ITCNT++;
		if (ITCNT > 10)
		{
			t_z_apo += 24.0*3600.0;
			ITCNT = 0;
		}
		if (dlng > PI) dlng = dlng - PI2;
		else if (dlng < -PI) dlng = PI2 + dlng;
		if (KIP == 0)
		{
			t_z_apo = t_z;
		}
		t_z_apo += dlng / w_E;
		if (t_z_apo < t_z1)
		{
			t_z_apo += 24.0*3600.0;
			ITCNT = 0;
		}
		if (t_z_apo > t_zmax)
		{
			return false;
		}
		LOPCNT = 0;
		KIP = 1;

		if (KIP)
		{
			indvar = t_z_apo;
		}
		else
		{
			indvar = u_r;
		}
		REP = MCDRIV(sv0.R, sv0.V, sv0.GMT, indvar, q_m, i_r, INTER, KIP, t_zmin, Vig_apo, R_EI, V_EI, t_z, NIR, i_r_apo, r_p, q_d);
		dv = length(Vig_apo - sv0.V);
		EntryCalculations::LNDING(R_EI, V_EI, t_z, alpha_SID0, LD, ICRNGG, r_rbias, theta_long, theta_lat, GMT_L);
		LOPCNT = 0;
	} while (ITCNT <= 0 || abs(dlng) >= eps);

	i_rmin = 10.0;

	//This checks if the inclination was specified
	if (eps != 0.05)
	{
		goto RTEMoon_CLL_17_B;
	}

	TOL = 0.01745;
	ISUB = 0;
	DVSS = pow(10, 10);
	jj = 0;
	INTER = -1.0;
	i_r = i_rmax;
	ICNT = 0;
	KOUNT = 0;
	IOPT = true;

	//Main iteration loop for the DV optimization
	while (KOUNT <= 10)
	{
		ii = 0;
		do
		{
			REP = MCDRIV(sv0.R, sv0.V, sv0.GMT, t_z_apo, q_m, i_r, INTER, true, t_zmin, Vig_apo, R_EI, V_EI, t_z, NIR, i_r_apo, r_p, q_d);
			EntryCalculations::LNDING(R_EI, V_EI, t_z, alpha_SID0, LD, ICRNGG, r_rbias, theta_long, theta_lat, GMT_L);
			EntryCalculations::TBLOOK(LINE, theta_lat, EntryLng);

			dlng = theta_long - EntryLng;
			if (abs(dlng) > PI)
			{
				dlng = dlng - OrbMech::sign(dlng)*PI2;
			}

			dt = dlng / w_E;
			t_z_apo += dt;
			ii++;
		} while (abs(dt) > 1.0);

		h_p = r_p - R_M;
		dv = length(Vig_apo - sv0.V);
		if (q_d && h_p < h_min)
		{
			dv = pow(10, 10);
		}

		if (NIR && IOPT)
		{
			delta_S = i_r_apo;
		}

		if (dv <= DVSS)
		{
			DVSS = dv;
			i_rs = i_r_apo;
			INS = INTER;
			NIRS = NIR;
		}

		if (IOPT)
		{
			IRTAB.data[jj] = i_r_apo * INTER;
			DVTAB.data[jj] = dv;

			if (jj == 0)
			{
				INTER = 1.0;
				i_r = 0.0;
				DV_min = 1e-8;
				ICONVG = 0;
			}
			else if (jj == 1)
			{
				i_r = i_rmax;
			}
			else
			{
				IRTAB.data[0] = (abs(IRTAB.data[0]) - delta_S)*IRTAB.data[0] / abs(IRTAB.data[0]);
				IRTAB.data[1] = (abs(IRTAB.data[1]) - delta_S)*IRTAB.data[1] / abs(IRTAB.data[1]);
				IRTAB.data[2] = (abs(IRTAB.data[2]) - delta_S)*IRTAB.data[2] / abs(IRTAB.data[2]);
			}
			jj++;
		}
		else
		{
			i_rc = (i_r_apo - delta_S)*INTER;
		}
		if (jj < 3) continue;

		if (i_rk == 0)
		{
			if (abs(dv - DV_min) <= 0.1772) //0.0001 er/hr
			{
				ICONVG++;
				if (ICONVG != 1)
				{
					break;
				}
			}
			if ((KOUNT >= 10 || abs(i_rmin) < 0.25*RAD) || (ISUB != 0 && (dv - DVTAB.data[ISUB - 1]) >= 0.0))
			{
				break;
			}

			ISUB = EntryCalculations::MINMIZ(IRTAB, DVTAB, ZTAB, IOPT, _V(i_rc, dv, 0.0), TOL, i_rmin, DV_min);
			IOPT = false;
			KOUNT++;
			D1 = 5.0*IRTAB.y - 4.0*IRTAB.x;
			D2 = 5.0*IRTAB.y - 4.0*IRTAB.z;

			if (ISUB == 0)
			{
				if ((IRTAB.x <= i_rmin) && (i_rmin <= IRTAB.y) && (DVTAB.z > pow(10, 8)))
				{
					i_rmin = (IRTAB.y + IRTAB.z) / 2.0;
				}
				else if ((IRTAB.y <= i_rmin) && (i_rmin <= IRTAB.z) && (DVTAB.x > pow(10, 8)))
				{
					i_rmin = (IRTAB.x + IRTAB.y) / 2.0;
				}
				else if (D1 < IRTAB.z)
				{
					i_rmin = (IRTAB.y + IRTAB.z) / 2.0;
				}
				else if (D2 >= IRTAB.x)
				{
					i_rmin = (IRTAB.x + IRTAB.y) / 2.0;
				}
			}

			INTER = i_rmin / abs(i_rmin);
			i_r = abs(i_rmin + INTER * delta_S);
		}
	}

	//Restore best solution
	i_r_apo = i_rs;
	INTER = INS;
	i_r = i_r_apo;
	dv = DVSS;

RTEMoon_CLL_17_B:
	//Store if DV is smaller than stored solution
	if (dv < solution.dv)
	{
		STORE(1, dv, i_r, INTER, t_z, q_m);
	}
	if (KFLG)
	{
		//Other direction of motion still to do?
		goto RTEMoon_CLL_1_AA;
	}

	bool ISOL = false;

	if (solution.dv < DV_max)
	{
		ISOL = true;
		STORE(2, dv, i_r, INTER, t_z, q_m);
	}

	return ISOL;
}

bool RTEMoon::MCUA(double &i_r, double &INTER, bool &q_m, double &t_z, double &dv)
{
	VECTOR3 IRTAB, DVTAB, ZTAB, DVTAB1, IRTAB1, TZTAB1, Vig_apo;
	double u_r, r_p, di_r, i_rmin, Di_r, Dt_z, TOL, zc, DV_est1, DV_est2, D1, D2, indvar, i_rmax_apo, SDV, Si_r, SSDV, SSi_r, t_z_apo, eps_ir;
	double SSt_z, DVSSS, i_rest, i_rmins, i_rmaxs, Xi_r, t_zmin_apo, eps_dv;
	int LOOP, LOCATE, ISUB, LOOPTZ, ISUBP, REP;
	bool NIR, KIP, IOPT, IEND, IOPT1, IRFLAG, IRSCAN, ISOL, q_d;

	zc = 0.0;

	solution.dv = pow(10, 10);
	i_rmax_apo = i_rmax;
	t_zmin_apo = t_zmin;
	eps_dv = 0.1772; //0.0001 er/hr
	if (CIRCUM < 2)
	{
		q_m = false;
	}
	else
	{
		q_m = true;
	}
RTEMoon_MCUA_1_A:
	i_r = 0.0;
	LOOP = LOOPTZ = 0;
	di_r = 0.0;
	u_r = u_rmax;
	eps_ir = 0.01;
	DV_est1 = DV_est2 = pow(10, 10);
	SDV = SSDV = pow(10, 10);
	KIP = false;
	LOCATE = 5;
	IRSCAN = 0;

	do
	{
		do
		{
			if (i_r != 0)
			{
				INTER = i_r / abs(i_r);
			}
			else
			{
				INTER = 1.0;
			}
			i_r = abs(i_r) + di_r;

			if (KIP)
			{
				indvar = t_z_apo;
			}
			else
			{
				indvar = u_r;
			}
			REP = MCDRIV(sv0.R, sv0.V, sv0.GMT, indvar, q_m, i_r, INTER, KIP, t_zmin, Vig_apo, R_EI, V_EI, t_z, NIR, Xi_r, r_p, q_d);
			if (REP == 0)
			{
				dv = pow(10, 10);
			}
			else
			{
				dv = length(Vig_apo - sv0.V);
			}
			i_r = (i_r - di_r)*INTER;

			if (dv <= SDV)
			{
				Si_r = i_r;
				SDV = dv;
			}

			if (dv <= SSDV)
			{
				SSDV = dv;
				SSi_r = i_r;
				SSt_z = t_z;
			}

			LOOP++;

			if (LOCATE == 5)
			{
				t_zmin = t_z;
				di_r = Xi_r;
				i_rmax = i_rmax - di_r;
				i_rmin = -i_rmax;
				LOCATE = 1;
				t_z_apo = t_zmin;
				i_r = i_rmin;
				LOOP = 0;
				LOOPTZ = 0;
				SDV = pow(10, 10);
				SSDV = pow(10, 10);
				KIP = true;
				Di_r = (i_rmax - i_rmin) / 2.0;
				Dt_z = (t_zmax - t_zmin) / 2.0;
				if (t_zmin > t_zmax)
				{
					break;
				}
				continue;
			}
			else if (LOCATE == 3)
			{
				break;
			}
			else if (LOCATE < 3)
			{
				if (LOOP <= 3)
				{
					IOPT = 1;
					ISUB = 0;
					IRTAB.data[LOOP - 1] = i_r;
					DVTAB.data[LOOP - 1] = dv;
					if (LOOP != 3)
					{
						i_r += Di_r;
						continue;
					}
				}
				TOL = min(0.017, 0.5*Di_r);
				if (LOOP > 20 || abs(i_r) < 0.001 || abs(dv - DV_est1) < eps_dv || (ISUB != 0 && dv > DVTAB.data[ISUB - 1]))
				{
					break;
				}
				else
				{
					ISUB = EntryCalculations::MINMIZ(IRTAB, DVTAB, ZTAB, IOPT, _V(i_r, dv, zc), TOL, i_r, DV_est1);
					D1 = 5.0*IRTAB.y - 4.0*IRTAB.x;
					D2 = 5.0*IRTAB.y - 4.0*IRTAB.z;
					IOPT = 0;
					if (ISUB == 0)
					{
						if (D1 < IRTAB.z)
						{
							i_r = (IRTAB.y + IRTAB.z) / 2.0;
						}
						if (D2 >= IRTAB.x)
						{
							i_r = (IRTAB.x + IRTAB.y) / 2.0;
						}
					}
					continue;
				}
			}

		} while (LOOP <= 20);

		if (t_zmin > t_zmax)
		{
			break;
		}

		LOOPTZ++;
		if (LOOPTZ <= 3)
		{
			IEND = 0;
			i_r = i_rmin;
			DVTAB1.data[LOOPTZ - 1] = SDV;
			TZTAB1.data[LOOPTZ - 1] = t_z_apo;
			IRTAB1.data[LOOPTZ - 1] = Si_r;
			t_z_apo += Dt_z;
			ISUBP = 0;
			LOOP = 0;
			LOCATE = 1;
			IOPT1 = 1;
			if (LOOPTZ != 3)
			{
				SDV = pow(10, 10);
				continue;
			}
		}
		IRFLAG = false;
		if (!(abs(IRTAB1.x - IRTAB1.z) >= eps_ir || abs(IRTAB1.x - IRTAB1.y) >= eps_ir))
		{
			LOCATE = 3;
			IRFLAG = true;
		}
		dv = SDV;
		i_r = Si_r;
		if (LOOPTZ > 20 || (ISUBP != 0 && SDV > DVTAB1.data[ISUBP - 1]) || abs(dv - DV_est2) < eps_dv)
		{
			i_r = SSi_r;
			t_z_apo = SSt_z;
			LOOP = 0;
			DVSSS = pow(10, 10);
			LOOPTZ = 0;
			break;
		}

		if (IRFLAG == true)
		{
			i_rest = IRTAB1.x;
		}

		ISUBP = EntryCalculations::MINMIZ(TZTAB1, DVTAB1, IRTAB1, IOPT1, _V(t_z, dv, i_r), 3600.0, t_z_apo, DV_est2);
		IOPT1 = 0;
		if (IRSCAN == 1)
		{
			IRSCAN = 0;
			i_rmin = i_rmins;
			i_rmax = i_rmaxs;
		}
		if (ISUBP == 0)
		{
			D1 = 5.0*TZTAB1.y - 4.0*TZTAB1.x;
			D2 = 5.0*TZTAB1.y - 4.0*TZTAB1.z;
			if (D1 < TZTAB1.z)
			{
				t_z_apo = (TZTAB1.y + TZTAB1.z) / 2.0;
			}
			if (D2 >= TZTAB1.x)
			{
				t_z_apo = (TZTAB1.x + TZTAB1.y) / 2.0;
			}
		}
		else
		{
			i_rmins = i_rmin;
			i_rmaxs = i_rmax;
			Di_r = 0.25*i_rmax;
			if (Di_r > 0.17453293)
			{
				IRSCAN = 1;
				i_rmin = max(i_rmins, SSi_r);
				i_rmax = min(i_rmax, i_rmin + 2.0*Di_r);
			}
		}

		i_r = i_rmin;
		SDV = pow(10, 10);
		LOOP = 0;
		Di_r = (i_rmax - i_rmin) / 2.0;
		if (LOCATE == 3)
		{
			i_r = i_rest;
		}
	} while (LOOPTZ <= 20);

	if (SSDV <= pow(10, 9))
	{
		if (i_r != 0)
		{
			INTER = i_r / abs(i_r);
		}
		else
		{
			INTER = 1.0;
		}
		i_r = abs(i_r) + di_r;
		REP = MCDRIV(sv0.R, sv0.V, sv0.GMT, t_z_apo, q_m, i_r, INTER, KIP, t_zmin, Vig_apo, R_EI, V_EI, t_z, NIR, Xi_r, r_p, q_d);
		dv = length(Vig_apo - sv0.V);
		LOOP++;

		//Store solution
		if (dv < solution.dv)
		{
			STORE(1, dv, i_r, INTER, t_z, q_m);
		}
	}

	i_rmax = i_rmax_apo;
	t_zmin = t_zmin_apo;
	ISOL = false;

	if (CIRCUM == 0 && q_m == false)
	{
		q_m = true;
		goto RTEMoon_MCUA_1_A;
	}

	if (solution.dv <= pow(10, 9))
	{
		//Restore optimum solution
		STORE(2, dv, i_r, INTER, t_z, q_m);
		ISOL = true;
	}

	return ISOL;
}

VECTOR3 RTEMoon::ThreeBodyAbort(VECTOR3 R_I, VECTOR3 V_I, double t_I, double t_EI, bool q_m, double Incl, double INTER, VECTOR3 &R_EI, VECTOR3 &V_EI)
{
	//INPUT:
	//q_m: 0 = noncircumlunar, 1 = circumlunar
	//QDFLG: 0 = use circumlunar or noncircumlunar, 1 = use retrograde motion

	EphemerisData sv1, sv2;
	VECTOR3 R_I_star, delta_I_star, delta_I_star_dot, R_I_sstar, V_I_sstar, V_I_star, R_S, R_I_star_apo, R_E_apo, V_E_apo, V_I_apo;
	VECTOR3 R_m, V_m;
	double t_S, tol, dt_S, Incl_apo, r_s, a, e, u_r, beta_r, beta_r_apo, t_z, a_H, e_H, p_H, theta, beta_a, beta_x;
	int ITS, INFO;
	bool q_m_out, q_d, q_a, NIR;

	tol = 20.0;

	//Normally the pseudostate sphere is 24 Earth radii. Probably doesn't iterate very well if the spacecraft is close to that, so use a slightly larger radius then
	if (length(sv0.R) >= 23.0*R_E)
	{
		r_s = length(sv0.R) + R_E;
	}
	else
	{
		r_s = 24.0*R_E;
	}

	pRTCC->PLEFEM(4, t_I / 3600.0, 0, &R_m, &V_m, NULL, NULL);

	R_I_star = delta_I_star = delta_I_star_dot = _V(0.0, 0.0, 0.0);
	V_I_star = V_I;

	sv1.R = R_I;
	sv1.GMT = t_I;
	sv1.RBI = BODY_MOON;

	do
	{
		do
		{
			R_I_sstar = R_m + R_I_star + delta_I_star;
			V_I_sstar = V_m + V_I_star + delta_I_star_dot;

			beta_r = 96.5*RAD;

			do
			{
				EntryCalculations::AESR(r_r, length(R_I_sstar), PI - beta_r, t_EI - t_I, R_E, mu_E, 0.001, a, e, q_a, INFO, u_r);
				NIR = FINDUX(R_I_sstar, t_I, r_r, u_r, beta_r, Incl, INTER, q_a, mu_E, V_I_sstar, R_EI, V_EI, t_z, Incl_apo);

				beta_r_apo = beta_r;
				beta_r = EntryCalculations::ReentryTargetLine(u_r, ICRNGG == 1);

			} while (abs(beta_r - beta_r_apo) > 0.0002);

			V_I_star = V_I_sstar - V_m - delta_I_star_dot;

			INRFV(R_I, V_I_star, r_s, mu_M, q_m, a_H, e_H, p_H, theta, V_I_apo, R_S, dt_S, q_m_out, q_d, beta_a, beta_x);
			t_S = t_I + dt_S;
			R_I_star_apo = R_I_star;
			R_I_star = R_S + V_I_star * (t_I - t_S);

		} while (length(R_I_star - R_I_star_apo) > tol);

		sv1.V = V_I_apo;
		pRTCC->PMMCEN(sv1, 0.0, 0.0, 1, t_EI - t_I, 1.0, sv2, ITS);
		if (sv2.RBI != BODY_EARTH)
		{
			//Error
		}
		R_E_apo = sv2.R;
		V_E_apo = sv2.V;
		//OrbMech::oneclickcoast(R_I, V_I_apo, t_I, (t_EI - t_I), R_E_apo, V_E_apo, hMoon, hEarth);
		OrbMech::rv_from_r0v0(R_E_apo, V_E_apo, (t_I - t_EI), R_I_sstar, V_I_sstar, mu_E);
		delta_I_star = R_I_sstar - R_m - R_I_star;
		delta_I_star_dot = V_I_sstar - V_m - V_I_star;

	} while (length(R_EI - R_E_apo) > tol);

	return V_I_apo;
}

int RTEMoon::MCDRIV(VECTOR3 Y_0, VECTOR3 V_0, double t_0, double var, bool q_m, double Incl, double INTER, bool KIP, double t_zmin, VECTOR3 &V_a, VECTOR3 &R_EI, VECTOR3 &V_EI, double &T_EI, bool &NIR, double &Incl_apo, double &y_p, bool &q_d)
{
	//INPUT:
	//Y_0: Selenocentric preabort position vector
	//V_0: Selenocentric preabort velocity vector
	//t_0: Preabort time
	//var: either reentry velocity (KIP=false) or landing time (KIP=true)
	//q_m: 0 = noncircumlunar motion postabort, 1 = circumlunar motion postabort
	//Incl: Return inclination
	//INTER: -1 = A_zr < reentry azimuth for i_r = delta_x, +1 = A_zr > reentry azimuth for i_r = delta_x
	//KIP: 0 = reentry velocity is the independent variable, 1 = landing time is the independent variable
	//t_zmin: minimum landing time allowed
	//OUTPUT:
	//REP: 0 = no MCDRIV solution has been found, >0 = a solution has been found in MCDRIV
	//V_a: Postabort velocity vector

	//Geocentric transearth pseudostate position vector
	VECTOR3 X_x;
	//Geocentric transearth pseudostate velocity vector
	VECTOR3 U_x;
	//Selenocentric position vector at PTS exit
	VECTOR3 Y_x;
	//Previous value of selenocentric PTS exit position
	VECTOR3 Y_x_apo;
	//Selenocentric velocity vector at PTS exit
	VECTOR3 V_x;
	//Previous value of selenocentric PTS exit velocity
	VECTOR3 V_x_apo;
	//Geocentric position vector of the moon at the postpericynthion pseudostate transformation time
	VECTOR3 X_mx;
	//Geocentric velocity vector of the moon at the postpericynthion pseudostate transformation time
	VECTOR3 U_mx;
	//Fictitious abort position vector
	VECTOR3 Y_a_apo;
	//Selenocentric orbital parameters
	double a_h, e_h, p_h;

	double tol, beta_r, u_r, a, e, beta_r_apo, dy_x, dy_x_apo, v_m, T_EI_apo, T_a, P, T_x, t_x, theta, dt_S, beta_a, beta_x, delta_t, Dy_0, t_x_apo;
	int INFO, KOUNT, k_x;
	bool q_m_out, q_a, dy_xf;

	tol = 63.78165;
	q_a = false;

	if (KIP)
	{
		T_EI_apo = var;
		u_r = EntryCalculations::URF((T_EI_apo - t_0) / 3600.0, length(X_m0) / 6378165.0)*6378165.0 / 3600.0;
	}
	else
	{
		u_r = var;
	}

	beta_r = EntryCalculations::ReentryTargetLine(u_r, ICRNGG == 1);

RTEMoon_MCDRIV_1_X:
	//Initialize iteration counters
	k_x = 1;
	KOUNT = 0;
	//Initial guess for pseudostate. Position at the center of the Moon, velocity using preabort value
	t_x = t_0;
	X_mx = X_m0;
	U_mx = U_m0;
	Y_a_apo = Y_0;
	X_x = X_mx;
	Y_x = X_x - X_mx;
	V_x = V_0; //TBD, not necessary
RTEMoon_MCDRIV_5_S:
	//Main loop
	if (KIP)
	{
		//Compute velocity at reentry, if it is not input
		EntryCalculations::AESR(r_r, length(X_x), PI - beta_r, T_EI_apo - t_x, R_E, mu_E, 0.001, a, e, q_a, INFO, u_r);
	}

	//Compute the geocentric conic (velocity vector) from the geocentric pseudostate
	NIR = FINDUX(X_x, t_x, r_r, u_r, beta_r, Incl, INTER, q_a, mu_E, U_x, R_EI, V_EI, T_EI, Incl_apo);
	//Save current value of beta_r
	beta_r_apo = beta_r;
	//Reset convergence flag
	dy_xf = false;
	//Save previous value of V_x
	V_x_apo = V_x;
	//Transform the abort velocity vector to selenocentric reference
	V_x = U_x - U_mx;
RTEMoon_MCDRIV_7_R:
	//Given initial position Y_a_apo and velocity at PTS V_x, calculate initial velocity V_a and position at PTS Y_x_apo
	INRFV(Y_a_apo, V_x, r_s, mu_M, q_m, a_h, e_h, p_h, theta, V_a, Y_x_apo, dt_S, q_m_out, q_d, beta_a, beta_x);

	//Near parabolic orbit?
	if (abs(e_h - 1.0) < 1e-5)
	{
		a_h = p_h;
	}
	//Calculate time from pericynthion to PTS
	pRTCC->PITFPC(mu_M, 0, a_h, e_h, r_s, T_x, P, false);
	//Calculate pericynthion radius
	y_p = p_h / (1.0 + e_h);
	//Near pericynthion
	if (length(Y_a_apo) < y_p + 1e-6*R_E)
	{
		//Assume time to pericynthion is zero
		T_a = 0.0;
	}
	else
	{
		//Calculate time from pericynthion to abort
		pRTCC->PITFPC(mu_M, q_d, a_h, e_h, length(Y_a_apo), T_a, P, false);
	}

	//Update pseudstate
	PSTATE(a_h, e_h, p_h, t_0, T_x, Y_0, Y_a_apo, V_x, theta, beta_a, beta_x, T_a, V_a, t_x_apo, Y_x_apo, Dy_0, delta_t, X_mx, U_mx);
	//
	dy_x = length(Y_x - Y_x_apo);

	if (dy_xf == false)
	{
		//Has converged?
		if (dy_x <= tol && Dy_0 / length(Y_0) <= tol / 63781650.0 && abs(t_x_apo - t_x) < 0.1)
		{
			//Does solution violate minimum landing time constraint?
			if (KIP == 0 && T_EI < t_zmin)
			{
				KIP = 1;
				T_EI_apo = t_zmin;
				goto RTEMoon_MCDRIV_1_X;
			}
			//Solution has converged
			goto RTEMoon_MCDRIV_11_W;
		}
	}
	k_x++;
	if (k_x >= k_max)
	{
		//Error return
		return 0;
	}

	//Are conics diverging?
	if (k_x % 3 == 0 && (dy_x_apo < 1.15*dy_x && dy_x >= tol))
	{
		//Average exit velocity vector
		v_m = (length(V_x) + length(V_x_apo)) / 2.0;
		dy_x_apo = 200.0*6378165.0;
		dy_xf = true;
		V_x = (V_x + V_x_apo) / 2.0;
		V_x = unit(V_x)*v_m;
		goto RTEMoon_MCDRIV_7_R;
	}
	else
	{
		t_x = t_x_apo;
		Y_x = Y_x_apo;
		X_x = Y_x + X_mx;
		dy_x_apo = dy_x;
		goto RTEMoon_MCDRIV_5_S;
	}

RTEMoon_MCDRIV_11_W:
	//Solution has converged, check if reentry flight-path angle needs to be adjusted
	if (KOUNT <= 4)
	{
		beta_r = EntryCalculations::ReentryTargetLine(u_r, ICRNGG == 1);
		KOUNT++;
		if (abs(beta_r - beta_r_apo) >= 0.0002)
		{
			KOUNT++;
			k_x = 1;
			goto RTEMoon_MCDRIV_5_S;
		}
	}

	//Verification
	/*char Buffer[128];
	VECTOR3 rr_S, v_S, v_I_star, r_I_star, R_I_star, V_I_star, R_EI2, V_EI2;
	double dt;

	if (T_a < -3600.0)
	{
		double t_I, t_N;

		//Conic propagation to sphere entry point
		dt = OrbMech::time_radius(Y_0, -V_a, r_s, 1.0, mu_M);
		dt = -dt;
		t_N = t_0 + dt;
		OrbMech::rv_from_r0v0(Y_0, V_a, dt, rr_S, v_S, mu_M);
		//Linear propagation to original time
		v_I_star = v_S;
		r_I_star = rr_S + v_S * (t_0 - t_N);
		//Convert to geocentric pseudostate
		R_I_star = X_m0 + r_I_star;
		V_I_star = U_m0 + v_I_star;
		//Estimate perilune time
		dt = OrbMech::timetoperi(Y_0, V_a, mu_M);
		t_I = t_0 + dt;
		//Propagate geocentric pseudostate to perilune time
		OrbMech::rv_from_r0v0(R_I_star, V_I_star, dt, R_I_star, V_I_star, mu_E);
		//Get Moon state vector at new time
		pRTCC->PLEFEM(4, t_I / 3600.0, 0, &X_mx, &U_mx, NULL, NULL);
		//Convert to selenocentric pseudostate
		r_I_star = R_I_star - X_mx;
		v_I_star = V_I_star - U_mx;
		//Linear propagation to sphere entrance
		dt = (-dotp(r_I_star, v_I_star) - sqrt(pow(dotp(r_I_star, v_I_star), 2) + dotp(v_I_star, v_I_star) * (pow(r_s, 2) - dotp(r_I_star, r_I_star)))) / dotp(v_I_star, v_I_star);
		t_N = t_I + dt;
		rr_S = r_I_star + v_I_star * dt;
		v_S = v_I_star;
		//Conic propagation to specified time. Real perilune state
		OrbMech::rv_from_r0v0(rr_S, v_S, t_I - t_N, rr_S, v_S, mu_M);

		//Then same as post-pericynthion
		dt = OrbMech::time_radius(rr_S, v_S, r_s, 1.0, mu_M);
		OrbMech::rv_from_r0v0(rr_S, v_S, dt, rr_S, v_S, mu_M);
		v_I_star = v_S;
		r_I_star = rr_S - v_S * dt;
		R_I_star = X_mx + r_I_star;
		V_I_star = U_mx + v_I_star;
		OrbMech::rv_from_r0v0(R_I_star, V_I_star, T_EI - t_I, R_EI2, V_EI2, mu_E);
	}
	else
	{
		dt = OrbMech::time_radius(Y_0, V_a, r_s, 1.0, mu_M);
		OrbMech::rv_from_r0v0(Y_0, V_a, dt, rr_S, v_S, mu_M);
		v_I_star = v_S;
		r_I_star = rr_S - v_S * dt;
		R_I_star = X_m0 + r_I_star;
		V_I_star = U_m0 + v_I_star;
		OrbMech::rv_from_r0v0(R_I_star, V_I_star, T_EI - t_0, R_EI2, V_EI2, mu_E);
	}
	sprintf(Buffer, "MCDRIV Debug: Error %lf T_a %lf", length(R_EI - R_EI2), T_a);
	oapiWriteLog(Buffer);*/

	return 1;
}

double RTEMoon::SEARCH(int &IPART, VECTOR3 &DVARR, VECTOR3 &TIGARR, double tig, double dv, bool &IOUT)
{
	double DVTEST, dt;

	if (IPART == 1)
	{
		DVARR = _V(1e10, 1e10, 1e10);
		TIGARR = _V(1e10, 1e10, 1e10);
		IPART = 2;
	}

	DVARR.x = DVARR.y;
	DVARR.y = DVARR.z;
	DVARR.z = dv;
	TIGARR.x = TIGARR.y;
	TIGARR.y = TIGARR.z;
	TIGARR.z = tig;
	DVTEST = DVARR.z - DVARR.y;
	dt = (TIGARR.z - TIGARR.y);
	if (IPART == 4 && (abs(dt) < 1.0 || abs(DVTEST) < 0.2*0.3048))
	{
		IOUT = true;
	}
	else
	{
		IOUT = false;
	}
	if (IPART == 2)
	{
		IPART = 3;
		return 120.0;
	}
	else if (DVTEST <= 0)
	{
		//Most recent DV smaller than the previous one, continue in this direction
		return dt;
	}
	else
	{
		//DV increasing again, we went past a minimum
		IPART = 4;
		return -dt / 2.0;
	}
}

bool RTEMoon::FINDUX(VECTOR3 X_x, double t_x, double r_r, double u_r, double beta_r, double i_r, double INTER, bool q_a, double mu, VECTOR3 &U_x, VECTOR3 &R_EI, VECTOR3 &V_EI, double &T_EI, double &Incl_apo) const
{
	//INPUTS:
	//X_x: Earth-centered position vector at exit from moon's sphere of action (MSA)
	//t_x: Time of MSA exit from base time
	//r_r: Reentry radius
	//beta_r: Reentry flight path angle
	//i_r: Return inclination
	//INTER: Indicator for which of the two possible orbital planes for a given i_r is specified
	//q_a: Apogee passage flag

	//OUTPUT:
	//U_x: Velocity vector at MSA exit
	//t_z: Time of landing from base time

	VECTOR3 X_x_u, R_1, U_x_u;
	double x_x, E, e, a, eta_r, eta_x, eta_xr, T_r, T_x, P, beta_x, alpha_x, delta_x, sin_delta_r, cos_delta_r, theta, alpha_r, eta_x1, t_z, T_xr, T_rz;
	bool NIR;

	Incl_apo = i_r;
	NIR = false;
	T_rz = 0.0; //TBD: for now...

	x_x = length(X_x);
	X_x_u = unit(X_x);
	OrbMech::ra_and_dec_from_r(X_x_u, alpha_x, delta_x);

	E = u_r * u_r / mu - 2.0 / r_r;
	if (abs(E) < 1e-3 / 6378165.0)
	{
		e = 1.0;
	}
	else
	{
		a = -1.0 / E;
		e = sqrt(1.0 - r_r * r_r*u_r*u_r*sin(beta_r)*sin(beta_r) / (mu*a));
	}
	if (abs(e - 1.0) < 1e-5)
	{
		a = pow(r_r, 2) * pow(u_r, 2)*pow(sin(beta_r), 2) / mu;
		eta_r = PI2 - acos(a / r_r - 1.0);
		eta_x = acos(a / x_x - 1.0);
	}
	else
	{
		eta_r = PI2 - acos((a*(1.0 - e * e) / r_r - 1.0) / e);
		eta_x = acos((a*(1.0 - e * e) / x_x - 1.0) / e);
	}
	if (q_a == 0)
	{
		eta_x = PI2 - eta_x;
	}
	eta_xr = eta_r - eta_x;
	pRTCC->PITFPC(mu, 0, a, e, r_r, T_r, P, false);
	pRTCC->PITFPC(mu, 0, a, e, x_x, T_x, P, false);
	if (q_a == 0 || (q_a == 1 && E >= 0))
	{
		t_z = t_x + T_x - T_r + T_rz;
	}
	else
	{
		t_z = t_x + P - T_x - T_r + T_rz;
	}
	T_xr = t_z - t_x - T_rz;
	T_EI = t_x + T_xr;

	beta_x = atan2(1.0 + e * cos(eta_x), e*sin(eta_x));
	if (Incl_apo <= abs(delta_x))
	{
		Incl_apo = abs(delta_x) + 0.0001;
		NIR = true;
	}
	sin_delta_r = sin(delta_x)*cos(eta_xr) + sin(eta_xr)*INTER*sqrt(sin(Incl_apo)*sin(Incl_apo) - sin(delta_x)*sin(delta_x));
	cos_delta_r = sqrt(1.0 - sin_delta_r * sin_delta_r);

	theta = asin(sin(eta_xr)*cos(Incl_apo) / (cos_delta_r*cos(delta_x)));
	if (cos(eta_xr) < 0)
	{
		theta = PI - theta;
	}
	if (theta < 0)
	{
		theta = PI2 + theta;
	}

	alpha_r = alpha_x + theta;
	R_1 = _V(cos(alpha_r)*cos_delta_r, sin(alpha_r)*cos_delta_r, sin_delta_r);
	eta_x1 = eta_xr;
	U_x_u = EntryCalculations::TVECT(X_x_u, R_1, eta_x1, beta_x);
	U_x = U_x_u * sqrt(u_r*u_r - 2.0*mu*(1.0 / r_r - 1.0 / x_x));

	OrbMech::rv_from_r0v0(X_x, U_x, T_xr, R_EI, V_EI, mu);

	return NIR;
}

void RTEMoon::INRFV(VECTOR3 R_1, VECTOR3 V_2, double r_2, double mu, bool k3, double &a, double &e, double &p, double &theta, VECTOR3 &V_1, VECTOR3 &R_2, double &dt_2, bool &q_m, bool &k_1, double &beta_1, double &beta_2) const
{
	//INPUTS:
	//R_1: Initial positon vector
	//V_2: Velocity vector at the second position
	//r_2: magnitude of the second position vector
	//mu: gravitational constant
	//k3: 0 = noncircumlunar or retrograde motion as specified by QDFLG, 1 = circumlunar motion
	//QDFLG: 0 = use circumlunar or noncircumlunar motion, 1 = use retrograde motion

	//OUTPUTS:
	//I: counts the roots from the quartic tested, error flag if I > 5
	//q_m: 0 = noncircumlunar, 1 = circumlunar
	//k_1: 0 = no pericenter passage, 1 = pericenter passage

	VECTOR3 r_1_cf, v_2_cf, c;
	double psi, cos_psi, sin_psi, A, B, C, r_1, v_2, sin_beta_2, v_1, f, g, DIFG;
	double AA[5], RR[4];
	int N;

	DIFG = 1.0;
	r_1 = length(R_1);
	v_2 = length(V_2);

	if (QDFLG)
	{
		DIFG = -1.0;
	}

	psi = EntryCalculations::XDOTX2(unit(R_1), unit(V_2), DIFG);

	if (QDFLG)
	{
		if (psi < PI)
		{
			q_m = false;
		}
		else
		{
			q_m = true;
		}
	}
	else
	{
		q_m = k3;
		if (psi < PI || !k3)
		{
			if (psi >= PI || k3)
			{
				psi = PI2 - psi;
			}
		}
	}

	r_1 = length(R_1);
	v_2 = length(V_2);
	r_1_cf = unit(R_1);
	v_2_cf = unit(V_2);

	cos_psi = cos(psi);
	sin_psi = sin(psi);

	A = r_2 * r_2*v_2*v_2 / mu / r_1;
	B = (1.0 - r_2 * v_2*v_2 / mu)*sin_psi;
	C = cos_psi * cos_psi;

	//solve quartic
	AA[0] = A * A;
	AA[1] = 2.0*A*B;
	AA[2] = B * B + C - 2.0*A;
	AA[3] = -2.0*B;
	AA[4] = 1.0 - C;

	OrbMech::SolveQuartic(AA, RR, N);

	if (cos_psi > 0)
	{
		sin_beta_2 = RR[N - 2];
	}
	else
	{
		sin_beta_2 = RR[N - 1];
	}

	beta_2 = asin(sin_beta_2);
	c = unit(crossp(r_1_cf, v_2_cf) / sin_psi);
	theta = psi - beta_2;
	R_2 = (r_1_cf*cos(theta) + crossp(c, r_1_cf)*sin(theta))*r_2;
	v_1 = sqrt(v_2 - 2.0*mu / r_2 + 2.0*mu / r_1);
	p = r_2 * r_2*v_2*v_2*sin_beta_2*sin_beta_2 / mu;
	f = 1.0 - r_2 * (1.0 - cos(theta)) / p;
	g = r_2 * r_1*sin(theta) / sqrt(mu*p);
	V_1 = (R_2 - R_1 * f) / g;
	a = 1.0 / (2.0 / r_2 - v_2 * v_2 / mu);
	e = sqrt(1.0 - p / a);

	OrbMech::time_theta(R_1, V_1, theta, mu, dt_2);
	//VECTOR3 R2_apo, V2_apo;
	//OrbMech::rv_from_r0v0(R_1, V_1, dt_2, R2_apo, V2_apo, mu);

	//Determine pericenter passage
	//r_1 near pericenter?
	if (r_1 < p / (1.0 + e) + 1e-6*R_E)
	{
		//Close to periapsis solution
		e = p / r_1 - 1.0;
		beta_1 = PI05;
		k_1 = true;
	}
	else
	{
		//Not close to periapsis
		beta_1 = EntryCalculations::XDOTX2(R_1, V_1, 0.0);
		if (beta_1 > PI05)
		{
			k_1 = true;
		}
		else
		{
			k_1 = false;
		}
	}
}

void RTEMoon::STORE(int opt, double &dv, double &i_r, double &INTER, double &t_z, bool &q_m)
{
	//1 = store, 2 = restore
	if (opt == 1)
	{
		solution.dv = dv;
		solution.i_r = i_r;
		solution.INTER = INTER;
		solution.q_m = q_m;
		solution.t_z = t_z;
	}
	else
	{
		dv = solution.dv;
		i_r = solution.i_r;
		INTER = solution.INTER;
		q_m = solution.q_m;
		t_z = solution.t_z;
	}
}

void RTEMoon::PSTATE(double a_H, double e_H, double p_H, double t_0, double T_x, VECTOR3 Y_0, VECTOR3 &Y_a_apo, VECTOR3 V_x, double theta, double beta_a, double beta_x, double T_a, VECTOR3 &V_a, double &t_x_aaapo, VECTOR3 &Y_x_apo, double &Dy_0, double &deltat, VECTOR3 &X_mx, VECTOR3 &U_mx) const
{
	//INPUTS:
	//a_h: semimajor axis of selenocentric conic
	//e_H: eccentricity of selenocentric conic
	//p_H: semilatus rectum of selenocentric conic
	//t_0: time of abort
	//T_x: flight time from pericynthion to PTS exit
	//Y_0: Selenocentric preabort position vector
	//Y_a_apo: Fictitious abort position vector used in last call to INRFV
	//V_x: Selenocentric PTS exit velocity vector
	//theta: transfer angle from Y_a_apo to Y_x_apo
	//beta_a: postabort flight-path angle
	//beta_x: PTS exit flight-path angle of selenocentric conic
	//T_a: flight time from pericynthion to the maneuver position
	//V_a: Selenocentric postabort velocity vector
	//Y_x_apo: Selenocentric PTS exit position vector
	//OUTPUTS:
	//V_a: Corrected postabort velocity vector
	//t_x_aaapo: Postpericynthion pseudostate transformation time for next iteration
	//Y_x_apo: Selenocentric PTS exit position vector for next iteration
	//Dy_0: Magnitude of the abort position mismatch

	if (T_a < -3600.0)
	{
		t_x_aaapo = t_0 - T_a;
		deltat = T_x;

		bool IERR = pRTCC->PLEFEM(4, t_x_aaapo / 3600.0, 0, &X_mx, &U_mx, NULL, NULL);
		if (IERR)
		{
			//Error
			return;
		}

		VECTOR3 Y_u, V_u, Y_n_u, V_n_u, Y_n, V_n, U_p_n, X_p_n, X_a_n, U_a_n, Y, V, Y_a, DY_0;
		double t_n, f_a, f_x, gamma_p, gamma_v, A, B, C, Deltat;

		t_n = t_x_aaapo - T_x;
		f_a = T_a / abs(T_a)*acos((p_H / length(Y_a_apo) - 1.0) / e_H); //Error in RTCC document
		f_x = theta + f_a;
		Y_u = unit(Y_a_apo);
		V_u = unit(V_a);

		gamma_p = beta_a + f_x + f_a;
		gamma_v = gamma_p + beta_x - PI;

		Y_n_u = EntryCalculations::TVECT(V_u, Y_u, beta_a, gamma_p);
		V_n_u = EntryCalculations::TVECT(V_u, Y_u, beta_a, gamma_v);

		//Pseudostate vector at PTS entry
		Y_n = Y_n_u * r_s;
		V_n = V_n_u * length(V_x);

		//Linear projection to pericynthion, then convert to geocentric pseudostate
		U_p_n = V_n + U_mx;
		X_p_n = Y_n + V_n * T_x + X_mx;

		//Propagate geocentric pseudostate from t_x_aaapo to t_0. X_a_n and U_a_n are geocentric pseudostate at time t_0
		Deltat = t_x_aaapo - t_0;
		OrbMech::rv_from_r0v0(X_p_n, -U_p_n, Deltat, X_a_n, U_a_n, mu_E);
		U_a_n = -U_a_n;

		//Convert to selenocentric pseudostate at time t_0
		Y = X_a_n - X_m0;
		V = U_a_n - U_m0;

		//Calculate linear from time from t_0 to PTS entry point
		A = dotp(V, V);
		B = dotp(V, Y);
		C = r_s * r_s - dotp(Y, Y);

		Deltat = (-B - sqrt(B*B + A * C)) / A;

		//Calculate sphere entrance state vector
		t_n = t_0 + Deltat;
		V_n = V;
		Y_n = Y + V * Deltat;

		//Propagate to t_0. New postabort state vector
		OrbMech::rv_from_r0v0(Y_n, V_n, -Deltat, Y_a, V_a, mu_M);

		DY_0 = Y_0 - Y_a;
		Y_a_apo = Y_a_apo + DY_0;
		Dy_0 = length(DY_0);
	}
	else
	{
		Dy_0 = 0.0;
		t_x_aaapo = t_0;
		deltat = T_x - T_a;
		U_mx = U_m0;
		X_mx = X_m0;
		Y_a_apo = Y_0;
	}

	Y_x_apo = Y_x_apo - V_x * deltat;
}