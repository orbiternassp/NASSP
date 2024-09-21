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
const double EnckeFreeFlightIntegrator::CONS = 6.373338e6 + 700.0*1000.0;
const double EnckeFreeFlightIntegrator::drag_threshold = 0.1;

EnckeFreeFlightIntegrator::EnckeFreeFlightIntegrator(RTCC *r) : RTCCModule(r)
{
	for (int i = 0;i < 4;i++)
	{
		pEph[i] = NULL;
	}
	for (int i = 0;i < 9;i++)
	{
		C[i] = S[i] = 0.0;
	}

	P_S = 0;
	SRTB = SRDTB = SY = SYP = _V(0, 0, 0);
	STRECT = SDELT = 0.0;
}

EnckeFreeFlightIntegrator::~EnckeFreeFlightIntegrator()
{
}

void EnckeFreeFlightIntegrator::Propagate(EMMENIInputTable &in)
{
	//Initialize
	t0 = in.AnchorVector.GMT;
	R0 = R = R_CON = in.AnchorVector.R;
	V0 = V = V_CON = in.AnchorVector.V;
	TMAX = in.MaxIntegTime;
	STOPVAE = in.EarthRelStopParam;
	STOPVAM = in.MoonRelStopParam;
	HMULT = in.IsForwardIntegration;
	UseFixedStepLength = in.UseFixedStepLength;
	FixedStepLength = in.FixedStepLength;
	if (UseFixedStepLength)
	{
		//With integration fixed step length, turn off step size multiplier
		if (HMULT >= 0.0)
		{
			HMULT = 1.0;
		}
		else
		{
			HMULT = -1.0;
		}
	}
	DRAG = in.DensityMultiplier;
	VENT = in.VentPerturbationFactor;
	CSA = -0.5*in.Area*pRTCC->SystemParameters.MCADRG;
	if (in.Weight == 0.0)
	{
		//Might happen with uninitialized MPT
		WT = 99999999.9;
	}
	else
	{
		WT = in.Weight;
	}

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
	MinEphemDT = in.MinEphemDT;

	a_drag = _V(0, 0, 0);
	a_vent = _V(0, 0, 0);
	MDOT_vent = 0.0;
	delta = _V(0, 0, 0);
	nu = _V(0, 0, 0);
	HD2 = H2D2 = H2D8 = HD6 = HP = 0.0;
	dt = dt_lim;

	//Normally 9 Er
	r_SPH = 9.0*OrbMech::R_Earth;
	if (ISTOPS == 5)
	{
		//If we want to find a reference switch, set SOI to 14 Er, Moon relative stop variable to actual SOI (9 Er) and stop reference to Moon only
		r_SPH = 14.0*OrbMech::R_Earth;
		STOPVAM = 9.0*OrbMech::R_Earth;
		StopParamRefFrame = 1;
	}
	//If we want to find a radius relative to Moon and it's between 8 and 10 Er, set SOI to 14 Er
	if (ISTOPS == 2 && StopParamRefFrame > 0 && (STOPVAM > 8.0*OrbMech::R_Earth && STOPVAM < 10.0*OrbMech::R_Earth))
	{
		r_SPH = 14.0*OrbMech::R_Earth;
	}

	if (ISTOPS == 2 || ISTOPS == 3 || ISTOPS == 5)
	{
		//1 meter tolerance for radius and height
		DEV = 1.0;
	}
	else if (ISTOPS == 4 || ISTOPS == 6 || ISTOPS == 7 || ISTOPS == 8)
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
	adfunc();

	do
	{
		Edit();
		if (INITE != 1)
		{
			EphemerisStorage();
		}
		if (IEND == 0)
		{
			Step();
		}
	} while (IEND == 0);

	EphemerisStorage(true);
	WriteEphemerisHeader();

	in.sv_cutoff.R = R_CON + delta;
	in.sv_cutoff.V = V_CON + nu;
	in.sv_cutoff.GMT = CurrentTime();
	in.sv_cutoff.RBI = P;
	in.TerminationCode = ISTOPS;
}

void EnckeFreeFlightIntegrator::Edit()
{
	double rr, dt_max;

	//Bounded?
	if (INITE != 1)
	{
		if (P == BODY_MOON)
		{
			//Are we leaving the sphere of influence?
			if (PWRM > r_SPH)
			{
				R_CON = R_CON + R_EM;
				V_CON = V_CON + V_EM;

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
				R_CON = R_CON - R_EM;
				V_CON = V_CON - V_EM;

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
	rr = length(R);
	if (UseFixedStepLength)
	{
		dt_max = FixedStepLength;
	}
	else
	{
		if (DRAG > 0.0 && P == BODY_EARTH && rr < 6499920.0)
		{
			//Special atmospheric integration step logic
			dt_max = 10.0;
		}
		else
		{
			dt_max = min(dt_lim, K*OrbMech::power(rr, 1.5) / sqrt(mu));
		}
	}

	//Should we even check?
	if (ISTOPS == 1)
	{
		RCALC = TIME - TMAX;
	}
	else
	{
		RCALC = 1000000000.0;
	}
	if (ISTOPS > 1 && (StopParamRefFrame == 2 || P == StopParamRefFrame))
	{
		if (ISTOPS == 2 || ISTOPS == 5)
		{
			//Radius
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
			//Altitude
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
			//Sine of flight path angle
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
		else if (ISTOPS == 6)
		{
			//Ascending node
			VECTOR3 NVEC, H_ECT;
			EphemerisData2 sv_temp, sv_ECT;

			sv_temp.R = R;
			sv_temp.V = V;
			sv_temp.GMT = CurrentTime();

			pRTCC->ELVCNV(sv_temp, 0, 1, sv_ECT);

			H_ECT = unit(crossp(sv_ECT.R, sv_ECT.V));
			NVEC = unit(crossp(_V(0, 0, 1), H_ECT));
			RCALC = OrbMech::PHSANG(sv_ECT.R, sv_ECT.V, NVEC);
		}
		else if (ISTOPS == 7)
		{
			//Longitude
			double lat;
			OrbMech::latlong_from_r(R_EF, lat, FUNCT);

			if (P == BODY_EARTH)
			{
				RCALC = FUNCT - STOPVAE - OrbMech::w_Earth*CurrentTime();
			}
			else
			{
				RCALC = FUNCT - STOPVAM;
			}

			//modulo between -PI and PI
			if (RCALC > 0)
			{
				RCALC = fmod(RCALC + PI, PI2) - PI;
			}
			else
			{
				RCALC = fmod(RCALC - PI, PI2) + PI;
			}
		}
		else if (ISTOPS == 8)
		{
			//Latitude

			double lng;
			OrbMech::latlong_from_r(R_EF, FUNCT, lng);

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
		bool found = false;

		if (RCALC*RES1 < 0.0)
		{
			//For longitude search, prevent ambiguity
			if (!(ISTOPS == 7 && abs(RCALC) > PI05))
			{
				found = true;
			}
		}

		if (!found)
		{
			//No sign switch, not bounded
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
		ISTOPS = 1;
		if (TMAX != 0.0)
		{
			RestoreVariables();
		}
		//Mark as bounding
		INITE = 1;
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
	if (dt*dtesc[0] <= 0.0)
	{
		if (dt*dtesc[1] <= 0.0)
		{
			//Both solutions bad
			goto EMMENI_Edit_7A;
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
	goto EMMENI_Edit_7B;
EMMENI_Edit_7A:
	//sprintf(oapiDebugString(), "EMMENI: How did we get here?");
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
	IEND = 0;
	return;
}

void EnckeFreeFlightIntegrator::Step()
{
	VECTOR3 alpha; //YS
	VECTOR3 beta; //YPS
	VECTOR3 F1, F2, F3;

	//Start
	if (dt - HP != 0.0)
	{
		HD2 = dt / 2.0;
		H2D2 = HD2 * dt;
		H2D8 = H2D2 / 4.0;
		HD6 = dt / 6.0;
		HP = dt;
	}

	//Save base and build state for 2nd derivative (2nd term)
	F1 = YPP;
	alpha = delta;
	beta = nu;
	delta = alpha + beta * HD2 + F1 * H2D8;
	nu = beta + F1 * HD2;
	tau = tau + HD2;
	//Get 2nd derivative (F2)
	adfunc();
	//Save F2 and build state for 2nd deriv evaluation F3
	F2 = YPP;
	nu = beta + F2 * HD2;
	adfunc();
	F3 = YPP;
	delta = alpha + beta * dt + F3 * H2D2;
	nu = beta + F3 * dt;
	tau = tau + HD2;
	//Get 2nd deriv F4
	adfunc();
	//Weighted sum for state at tau + dt
	delta = alpha + (beta + (F1 + F2 + F3)*HD6)*dt;
	nu = beta + (F1 + (F2 + F3)*2.0 + YPP) *HD6;
	//Final acceleration
	adfunc();

	if (VENT > 0.0)
	{
		WT = WT - MDOT_vent * dt;
	}
}

double EnckeFreeFlightIntegrator::fq(double q)
{
	return q * (3.0 + 3.0 * q + q * q) / (1.0 + pow(1.0 + q, 1.5));
}

void EnckeFreeFlightIntegrator::adfunc()
{
	double r, q;
	VECTOR3 a_dP, a_d, a_dQ, a_dS;

	a_dP = a_dQ = a_dS = _V(0, 0, 0);

	if (INITF == false || tau != TS)
	{
		if (INITF == false)
		{
			INITF = true;

			//Get Earth rotation matrix only during initialization. For the Moon the libration matrix is updated by the PLEFEM call below
			if (P == BODY_EARTH)
			{
				pRTCC->ELVCNV(CurrentTime(), RTCC_COORDINATES_ECI, RTCC_COORDINATES_ECT, Rot);
				U_Z = tmul(Rot, _V(0, 0, 1));
			}
		}

		TS = tau;
		OrbMech::rv_from_r0v0(R0, V0, tau, R_CON, V_CON, mu);
		pRTCC->PLEFEM(P == BODY_EARTH ? 1 : 2, CurrentTime() / 3600.0, 0, &R_EM, &V_EM, &R_ES, &Rot); //Get Sun and Moon ephemerides and libration matrix (MCI only)
	}

	//Calculate actual state
	R = R_CON + delta;
	V = V_CON + nu;
	r = length(R);

	//Only calculate perturbations if we are above surface of primary body
	if (r > R_E)
	{
		ACCEL_GRAV();
		a_dP = G_VEC;

		VECTOR3 R_PS, R_SC, R_PQ, R_QC;
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

	if (P == BODY_EARTH)
	{
		PWRM = length(R - R_EM);
		if (DRAG)
		{
			if (r < CONS)
			{
				//Compute altitude
				FACT1 = r - OrbMech::R_Earth;
				//Get density
				pRTCC->GLFDEN(FACT1, DENS, SPOS);
				//Compute drag acceleration
				CDRAG = DENS*DRAG * CSA / WT;
				V_R = V - crossp(U_Z*OrbMech::w_Earth, R);
				VRMAG = length(V_R);
				//When integrating in the atmosphere, if the altitude/relative velocity ratio reaches a certain minimum value,
				//the drag perturbation will not be recomputed; but the last computed drag perturbation will be used.
				if (FACT1 / VRMAG >= drag_threshold)
				{
					a_drag = V_R * VRMAG*CDRAG;
				}
				a_d += a_drag;
			}
		}
		if (VENT > 0.0)
		{
			VECTOR3 VENTDIR = crossp(R, V);
			//Protect against radial trajectories
			if (length(VENTDIR) < 1e-10)
			{
				//Apply along velocity vector instead
				VENTDIR = unit(V);
			}
			else
			{
				//Normal case
				VENTDIR = unit(crossp(unit(VENTDIR), R));
			}

			double TV = CurrentTime() - pRTCC->GetGMTLO()*3600.0 - pRTCC->SystemParameters.MCGVEN;

			if (TV < 0.0)
			{
				//No venting before MCGVEN
				a_vent = _V(0.0, 0.0, 0.0);
				MDOT_vent = 0.0;
			}
			else
			{
				int i;
				for (i = 0; i < 8 && pRTCC->SystemParameters.MDTVTV[1][i + 1] < TV; i++);
				double f = (TV - pRTCC->SystemParameters.MDTVTV[1][i]) / (pRTCC->SystemParameters.MDTVTV[1][i + 1] - pRTCC->SystemParameters.MDTVTV[1][i]);
				double F_vent = pRTCC->SystemParameters.MCTVEN*(pRTCC->SystemParameters.MDTVTV[0][i] + (pRTCC->SystemParameters.MDTVTV[0][i + 1] - pRTCC->SystemParameters.MDTVTV[0][i]) * f);
				MDOT_vent = F_vent / pRTCC->SystemParameters.MCTVSP;

				a_vent = VENTDIR * F_vent / WT;
				a_d += a_vent;
			}
		}
	}
	else
	{
		PWRM = r;
	}

	q = dotp((delta - R * 2.0), delta) / pow(r, 2.0);
	YPP = -(R*fq(q) + delta)*mu / pow(length(R_CON), 3.0) + a_d;
}

void EnckeFreeFlightIntegrator::SetBodyParameters(int p)
{
	if (p == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		mu_Q = OrbMech::mu_Moon;
		R_E = OrbMech::R_Earth;
		rect1 = 0.75*OrbMech::power(2.0, 21.0);
		rect2 = 0.75*OrbMech::power(2.0, 2.0)*100.0;
		P = BODY_EARTH;
		GMD = 4;
		GMO = 0; //4 to use the full tesseral data
		ZONAL[0] = 0.0; ZONAL[1] = OrbMech::J2_Earth; ZONAL[2] = OrbMech::J3_Earth; ZONAL[3] = OrbMech::J4_Earth;
		//Use this when Orbiter simulates it
		//C[0] = -1.1619e-9; C[1] =  1.5654e-6; C[2] = 2.1625e-6; C[3] =  3.18750e-7; C[4] = 9.7078e-8; C[5] = -5.1257e-7; C[6] = 7.739e-8; C[7] =  5.7700e-8; C[8] = -3.4567e-9;
		//S[0] = -4.1312e-9; S[1] = -8.9613e-7; S[2] = 2.6809e-7; S[3] = -2.15567e-8; S[4] = 1.9885e-7; S[5] = -4.4095e-7; S[6] = 1.497e-7; S[7] = -1.2389e-8; S[8] =  6.4464e-9;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		mu_Q = OrbMech::mu_Earth;
		R_E = OrbMech::R_Moon;
		rect1 = 0.75*OrbMech::power(2.0, 17.0);
		rect2 = 0.75*OrbMech::power(2.0, -2.0)*100.0;
		P = BODY_MOON;
		GMD = 3;
		GMO = 0; //3 with L1 model
		ZONAL[0] = 0.0; ZONAL[1] = OrbMech::J2_Moon; ZONAL[2] = OrbMech::J3_Moon; ZONAL[3] = 0.0;
		//L1 model, use this when Orbiter simulates it
		//C[0] = 0.0; C[1] = 0.20715e-4; C[2] = 0.34e-4; C[4] = 0.02583e-4;
	}
}

void EnckeFreeFlightIntegrator::Rectification()
{
	R0 = R_CON + delta;
	V0 = V_CON + nu;
	TRECT = TRECT + tau;
	delta = _V(0, 0, 0);
	nu = _V(0, 0, 0);
	//x = 0;
	tau = 0;
	INITF = false;
	adfunc();
}

void EnckeFreeFlightIntegrator::StoreVariables()
{
	P_S = P;
	SRTB = R_CON;
	SRDTB = V_CON;
	SY = delta;
	SYP = nu;
	SDELT = tau;
	STRECT = TRECT;
	RES1 = RCALC;
	SWT = WT;
}

void EnckeFreeFlightIntegrator::RestoreVariables()
{
	R_CON = SRTB;
	V_CON = SRDTB;
	delta = SY;
	nu = SYP;
	tau = SDELT;
	TRECT = STRECT;
	WT = SWT;
	if (P != P_S)
	{
		SetBodyParameters(P_S);
		INITF = false;
		adfunc();
	}
	Rectification();
}

double EnckeFreeFlightIntegrator::CurrentTime()
{
	return (t0 + TRECT + tau);
}

void EnckeFreeFlightIntegrator::EphemerisStorage(bool last)
{
	if (EphemerisBuildIndicator == false) return;

	bool store = false, wastested = false;

	for (int i = 0;i < 4;i++)
	{
		if (bStoreEphemeris[i] && pEph[i])
		{
			//Only go through this once
			if (wastested == false)
			{
				//Always store first and last vector and store them all if MinEphemDT is zero
				if (pEph[i]->table.size() == 0 || MinEphemDT == 0.0 || last)
				{
					store = true;
				}
				else
				{
					double T_last, T_next, T_cur;

					//Last stored state vector
					T_last = pEph[i]->table.back().GMT;
					//Next desired time to store state vector
					T_next = T_last + MinEphemDT;
					//Current time
					T_cur = CurrentTime();
					//Don't store if state vectors are too close
					if (abs(T_last - T_cur) > 0.001)
					{
						double dt_next;

						//Time difference from current to next desired store time
						dt_next = T_next - T_cur;
						//Algorithm: "when the time required to reach the next minimum output point is less than one-half of the time span of the last integration step"
						if (dt_next < 0.5*dt)
						{
							store = true;
						}
					}
				}
				wastested = true;
			}

			if (store)
			{
				EphemerisData2 sv, sv2;
				int in;

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
				pRTCC->ELVCNV(sv, in, i, sv2);

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

void EnckeFreeFlightIntegrator::ACCEL_GRAV()
{
	//This function is based on the Space Shuttle onboard navigation (JSC internal note 79-FM-10)

	//Null gravitation acceleration vector
	G_VEC = _V(0, 0, 0);
	//Transform position vector to planet fixed coordinates
	R_EF = mul(Rot, R);
	//Components of the planet fixed position unit vector
	R_INV = 1.0 / length(R);
	UR = R_EF * R_INV;
	//Starting values for recursive relations used in Pines formulation
	R0_ZERO = R_E * R_INV;
	R0_N = R0_ZERO * mu*R_INV*R_INV;
	MAT_A[0][1] = 3.0*UR.z;
	MAT_A[1][1] = 3.0;
	ZETA_REAL[0] = 1.0;
	ZETA_IMAG[0] = 0.0;
	L = 1;
	AUXILIARY = 0.0;
	//Effects of tesseral harmonics, terms that depend on the vehicle's longitude
	for (I = 1;I <= GMO;I++)
	{
		ZETA_REAL[I] = UR.x*ZETA_REAL[I - 1] - UR.y*ZETA_IMAG[I - 1];
		ZETA_IMAG[I] = UR.x*ZETA_IMAG[I - 1] + UR.y*ZETA_REAL[I - 1];
	}
	for (N = 2;N <= GMD;N++)
	{
		//Derived Legendre functions by means of recursion formulas, multiplied by appropiate combinations of tesseral harmonics (Legendre polynomials shall be multiplied by
		//zonal harmonics coefficients), and stored as certain auxiliary variables F1-F4.
		MAT_A[N][0] = 0.0;
		MAT_A[N][1] = (2.0*(double)N + 1.0)*MAT_A[N - 1][1];
		MAT_A[N - 1][0] = MAT_A[N - 1][1];
		MAT_A[N - 1][1] = UR.z*MAT_A[N][1];
		for (J = 2;J <= N;J++)
		{
			MAT_A[N - J][0] = MAT_A[N - J][1];
			MAT_A[N - J][1] = (UR.z*MAT_A[N - J + 1][1] - MAT_A[N - J + 1][0]) / ((double)J);
		}
		F1 = 0.0;
		F2 = 0.0;
		F3 = -MAT_A[0][0] * ZONAL[N - 1];
		F4 = -MAT_A[0][1] * ZONAL[N - 1];
		//If the maximum order of tesserals wanted has not been attained, do for N1=1 to N (these take into account contributions of tesseral and sectorial harmonics):
		if (N <= GMO)
		{
			for (N1 = 1;N1 <= N;N1++)
			{
				F1 = F1 + (double)N1*MAT_A[N1 - 1][0] * (C[L - 1] * ZETA_REAL[N1 - 1] + S[L - 1] * ZETA_IMAG[N1 - 1]);
				F2 = F2 + (double)N1*MAT_A[N1 - 1][0] * (S[L - 1] * ZETA_REAL[N1 - 1] - C[L - 1] * ZETA_IMAG[N1 - 1]);
				DNM = C[L - 1] * ZETA_REAL[N1] + S[L - 1] * ZETA_IMAG[N1];
				F3 = F3 + DNM * MAT_A[N1][0];
				F4 = F4 + DNM * MAT_A[N1][1];
				L++;
			}
		}
		//Multiply the sum of zonal and tesseral effects by appropiate distance-related factors, store the results as components of the acceleration vector G_VEC, and prepare for 
		//final computation by obtaining the intermediate scalar variable AUXILIARY, which accounts for an additional effect proportional to the unit radius vector UR.
		R0_N = R0_N * R0_ZERO;
		G_VEC.x = G_VEC.x + R0_N * F1;
		G_VEC.y = G_VEC.y + R0_N * F2;
		G_VEC.z = G_VEC.z + R0_N * F3;
		AUXILIARY = AUXILIARY + R0_N * F4;
	}
	//Lastly, the planet fixed acceleration vector shall be obtained and rotated to inertial coordinates
	G_VEC = G_VEC - UR * AUXILIARY;
	G_VEC = tmul(Rot, G_VEC);
}