/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC CSM and LM Numerical Integrator and Guidance (Header)

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
#include "RTCCTables.h"

class RTCC;

struct PMMRKJInputArray
{
	//SV at maneuver initiation (first phase)
	EphemerisData sv0;
	//Ephemeris output time cycle (sec.)
	double DTOUT;
	//Weight loss rate multiplier
	double WDMULT = 1.0;
	//Time of LM jettison of descent stage
	double LMDESCJETT = 1e10;
	//Density multiplier
	double DENSMULT = 1.0;
	//Vehicle Cross Section
	double A = 0.0;
	//Weight of specified configuration
	double CAPWT = 0.0;
	//Ephemeris output option: -1 for R only, 0 for none, 1 for R and V, 2 for R and V and weights
	int KEPHOP;
	//Auxiliary output option (1 for table desired, 0 for table not desired)
	bool KAUXOP;
	double CSMWT = 0.0;
	double LMAWT = 0.0;
	double LMDWT = 0.0;
	double SIVBWT = 0.0;
	//0 = no maneuver, 1 = fixed inertial, 2 = manual holding body orientation, 3 = Lambert Guidance, 4 = External DV (primary), 5 = External DV (LM AGS)
	int MANOP;
	int ThrusterCode;
	//0 = two thrusters, 1 = four thrusters
	bool UllageOption;
	//Configuration code at maneuver initiation (0 for CSM, 1 for LM, 2 for CSM and LM)
	int IC;
	//DT of ullage (=0 for RCS)
	double DTU = 0.0;
	//DT of maneuver (no pure ullage or tailoff, =0 if DV is given)
	double DTMAN = 0.0;
	//DV desired (if <0 it is DV along X-body excluding tailoff)
	double DVMAN = 0.0;
	//Unit thrust vector at burn initiate
	VECTOR3 AT;
	//Body axes vectors
	VECTOR3 XB, YB, ZB;
	//DT at 10% thrust level for DPS thruster
	double DTPS10 = 26.0;
	//Maneuvering vehicle code (1 = CSM, 2 = S-IVB, 3 = LM)
	int TVC;
	//Thruster trim angle indicator (-1 = compute angles, 1 = system parameters)
	int KTRIMOP;
	//Docking angle
	double DOCKANG;
	//Velocity-to-be-gained for External DV
	VECTOR3 VG;
	//DPS scale factor
	double DPSScale = 0.925;
	//Heads up/down indicator, false = down, true = up
	bool HeadsUpDownInd;
	//false = inertial, true = P30
	bool ExtDVCoordInd;
};

class CSMLMPoweredFlightIntegration
{
public:
	CSMLMPoweredFlightIntegration(RTCC *r, PMMRKJInputArray &T, int &I, EphemerisDataTable *E, RTCCNIAuxOutputTable *A);
	void PMMRKJ();
private:
	void PCINIT();
	void PCRUNG(EphemerisDataTable *E, std::vector<double> &W);
	void PCRDD();
	void PCGUID();
	void CalcBodyAttitude();

	//Current position vector
	VECTOR3 R;
	//Current working position vector
	VECTOR3 RP;
	//Current velocity vector
	VECTOR3 V;
	//Current working velocity vector
	VECTOR3 VP;
	//Thruster switch
	int KTHSWT;
	//Current weight
	double WT;
	//Thrust level
	double THRUST;
	//Weight loss rate
	double WTLRT;
	double DT, DT2, DT4, DT6, DTPREV;
	//Maneuver initiation time (first phase, ullage)
	double TBM = 0.0;
	//Maneuver initiate time (second phase, maneuver engine on time)
	double TBI;
	//Current Time
	double T;
	//Time of next ephemeris storage
	double TNEXT;
	//Time of last ephemeris or weight storage
	double TLOP;
	//Indicator
	double TLARGE;
	//Previous time
	double TPREV;
	//Time of next event
	double TI;
	//End of current integration step
	double TAU;
	//End of current step
	double TE;
	//Manuever phase
	//1 = Ullage
	//2 = Thrust buildup during ullage
	//3 = Mainstage thrust during ullage
	//7 = Max thrust phase
	//8 = Tailoff
	int MPHASE;
	//Short maneuver test
	int KGN;
	//Current integration step
	double STEP;
	int KEND;
	//0 = guidance started, 1 = before steering started
	int IATT;
	//Thrust along x-axis
	double THX;
	//Thrust unit vector
	VECTOR3 A_T;
	//Code for Runge-Kutta function
	int KCODE;
	//Potential acceleration
	VECTOR3 RDDP;
	//Calculated acceleration
	VECTOR3 RDD;
	//Thrust acceleration
	VECTOR3 RDDT;
	//DeltaV to go
	double DVGO;
	//Current thrust level
	double T_c;
	//Time to begin guidance
	double Tg;
	//Accumulated DV
	double DV;
	//Accumulated DV alongf X-axis
	double DVX;
	//Weight limit
	double WTLIM;
	double TEND;
	double P_G = 0.0;
	double Y_G = 0.0;
	double WC, WL, WS;
	int IA, IJ;
	double DTUL;
	double SIGN;
	//Area over mass ratio
	double AOM;
	double CD = 0.0;
	//Tailoff DV
	double DVTO = 0.0;
	//Tailoff DV along x-axis
	double DVTOX = 0.0;
	double DTGO;
	double TCO;
	VECTOR3 X_B, Y_B, Z_B;
	bool AttGiven;
	VECTOR3 A_TR, A_TM, A_TL;
	VECTOR3 VG;
	//Previous step through guidance
	double DTP;
	//Current step through guidance
	double DTN;
	VECTOR3 Vn_apo, g_apo;
	double Vg_apo;
	//Steering enabled: 0 = no, 1 = yes
	int Kg;
	//DT of maneuver (no pure ullage or tailoff, =0 if DV is given)
	double DTMAN = 0.0;

	//Thrust and weight loss rate tables
	//0 = Ullage
	//1 = phase 2 (buildup + ullage)
	//2 = phase 3 (main + ullage)
	//3 = 10% thrust level (DPS only)
	//4 = buildup to maximum thrust level (DPS only)
	//5 = ?
	//6 = thrust level for short maneuver test
	//7 = tail-off thrust level
	//8 = ?
	//9 = full load thrust level
	double THPS[10];
	double WDOTPS[10];

	//Thruster specific times
	//0 = ullage?
	//1 = phase 2 burn time
	//2 = phase 3 burn time
	//3 = burn time for short maneuver test
	//4 = build-up to maximum burn time (DPS only)
	//5 = elapsed time from engine on to initial steer
	//6 = Minimum impulse time, time for ten percent test (DPS)
	//7 = tail-off burn time
	//8 = total ullage overlap
	double DTSPAN[9];

	//Minimum impulse constants
	//0 = Impulse in one second SPS/APS/DPS burn
	//1 = Initial value of SPS/APS/DPS minimum impulse curve
	//2 = Slope of SPS/APS/DPS curve
	//3 = RCS ullage impulse
	double XK[4];

	//Stored for Aux Table
	VECTOR3 A_T_out;
	double PGBI;
	double YGBI;
	double DV_ul;
	double DTMANE;
	EphemerisData sv1, sv2, sv_ff;
	double WTENGON;
	VECTOR3 VGN;
	double DTTOC;
	double RCSFUELUSED;
	double MAINFUELUSED;

	RTCC *rtcc;
	PMMRKJInputArray &TArr;
	int &IERR;
	RTCCNIAuxOutputTable *Aux;
	EphemerisDataTable *Eph;
	std::vector<double> WeightTable;
};