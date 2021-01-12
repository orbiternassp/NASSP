/****************************************************************************
This file is part of Project Apollo - NASSP

Encke Numerical Free Flight Integrator, RTCC Module EMMENI (Header)

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
#include "RTCCTables.h"

class EnckeFreeFlightIntegrator : public RTCCModule
{
public:
	EnckeFreeFlightIntegrator(RTCC *r);
	~EnckeFreeFlightIntegrator();
	void Propagate(EMSMISSInputTable &in);

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
	void EphemerisStorage();
	void WriteEphemerisHeader();

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
	//Stop variable (Earth and Moon)
	double STOPVAE, STOPVAM;
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
	//Maximum time to integrate
	double TMAX;
	//Stop condition (0 = none, 1 = radial distance, 2 = first reference switch, 3 = altitude above Earth or moon, 4 = flight-path angle)
	int ISTOPS;
	//Reference frame of desired stopping parameter (0 = Earth, 1 = Moon, 2 = both)
	int StopParamRefFrame;
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
	//Ephemeris storage flags
	bool bStoreEphemeris[4];
	EphemerisDataTable2 *pEph[4];

	//Constants
	static const double K, dt_lim;

	//Stored data
	int P_S;
	VECTOR3 R_S, V_S;
	double T_S;
};