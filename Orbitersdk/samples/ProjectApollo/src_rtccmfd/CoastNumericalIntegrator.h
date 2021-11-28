/****************************************************************************
This file is part of Project Apollo - NASSP

Coast Numerical Integrator, RTCC Module PMMCEN (Header)

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

#pragma once

#include "Orbitersdk.h"
#include "RTCCModule.h"

class CoastIntegrator2 : public RTCCModule
{
public:
	CoastIntegrator2(RTCC *r);
	~CoastIntegrator2();
	bool Propagate(VECTOR3 R00, VECTOR3 V00, double gmt, double tmax, double tmin, double deltat, double dir, int planet, int stopcond);

	VECTOR3 R2, V2;
	double T2;
	int outplanet;
	//End condition
	int ITS;
private:
	void Edit();
	void Step();
	void Rectification();
	void SetBodyParameters(int p);
	void StoreVariables();
	void RestoreVariables();
	VECTOR3 f(VECTOR3 alpha, VECTOR3 R, VECTOR3 a_d);
	double fq(double q);
	VECTOR3 adfunc(VECTOR3 R);
	double CurrentTime();

	double R_E, mu;
	//State vector at last rectification
	VECTOR3 R0, V0;
	//Latest conic state vector propagated from R0, V0
	VECTOR3 R_CON, V_CON;
	VECTOR3 R_QC, R_PQ;
	//Time of initial state vector
	double t0;
	//Time since rectification
	double tau;
	//Time of last rectification
	double TRECT;
	//Stop variable
	double STOPVA;
	double x;
	VECTOR3 delta, nu;
	double r_dP;
	double mu_Q;
	double rect1, rect2;
	//Rotation axis of the primary body
	VECTOR3 U_Z;
	//Primary Body, 0 = Earth, 1 = Moon
	int P;
	//Current step length
	double dt;
	//Current position and velocity vector
	VECTOR3 R, V;
	//Parameters initialized
	bool INITF;
	//Moon/Sun Ephemerides
	VECTOR3 R_EM, V_EM, R_ES;
	//Ending flag
	int IEND;
	//Temporary time variable
	double TIME;
	//Function variable
	double FUNCT;
	//Difference between current and desired values
	double RCALC;
	//Value of RCALC at the state before bounding
	double RES1;
	//Previous values of RCALC
	double RES2;
	//Previous value of dt
	double VAR;
	//Minimum time until checking of end condition
	double TMIN;
	//Maximum time to integrate
	double TMAX;
	//Stop condition (1 = time, 2 = flight path angle, 3 = radius)
	int ISTOPS;
	//Size of the sphere of influence of the Moon
	double r_SPH;
	//Direction control and step size multiplier
	double HMULT;
	//Divisor for termination control
	double DEV;
	//Bounding variable. 0 = first pass, -1 = not bounded, 1 = bounded
	int INITE;
	double DEL, AQ, BQ, DISQ, dtesc[2];
	double dt_temp;
	//Stored time of last forcing function init
	double TS;

	//Constants
	static const double K, dt_lim;

	//Stored data
	int P_S;
	VECTOR3 R_S, V_S;
	double T_S;
};

class CoastIntegrator3 : public RTCCModule
{
public:
	CoastIntegrator3(RTCC *r);
	~CoastIntegrator3();
	bool Propagate(VECTOR3 R00, VECTOR3 V00, double gmt, double tmax, double tmin, double deltat, double dir, int planet, int stopcond);

	VECTOR3 R_out, V_out;
	double T_out;
	int outplanet;
	//End condition
	int ITS;
private:
	//Forcing Function
	void PCCIFF();
	//Gauss-Jackson
	void PCCIGJ();
	//Runge Kutta
	void PCCIRK();
	//Editor
	void PCCIED();
	void Rectification();
	void SetBodyParameters(int p);
	void StoreVariables();
	void RestoreVariables();
	double fq(double q);
	double CurrentTime();

	//Gauss-Jackson state (0 = needs to be initialized, 1-7: Building up table, negative: Gauss-Jackson mode)
	int ISTART;
	//Parameters initialized
	bool INITF;
	//Ending flag
	int IEND;

	//Base position vector, at last rectification
	VECTOR3 RBASE;
	//Base velocity vector, at last rectification (beta units)
	VECTOR3 VBASE;
	//Actual position vector
	VECTOR3 R;
	//Actual velocity vector, time units
	VECTOR3 V;

	double R_E, mu, SQRTMU;
	//Time of initial state vector
	double t0;
	//Time since rectification
	double DELT;
	//Time of last rectification
	double TRECT;
	//Current beta
	double T;
	//Stored beta of last forcing function init
	double TS;
	//Stop variable
	double STOPVA;
	//Position deviation vector
	VECTOR3 Y;
	//Velocity deviation vector
	VECTOR3 YP;
	//Acceleration deviation vector
	VECTOR3 YPP;
	double r_dP;
	double mu_Q, mu_S;
	//Rotation axis of the primary body
	VECTOR3 U_Z;
	//Primary Body, 0 = Earth, 1 = Moon
	int P;
	//Moon/Sun Ephemerides
	VECTOR3 R_EM, V_EM, R_ES;
	//Temporary time variable
	double TIME;
	//Function variable
	double FUNCT;
	//Difference between current and desired values
	double RCALC;
	//Value of RCALC at the state before bounding
	double RES1;
	//Previous values of RCALC
	double RES2;
	//Previous value of dt
	double VAR;
	//Minimum time until checking of end condition
	double TMIN;
	//Maximum time to integrate
	double TMAX;
	//Stop condition (1 = time, 2 = flight path angle, 3 = radius)
	int ISTOPS;
	//Size of the sphere of influence of the Moon
	double r_SPH;
	//Direction control and step size multiplier
	double HMULT;
	//Divisor for termination control
	double DEV;
	//Bounding variable. 0 = first pass, -1 = not bounded, 1 = bounded
	int INITE;
	double DEL, AQ, BQ, DISQ, htesc[2];
	//Coefficients to convert between time and beta units
	double COEF1, COEF2, COEF3;
	//Array of step lengths
	double HBETA[2];
	//Size of RBASE
	double RBASE1;
	//Size of RBASE squared
	double RBASE2;
	//Temporary variable
	double TEMP;
	//Size of VBASE squared, beta units
	double VBASE2;
	double DOT;
	double OVERA;
	double ROVERA;
	double F[4];
	//Two-body
	double FF, GG, FDOT, GDOT;
	double RTBMAG, RTBMAG2, RTBMAG3;
	VECTOR3 RTB, RDTB;
	//Step length
	double H, H2, HRK, HP, HD2, H2D2, H2D8, HD6, H_temp;
	//Temporary in PCCIRK
	VECTOR3 F1, F2, F3, YS, YPS;
	//Distance to Moon
	double PWRM;
	//
	double EPSQR;
	//Once/twice the actual position vector magnitude
	double CBODY1, CBODY2;
	//Acceleration vectors. 0 = Encke, 1 = other body, 2 = Sun, 3 = nonspherical gravity
	VECTOR3 TT[4];

	//Stored data
	double SAVET;
	int P_S;
	VECTOR3 SRTB;
	VECTOR3 SY;
	VECTOR3 SYP;
	double SDELT, STRECT;
};