/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

LM Guidance Simulation for the RTCC

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
#include "OrbMech.h"
#include "LMGuidanceSim.h"

const double AscentGuidance::F_APS = 15297.43;
const double AscentGuidance::F_DPS = 43203.3275;
const double AscentGuidance::Isp_APS = (308.8 * G);
const double AscentGuidance::Isp_DPS = 3107.0;
const double AscentGuidance::mu_M = GGRAV * 7.34763862e+22;
const double AscentGuidance::t_2 = 2.0;
const double AscentGuidance::t_3 = 10.0;
const double AscentGuidance::PRLIMIT = -0.1*0.3048;

AscentGuidance::AscentGuidance()
{
	Q = _V(0, 0, 0);
	t_go = 0.0;
	R_D = 0.0;
	Y_D = 0.0;
	R_D_dot = 0.0;
	Y_D_dot = 0.0;
	Z_D_dot = 0.0;
	t_cut = 0.0;
	FLVP = false;
	FLENG2 = false;
}

void AscentGuidance::Init(VECTOR3 R_C, VECTOR3 V_C, double m0, double rls, double v_hor, double v_rad, bool aps)
{
	Q = unit(crossp(V_C, R_C));

	SetThrustParams(aps);

	m_dot = F / Isp;
	a_T = F / m0;
	v_e = F / m_dot;
	tau = m0 / m_dot;
	r_LS = rls;
	R_D = r_LS + 60000.0*0.3048;
	Z_D_dot = v_hor;
	R_D_dot = v_rad;
	t_go = 450.0;
	t_cut = 0.0;
	FLVP = true;
	FLENG2 = false;
}

void AscentGuidance::SetThrustParams(bool aps)
{
	if (aps)
	{
		F = F_APS;
		Isp = Isp_APS;
	}
	else
	{
		F = F_DPS;
		Isp = Isp_DPS;
	}
	m_dot = F / Isp;
	v_e = F / m_dot;
}

void AscentGuidance::SetTGO(double tgo)
{
	t_go = tgo;
}

void AscentGuidance::Guidance(VECTOR3 R, VECTOR3 V, double M, double t_cur, VECTOR3 &U_FDP, double &ttgo, double &Thrust, double &isp)
{
	tau = M / m_dot;
	a_T = v_e / tau;

	r = length(R);
	h = r - r_LS;
	U_R = unit(R);
	U_Z = crossp(U_R, Q);
	U_Y = crossp(U_Z, U_R);
	R_dot = dotp(V, U_R);
	Y_dot = dotp(V, U_Y);
	Z_dot = dotp(V, U_Z);
	Y = r * asin(dotp(U_R, Q));
	V_G = U_R * (R_D_dot - R_dot) + U_Y * (Y_D_dot - Y_dot) + U_Z * (Z_D_dot - Z_dot);

	g_eff = pow(length(crossp(R, V)), 2) / pow(r, 3) - mu_M / r / r;
	t_go -= 2.0;
	V_G = V_G - U_R * 0.5*t_go*g_eff;
	t_go = tau * length(V_G) / v_e * (1.0 - 0.5*length(V_G) / v_e);

	if (FLENG2 == false)
	{
		if (t_go < 4.0)
		{
			t_cut = t_cur + t_go;
			FLENG2 = true;
		}
	}

	if (t_go >= t_2)
	{
		L = log(1.0 - t_go / tau);
		D_12 = tau + t_go / L;

		if (t_go < t_3)
		{
			B = D = 0.0;
		}
		else
		{

			D_21 = t_go - D_12;
			E = t_go / 2.0 - D_21;
			B = (D_21*(R_D_dot - R_dot) - (R_D - r - R_dot * t_go)) / (t_go * E);
			D = (D_21*(Y_D_dot - Y_dot) - (Y_D - Y - Y_dot * t_go)) / (t_go * E);
		}

		if (B > 0)
		{
			B = 0.0;
		}
		else
		{
			if (B < PRLIMIT*tau)
			{
				B = PRLIMIT * tau;
			}
		}
		A = -D_12 * B - (R_D_dot - R_dot) / L;
		C = -D_12 * D - (Y_D_dot - Y_dot) / L;
	}

	a_TR = (A + B) / tau - g_eff;
	a_TY = (C + D) / tau;
	A_H = U_Y * a_TY + U_R * a_TR;
	a_H = length(A_H);
	if (a_H < a_T)
	{
		a_TP = sqrt(a_T*a_T - a_H * a_H)*OrbMech::sign(Z_D_dot - Z_dot);
		A_T = A_H + U_Z * a_TP;
	}
	else
	{
		double K_H;
		K_H = a_T / a_H;
		A_T = A_H * K_H;
		a_TR = K_H * a_TR;
		a_TY = K_H * a_TY;
		a_TP = 0.0;
	}
	U_FDP = unit(A_T);

	if (FLENG2)
	{
		ttgo = t_cut - t_cur;
	}
	else
	{
		ttgo = t_go;
	}

	if (FLVP)
	{
		if (h > 25000.0*0.3048 || R_dot > 40.0*0.3048)
		{
			FLVP = false;
		}
		else
		{
			U_FDP = unit(R);
		}
	}
	Thrust = F;
	isp = Isp;
}

const double DescentGuidance::UT = 7.5;
const double DescentGuidance::TRMT = 26.0;
const double DescentGuidance::THRUL = 200.0*4.4482216152605;
const double DescentGuidance::THRTRM = 1050.0*4.4482216152605;
const double DescentGuidance::THRMIN = 1050.0*4.4482216152605;
const double DescentGuidance::THRMAX = 9710.0*4.4482216152605;
const double DescentGuidance::ULISP = 268.0*G;
const double DescentGuidance::XKISP = 303.0*G;
const double DescentGuidance::mu_M = GGRAV * 7.34763862e+22;
const double DescentGuidance::MAXFORCE = 28023.8;
const double DescentGuidance::MINFORCE = 4359.26;
const double DescentGuidance::LOWCRIT = 5985.0*4.4482216152605;
const double DescentGuidance::HIGHCRIT = 6615.0*4.4482216152605;
const double DescentGuidance::DELHTR = 2.525;
const double DescentGuidance::DELISP = 200.0*2.525 / (10500.0*4.4482216152605)*G;
const double DescentGuidance::XISP2 = 4.4429572356347e-8;
const double DescentGuidance::XISP1 = 6.177486342939e-4;
const double DescentGuidance::XISP0 = 2865.8065891221;

DescentGuidance::DescentGuidance()
{
	PHASE = 0;
	t_go = 0.0;
	Thrust_old = 0.0;
}

void DescentGuidance::Init(VECTOR3 R_C, VECTOR3 V_C, double m0, double t_I, MATRIX3 REFS, VECTOR3 R_LSP_init, double t_P, VECTOR3 W)
{
	t_IG = t_I;
	PHASE = -2;
	XJD[0] = 0.;
	XJD[1] = 0.;
	XJD[2] = -0.011885*0.3048;
	XJD[3] = 0.;
	XJD[4] = 0.;
	XJD[5] = 0.034336*0.3048;
	XAD[0] = 0.6241*0.3048;
	XAD[1] = 0.;
	XAD[2] = -9.1044*0.3048;
	XAD[3] = 0.05*0.3048;
	XAD[4] = 0.;
	XAD[5] = -0.65*0.3048;
	XRD[0] = 171.835*0.3048;
	XRD[1] = 0.;
	XRD[2] = -10678.596*0.3048;
	XRD[3] = 111.085*0.3048;
	XRD[4] = 0.;
	XRD[5] = -26.794*0.3048;
	XVD[0] = -105.876*0.3048;
	XVD[1] = 0.;
	XVD[2] = -1.04*0.3048;
	XVD[3] = -4.993*0.3048;
	XVD[4] = 0.;
	XVD[5] = 0.248*0.3048;

	t_go = 664.4;
	REFSMMAT = REFS;
	R_LSP = R_LSP_init;
	t_pip = t_P;
	r_LS = length(R_LSP);
	WP = W;

	RDG = _V(XRD[0], XRD[1], XRD[2]);
	VDG = _V(XVD[0], XVD[1], XVD[2]);
	ADG = _V(XAD[0], XAD[1], XAD[2]);
	JDG = _V(XJD[0], XJD[1], XJD[2]);
}

void DescentGuidance::Guidance(VECTOR3 R, VECTOR3 V, double M, double t_cur, VECTOR3 &U_FDI, double &ttgo, double &Thrust, double &isp)
{
	if (PHASE == -2)
	{
		if (t_cur - t_IG > -UT)
		{
			PHASE++;
		}
	}
	else if (PHASE == -1)
	{
		if (t_cur - t_IG > TRMT)
		{
			PHASE = 0;
		}
	}
	else if (PHASE == 0)
	{
		if (t_go < 60.0)
		{
			PHASE = 1;
			RDG = _V(XRD[3], XRD[4], XRD[5]);
			VDG = _V(XVD[3], XVD[4], XVD[5]);
			ADG = _V(XAD[3], XAD[4], XAD[5]);
			JDG = _V(XJD[3], XJD[4], XJD[5]);
		}
	}
	else if (PHASE == 1)
	{
		if (t_go < 10.0)
		{
			PHASE = 2;
			VDG = _V(-3.0*0.3048, 0., 0.);
		}
	}

	if (PHASE == -2)
	{
		Thrust = 0.0;
		isp = 1.0;
	}
	else if (PHASE == -1)
	{
		if (t_cur - t_IG < 0.0)
		{
			Thrust = THRUL;
			isp = ULISP;
		}
		else
		{
			Thrust = THRTRM;
			isp = XKISP;
		}
	}
	else
	{
		Thrust = THRMAX;
		isp = XKISP;
	}

	RP = mul(REFSMMAT, R);
	VP = mul(REFSMMAT, V);
	GP = -RP / pow(length(RP), 3)*mu_M;
	
	t_pipold = t_pip;
	t_pip = t_cur;
	R_LSP = unit(R_LSP + crossp(WP, R_LSP)*(t_pip - t_pipold))*r_LS;
	ULP = unit(R_LSP);

	if (PHASE != 2)
	{
		UXGP = ULP;
		WXR = crossp(WP, RP);
		UYGP = unit(crossp(UXGP, RP - R_LSP - (VP - WXR)*t_go / 4.0));
		UZGP = crossp(UXGP, UYGP);
		C_GP = _M(UXGP.x, UXGP.y, UXGP.z, UYGP.x, UYGP.y, UYGP.z, UZGP.x, UZGP.y, UZGP.z);
	}

	RG = mul(C_GP, RP - R_LSP);
	VG = mul(C_GP, VP - WXR);

	if (PHASE == 2)
	{
		t_go = (length(RP) - length(R_LSP) - 15.0*0.3048) / length(VG);
	}
	else
	{
		eps = abs(t_go / 128.0);
		do
		{
			DELTGO = (-JDG.z*t_go*t_go*t_go + 6.0*ADG.z*t_go*t_go - (18.0*VDG.z + 6.0*VG.z)*t_go + 24.0*(RDG.z - RG.z)) / (3.0*JDG.z*t_go*t_go - 12.0*ADG.z*t_go + 6.0*(3.0*VDG.z + VG.z));
			t_go += DELTGO;
		} while (abs(DELTGO) > eps);
	}

	if (PHASE == 2)
	{
		ACG = (VDG - VG) / 1.5;
	}
	else
	{
		ACG = ADG - (VDG + VG) / t_go * 6.0 + (RDG - RG) / t_go / t_go * 12.0;
	}
	A_FDP = tmul(C_GP, ACG) - GP;

	//P63 and P64
	if (PHASE == 0 || PHASE == 1)
	{
		FC = length(A_FDP)*M;

		if (Thrust_old > HIGHCRIT)
		{
			if (FC > LOWCRIT)
			{
				//Hold throttle up
				Thrust = THRMAX + DELHTR * (t_cur - t_IG - TRMT);
			}
			else
			{
				//Throttle down
				Thrust = FC;
			}
		}
		else
		{
			if (FC > HIGHCRIT)
			{
				//Throttle up
				Thrust = THRMAX + DELHTR * (t_cur - t_IG - TRMT);
			}
			else
			{
				//Continuous Throttling
				Thrust = FC;
			}
		}

		if (Thrust > HIGHCRIT)
		{
			isp = XKISP - DELISP * (t_cur - t_IG - TRMT);
		}
		else
		{
			isp = XISP2 * Thrust*Thrust + XISP1 * Thrust + XISP0;
		}
	}
	//P66
	else if (PHASE == 2)
	{
		FC = length(A_FDP)*M;
		if (FC > MAXFORCE)
		{
			Thrust = MAXFORCE;
		}
		else if (FC < MINFORCE)
		{
			Thrust = MINFORCE;
		}

		isp = XISP2 * Thrust*Thrust + XISP1 * Thrust + XISP0;
	}

	Thrust_old = Thrust;

	U_FDI = tmul(REFSMMAT, unit(A_FDP));
	ttgo = t_go;
}

const double AscDescIntegrator::mu_M = GGRAV * 7.34763862e+22;

AscDescIntegrator::AscDescIntegrator()
{
	dt = 0.0;
	dt_max = 2.0;
	max_rate = 10.0*RAD;
}

void AscDescIntegrator::Init(VECTOR3 U_TD_init)
{
	U_TD_cur = U_TD_init;
}

bool AscDescIntegrator::Integration(VECTOR3 &R, VECTOR3 &V, double &mnow, double &t_total, VECTOR3 U_TD, double t_remain, double Thrust, double Isp)
{
	dt = min(dt_max, t_remain);

	if (acos2(dotp(U_TD, U_TD_cur)) < max_rate*dt)
	{
		U_TD_cur = U_TD;
	}
	else
	{
		VECTOR3 k = unit(crossp(U_TD_cur, U_TD));
		U_TD_cur = U_TD_cur * cos(max_rate*dt) + crossp(k, U_TD_cur)*sin(max_rate*dt) + k * dotp(k, U_TD_cur)*(1.0 - cos(max_rate*dt));
	}

	DVDT = U_TD_cur * Thrust / mnow * dt;
	G_P = gravity(R);
	R = R + (V + G_P * dt / 2.0 + DVDT / 2.0)*dt;
	G_PDT = gravity(R);
	V = V + (G_PDT + G_P)*dt / 2.0 + DVDT;
	mnow -= Thrust / Isp * dt;
	t_total += dt;

	if (t_remain <= dt_max)
	{
		return true;
	}

	return false;
}

VECTOR3 AscDescIntegrator::gravity(VECTOR3 R)
{
	VECTOR3 U_R, g;
	double rr;

	U_R = unit(R);
	rr = dotp(R, R);
	g = -U_R * mu_M / rr;

	return g;
}