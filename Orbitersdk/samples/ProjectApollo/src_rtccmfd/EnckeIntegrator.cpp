/****************************************************************************
This file is part of Project Apollo - NASSP

Encke Numerical Free Flight Integrator, RTCC Module EMMENI

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

#include "EnckeIntegrator.h"
#include "OrbMech.h"
#include "rtcc.h"

const double EnckeFreeFlightIntegrator::K = 0.1;
const double EnckeFreeFlightIntegrator::dt_lim = 1000.0;

EnckeFreeFlightIntegrator::EnckeFreeFlightIntegrator(RTCC *r) : RTCCModule(r)
{
	for (int i = 0;i < 4;i++)
	{
		pEph[i] = NULL;
	}
}

EnckeFreeFlightIntegrator::~EnckeFreeFlightIntegrator()
{
}

void EnckeFreeFlightIntegrator::Propagate(EMSMISSInputTable &in)
{
	//Initialize
	t0 = in.AnchorVector.GMT;
	R0 = R = R_CON = in.AnchorVector.R;
	V0 = V = V_CON = in.AnchorVector.V;
	TMAX = in.MaxIntegTime;
	STOPVAE = in.EarthRelStopParam;
	STOPVAM = in.MoonRelStopParam;
	HMULT = in.IsForwardIntegration;

	SetBodyParameters(in.AnchorVector.RBI);
	ISTOPS = in.CutoffIndicator;
	StopParamRefFrame = in.StopParamRefFrame;

	bStoreEphemeris[0] = in.ECIEphemerisIndicator;
	bStoreEphemeris[1] = in.ECTEphemerisIndicator;
	bStoreEphemeris[2] = in.MCIEphemerisIndicator;
	bStoreEphemeris[3] = in.MCTEphemerisIndicator;
	pEph[0] = in.ECIEphemTableIndicator;
	pEph[1] = in.ECTEphemTableIndicator;
	pEph[2] = in.MCIEphemTableIndicator;
	pEph[3] = in.MCTEphemTableIndicator;
	EphemerisBuildIndicator = in.EphemerisBuildIndicator;

	delta = _V(0, 0, 0);
	nu = _V(0, 0, 0);
	x = 0;
	dt = dt_lim;
	//Normally 9 Er
	r_SPH = 9.0*OrbMech::R_Earth;
	if (ISTOPS == 2)
	{
		//If we want to find a reference switch, set SOI to 14 Er, Moon relative stop variable to actual SOI (9 Er) and stop reference to Moon only
		r_SPH = 14.0*OrbMech::R_Earth;
		STOPVAM = 9.0*OrbMech::R_Earth;
		StopParamRefFrame = 1;
	}
	//If we want to find a radius relative to Moon and it's between 8 and 10 Er, set SOI to 14 Er
	if (ISTOPS == 1 && StopParamRefFrame > 0 && (STOPVAM > 8.0*OrbMech::R_Earth && STOPVAM < 10.0*OrbMech::R_Earth))
	{
		r_SPH = 14.0*OrbMech::R_Earth;
	}

	if (ISTOPS == 1 || ISTOPS == 2 || ISTOPS == 3)
	{
		//1 meter tolerance for radius and height
		DEV = 1.0;
	}
	else if (ISTOPS == 4)
	{
		//0.0001° tolerance
		DEV = 0.0001*RAD;
	}
	else
	{
		//Doesn't matter
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
		if (INITE != 1)
		{
			EphemerisStorage();
		}
		if (IEND == -1)
		{
			Step();
		}
	} while (IEND == -1);

	EphemerisStorage();
	WriteEphemerisHeader();

	in.NIAuxOutputTable.sv_cutoff.R = R_CON + delta;
	in.NIAuxOutputTable.sv_cutoff.V = V_CON + nu;
	in.NIAuxOutputTable.sv_cutoff.GMT = CurrentTime();
	in.NIAuxOutputTable.sv_cutoff.RBI = P;
	in.NIAuxOutputTable.TerminationCode = ISTOPS;
}

void EnckeFreeFlightIntegrator::Edit()
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
EMMENI_Edit_3B:
	TIME = abs(TRECT + tau);
	dt_max = min(dt_lim, K*OrbMech::power(rr, 1.5) / sqrt(mu));

	//Should we even check?
	if (ISTOPS == 0)
	{
		RCALC = TIME - TMAX;
	}
	else
	{
		RCALC = 1000000000.0;
	}
	if (ISTOPS > 0 && (StopParamRefFrame == 2 || P == StopParamRefFrame))
	{
		if (ISTOPS == 1 || ISTOPS == 2)
		{
			FUNCT = length(R);
			if (P == BODY_EARTH)
			{
				RCALC = FUNCT - STOPVAE;
			}
			else
			{
				RCALC = FUNCT - STOPVAM;
			}
		}
		else if (ISTOPS == 3)
		{
			if (P == BODY_EARTH)
			{
				FUNCT = length(R) - OrbMech::R_Earth;
				RCALC = FUNCT - STOPVAE;
			}
			else
			{
				FUNCT = length(R) - pRTCC->BZLAND.rad[RTCC_LMPOS_BEST];
				RCALC = FUNCT - STOPVAM;
			}
		}
		else if (ISTOPS == 4)
		{
			FUNCT = dotp(unit(R), unit(V));
			if (P == BODY_EARTH)
			{
				RCALC = FUNCT - STOPVAE;
			}
			else
			{
				RCALC = FUNCT - STOPVAM;
			}
		}
	}

	IEND = ISTOPS;

	//Special time logic
	if (ISTOPS == 0)
	{
		dt = HMULT * min(abs(RCALC), dt_max);
		if (abs(dt) > 1e-6)
		{
			IEND = -1;
		}
		return;
	}
	//Other than time

	//Initial guess for dt
	dt_temp = HMULT * dt_max;

	//Termination check
	if (abs(RCALC / DEV) <= 1.0 || abs(dt) < 1e-6)
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
			goto EMMENI_Edit_4A;
		}

		//Found it. Go back to previous step
		RestoreVariables();
		VAR = dt;
		dt_temp = (VAR*RES1) / (RES1 - RCALC);
		RES2 = RCALC;

		INITE = 1;
		goto EMMENI_Edit_7B;
	}
	else
	{
		//bounded
		goto EMMENI_Edit_5C;
	}

EMMENI_Edit_4A:
	//TMAX check
	if (TMAX <= abs(TRECT + tau))
	{
		//Now try to find TMAX
		ISTOPS = 0;
		RestoreVariables();
		//Go back to find new dt
		goto EMMENI_Edit_3B;
	}
	else
	{
		StoreVariables();
	}
	goto EMMENI_Edit_7B;
EMMENI_Edit_5C: //New step size
	//Calculate quadratic
	DEL = dt * VAR*VAR - VAR * dt*dt;
	AQ = (dt*(RES2 - RES1) - VAR * (RCALC - RES1)) / DEL;
	if (AQ == 0.0) goto EMMENI_Edit_7A;
	BQ = (VAR*VAR*(RCALC - RES1) - dt * dt*(RES2 - RES1)) / DEL;
	DISQ = BQ * BQ - 4.0*AQ*RES1;
	if (DISQ < 0.0) goto EMMENI_Edit_7A;
	DISQ = sqrt(DISQ);
	dtesc[0] = (-BQ + DISQ) / (2.0*AQ);
	dtesc[1] = (-BQ - DISQ) / (2.0*AQ);

	//Direction of solution?
	if (dt*dtesc[0] >= 0.0)
	{
		//Direction of solution is good, which one is closer to initial state?
		if (abs(dtesc[0]) < abs(dtesc[1]))
		{
			dt_temp = dtesc[0];
		}
		else
		{
			dt_temp = dtesc[1];
		}
	}
	else
	{
		dt_temp = dtesc[1];
	}
	VAR = dt;
	RestoreVariables();
	RES2 = RCALC;
	goto EMMENI_Edit_7B;
EMMENI_Edit_7A:
	sprintf(oapiDebugString(), "EMMENI: How did we get here?");
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
EMMENI_Edit_7B: //Don't stop yet
	dt = dt_temp;
	IEND = -1;
	return;
}

void EnckeFreeFlightIntegrator::Rectification()
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

void EnckeFreeFlightIntegrator::Step()
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

VECTOR3 EnckeFreeFlightIntegrator::f(VECTOR3 alpha, VECTOR3 R, VECTOR3 a_d)
{
	VECTOR3 R_CON;
	double q;

	R_CON = R - alpha;
	q = dotp((alpha - R * 2.0), alpha) / (OrbMech::power(length(R), 2.0));
	return -(R*fq(q) + alpha)*mu / OrbMech::power(length(R_CON), 3.0) + a_d;
}

double EnckeFreeFlightIntegrator::fq(double q)
{
	return q * (3.0 + 3.0 * q + q * q) / (1.0 + OrbMech::power(1 + q, 1.5));
}

VECTOR3 EnckeFreeFlightIntegrator::adfunc(VECTOR3 R)
{
	double r, costheta, P2, P3, P4, P5;
	VECTOR3 U_R, a_dP, a_d, a_dQ, a_dS;

	a_dP = a_dQ = a_dS = _V(0, 0, 0);

	if (INITF == false || tau != TS)
	{
		if (INITF == false)
		{
			INITF = true;
			MATRIX3 obli = OrbMech::GetObliquityMatrix(P, pRTCC->GetGMTBase() + CurrentTime() / 24.0 / 3600.0);
			U_Z = mul(obli, _V(0, 1, 0));
			U_Z = _V(U_Z.x, U_Z.z, U_Z.y);
		}

		TS = tau;
		pRTCC->PLEFEM(1, CurrentTime() / 3600.0, 0, R_EM, V_EM, R_ES);
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

void EnckeFreeFlightIntegrator::SetBodyParameters(int p)
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

void EnckeFreeFlightIntegrator::StoreVariables()
{
	P_S = P;
	R_S = R_CON + delta;
	V_S = V_CON + nu;
	T_S = TRECT + tau;
	RES1 = RCALC;
}

void EnckeFreeFlightIntegrator::RestoreVariables()
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

double EnckeFreeFlightIntegrator::CurrentTime()
{
	return (t0 + TRECT + tau);
}

void EnckeFreeFlightIntegrator::EphemerisStorage()
{
	if (EphemerisBuildIndicator == false) return;

	EphemerisData sv, sv_out;
	EphemerisData2 sv2;
	int in;

	for (int i = 0;i < 4;i++)
	{
		if (bStoreEphemeris[i] && pEph[i])
		{
			if (pEph[i]->table.size() == 0 || abs(pEph[i]->table.back().GMT - CurrentTime()) > 3.6e-11)
			{
				if (P == BODY_EARTH)
				{
					in = 0;
				}
				else
				{
					in = 2;
				}
				
				sv.R = R_CON + delta;
				sv.V = V_CON + nu;
				sv.GMT = CurrentTime();
				sv.RBI = P;
				pRTCC->ELVCNV(sv, in, i, sv_out);
				sv2.R = sv_out.R;
				sv2.V = sv_out.V;
				sv2.GMT = sv_out.GMT;

				pEph[i]->table.push_back(sv2);
			}
		}
	}
}

void EnckeFreeFlightIntegrator::WriteEphemerisHeader()
{
	if (EphemerisBuildIndicator == false) return;

	for (int i = 0;i < 4;i++)
	{
		if (bStoreEphemeris[i] && pEph[i])
		{
			pEph[i]->Header.CSI = i;
			pEph[i]->Header.NumVec = pEph[i]->table.size();
			pEph[i]->Header.Offset = 0;
			pEph[i]->Header.Status = 0;
			pEph[i]->Header.TL = pEph[i]->table.front().GMT;
			pEph[i]->Header.TR = pEph[i]->table.back().GMT;
			pEph[i]->Header.TUP = 0;
			pEph[i]->Header.VEH = 0;
		}
	}
}