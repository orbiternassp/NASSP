/****************************************************************************
  This file is part of Shuttle FDO MFD for Orbiter Space Flight Simulator
  Copyright (C) 2019 Niklas Beug

  Launch Window Processor

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
  **************************************************************************/

#pragma once

#include "OrbMech.h"

using namespace OrbMech;

struct LWPSummary
{
	int LWPERROR = 0;
	double MJDLO = 0.0;
	double MJDINS = 0.0;
	double AZL = 0.0;
	double VIGM = 0.0;
	double RIGM = 0.0;
	double GIGM = 0.0;
	double IIGM = 0.0;
	double TIGM = 0.0;
	double TDIGM = 0.0;
	double DN = 0.0;
	double DELNO = 0.0;
	double PA = 0.0;
	double MJDPLANE = 0.0;
	double LATLS = 0.0;
	double LONGLS = 0.0;
	SV sv_P;
};

struct LWPParameterTable
{
	double MJDLO[2] = { 0.0, 0.0 };
	double GPAZ[2] = { 0.0, 0.0 };
	double AZL[2] = { 0.0, 0.0 };
	double YP[2] = { 0.0, 0.0 };
	double WEDGE[2] = { 0.0, 0.0 };
	double DVPC[2] = { 0.0, 0.0 };
	double PHASE[2] = { 0.0, 0.0 };
	double TIGM[2] = { 0.0, 0.0 };
};

struct LWPSettings
{
	//Inplane launch window opening and closing times option
	//0 = inplane opening (ascending node), 1 = inplane closing (descending node), 2 = opening and closing (both)
	int NS;
	//0 = conic, 1 = integrated
	int SVPROP;
	//Delta time to be subtracted from analytical inplane launch time to obtain empirical inplane launch time
	double DTOPT;
	//Flag to wrap initial phase angle (add 2NPI to phase angle)
	int WRAP;
	//Initial phase angle control flag
	//0 = 0 to 2PI, 1 = -2PI to 0, 2 = -PI to PI
	int NEGTIV;
	//Flightpath angle at insertion
	double GAMINS;
	//Geocentric latitude of launch site
	double LATLS;
	//Geocentric longitude of launch site
	double LONGLS;
	//Powered flight time
	double PFT;
	//Powered flight arc
	double PFA;
	//Radius of insertion
	double RINS;
	//Velocity magnitude of insertion
	double VINS;
	//Yaw steering limit
	double YSMAX;
	//MJD of inplane lift-off time (normally computed internally)
	double MJDPLANE;
	//Launch window/launch targeting options
	//0 = LW, 1 = LT, 2 = LW and LT
	int LW;
	//Delta time prior to in-plane time to start parameter table
	double TSTART;
	//Delta time after in-plane time to stop parameter table
	double TEND;
	//Target state vector
	SV TRGVEC;
	//Angle that is added to the target descending node to account for differential nodal regression
	double DELNO;
	//Lift-off time options for launch targeting
	//1 = input time, 2 = phase angle offset, 3 = biased phase zero (GMTLOR threshold), 4 = biased phase zero (TPLANE threshold), 5 = in-plane, 6 = in-plane with nodal regression
	int LOT;
	//Chaser vehicle weight at OMS-2
	double CWHT;
	LWPParameterTable *lwp_param_table;
};

class LaunchWindowProcessor
{
public:
	LaunchWindowProcessor();
	void Init(LWPSettings &set);
	void SetGlobalConstants(double mu, double w_E, double R_E);
	void LWP();
	void GetOutput(LWPSummary &out);
protected:
	void LWT();
	SV UPDATE(SV sv0, double dt);
	void NPLAN(double &MJDIP);
	void LENSR(double mjdlo, double ysmax, SV &sv_P, double &delta);
	double PHANG(SV sv_P, int phcont, int wrp);
	double GMTLS(double MJDI);
	void RLOT();
	void TARGT(SV &sv_P);
	SV PositionMatch(SV sv_A, SV sv_P);
	void NSERT(double MJDLO, double &UINS, SV &sv_P, double &DH);
	void LWPT();
	void LWDSP();
	void RLOTD();
	void LWPOT();

	SV coast(SV sv0, double dt);
	SV coast_osc(SV sv0, double dt, double mu);

	//0 = conic, 1 = integrated
	int SVPROP;
	//standard gravitational parameter
	double mu;
	//angular rate of Earth
	double w_E;
	//Inplane launch window opening and closing times option
	//0 = inplane opening (ascending node), 1 = inplane closing (descending node), 2 = opening and closing (both)
	int NS;
	//Delta time to be subtracted from analytical inplane launch time to obtain empirical inplane launch time
	double DTOPT;
	//Flag to wrap initial phase angle (add 2NPI to phase angle)
	int WRAP;
	//Initial phase angle control flag
	//0 = 0 to 2PI, 1 = -2PI to 0, 2 = -PI to PI
	int NEGTIV;
	//Flightpath angle at insertion
	double GAMINS;
	//Geocentric latitude of launch site
	double LATLS;
	//Geocentric longitude of launch site
	double LONGLS;
	//Powered flight time
	double PFT;
	//Powered flight arc
	double PFA;
	//Radius of insertion
	double RINS;
	//Velocity magnitude of insertion
	double VINS;
	//Yaw steering limit
	double YSMAX;
	//Maximum iterations limit
	int CMAX;
	//Insertion cutoff conditions option flag
	//1 = Input VINS, GAMINS, RINS; 2 = Input GAMINS, RINS and height difference, 3 = Input GAMINS RINS, altitude at input angle from insertion
	int INSCO;
	//Nominal semimajor axis at insertion
	double ANOM;
	//Lift-off time options for launch targeting
	//1 = input time, 2 = phase angle offset, 3 = biased phase zero (GMTLOR threshold), 4 = biased phase zero (TPLANE threshold), 5 = in-plane, 6 = in-plane with nodal regression
	int LOT;
	//Delta time added to inplane time to obtain lift-off time
	double TRANS;
	//MJD of inplane lift-off time (normally computed internally)
	double MJDPLANE;
	//Recommended or threshold lift-off MJD
	double MJDLOR;
	//Time iteration tolerance
	double DET;
	//Radius iteration tolerance
	double DELH;
	//Flag for option to compute differential nodal regression from insertion to rendezvous
	//false = input DELNO, true = compute DELNO
	bool DELNOF;
	//Angle that is added to the target descending node to account for differential nodal regression
	double DELNO;
	//DT from lift-off, which defines the time of guidance reference release
	double DTGRR;
	//Bias that is added to MJDLOSTAR to produce lift-off time
	double BIAS;
	//Differential nodal precession
	double DELNOD;
	//Launch window/launch targeting options
	//0 = LW, 1 = LT, 2 = LW and LT
	int LW;
	//Target vehicle state vector
	SV sv_T;
	//Delta time prior to in-plane time to start parameter table
	double TSTART;
	//Delta time after in-plane time to stop parameter table
	double TEND;
	//Time step in parameter table
	double TSTEP;
	//First guess change in value of independent variable (used in subroutine ITER)
	double DX1;
	//Tolerance on plane change DV for calculating launch window inplane launch points
	double DVTOL;
	//Phase angle at window closing (descending node)
	double PAC;
	//Phase angle at window opening (ascending node)
	double PAO;
	//MJD of lift-off for launch window opening (ascending node)
	double MJDOPEN;
	//MJD of lift-off for launch window closing (descending node)
	double MJDCLOSE;
	//MJD of insertion
	double MJDINS;
	//Wedge angle between planes
	double WEDGE;
	//Launch window parameter table options
	//0 = do not generate table, 1 = window opening, 2 = window closing, 3 = both, 4 = entire window
	int LPT;
	//Reference radius
	double RREF;
	//Phase angle desired at insertion
	double OFFSET;
	//Launch azimuth coefficients
	double LAZCOE[4];
	//MJD of lift-off
	double MJDLO;
	//Inclination of chaser at insertion
	double IIGM;
	//Angle measured from launch site meridian to chaser descending node
	double TIGM;
	//Rate of change of TIGM
	double TDIGM;
	//Optimum launch azimuth
	double AZL;
	//Target vector Earth-fixed descending node
	double DN;
	//Phase angle at insertion
	double PA;
	//RLOT chaser state vector
	SV sv_P1;

	LWPParameterTable *lwp_param_table;
	int error;
	OBJHANDLE hEarth;
};