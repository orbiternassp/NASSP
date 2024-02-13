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
#include "RTCCModule.h"

class RTCC;

struct LWPGlobalConstants
{
	//standard gravitational parameter
	double mu;
	//angular rate of Earth
	double w_E;
	//Reference radius
	double RREF;
};

struct LWPProgramConstants
{
	//Maximum iterations limit
	int CMAX = 20;
	//First guess in change of independent variable
	double DX1 = 25.0; //Unit?
	//Transfer angle tolerance
	double TransferAngleTol = 2.e-4;
	//Eccentricity tolerance
	double EccentricityTol = 6.e-4;
	//Travel angle convergence tolerance
	double DOS = 2.e-4;
	//Radius iteration tolerance
	double DELH = 250.0*0.3048;
	//Time iteration tolerance
	double DET = 1.0;
	//Tolerance on plane change DV for calculating launch window in plane launch points
	double DVTOL = 0.1*0.3048;
};

struct LaunchWindowTimesTable
{
	int DAY;
	int DATE[3];
	double GMT_OPEN;
	double GMT_CLOSE;
	double EST_OPEN;
	double EST_CLOSE;
	double PST_OPEN;
	double PST_CLOSE;
	double PAO;
	double PAC;
	double HA;
	double HP;
	double LATLS;
	double LONGLS;
};

struct GMTLOTable
{
	double STAR[10];
	double LATLS;
	double LONGLS;
	double DELTA[10];
};

struct LWPSummary
{
	int LWPERROR = 0;
	double GMTLO = 0.0;
	double TINS = 0.0;
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
	double TPLANE = 0.0;
	double LATLS = 0.0;
	double LONGLS = 0.0;
	double TGRR = 0.0;
};

struct RLOTDisplay
{
	double DELNOD = 0.0;
	double GSTAR = 0.0;
	double TYAW = 0.0;
	double DH = 0.0;
};

struct LWPStateVectorTable
{
	//Target initial
	AEGDataBlock sv_T0;
	//Target at insertion
	AEGDataBlock sv_T1;
	//Chaser at insertion
	AEGDataBlock sv_C;
};

struct LWPInputTable
{
	LWPInputTable();
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
	//Launch window/launch targeting options
	//0 = LW, 1 = LT, 2 = LW and LT
	int LW;
	//Delta time prior to in-plane time to start parameter table
	double TSTART;
	//Delta time after in-plane time to stop parameter table
	double TEND;
	//Position/velocity input state vector for target vehicle
	VECTOR3 RT, VT;
	double TT;
	//Flag for option to compute differential nodal regression from insertion to rendezvous
	//false = input DELNO, true = compute DELNO
	bool DELNOF;
	//Angle that is added to the target descending node to account for differential nodal regression
	double DELNO;
	//Lift-off time options for launch targeting
	//1 = input time, 2 = phase angle offset, 3 = biased phase zero (GMTLOR threshold), 4 = biased phase zero (TPLANE threshold), 5 = in-plane, 6 = in-plane with nodal regression
	int LOT;
	//Chaser vehicle weight at OMS-2
	double CWHT;
	//GMTLO* table flag. 1 = compute table, 2 = don't compute table
	int STABLE;
	//Delta time prior to each inplane launch point to start GMTLO* search
	double STARS;
	//Delta time after each inplane launch to end GMTLO* search
	double STARE;
	//0 = do not generate table, 1 = window opening, 2 = window closing, 3 = both, 4 = entire window
	int LPT;
	//Phase angle desired at insertion
	double OFFSET;
	//1 = Input VINS, GAMINS, RINS; 2 = Input GAMINS, RINS and height difference, 3 = Input GAMINS RINS, altitude at input angle from insertion
	int INSCO;
	//Launch azimuth coefficients
	double LAZCOE[4];
	//Recommended or threshold lift-off GMT
	double GMTLOR;
	//Delta time added to inplane time to obtain lift-off time
	double TRANS;
	//Desired height difference between chaser and target, or altitude of chaser, at input angle from insertion
	double DHW;
	//Angle from insertion to obtain a given altitude, or delta altitude
	double DU;
	//Bias that is added to GMTLO* to produce lift-off time
	double BIAS;
	//DT from lift-off, which defines the time of guidance reference release
	double DTGRR;
	//Nominal semimajor axis at insertion
	double ANOM;
	//Launchpad (1 = CSM, 2 = LM)
	int Pad;
};

class LaunchWindowProcessor : public RTCCModule
{
public:
	LaunchWindowProcessor(RTCC *r);
	void SetGlobalConstants(const LWPGlobalConstants &gl);
	void LWP(const LWPInputTable &in);

	LaunchWindowTimesTable lwtt;
	GMTLOTable gmtt;
	LWPSummary lwsum;
	RLOTDisplay rlott;
	LWPStateVectorTable svtab;
protected:
	void LWT();
	void UPDAT(VECTOR3 &R, VECTOR3 &V, double &T, double T_des);
	void NPLAN();
	void LENSR(double GMTLO);
	//GMT liftoff for a star time routine (zero phase angle)
	void GMTLS(double TI, double TF);
	//Recommended Lift-off Time routine
	void RLOT();
	//Compute launch targeting quantities and produce chaser insertion vector
	void TARGT();
	//Insertion parameters routine
	void NSERT(VECTOR3 RB, VECTOR3 VB, double GMTB, double &UINS);
	//Launch Window Parameters Routine
	void LWPT();
	//Launch Window Display routine
	void LWDSP();
	void RLOTD();
	//Launch Window Processor Output Tables routine
	void LWPOT();

	//Utility functions:
	//Compute phase angle between RT and RP
	double PHANG();
	//Compute time to travel phase angle
	double TTHET(double phase);

	//Analytical in-plane time
	double TIP;
	//GMT of lift-off
	double GMTLO;
	
	//GMT of inplane lift-off time (normally computed internally)
	double TPLANE;
	//Differential nodal precession
	double DELNOD;

	//Launch window/launch targeting options

	//Target vehicle state vector
	VECTOR3 RT, VT;
	double TT;
	//Delta time prior to in-plane time to start parameter table
	double TSTART;
	//Delta time after in-plane time to stop parameter table
	double TEND;
	//Time step in parameter table
	double TSTEP;
	//Phase angle at window closing (descending node)
	double PAC;
	//Phase angle at window opening (ascending node)
	double PAO;
	//GMT of lift-off for launch window opening (ascending node)
	double OPEN;
	//GMT of lift-off for launch window closing (descending node)
	double CLOSE;
	//GMT of insertion
	double TINS;
	//Wedge angle between planes
	double WEDGE;
	//Time of minimum yaw steering lift-off
	double TYAW;

	//Launch window parameter table options
	
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
	//Gemini parallel launch azimuth
	double GPAZ;
	//Phase angle at insertion
	double PA;
	//Chaser state vector
	VECTOR3 RP, VP;
	double TP;
	//Number of GMTLO* times computed
	int K25;
	//GMTLO* time array
	double STAR[10];
	//Last GMTLO* time computed
	double GSTAR;
	//Delta height at insertion
	double DH;

	//Geocentric latitude of launch site
	double LATLS;
	//Geocentric longitude of launch site
	double LONGLS;

	int error;

	LWPGlobalConstants GLOCON;
	LWPProgramConstants PROCON;
	LWPInputTable inp;

	AEGDataBlock aegdata;
};