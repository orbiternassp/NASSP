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

struct EMMENIInputTable
{
	EphemerisData AnchorVector;
	//Minimum time of integration
	double MinIntegTime = 0.0;
	//Maximum time of integration
	double MaxIntegTime = 10.0*24.0*3600.0;
	//Minimum time between ephemeris points
	double MinEphemDT = 0.0;
	//Integration direction indicator (+X-forward, -X-backward)
	double IsForwardIntegration = 1.0;
	//Fixed integration step length
	bool UseFixedStepLength = false;
	double FixedStepLength = 10.0;
	//Desired value of stopping parameter relative to the Earth
	double EarthRelStopParam = 0.0;
	//Desired value of stopping parameter relative to the Moon
	double MoonRelStopParam = 0.0;
	//Reference frame of desired stopping parameter (0 = Earth, 1 = Moon, 2 = both)
	int StopParamRefFrame = 2;
	//Density multiplier value
	double DensityMultiplier = 0.0;
	//Vent Indicator (negative means no venting)
	double VentPerturbationFactor = -1.0;
	//Integration Mode Indicator (0 = Encke-Beta, 1 = Encke-Time, 2 = Cowell-Time)
	int IntegMode = 1;
	//Cut-off indicator (1 = Time, 2 = radial distance, 3 = altitude above Earth or moon, 4 = flight-path angle, 5 = first reference switch)
	int CutoffIndicator = 1;
	//Input area
	double Area = 0.0;
	//Input weight
	double Weight = 1.0;
	//Ephemeris build indicator
	bool EphemerisBuildIndicator = false;
	bool ECIEphemerisIndicator = false;
	bool ECTEphemerisIndicator = false;
	bool MCIEphemerisIndicator = false;
	bool MCTEphemerisIndicator = false;
	EphemerisDataTable2 *ECIEphemTableIndicator = NULL;
	EphemerisDataTable2 *ECTEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCIEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCTEphemTableIndicator = NULL;

	EphemerisData sv_cutoff;
	//1 = maximum time, 2 = radius, 3 = altitude, 4 = flight path angle, 5 = reference switch, 6 = beginning of maneuver, 7 = end of maneuver, 8 = ascending node
	int TerminationCode;
};

class EnckeFreeFlightIntegrator : public RTCCModule
{
public:
	EnckeFreeFlightIntegrator(RTCC *r);
	~EnckeFreeFlightIntegrator();
	void Propagate(EMMENIInputTable &in);

private:
	void Edit();
	void Step();
	void Rectification();
	void SetBodyParameters(int p);
	void StoreVariables();
	void RestoreVariables();
	double fq(double q);
	void adfunc();
	double CurrentTime();
	void EphemerisStorage(bool last = false);
	void WriteEphemerisHeader();
	void ACCEL_GRAV();

	double R_E, mu;
	//State vector at last rectification (RBASE)
	VECTOR3 R0, V0;
	//Latest conic state vector propagated from R0, V0
	VECTOR3 R_CON, V_CON;
	//Time of initial state vector (TSTART)
	double t0;
	//Time since rectification (T)
	double tau;
	//Time of last rectification (TRECT)
	double TRECT;
	//Stop variable (Earth and Moon)
	double STOPVAE, STOPVAM;
	//Difference position vector (Y)
	VECTOR3 delta;
	//Difference velocity vector (YP)
	VECTOR3 nu;

	double mu_Q;
	double rect1, rect2;
	//Rotation axis of the primary body
	VECTOR3 U_Z;
	//Primary Body, 0 = Earth, 1 = Moon
	int P;
	//Current step length
	double dt;
	//Current position and velocity vector (DSTATE)
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
	//Stop condition (1 = time, 2 = radial distance, 3 = altitude above Earth or moon, 4 = flight-path angle, 5 = first reference switch, 6 = first ascending node relative to the Earth, 7 = longitude, 8 = latitude)
	int ISTOPS;
	//Reference frame of desired stopping parameter (0 = Earth, 1 = Moon, 2 = both)
	int StopParamRefFrame;
	//Size of the sphere of influence of the Moon
	double r_SPH;
	//Direction control and step size multiplier
	double HMULT;
	//Fixed integration step length
	bool UseFixedStepLength;
	double FixedStepLength;
	//Divisor for termination control
	double DEV;
	//Density multiplier (0 if no drag)
	double DRAG;
	//Vent multiplier (negative if no venting)
	double VENT;
	//Bounding variable. 0 = first pass, -1 = not bounded, 1 = bounded
	int INITE;
	double DEL, AQ, BQ, DISQ, dtesc[2];
	double dt_temp;
	//Stored time of last forcing function init
	double TS;
	//Ephemeris storage flags
	bool bStoreEphemeris[4];
	EphemerisDataTable2 *pEph[4];
	bool EphemerisBuildIndicator;
	//Distance from Moon
	double PWRM;
	//Density and speed of sound
	double DENS, SPOS;
	//Velocity relative to the atmosphere
	VECTOR3 V_R;
	double VRMAG;
	//Drag factor
	double CDRAG;
	//Drag multiplier (-0.5*coefficient of drag*Area)
	double CSA;
	//Vehicle weight
	double WT;
	//Drag acceleration
	VECTOR3 a_drag;
	//Vent acceleration
	VECTOR3 a_vent;
	//Venting mass loss rate
	double MDOT_vent;
	//Minimum output step
	double MinEphemDT;

	//ACCEL
	//Rotation matrix from global to local coordinates
	MATRIX3 Rot;
	//Planet fixed position vector
	VECTOR3 R_EF;
	//Inverse of position radius
	double R_INV;
	//Unit position vector in planet fixed coordinates
	VECTOR3 UR;
	//Ratio of position and Earth radii
	double R0_ZERO;
	//Term in gravity calculation
	double R0_N;
	double MAT_A[5][2];
	double ZETA_REAL[5], ZETA_IMAG[5];
	//Degree and order of gravity calculations
	int GMD, GMO;
	int L, I, N, N1, J;
	double AUXILIARY;
	double F1, F2, F3, F4, DNM;
	VECTOR3 G_VEC;
	double ZONAL[4];
	double C[9], S[9];

	//Values used in Step
	double HP, HD2, H2D2, H2D8, HD6;

	//Temporary variables
	double FACT1;

	//DEPENDENT VARIABLES CALCULATED BY ADFUNC

	//Latest acceleration vector
	VECTOR3 YPP;

	//Constants
	static const double K, dt_lim;
	//700km as radius (used to determine drag or no drag calculation)
	static const double CONS;
	//Threshold at which drag isn't recomputed because it would be too large
	static const double drag_threshold;

	//Stored data
	int P_S;
	VECTOR3 SRTB, SRDTB, SY, SYP;
	double SDELT, STRECT;
	double SWT;
};