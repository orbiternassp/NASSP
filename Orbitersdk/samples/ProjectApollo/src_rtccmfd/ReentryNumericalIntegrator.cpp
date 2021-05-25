/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Reentry Numerical Integrator Module

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

#include "ReentryNumericalIntegrator.h"
#include "rtcc.h"

GNDataTab::GNDataTab()
{
	VREF_TAB[0] = 994.0; VREF_TAB[1] = 2103.0; VREF_TAB[2] = 3922.0; VREF_TAB[3] = 6295.0; VREF_TAB[4] = 8531.0; VREF_TAB[5] = 10101.0; VREF_TAB[6] = 14014.0;
	VREF_TAB[7] = 15951.0; VREF_TAB[8] = 18357.0; VREF_TAB[9] = 20829.0; VREF_TAB[10] = 23090.0; VREF_TAB[11] = 23500.0; VREF_TAB[12] = 35000.0;

	RDTRF_TAB[0] = -690.9; RDTRF_TAB[1] = -719.0; RDTRF_TAB[2] = -694.0; RDTRF_TAB[3] = -609.0; RDTRF_TAB[4] = -493.0; RDTRF_TAB[5] = -416.0; RDTRF_TAB[6] = -352.0;
	RDTRF_TAB[7] = -416.0; RDTRF_TAB[8] = -566.0; RDTRF_TAB[9] = -781.0; RDTRF_TAB[10] = -927.0;RDTRF_TAB[11] = -820.0; RDTRF_TAB[12] = -820.0;

	AREF_TAB[0] = 41.15; AREF_TAB[1] = 60.0; AREF_TAB[2] = 81.5; AREF_TAB[3] = 93.9; AREF_TAB[4] = 98.5; AREF_TAB[5] = 102.3; AREF_TAB[6] = 118.7;
	AREF_TAB[7] = 125.2; AREF_TAB[8] = 120.4; AREF_TAB[9] = 95.4; AREF_TAB[10] = 28.1; AREF_TAB[11] = 6.4; AREF_TAB[12] = 6.4;

	FRDT_TAB[0] = 0.002507; FRDT_TAB[1] = 0.003582; FRDT_TAB[2] = 0.007039; FRDT_TAB[3] = 0.01446; FRDT_TAB[4] = 0.02479; FRDT_TAB[5] = 0.03391; FRDT_TAB[6] = 0.06139;
	FRDT_TAB[7] = 0.07683; FRDT_TAB[8] = 0.09982; FRDT_TAB[9] = 0.1335; FRDT_TAB[10] = 0.2175; FRDT_TAB[11] = 0.3046; FRDT_TAB[12] = 0.3046;

	FA_TAB[0] = -0.0346; FA_TAB[1] = -0.05551; FA_TAB[2] = -0.09034; FA_TAB[3] = -0.1410; FA_TAB[4] = -0.1978; FA_TAB[5] = -0.2372; FA_TAB[6] = -0.3305;
	FA_TAB[7] = -0.3605; FA_TAB[8] = -0.4956; FA_TAB[9] = -0.6483; FA_TAB[10] = -2.021; FA_TAB[11] = -3.354; FA_TAB[12] = -3.354;

	RTOGO_TAB[0] = 3.7; RTOGO_TAB[1] = 10.4; RTOGO_TAB[2] = 23.6; RTOGO_TAB[3] = 46.3; RTOGO_TAB[4] = 75.4; RTOGO_TAB[5] = 99.9; RTOGO_TAB[6] = 170.9;
	RTOGO_TAB[7] = 210.3; RTOGO_TAB[8] = 266.8; RTOGO_TAB[9] = 344.3; RTOGO_TAB[10] = 504.8; RTOGO_TAB[11] = 643.0; RTOGO_TAB[12] = 794.3;

	PP_TAB[0] = 12.20; PP_TAB[1] = 21.82; PP_TAB[2] = 43.28; PP_TAB[3] = 96.70; PP_TAB[4] = 187.44; PP_TAB[5] = 282.2; PP_TAB[6] = 329.4;
	PP_TAB[7] = 465.5; PP_TAB[8] = 682.7; PP_TAB[9] = 980.5; PP_TAB[10] = 1385.0; PP_TAB[11] = 1508.0; PP_TAB[12] = 1508.0;

	//Convert to corect units
	for (int i = 0;i < 13;i++)
	{
		VREF_TAB[i] *= 0.3048;
		RDTRF_TAB[i] *= 0.3048;
		AREF_TAB[i] *= 0.3048;
		FRDT_TAB[i] *= 1852.0 / 0.3048;
		FA_TAB[i] *= 1852.0 / 0.3048;
		RTOGO_TAB[i] *= 1852.0;
		PP_TAB[i] *= 1852.0;
	}
}

ReentryNumericalIntegrator::ReentryNumericalIntegrator(RTCC *r) : RTCCModule(r)
{
	N = 12.021653 / 5498.219913;
	Bank = 0.0;
	BRATE = 0.0;
	K1 = 0.0;
	K2 = 55.0*RAD;
	K05G = false;
	KGC = false;
	A_X = 0.0;
	DRAG = 0.0;
	U_Z = _V(0, 0, 1);

	VSAT = 25766.1973*0.3048;
	VMIN = VSAT / 2.0;
	KWE = 1546.70168*0.3048;
	GS = 32.2*0.3048;
	C16 = 0.01/0.3048;
	C17 = 0.001/0.3048;
	HS = 28500.0*0.3048;
	RE = 21202900.0*0.3048;
	ATK = 3437.7468*1852.0;
}

void ReentryNumericalIntegrator::Main(const RMMYNIInputTable &in, RMMYNIOutputTable &out)
{
	R_cur = in.R0;
	V_cur = in.V0;
	GMT0 = in.GMT0;
	LiftMode = in.KSWCH;
	g_c_BU = in.g_c_BU;
	g_c_GN = in.g_c_GN;
	D0 = in.D0;
	LAD = in.LAD;
	LOD = in.LOD;
	RLDIR = in.RLDIR;
	GNData.C10 = in.C10;
	t_RB = in.t_RB;
	H_EMS = in.H_EMS;
	K1 = in.K1;
	K2 = in.K2;
	EphemerisBuildInd = false; //TBD

	gmax = 0;
	T = T_prev = 0.0;
	ISGNInit = false;
	t_2G = 0.0;
	droguedeployed = false;
	maindeployed = false;
	TNEXT = 0.0;
	STEP = 2.0;
	TE = STEP;
	IREVBANK = false;
	EPS = 1e-14*3600.0;

	//Null output table
	out.lat_IP = 0.0;
	out.lng_IP = 0.0;
	out.t_05g = 0.0;
	out.t_2g = 0.0;
	out.t_lc = 0.0;
	out.t_gc = 0.0;
	out.t_drogue = 0.0;
	out.t_main = 0.0;

	double v, fpa;

	IEND = 0;

	if (LiftMode == 3)
	{
		GNData.URTO = OrbMech::r_from_latlong(in.lat_T, in.lng_T);
	}

	do
	{
		//Determine next step
		if (EphemerisBuildInd)
		{
			//Should we store ephemeris?
			if (TNEXT < TE)
			{
				TE = TNEXT;
			}
		}
		//Time to reverse bank angle?
		if (LiftMode == 4 || LiftMode == 8)
		{
			if (IREVBANK == false && t_RB < TE)
			{
				TE = t_RB;
			}
		}

		R_prev = R_cur;
		V_prev = V_cur;
		T_prev = T;
		if (droguedeployed == false)
		{
			GuidanceRoutine(R_cur, V_cur);
		}

		DT = TE - T;

		RungeKuttaIntegrationRoutine(R_prev, V_prev, DT, R_cur, V_cur);
		T += DT;

		//Switch to reverse bank angle?
		if (LiftMode == 4 || LiftMode == 8)
		{
			if (IREVBANK == false && abs(t_RB - T) < EPS)
			{
				IREVBANK = true;
			}
		}

		alt = length(R_cur) - OrbMech::R_Earth;
		v = length(V_cur);
		fpa = asin(dotp(unit(R_cur), unit(V_cur)))*DEG;
		CalculateDragAcceleration(R_cur, V_cur);
		

		//Configuration changes
		if (droguedeployed == false && alt < 23500.0*0.3048)
		{
			N = 80.0 / 5498.219913;
			t_drogue = T;
			droguedeployed = true;
		}
		if (maindeployed == false && alt < 10000.0*0.3048)
		{
			N = 140.0 / 5498.219913;
			t_main = T;
			maindeployed = true;
		}

		if (T > 90.0*60.0)
		{
			//Time limit
			IEND = 1;
		}
		else if (alt < 0.0)//10000.0*0.3048)
		{
			//Impact
			IEND = 2;
		}
		else if (alt > 750000.0*0.3048 && fpa > 0)
		{
			//Skipout
			IEND = 3;
		}

		TE += STEP;
	} while (IEND == 0);

	out.IEND = IEND;
	if (IEND == 2)
	{
		//Output
		double lat, lng;
		OrbMech::latlong_from_r(R_cur, lat, lng);
		lng -= OrbMech::w_Earth*T;
		while (lng < -PI)
		{
			lng += PI2;
		}
		out.lat_IP = lat;
		out.lng_IP = lng;
		out.t_lc = GMT0 + T;
		if (K05G)
		{
			out.t_05g = GMT0 + t_05G;
		}
		if (KGC)
		{
			out.t_gc = GMT0 + t_gc;
		}
		out.t_gmax = GMT0+ t_gmax;
		out.gmax = gmax;
		if (droguedeployed)
		{
			out.t_drogue = GMT0 + t_drogue;
		}
		if (maindeployed)
		{
			out.t_main = GMT0 + t_main;
		}
	}

}

void ReentryNumericalIntegrator::RungeKuttaIntegrationRoutine(VECTOR3 R_N, VECTOR3 V_N, double dt, VECTOR3 &R_N1, VECTOR3 &V_N1)
{
	VECTOR3 K1, K2, K3, K4;

	K1 = SecondDerivativeRoutine(R_N, V_N)*dt;
	Bank += BRATE * dt / 2.0;
	Limit02PI(Bank);
	K2 = SecondDerivativeRoutine(R_N + V_N * dt / 2.0 + K1 * dt / 8.0, V_N + K1 / 2.0)*dt;
	K3 = SecondDerivativeRoutine(R_N + V_N * dt / 2.0 + K1 * dt / 8.0, V_N + K2 / 2.0)*dt;
	Bank += BRATE * dt / 2.0;
	Limit02PI(Bank);
	K4 = SecondDerivativeRoutine(R_N + V_N * dt + K3 * dt / 2.0, V_N + K3)*dt;

	R_N1 = R_N + (V_N + (K1 + K2 + K3) / 6.0)*dt;
	V_N1 = V_N + (K1 + K2 * 2.0 + K3 * 2.0 + K4) / 6.0;

	//TBD: EMS integration
	//R_EMS1 = R_EMS + (V_EMS + (K1EMS + K2EMS + K3EMS) / 6.0)*dt;
	//V_EMS1 = V_EMS + (K1EMS + K2EMS * 2.0 + K3EMS * 2.0 + K4EMS) / 6.0;
}

VECTOR3 ReentryNumericalIntegrator::SecondDerivativeRoutine(VECTOR3 R, VECTOR3 V)
{
	double AOA;
	return GravityAcceleration(R) + LiftDragAcceleration(R, V, AOA);
}

VECTOR3 ReentryNumericalIntegrator::GravityAcceleration(VECTOR3 R)
{
	VECTOR3 U_R, g_b;
	double r, r2, cos_theta;

	U_R = unit(R);
	r = length(R);
	r2 = r * r;
	cos_theta = dotp(U_R, U_Z);
	g_b = -(U_R*(1.0 - 5.0*cos_theta*cos_theta) + U_Z * 2.0*cos_theta)*OrbMech::mu_Earth / r2 * 1.5*OrbMech::J2_Earth*pow(OrbMech::R_Earth / r, 2);
	return -U_R * OrbMech::mu_Earth / r2 + g_b;
}

VECTOR3 ReentryNumericalIntegrator::LiftDragAcceleration(VECTOR3 R, VECTOR3 V, double &AOA)
{
	VECTOR3 A_D, A_L, p_apo, h_apo;
	double alt, rho, spos, mach, C_L, C_D;

	alt = length(R) - OrbMech::R_Earth;
	pRTCC->GLFDEN(alt, rho, spos);

	V_R.x = V.x + R.y*OrbMech::w_Earth;
	V_R.y = V.y - R.x*OrbMech::w_Earth;
	V_R.z = V.z;
	v_R = length(V_R);

	p_apo = unit(crossp(V_R, crossp(R, V_R)));
	h_apo = unit(crossp(R, V_R));

	mach = v_R / spos;
	CalculateLiftDrag(mach, C_L, C_D, AOA);

	A_D = -V_R * 0.5*C_D*rho*N*v_R;
	A_L = (p_apo*cos(Bank) + h_apo * sin(Bank))*0.5*C_L*rho*N*v_R*v_R; //changed from -sin(Bank)
	return A_D + A_L;
}

void ReentryNumericalIntegrator::CalculateLiftDrag(double mach, double &CL, double &CD, double &AOA)
{
	if (maindeployed)
	{
		CL = 0.0;
		CD = 1.5;
		AOA = PI;
	}
	else if (droguedeployed)
	{
		CL = 0.0;
		CD = 1.5;
		AOA = PI;
	}
	else
	{
		pRTCC->RLMCLD(mach, 0, CD, CL, AOA);
	}
}

void ReentryNumericalIntegrator::GuidanceRoutine(VECTOR3 R, VECTOR3 V)
{
	if (K05G == false)
	{
		if (A_X > 0.05*9.80665)
		{
			K05G = true;
			t_05G = T;
		}
	}
	if (t_2G == 0.0)
	{
		if (A_X > 0.2*9.80665)
		{
			t_2G = T;
		}
	}
	if (KGC == false)
	{
		if (A_X > g_c_BU*9.80665)
		{
			KGC = true;
			t_gc = T;
		}
	}
	//Store max g
	if (A_X > gmax*9.80665)
	{
		gmax = A_X / 9.80665;
		t_gmax = T;
	}

	if (LiftMode == 1)
	{
		//Zero lift to 0.05G, then rolling reentry
		if (K05G == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			BRATE = 20.0*RAD;
		}
	}
	else if (LiftMode == 2)
	{
		//Maximum lift
		Bank = 0.0;
		BRATE = 0.0;
	}
	else if (LiftMode == 3)
	{
		if (ISGNInit == false)
		{
			GNInitialization();
			ISGNInit = true;
		}
		GNTargeting();
		//TBD: DAP
		Bank = ROLLC;
		BRATE = 0.0;
	}
	else if (LiftMode == 4)
	{
		//Bank angle - time to reverse bank angle
		if (K05G == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			if (IREVBANK == false)
			{
				Bank = K2;
			}
			else
			{
				Bank = -K2;
			}
			BRATE = 0.0;
		}
	}
	else if (LiftMode == 5)
	{
		//Constant bank angle
		if (K05G == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			Bank = K2;
			BRATE = 0.0;
		}
	}
	else if (LiftMode == 6)
	{
		//Bank angle to a G-level then zero lift
		if (KGC == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			BRATE = 20.0*RAD;
		}
	}
	else if (LiftMode == 7)
	{
		//Bank angle to a G-level then maximum lift
		if (KGC == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			Bank = 0.0;
			BRATE = 0.0;
		}
	}
	else if (LiftMode == 8)
	{
		//Bank angle to a G-level then angle-time to reverse bank angle
		if (KGC == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			if (IREVBANK == false)
			{
				Bank = K2;
			}
			else
			{
				Bank = -K2;
			}
			BRATE = 0.0;
		}
	}
	else if (LiftMode == 9)
	{
		//Bank angle to a G-level then another bank angle to impact prediction
		if (KGC == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			Bank = K2;
			BRATE = 0.0;
		}
	}
	else if (LiftMode == 10)
	{
		//Bank angle to a G-level then constant G
		if (KGC == false)
		{
			Bank = K1;
			BRATE = 0.0;
		}
		else
		{
			Bank = ConstantGLogic(unit(R), V, DRAG);
			BRATE = 0.0;
		}
	}
}

double ReentryNumericalIntegrator::ConstantGLogic(VECTOR3 unitR, VECTOR3 VI, double D)
{
	VECTOR3 V;
	double v, VSQ, LEQ, RDOT, LD;

	if (length(VI) - VMIN > 0)
	{
		V = VI;
	}
	else
	{
		V = VI - crossp(U_Z, unitR)*KWE;
	}
	v = length(V);
	VSQ = v * v / (VSAT*VSAT);
	LEQ = (VSQ - 1.0)*GS;
	RDOT = dotp(V, unitR);
	LD = -LEQ / D0 + C16 * (D - D0) - C17 * (RDOT + 2.0*HS*D0 / v);
	if (abs(LD / LAD) - 1.0 > 0.0)
	{
		LD = LAD * LD / abs(LD);
	}
	return RLDIR * acos(LD / LAD);
}

void ReentryNumericalIntegrator::CalculateDragAcceleration(VECTOR3 R, VECTOR3 V)
{
	VECTOR3 A_aero;
	double AOA;

	A_aero = LiftDragAcceleration(R, V, AOA);
	DRAG = length(A_aero);
	A_X = -DRAG *cos(AOA);
}

void ReentryNumericalIntegrator::Limit02PI(double &val)
{
	if (val >= PI2)
	{
		val -= PI2;
	}
	else if (val < 0)
	{
		val += PI2;
	}
}

void ReentryNumericalIntegrator::GNInitialization()
{
	GNData.RTE = crossp(U_Z, GNData.URTO);
	GNData.UTR = crossp(GNData.RTE, U_Z);
	GNData.WT = GNData.WIE*(GNData.TN + T);
	VECTOR3 RTINT = GNData.URTO + GNData.UTR*(cos(GNData.WT) - 1.0) + GNData.RTE*sin(GNData.WT);
	GNData.THETA = acos(dotp(unit(R_cur), unit(RTINT)));
	GNData.K2ROLL = -dotp(unit(RTINT), crossp(unit(V_cur), unit(R_cur)));
	if (GNData.K2ROLL >= 0.0)
	{
		GNData.K2ROLL = 1.0;
	}
	else
	{
		GNData.K2ROLL = -1.0;
	}
	GNData.KLAT = GNData.KLAT1 * LAD;
	GNData.LD_CMINR = LAD * cos(15.0*RAD);
	GNData.FACTOR = 1.0;
	GNData.Q7 = GNData.Q7F;
	GNData.LD = LAD * cos(GNData.C10);
	ROLLC = GNData.C10;
	GNData.DIFFOLD = 0.0;
	GNData.DLEWD = GNData.DLEWD0;
	GNData.LEWD = GNData.LEWDI;
	GNData.Q2 = (-1152.0 + 500.0*LAD)*1852.0;
	GNData.SELECTOR = 1;
}

void ReentryNumericalIntegrator::GNTargeting()
{
	VECTOR3 V, UNI;

	GNData.HUNTCN = 0;
	if (GNData.RELVELSW == false)
	{
		V = V_cur;
	}
	else
	{
		V = V_cur - crossp(U_Z, unit(R_cur))*KWE;
	}
	GNData.v = length(V);
	GNData.VSQ = GNData.v * GNData.v / VSAT / VSAT;
	GNData.LEQ = (GNData.VSQ - 1.0)*GS;
	GNData.RDOT = dotp(V, unit(R_cur));
	UNI = unit(crossp(V, unit(R_cur)));
	GNData.D = DRAG;
	GNData.LATSW = true;
	if (GNData.RELVELSW)
	{
		GNData.WT = GNData.WIE * T;
	}
	else
	{
		if (GNData.EGSW)
		{
			if (GNData.v - VMIN <= 0)
			{
				GNData.RELVELSW = true;
			}
			GNData.WT = GNData.WIE*(RE*GNData.THETA / GNData.v + T);
		}
		else
		{
			GNData.WT = GNData.WIE*(GNData.KTETA*GNData.THETA + T);
		}
	}
	GNData.URT = GNData.URTO + GNData.UTR * (cos(GNData.WT) - 1.0) + GNData.RTE * sin(GNData.WT);
	GNData.LATANG = dotp(GNData.URT, UNI);
	GNData.THETA = acos(dotp(GNData.URT, unit(R_cur)));
	GNData.THETNM = GNData.THETA*ATK;
	if (GNData.NOGOSW)
	{
		//Go to 380
		return;
	}
	else
	{

	}
	//TBD: Error checking
	if (GNData.D - 0.05*9.80665 < 0)
	{
		GNData.K05GSW = false;
	}
	else
	{
		GNData.K05GSW = true;
	}
	if (dotp(crossp(GNData.URT, unit(R_cur)), UNI) <= 0)
	{
		GNData.GONEBY = true;
	}
	GNModeSelector();
}

void ReentryNumericalIntegrator::GNModeSelector()
{
	switch (GNData.SELECTOR)
	{
	case 1:
		GNInitialRoll();
		break;
	case 2:
		GNHuntest();
		break;
	case 3:
		GNUpcontrol();
		break;
	case 4:
		GNBallistic();
		break;
	case 5:
		GNFinalPhase();
		break;
	}
}

void ReentryNumericalIntegrator::GNInitialRoll()
{
	if (GNData.INRLSW == false)
	{
		if (GNData.K05GSW)
		{
			GNData.INRLSW = true;
			GNData.GONEPAST = false;
			GNData.K05GSW = true;
		}
		else
		{
			GNRoutine310();
			return;
		}
		GNData.KA = (GNData.KA1*pow(GNData.LEQ / GS, 3) + GNData.KA2)*GS;
		if (GNData.KA - 1.5*GS > 0)
		{
			GNData.KA = 1.5*GS;
		}
		if (GNData.v - GNData.VFINAL1 < 0)
		{
			GNData.SELECTOR = 4;
			GNRoutine310();
			return;
		}
		GNData.D0 = GNData.KA3*GNData.LEQ / GS + GNData.KA4;
		if (GNData.v - GNData.VFINAL + GNData.K44*pow(GNData.RDOT / GNData.v, 3) > 0)
		{
			GNData.LD = -LAD;
		}
		else
		{
			GNData.LD = LAD;
		}
		GNRoutine310();
		return;
	}
	else
	{
		if (GNData.RDOT + GNData.VRCONTROL < 0)
		{
			if (GNData.D - GNData.KA > 0)
			{
				GNConstD();
				return;
			}
			else
			{
				GNRoutine310();
				return;
			}
		}
		else
		{
			GNData.SELECTOR = 2;
			GNHuntest();
			return;
		}
	}
}

void ReentryNumericalIntegrator::GNHuntest()
{
	if (GNData.RDOT < 0)
	{
		GNData.V1 = GNData.v + GNData.RDOT / LAD;
		GNData.A0 = pow(GNData.V1 / GNData.v, 2)*(GNData.D + GNData.RDOT*GNData.RDOT / (2.0*HS*GNData.C1*LAD));
		GNData.A1 = GNData.A0;
	}
	else
	{
		GNData.V1 = GNData.v + GNData.RDOT / GNData.LEWD;
		GNData.A0 = pow(GNData.V1 / GNData.v, 2)*(GNData.D + GNData.RDOT*GNData.RDOT / (2.0*HS*GNData.C1*GNData.LEWD));
		GNData.A1 = GNData.D;
	}
	if (GNData.LD < 0)
	{
		GNData.V1 = GNData.V1 - GNData.VQUIT;
	}
	GNData.ALP = 2.0*GNData.C1*GNData.A0*HS / (GNData.LEWD*GNData.V1*GNData.V1);
	GNData.FACT1 = GNData.V1 / (1.0 - GNData.ALP);
	GNData.FACT2 = GNData.ALP * (GNData.ALP - 1.0) / GNData.A0;
	GNData.VL = GNData.FACT1 * (1.0 - sqrt(GNData.FACT2*GNData.Q7 + GNData.ALP));
	if (GNData.VL - GNData.VLMIN < 0)
	{
		GNData.SELECTOR = 5;
		GNData.EGSW = true;
		GNFinalPhase();
		return;
	}
	if (GNData.VL - VSAT > 0)
	{
		GNConstD();
		return;
	}
	if (GNData.V1 - VSAT > 0)
	{
		GNData.VS1 = VSAT;
	}
	else
	{
		GNData.VS1 = GNData.V1;
	}
	double DVL = GNData.VS1 - GNData.VL;
	GNData.DHOOK = (pow(1.0 - GNData.VS1 / GNData.FACT1, 2) - GNData.ALP) / GNData.FACT2;
	GNData.AHOOK = GNData.CHOOK * (GNData.DHOOK / GNData.Q7 - 1.0) / DVL;
	GNData.GAMMAL1 = GNData.LEWD*(GNData.V1 - GNData.VL) / GNData.VL;
	GNData.GAMMAL = GNData.GAMMAL1 - (GNData.CH1*GS*DVL*DVL*(1.0 + GNData.AHOOK * DVL)) / (GNData.DHOOK*GNData.VL*GNData.VL);
	if (GNData.GAMMAL < 0)
	{
		GNData.VL = GNData.VL + GNData.GAMMAL *GNData.VL / (GNData.LEWD - (3.0*GNData.AHOOK*DVL*DVL + 2.0*DVL)*(GNData.CH1*GS / (GNData.DHOOK*GNData.VL)));
		GNData.Q7 = (pow(1.0 - GNData.VL / GNData.FACT1, 2) - GNData.ALP) / GNData.FACT2;
		GNData.GAMMAL = 0.0;
	}
	GNData.GAMMAL1 = GNData.GAMMAL1 * (1.0 - GNData.Q19) + GNData.Q19*GNData.GAMMAL;
	GNRangePrediction();
}

void ReentryNumericalIntegrator::GNRangePrediction()
{
	double VBARS = GNData.VL * GNData.VL / (VSAT*VSAT);
	double COSG = 1.0 - GNData.GAMMAL * GNData.GAMMAL / 2.0;
	double E = sqrt(1.0 + (VBARS - 2.0)*COSG*COSG*VBARS);
	double ASKEP = 2.0*ATK*asin(VBARS*COSG*GNData.GAMMAL / E);
	double ASP1 = GNData.Q2 + GNData.Q3*GNData.VL;
	double ASPUP = ATK / RE * (HS / GNData.GAMMAL1)*log(GNData.A0*GNData.VL*GNData.VL/(GNData.Q7*GNData.V1*GNData.V1));
	double ASP3 = GNData.Q5*(GNData.Q6 - GNData.GAMMAL);
	double ASPDWN = -GNData.RDOT*GNData.v*ATK / (GNData.A0*LAD*RE);
	double ASP = ASKEP + ASP1 + ASPUP + ASP3 + ASPDWN;
	double DIFF = GNData.THETNM - ASP;
	if (abs(DIFF) - 25.0*1852.0 < 0.0)
	{
		GNData.SELECTOR = 3;
		GNUpcontrol();
		return;
	}
	if (GNData.HIND == false)
	{
		if (DIFF < 0)
		{
			GNData.DIFFOLD = DIFF;
			GNData.Q7 = GNData.Q7F;
			GNConstD();
			return;
		}
	}
	if (GNData.HUNTCN == 7)
	{
		GNRoutine380();
		return;
	}
	GNData.DLEWD = GNData.DLEWD*DIFF / (GNData.DIFFOLD - DIFF);
	if (GNData.LEWD + GNData.DLEWD < 0)
	{
		GNData.DLEWD = -GNData.LEWD / 2.0;
	}
	GNData.LEWD = GNData.LEWD + GNData.DLEWD;
	GNData.HIND = true;
	GNData.DIFFOLD = DIFF;
	GNData.Q7 = GNData.Q7F;
	GNData.HUNTCN++;
	GNHuntest();
}

void ReentryNumericalIntegrator::GNConstD()
{
	GNData.LD = -GNData.LEQ / GNData.D0 + C16 * (GNData.D - GNData.D0) - C17 * (GNData.RDOT + 2.0*HS* GNData.D0 / GNData.v);
	GNNegTest();
}

void ReentryNumericalIntegrator::GNNegTest()
{
	if (GNData.D - GNData.C20 > 0)
	{
		GNData.LATSW = false;
		if (GNData.LD < 0)
		{
			GNData.LD = 0;
		}
	}
	GNRoutine310();
}

void ReentryNumericalIntegrator::GNUpcontrol()
{
	if (GNData.D - 140.0*0.3048 > 0)
	{
		GNData.NOSWITCH = true;
	}
	if (GNData.v - GNData.V1 > 0)
	{
		double RDTR = LAD * (GNData.V1 - GNData.v);
		double DR = pow(GNData.v / GNData.V1, 2)*GNData.A0 - RDTR * RDTR / (2.0*HS*GNData.C1*LAD);
		GNData.LD = LAD + C16 * (GNData.D - DR) - C17 * (GNData.RDOT - RDTR);
		GNNegTest();
		return;
	}
	if (GNData.D - GNData.Q7 <= 0)
	{
		GNData.SELECTOR = 4;
		GNBallistic();
		return;
	}
	if (GNData.RDOT < 0 && GNData.v - GNData.VL - GNData.C18 < 0)
	{
		GNData.SELECTOR = 5;
		GNData.EGSW = true;
		GNFinalPhase();
		return;
	}
	if (GNData.A0 - GNData.D < 0)
	{
		GNData.LD = LAD;
		GNRoutine310();
		return;
	}
	double VREF = GNData.FACT1*(1.0 - sqrt(GNData.FACT2*GNData.D + GNData.ALP));
	double RDOTREF;
	if (VREF - GNData.VS1 > 0)
	{
		RDOTREF = GNData.LEWD*(GNData.V1 - VREF);
	}
	else
	{
		RDOTREF = GNData.LEWD*(GNData.V1 - VREF) - GNData.CH1*GS*pow(GNData.VS1 - VREF, 2)*(1.0 + GNData.AHOOK*(GNData.VS1 - VREF)) / (GNData.DHOOK*VREF);
	}
	if (GNData.D - GNData.Q7 >= 0)
	{
		GNData.FACTOR = (GNData.D - GNData.Q7) / (GNData.A1 - GNData.Q7);
	}
	double TEM1B = -GNData.KB2 * GNData.FACTOR*(GNData.KB1*GNData.FACTOR*(GNData.RDOT - RDOTREF) + GNData.v - VREF);
	if (abs(TEM1B) - GNData.POINT1 >= 0)
	{
		double sgn_tem1b;
		if (TEM1B >= 0)
		{
			sgn_tem1b = 1.0;
		}
		else
		{
			sgn_tem1b = -1.0;
		}
		TEM1B = (GNData.POINT1 + GNData.POINT1 * (abs(TEM1B) - GNData.POINT1))*sgn_tem1b;
	}
	GNData.LD = GNData.LEWD + TEM1B;
	GNNegTest();
}

void ReentryNumericalIntegrator::GNBallistic()
{
	if (GNData.K05GSW == false)
	{
		ROLLC = 0.0;
	}
	if (GNData.D - (GNData.Q7F + GNData.KDMIN) > 0)
	{
		GNData.EGSW = true;
		GNData.SELECTOR = 5;
		GNModeSelector();
		return;
	}
	//Maintain attitude control
	GNRoutine380();
}

void ReentryNumericalIntegrator::GNFinalPhase()
{
	if (GNData.v - GNData.VQUIT <= 0)
	{
		GNRoutine380();
		return;
	}

	int i;
	for (i = 1;i < 13;i++)
	{
		if (GNData.VREF_TAB[i] - GNData.v > 0)
		{
			break;
		}
	}
	double GRAD = (GNData.v - GNData.VREF_TAB[i - 1]) / (GNData.VREF_TAB[i] - GNData.VREF_TAB[i - 1]);
	double F1 = GNData.FA_TAB[i - 1] + GRAD * (GNData.FA_TAB[i] - GNData.FA_TAB[i - 1]);
	double F2 = GNData.FRDT_TAB[i - 1] + GRAD * (GNData.FRDT_TAB[i] - GNData.FRDT_TAB[i - 1]);
	double RDTRF = GNData.RDTRF_TAB[i - 1] + GRAD * (GNData.RDTRF_TAB[i] - GNData.RDTRF_TAB[i - 1]);
	double DREFR = GNData.AREF_TAB[i - 1] + GRAD * (GNData.AREF_TAB[i] - GNData.AREF_TAB[i - 1]);
	double PREDANG1 = GNData.RTOGO_TAB[i - 1] + GRAD * (GNData.RTOGO_TAB[i] - GNData.RTOGO_TAB[i - 1]);
	double P = GNData.PP_TAB[i - 1] + GRAD * (GNData.PP_TAB[i] - GNData.PP_TAB[i - 1]);
	double PREDANG2 = F1 * (GNData.D - DREFR);
	double PREDANG3 = F2 * (GNData.RDOT - RDTRF);
	double PREDANGL = PREDANG1 + PREDANG2 + PREDANG3;
	double X = (GNData.THETA*ATK - PREDANGL) / P;
	if (GNData.GONEPAST)
	{
		GNData.LD = -LAD;
		GNGLimiter();
		return;
	}
	if (GNData.GONEBY)
	{
		GNData.GONEPAST = true;
		GNData.LD = -LAD;
		GNGLimiter();
		return;
	}
	if (abs(GNData.K13P*X) - 1.0 > 0)
	{
		GNData.LD = LAD;
		if (X < 0)
		{
			GNData.LD = -GNData.LD;
		}
		GNGLimiter();
		return;
	}
	GNData.LD = LOD + GNData.K13P*X;
	if (abs(GNData.LD) - 1.0 > 0.0)
	{
		GNData.LD = LAD;
		if (X < 0)
		{
			GNData.LD = -GNData.LD;
		}
	}
	GNGLimiter();
	return;
}

void ReentryNumericalIntegrator::GNGLimiter()
{
	if (GNData.GMAX / 2.0 - GNData.D > 0)
	{
		GNRoutine310();
		return;
	}
	if (GNData.GMAX - GNData.D <= 0)
	{
		GNData.LD = LAD;
		GNRoutine310();
		return;
	}
	double X = sqrt(2.0*HS*(GNData.GMAX - GNData.D)*(GNData.LEQ / GNData.GMAX + LAD) + pow(2.0*HS*GNData.GMAX / GNData.v, 2));
	if (GNData.RDOT + X > 0)
	{
		GNRoutine310();
		return;
	}
	GNData.LD = LAD;
	GNRoutine310();
	return;
}

void ReentryNumericalIntegrator::GNRoutine310()
{
	GNData.LD1 = GNData.LD;
	if (GNData.GONEPAST == false)
	{
		GNData.Y = GNData.KLAT*GNData.VSQ + GNData.LATBIAS;
		if (abs(GNData.LD) - GNData.LD_CMINR < 0)
		{
			if (GNData.K2ROLL*GNData.LATANG - GNData.Y > 0)
			{
				if (GNData.NOSWITCH == false)
				{
					GNData.K2ROLL = -GNData.K2ROLL;
					//Light switch light?
				}
				else
				{
					GNData.NOSWITCH = false;
				}
			}
		}
		else
		{
			GNData.Y = GNData.Y / 2.0;
			if (GNData.K2ROLL*GNData.LATANG > 0)
			{
				if (GNData.K2ROLL*GNData.LATANG - GNData.Y > 0)
				{
					if (GNData.NOSWITCH == false)
					{
						GNData.K2ROLL = -GNData.K2ROLL;
						//Light switch light?
					}
					else
					{
						GNData.NOSWITCH = false;
					}
				}
			}
			else
			{
				double sgn_LD;
				if (GNData.LD >= 0)
				{
					sgn_LD = 1.0;
				}
				else
				{
					sgn_LD = -1.0;
				}
				GNData.LD1 = GNData.LD_CMINR*sgn_LD;
			}
		}
	}
	double LD1_LAD;
	LD1_LAD = GNData.LD1 / LAD;
	if (abs(LD1_LAD) - 1.0 > 0)
	{
		if (GNData.LD >= 0)
		{
			LD1_LAD = 1.0;
		}
		else
		{
			LD1_LAD = -1.0;
		}
	}
	ROLLC = GNData.K2ROLL*acos(LD1_LAD);
	GNRoutine380();
}

void ReentryNumericalIntegrator::GNRoutine380()
{
	GNData.SWTCH2 = 0.0;
	if (GNData.ROLLSW == false)
	{
		if (GNData.D - g_c_GN * GS > 0)
		{
			GNData.ROLLSW = true;
		}
		else
		{
			ROLLC = GNData.C10;
		}
	}
}

bool ReentryNumericalIntegrator::IsInSBandBlackout(double v_r, double h)
{
	double v_r_fps, h_ft, h_bo;

	v_r_fps = v_r / 0.3048;
	h_ft = h / 0.3048;

	//Curve fit based on Apollo 17 SCOT figure 4.17-8
	h_bo = -3.335751e-4*v_r_fps*v_r_fps + 25.79421769*v_r_fps - 157525.8344;

	if (h < h_bo)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ReentryNumericalIntegrator::IsInVHFBlackout(double v_r, double h)
{
	double v_r_fps, h_ft, h_bo;

	v_r_fps = v_r / 0.3048;
	h_ft = h / 0.3048;

	//Curve fit based on Apollo 7 SCOT figure 13
	h_bo = -3.030303e-4*v_r_fps*v_r_fps + 21.51515152*v_r_fps - 41818.18182;

	if (h < h_bo)
	{
		return true;
	}
	else
	{
		return false;
	}
}