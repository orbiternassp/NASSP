/****************************************************************************
This file is part of Project Apollo - NASSP

Coast Numerical Integrator, RTCC Module PMMCEN

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

#include "CoastNumericalIntegrator.h"
#include "OrbMech.h"
#include "rtcc.h"

const double CoastIntegrator2::K = 0.1;
const double CoastIntegrator2::dt_lim = 1000.0;

CoastIntegrator2::CoastIntegrator2(RTCC *r) : RTCCModule(r)
{
	P_S = 0;
	R_S = V_S = _V(0, 0, 0);
	T_S = 0.0;
}

CoastIntegrator2::~CoastIntegrator2()
{
}

bool CoastIntegrator2::Propagate(VECTOR3 R00, VECTOR3 V00, double gmt, double tmax, double tmin, double deltat, double dir, int planet, int stopcond)
{
	//Initialize
	t0 = gmt;
	R0 = R = R_CON = R00;
	V0 = V = V_CON = V00;
	TMAX = tmax;
	TMIN = tmin;
	STOPVA = deltat;
	HMULT = dir;

	SetBodyParameters(planet);
	ISTOPS = stopcond;

	delta = _V(0, 0, 0);
	nu = _V(0, 0, 0);
	x = 0;
	dt = dt_lim;
	//Normally 9 Er
	r_SPH = 9.0*OrbMech::R_Earth;
	if (ISTOPS > 2)
	{
		//Radius
		if (STOPVA > 8.0*OrbMech::R_Earth && STOPVA < 10.0*OrbMech::R_Earth)
		{
			//If end condition is radius and close to normal reference switch, use 14 Er instead
			r_SPH = 14.0*OrbMech::R_Earth;
		}
	}
	else if (ISTOPS < 2)
	{
		//Time
		//Set HMULT to desired propagation direction. Then start using absolute value of STOPVA
		if (STOPVA >= 0.0)
		{
			HMULT = abs(HMULT);
		}
		else
		{
			HMULT = -abs(HMULT);
		}

		STOPVA = abs(STOPVA);
		//In time mode, don't check on termination until we are close to done
		TMIN = STOPVA - 2.0*dt_lim;
	}

	if (ISTOPS == 2)
	{
		//Flight path angle
		//Still gives really accurate flight path angle
		DEV = 100.0;
	}
	else if (ISTOPS == 3)
	{
		//Radius
		if (STOPVA == 0.0)
		{
			DEV = 1.0;
		}
		else
		{
			DEV = STOPVA;
		}
	}
	else
	{
		//Time
		DEV = 1.0;
	}

	tau = TRECT = 0.0;
	INITF = false;
	INITE = 0;

	//Initialize forcing function
	adfunc(R0);

	do
	{
		Edit();
		if (IEND == 0)
		{
			Step();
		}
	} while (IEND == 0);

	R2 = R_CON + delta;
	V2 = V_CON + nu;
	T2 = CurrentTime();
	outplanet = P;
	ITS = ISTOPS;
	return true;
}

void CoastIntegrator2::Edit()
{
	double rr, dt_max;

	rr = length(R);
	//Bounded?
	if (INITE != 1)
	{
		if (P == BODY_MOON)
		{
			//Are we leaving the sphere of influence?
			if (rr > r_SPH)
			{
				VECTOR3 V_PQ;

				R_PQ = -R_EM;
				V_PQ = -V_EM;
				R_CON = R_CON - R_PQ;
				V_CON = V_CON - V_PQ;

				SetBodyParameters(BODY_EARTH);
				Rectification();
				//Reset bounding logic
				INITE = 0;
			}
		}
		else
		{
			VECTOR3 V_PQ;

			R_PQ = R_EM;
			R_QC = R - R_PQ;

			if (length(R_QC) < r_SPH)
			{
				V_PQ = V_EM;
				R_CON = R_CON - R_PQ;
				V_CON = V_CON - V_PQ;

				SetBodyParameters(BODY_MOON);
				Rectification();
				//Reset bounding logic
				INITE = 0;
			}
		}
		if (length(delta) / length(R_CON) > 0.01 || length(delta) > rect1 || length(nu) > rect2)
		{
			Rectification();
		}
	}

	//Termination control
PMMCEN_Edit_3B:
	TIME = abs(TRECT + tau);
	dt_max = min(dt_lim, K*OrbMech::power(rr, 1.5) / sqrt(mu));

	if (TMIN > TIME)
	{
		//Minimum time not reached
		dt_temp = HMULT * dt_max;
		goto PMMCEN_Edit_7B;
	}
	if (ISTOPS == 1)
	{
		FUNCT = TIME;
	}
	else if (ISTOPS == 2)
	{
		FUNCT = dotp(unit(R), unit(V));
	}
	else
	{
		FUNCT = length(R);
	}
	RCALC = FUNCT - STOPVA;
	IEND = ISTOPS;

	//Special time logic
	if (ISTOPS == 1)
	{
		dt = HMULT * min(abs(RCALC), dt_max);
		if (abs(dt) > 1e-6)
		{
			IEND = 0;
		}
		return;
	}
	//Other than time

	//Initial guess for dt
	dt_temp = HMULT * dt_max;

	//Termination check
	if (abs(RCALC / DEV) <= 1.e-12 || abs(dt) < 1e-6)
	{
		return;
	}

	if (INITE == 0)
	{
		//First Pass
		INITE = -1;
	}
	else if (INITE == -1)
	{
		//Not bounded
		if (RCALC*RES1 >= 0)
		{
			goto PMMCEN_Edit_4A;
		}

		//Found it. Go back to previous step
		RestoreVariables();
		VAR = dt;
		dt_temp = (VAR*RES1) / (RES1 - RCALC);
		RES2 = RCALC;

		INITE = 1;
		goto PMMCEN_Edit_7B;
	}
	else
	{
		//bounded
		goto PMMCEN_Edit_5C;
	}
PMMCEN_Edit_4A:
	//TMAX check
	if (TMAX <= abs(TRECT + tau))
	{
		//Now try to find TMAX
		ISTOPS = 1;
		STOPVA = TMAX;
		if (TMAX != 0.0)
		{
			RestoreVariables();
		}
		//Go back to find new dt
		goto PMMCEN_Edit_3B;
	}
	else
	{
		StoreVariables();
	}
	goto PMMCEN_Edit_7B;
PMMCEN_Edit_5C: //New step size
	//Calculate quadratic
	DEL = dt * VAR*VAR - VAR * dt*dt;
	AQ = (dt*(RES2 - RES1) - VAR * (RCALC - RES1)) / DEL;
	if (AQ == 0.0) goto PMMCEN_Edit_7A;
	BQ = (VAR*VAR*(RCALC - RES1) - dt * dt*(RES2 - RES1)) / DEL;
	DISQ = BQ * BQ - 4.0*AQ*RES1;
	if (DISQ < 0.0) goto PMMCEN_Edit_7A;
	DISQ = sqrt(DISQ);
	dtesc[0] = (-BQ + DISQ) / (2.0*AQ);
	dtesc[1] = (-BQ - DISQ) / (2.0*AQ);

	//Direction of solution?
	if (dt*dtesc[0] <= 0.0)
	{
		if (dt*dtesc[1] <= 0.0)
		{
			//Both solutions bad
			goto PMMCEN_Edit_7A;
		}
		else
		{
			//It's the other one
			dt_temp = dtesc[1];
		}
	}
	else
	{
		if (dt*dtesc[1] <= 0.0)
		{
			//The other one is bad, use this
			dt_temp = dtesc[0];
		}
		else
		{
			//Both solutions good in theory, use the closest one
			if (abs(dtesc[0]) < abs(dtesc[1]))
			{
				dt_temp = dtesc[0];
			}
			else
			{
				dt_temp = dtesc[1];
			}
		}
	}

	VAR = dt;
	RestoreVariables();
	RES2 = RCALC;
	goto PMMCEN_Edit_7B;
PMMCEN_Edit_7A:
	//sprintf(oapiDebugString(), "PMMCEN: How did we get here?");
	//Chord method. Needs work.
	//Was the last step a step in the right direction?
	if (RCALC*RES2 > 0)
	{
		//No, go backwards
		VAR = dt;
		dt_temp = -dt / 2.0;
		RES2 = RCALC;
		Rectification();
	}
	else
	{
		VAR = VAR - dt;
		dt_temp = VAR / 2.0;
		StoreVariables();
	}
PMMCEN_Edit_7B: //Don't stop yet
	dt = dt_temp;
	IEND = 0;
	return;
}

void CoastIntegrator2::Rectification()
{
	TRECT = TRECT + tau;
	R0 = R_CON + delta;
	V0 = V_CON + nu;
	R = R_CON = R0;
	V = V_CON = V0;
	delta = _V(0, 0, 0);
	nu = _V(0, 0, 0);
	x = 0;
	tau = 0;
	//Re-initialize U_Z vector
	INITF = false;
}

void CoastIntegrator2::Step()
{
	VECTOR3 R_apo, V_apo, alpha, a_d, ff, k[3];
	double h, x_apo, gamma, s, alpha_N, x_t;

	h = 0;
	alpha = delta;
	R_apo = R_CON;
	V_apo = V_CON;
	x_apo = x;
	for (int j = 0; j < 3; j++)
	{
		R = R_CON + alpha;
		a_d = adfunc(R);
		ff = f(alpha, R, a_d);
		k[j] = ff;
		if (j < 2)
		{
			h = h + 0.5*dt;
			alpha = delta + (nu + ff * h*0.5)*h;
			tau = tau + 0.5*dt;
			s = sqrt(mu) / length(R_apo)*0.5*dt;
			gamma = dotp(R_apo, V_apo) / (length(R_apo)*sqrt(mu)*2.0);
			alpha_N = 2.0 / length(R0) - OrbMech::power(length(V0), 2.0) / mu;
			x_t = x_apo + s * (1.0 - gamma * s*(1.0 - 2.0 * gamma*s) - 1.0 / 6.0 * (1.0 / length(R_apo) - alpha_N)*s*s);
			OrbMech::rv_from_r0v0(R0, V0, tau, R_CON, V_CON, mu, x_t);
		}
	}
	delta = delta + (nu + (k[0] + k[1] * 2.0)*dt*1.0 / 6.0)*dt;
	nu = nu + (k[0] + k[1] * 4.0 + k[2]) * 1.0 / 6.0 *dt;

	R = R_CON + delta;
	V = V_CON + nu;
}

VECTOR3 CoastIntegrator2::f(VECTOR3 alpha, VECTOR3 R, VECTOR3 a_d)
{
	VECTOR3 R_CON;
	double q;

	R_CON = R - alpha;
	q = dotp((alpha - R * 2.0), alpha) / (OrbMech::power(length(R), 2.0));
	return -(R*fq(q) + alpha)*mu / OrbMech::power(length(R_CON), 3.0) + a_d;
}

double CoastIntegrator2::fq(double q)
{
	return q * (3.0 + 3.0 * q + q * q) / (1.0 + OrbMech::power(1 + q, 1.5));
}

VECTOR3 CoastIntegrator2::adfunc(VECTOR3 R)
{
	double r, costheta, P2, P3, P4, P5;
	VECTOR3 U_R, a_dP, a_d, a_dQ, a_dS;

	a_dP = a_dQ = a_dS = _V(0, 0, 0);

	if (INITF == false || tau != TS)
	{
		if (INITF == false)
		{
			INITF = true;
			//MATRIX3 Mat_J_B = SystemParameters.MAT_J2000_BRCS;
			MATRIX3 obli = OrbMech::GetObliquityMatrix(P, pRTCC->GetGMTBase() + CurrentTime() / 24.0 / 3600.0);
			//Convert unit z-axis vector to ecliptic
			U_Z = rhmul(obli, _V(0, 0, 1));
			//TBD: Use this in the future
			//U_Z = mul(Mat_J_B, rhmul(obli, _V(0, 0, 1)));
		}

		TS = tau;
		pRTCC->PLEFEM(1, CurrentTime() / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);
	}

	r = length(R);

	//Only calculate perturbations if we are above surface of primary body
	if (r > R_E)
	{
		if (r < r_dP)
		{
			U_R = unit(R);
			costheta = dotp(U_R, U_Z);

			P2 = 3.0 * costheta;
			P3 = 0.5*(15.0*costheta*costheta - 3.0);

			if (P == BODY_EARTH)
			{
				a_dP += (U_R*P3 - U_Z * P2)*OrbMech::J2_Earth * OrbMech::power(R_E / r, 2.0);
				P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
				a_dP += (U_R*P4 - U_Z * P3)*OrbMech::J3_Earth * OrbMech::power(R_E / r, 3.0);
				P5 = 0.25*(9.0*costheta*P4 - 5.0 * P3);
				a_dP += (U_R*P5 - U_Z * P4)*OrbMech::J4_Earth * OrbMech::power(R_E / r, 4.0);
			}
			else
			{
				a_dP += (U_R*P3 - U_Z * P2)*OrbMech::J2_Moon * OrbMech::power(R_E / r, 2.0);
			}

			a_dP *= mu / OrbMech::power(r, 2.0);
		}

		VECTOR3 R_PS, R_SC;
		double q_Q, q_S;

		if (P == BODY_EARTH)
		{
			R_PQ = R_EM;
			R_PS = R_ES;
		}
		else
		{
			R_PQ = -R_EM;
			R_PS = R_ES - R_EM;
		}
		R_QC = R - R_PQ;
		R_SC = R - R_PS;

		q_Q = dotp(R - R_PQ * 2.0, R) / OrbMech::power(length(R_PQ), 2.0);
		q_S = dotp(R - R_PS * 2.0, R) / OrbMech::power(length(R_PS), 2.0);
		a_dQ = -(R_PQ*fq(q_Q) + R)*mu_Q / OrbMech::power(length(R_QC), 3.0);
		a_dS = -(R_PS*fq(q_S) + R)*OrbMech::mu_Sun / OrbMech::power(length(R_SC), 3.0);
	}
	a_d = a_dP + a_dQ + a_dS;
	return a_d;
}

void CoastIntegrator2::SetBodyParameters(int p)
{
	if (p == BODY_EARTH)
	{
		r_dP = 80467200.0;
		mu = OrbMech::mu_Earth;
		mu_Q = OrbMech::mu_Moon;
		R_E = OrbMech::R_Earth;
		rect1 = 0.75*OrbMech::power(2.0, 21.0);
		rect2 = 0.75*OrbMech::power(2.0, 2.0)*100.0;
		P = BODY_EARTH;
	}
	else
	{
		r_dP = 16093440.0;
		mu = OrbMech::mu_Moon;
		mu_Q = OrbMech::mu_Earth;
		R_E = OrbMech::R_Moon;
		rect1 = 0.75*OrbMech::power(2.0, 17.0);
		rect2 = 0.75*OrbMech::power(2.0, -2.0)*100.0;
		P = BODY_MOON;
	}
}

void CoastIntegrator2::StoreVariables()
{
	P_S = P;
	R_S = R_CON + delta;
	V_S = V_CON + nu;
	T_S = TRECT + tau;
	RES1 = RCALC;
}

void CoastIntegrator2::RestoreVariables()
{
	R_CON = R_S;
	V_CON = V_S;
	TRECT = T_S;
	if (P != P_S)
	{
		SetBodyParameters(P_S);
		INITF = false;
		adfunc(R_CON);
	}
	tau = 0.0;
	delta = nu = _V(0, 0, 0);
	Rectification();
}

double CoastIntegrator2::CurrentTime()
{
	return (t0 + TRECT + tau);
}

CoastIntegrator3::CoastIntegrator3(RTCC *r) : RTCCModule(r)
{
	EPSQR = 0.0001;
}

CoastIntegrator3::~CoastIntegrator3()
{
	mu_S = OrbMech::mu_Sun;
}

bool CoastIntegrator3::Propagate(VECTOR3 R00, VECTOR3 V00, double gmt, double tmax, double tmin, double deltat, double dir, int planet, int stopcond)
{
	//Initialize
	t0 = gmt;
	RBASE = R00;
	VBASE = V00;
	TMAX = tmax;
	TMIN = tmin;
	STOPVA = deltat;
	HMULT = dir;

	HBETA[1] = HMULT * 0.03125*sqrt(6378165.0);
	HBETA[0] = HMULT * 0.0625*sqrt(6378165.0);

	SetBodyParameters(planet);
	ISTOPS = stopcond;

	COEF3 = sqrt(dotp(RBASE, RBASE) / mu);
	VBASE = VBASE * COEF3;
	//Normally 9 Er
	r_SPH = 9.0*OrbMech::R_Earth;
	if (ISTOPS > 2)
	{
		//Radius
		if (STOPVA > 8.0*OrbMech::R_Earth && STOPVA < 10.0*OrbMech::R_Earth)
		{
			//If end condition is radius and close to normal reference switch, use 14 Er instead
			r_SPH = 14.0*OrbMech::R_Earth;
		}
	}
	else if (ISTOPS < 2)
	{
		//Time
		//Set HMULT to desired propagation direction. Then start using absolute value of STOPVA
		if (STOPVA >= 0.0)
		{
			HMULT = abs(HMULT);
		}
		else
		{
			HMULT = -abs(HMULT);
		}

		STOPVA = abs(STOPVA);
		//In time mode, don't check on termination until we are close to done
		TMIN = STOPVA - 2.0*3600.0*HMULT;
	}

	if (ISTOPS == 2)
	{
		//Flight path angle
		DEV = 1.0;
	}
	else if (ISTOPS == 3)
	{
		//Radius
		DEV = 6.378e6;
	}
	else
	{
		//Time
		DEV = 3600.0;
	}

	Y = YP = _V(0, 0, 0);
	T = TRECT = 0.0;
	HP = HD2 = H2D2 = H2D8 = HD6 = 0.0;
	INITF = false;
	INITE = 0;
	ISTART = 0;

	//Initialize forcing function
	PCCIFF();

	do
	{
		PCCIED();
		if (IEND == 0)
		{
			PCCIGJ();
		}
	} while (IEND == 0);

	R_out = R;
	V_out = V;
	T_out = CurrentTime();
	outplanet = P;
	ITS = ISTOPS;
	return true;
}

void CoastIntegrator3::PCCIED()
{
	//Bounded?
	if (INITE != 1)
	{
		if (P == BODY_MOON)
		{
			//Are we leaving the sphere of influence?
			if (PWRM > r_SPH)
			{
				R = R + R_EM;
				V = V + V_EM;

				SetBodyParameters(BODY_EARTH);
				Rectification();
				//Reset bounding logic
				INITE = 0;
			}
		}
		else
		{
			if (PWRM < r_SPH)
			{
				R = R - R_EM;
				V = V - V_EM;

				SetBodyParameters(BODY_MOON);
				Rectification();
				//Reset bounding logic
				INITE = 0;
			}
		}

		//Rectification
		if (dotp(Y, Y) / CBODY2 - EPSQR >= 0.0)
		{
			Rectification();
		}
		else
		{
			TEMP = dotp(YP, YP);
			TEMP = TEMP * COEF3*COEF3;
			if (TEMP / dotp(V, V) - EPSQR >= 0.0)
			{
				Rectification();
			}
		}
	}

	//Termination control
PMMCEN_Edit_3B:
	TIME = abs(TRECT + DELT);

	if (TMIN > TIME)
	{
		goto PMMCEN_Edit_7B;
	}
	if (ISTOPS == 1)
	{
		FUNCT = TIME;
	}
	else if (ISTOPS == 2)
	{
		FUNCT = dotp(unit(R), unit(V));
	}
	else
	{
		FUNCT = length(R);
	}
	RCALC = FUNCT - STOPVA;
	IEND = ISTOPS;

	//Termination check
	if (abs(RCALC / DEV) <= 1.e-7 || abs(H) < 2.5e-7)
	{
		return;
	}

	if (INITE == 0)
	{
		//First Pass
		INITE = -1;
	}
	else if (INITE == -1)
	{
		//Not bounded
		if (RCALC*RES1 >= 0)
		{
			goto PMMCEN_Edit_4A;
		}

		//Found it. Go back to previous step
		RestoreVariables();
		VAR = H;
		H = (VAR*RES1) / (RES1 - RCALC);
		RES2 = RCALC;

		INITE = 1;
		goto PMMCEN_Edit_7B;
	}
	else
	{
		//bounded
		goto PMMCEN_Edit_5C;
	}
PMMCEN_Edit_4A:
	//TMAX check
	if (TMAX <= abs(TRECT + DELT))
	{
		//Now try to find TMAX
		ISTOPS = 1;
		STOPVA = TMAX;
		if (TMAX != 0.0)
		{
			RestoreVariables();
		}
		//Go back to find new dt
		goto PMMCEN_Edit_3B;
	}
	else
	{
		StoreVariables();
	}
	goto PMMCEN_Edit_7B;
PMMCEN_Edit_5C: //New step size
	//Calculate quadratic
	DEL = H * VAR*VAR - VAR * H*H;
	AQ = (H*(RES2 - RES1) - VAR * (RCALC - RES1)) / DEL;
	if (AQ == 0.0) goto PMMCEN_Edit_7A;
	BQ = (VAR*VAR*(RCALC - RES1) - H * H*(RES2 - RES1)) / DEL;
	DISQ = BQ * BQ - 4.0*AQ*RES1;
	if (DISQ < 0.0) goto PMMCEN_Edit_7A;
	DISQ = sqrt(DISQ);
	htesc[0] = (-BQ + DISQ) / (2.0*AQ);
	htesc[1] = (-BQ - DISQ) / (2.0*AQ);
	H_temp = H;
	//Direction of solution?
	if (H*htesc[0] <= 0.0)
	{
		if (H*htesc[1] <= 0.0)
		{
			//Both solutions bad
			goto PMMCEN_Edit_7A;
		}
		else
		{
			//It's the other one
			H = htesc[1];
		}
	}
	else
	{
		if (H*htesc[1] <= 0.0)
		{
			//The other one is bad, use this
			H = htesc[0];
		}
		else
		{
			//Both solutions good in theory, use the closest one
			if (abs(htesc[0]) < abs(htesc[1]))
			{
				H = htesc[0];
			}
			else
			{
				H = htesc[1];
			}
		}
	}

	VAR = H_temp;
	RestoreVariables();
	RES2 = RCALC;
	goto PMMCEN_Edit_7B;
PMMCEN_Edit_7A:
	//sprintf(oapiDebugString(), "PMMCEN: How did we get here?");
	//Chord method. Needs work.
	//Was the last step a step in the right direction?
	if (RCALC*RES2 > 0)
	{
		//No, go backwards
		VAR = H;
		H = -H / 2.0;
		RES2 = RCALC;
		Rectification();
	}
	else
	{
		VAR = VAR - H;
		H = VAR / 2.0;
		StoreVariables();
	}
PMMCEN_Edit_7B: //Don't stop yet
	IEND = 0;
	return;
}

void CoastIntegrator3::PCCIGJ()
{
	if (ISTART == 0)
	{
		//Restart requested - inizialize
		H2 = H * H;
		//Runge-Kutta step size is always 1/4 H
		HRK = H / 4.0;
	}
	//Save acceleration for this starter line
	ISTART++;

	//4 calls to Runge Kutta
	PCCIRK();
	PCCIRK();
	PCCIRK();
	PCCIRK();
}

void CoastIntegrator3::PCCIRK()
{
	//Start
	if (HRK != HP)
	{
		HD2 = HRK / 2.0;
		H2D2 = HD2 * HRK;
		H2D8 = H2D2 / 4.0;
		HD6 = HRK / 6.0;
		HP = HRK;
	}
	//Save base and build state for 2nd derivative (2nd term)
	F1 = YPP;
	YS = Y;
	YPS = YP;
	Y = YS + YPS * HD2 + F1 * H2D8;
	YP = YPS + F1 * HD2;
	T = T + HD2;
	//Get 2nd derivative (F2)
	PCCIFF();
	//Save F2 and build state for 2nd derivative evaluation F3
	F2 = YPP;
	YP = YPS + F2 * HD2;
	//Save F3 and build state for F4 evaluation
	PCCIFF();
	F3 = YPP;
	Y = YS + YPS * HRK + F3 * H2D2;
	YP = YPS + F3 * HRK;
	T = T + HD2;
	//Get 2nd derivative F4
	PCCIFF();
	//Weighted sum for state at T + HRK
	Y = YS + (YPS + (F1 + F2 + F3)*HD6)*HRK;
	YP = YPS + (F1 + (F2 + F3)*2.0 + YPP)*HD6;
	//Final acceleration
	PCCIFF();
}

double CoastIntegrator3::fq(double q)
{
	return q * (3.0 + 3.0 * q + q * q) / (1.0 + pow(1.0 + q, 1.5));
}

void CoastIntegrator3::PCCIFF()
{
	double costheta, P2, P3, P4, P5;
	VECTOR3 U_R;

	if (INITF == false || T != TS)
	{
		if (INITF == false)
		{
			INITF = true;
			//MATRIX3 Mat_J_B = SystemParameters.MAT_J2000_BRCS;
			MATRIX3 obli = OrbMech::GetObliquityMatrix(P, pRTCC->GetGMTBase() + CurrentTime() / 24.0 / 3600.0);
			//Convert unit z-axis vector to ecliptic
			U_Z = rhmul(obli, _V(0, 0, 1));
			//TBD: Use this in the future
			//U_Z = mul(Mat_J_B, rhmul(obli, _V(0, 0, 1)));

			//Set up base state for 2-body computation
			RBASE2 = dotp(RBASE, RBASE);
			RBASE1 = sqrt(RBASE2);
			TEMP = dotp(RBASE, VBASE);
			VBASE2 = dotp(VBASE, VBASE);
			//Compute constants for 2-body with beta as indep. variable
			DOT = TEMP / RBASE2;
			OVERA = VBASE2 / RBASE2 - 2.0 / RBASE1;
			ROVERA = 1.0 + OVERA * RBASE1;
		}

		TS = T;

		pRTCC->PIBETA(T, OVERA, F);
		FF = 1.0 - F[1] / RBASE1;
		GG = F[2] + F[1] * DOT;
		FDOT = -F[2] / RBASE1;
		GDOT = F[3] + F[2] * DOT;
		DELT = (GG*RBASE1 + F[0]) / SQRTMU;

		RTBMAG = GDOT * RBASE1 + F[1];
		RTB = RBASE * FF + VBASE * GG;
		RDTB = RBASE * FDOT + VBASE * GDOT;
		RTBMAG2 = RTBMAG * RTBMAG;
		RTBMAG3 = RTBMAG * RTBMAG2;

		COEF1 = (ROVERA*F[2] + DOT * RBASE1*F[3]) / RTBMAG;
		COEF2 = RTBMAG2 / mu;
		COEF3 = SQRTMU / RTBMAG;

		pRTCC->PLEFEM(1, CurrentTime() / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);
	}

	R = Y + RTB;
	V = YP + RDTB;
	//Convert to time units
	V = V * COEF3;

	//Position vector parameters
	CBODY2 = dotp(R, R);
	CBODY1 = sqrt(CBODY2);

	if (P == BODY_EARTH)
	{
		PWRM = length(R - R_EM);
	}
	else
	{
		PWRM = CBODY1;
	}

	//Only calculate perturbations if we are above surface of primary body
	if (CBODY1 > R_E)
	{
		TT[3] = _V(0, 0, 0);

		if (CBODY1 < r_dP)
		{
			U_R = unit(R);
			costheta = dotp(U_R, U_Z);

			P2 = 3.0 * costheta;
			P3 = 0.5*(15.0*costheta*costheta - 3.0);

			if (P == BODY_EARTH)
			{
				TT[3] += (U_R*P3 - U_Z * P2)*OrbMech::J2_Earth * OrbMech::power(R_E / CBODY1, 2.0);
				P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
				TT[3] += (U_R*P4 - U_Z * P3)*OrbMech::J3_Earth * OrbMech::power(R_E / CBODY1, 3.0);
				P5 = 0.25*(9.0*costheta*P4 - 5.0 * P3);
				TT[3] += (U_R*P5 - U_Z * P4)*OrbMech::J4_Earth * OrbMech::power(R_E / CBODY1, 4.0);
			}
			else
			{
				TT[3] += (U_R*P3 - U_Z * P2)*OrbMech::J2_Moon * OrbMech::power(R_E / CBODY1, 2.0);
			}

			TT[3] *= mu / OrbMech::power(CBODY1, 2.0);
		}

		VECTOR3 R_PS, R_SC, R_PQ, R_QC;
		double q;

		if (P == BODY_EARTH)
		{
			R_PQ = R_EM;
			R_PS = R_ES;
		}
		else
		{
			R_PQ = -R_EM;
			R_PS = R_ES - R_EM;
		}
		R_QC = R - R_PQ;
		R_SC = R - R_PS;


		q = dotp(Y - R * 2.0, Y) / CBODY2;
		TT[0] = -(R*fq(q) + Y)*mu / RTBMAG3;
		q = dotp(R - R_PQ * 2.0, R) / OrbMech::power(length(R_PQ), 2.0);
		TT[1] = -(R_PQ*fq(q) + R)*mu_Q / OrbMech::power(length(R_QC), 3.0);
		q = dotp(R - R_PS * 2.0, R) / OrbMech::power(length(R_PS), 2.0);
		TT[2] = -(R_PS*fq(q) + R)*OrbMech::mu_Sun / OrbMech::power(length(R_SC), 3.0);

		YPP = TT[0] + TT[1] + TT[2] + TT[3];
	}
	else
	{
		YPP = _V(0, 0, 0);
	}

	//Convert to beta units
	YPP = YPP * COEF2 + YP * COEF1;
}

void CoastIntegrator3::SetBodyParameters(int p)
{
	if (p == BODY_EARTH)
	{
		r_dP = 80467200.0;
		mu = OrbMech::mu_Earth;
		mu_Q = OrbMech::mu_Moon;
		R_E = OrbMech::R_Earth;
		P = BODY_EARTH;
		H = HBETA[0];
	}
	else
	{
		r_dP = 16093440.0;
		mu = OrbMech::mu_Moon;
		mu_Q = OrbMech::mu_Earth;
		R_E = OrbMech::R_Moon;
		P = BODY_MOON;
		H = HBETA[1];
	}
	SQRTMU = sqrt(mu);
	ISTART = 0;
}

void CoastIntegrator3::StoreVariables()
{
	P_S = P;
	SAVET = T;
	SRTB = RTB;
	SY = Y;
	SYP = V;
	SDELT = DELT;
	STRECT = TRECT;
	RES1 = RCALC;
}

void CoastIntegrator3::RestoreVariables()
{
	if (P != P_S)
	{
		SetBodyParameters(P_S);
	}

	R = SRTB + SY;
	V = SYP;
	TRECT = STRECT;
	DELT = SDELT;

	Rectification();
}

void CoastIntegrator3::Rectification()
{
	RBASE = R;
	Y = YP = _V(0, 0, 0);
	COEF3 = sqrt(dotp(RBASE, RBASE) / mu);
	VBASE = V * COEF3;
	TRECT = TRECT + DELT;
	T = DELT = 0.0;
	//Re-initialize U_Z vector
	INITF = false;
	PCCIFF();
	ISTART = 0;
}

double CoastIntegrator3::CurrentTime()
{
	return (t0 + TRECT + DELT);
}