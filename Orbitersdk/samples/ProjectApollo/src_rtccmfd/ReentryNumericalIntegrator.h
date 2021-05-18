/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Reentry Numerical Integrator Module - RMMYNI (Header)

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

struct GNDataTab
{
	GNDataTab();
	//Constants:
	//Earth rate (rad/sec)
	const double WIE = 72.9211505e-6;
	//Nominal time of flight (sec)
	const double TN = 500.0;
	//Lateral switch gain (rad)
	const double KLAT1 = 1.0 / 24.0;
	//Minimum drag for upcontrol (m/s^2)
	const double Q7F = 6.0*0.3048;
	//Initial variation in LEWD
	const double DLEWD0 = -0.05;
	//Nominal upcontrol L/D
	const double LEWDI = 0.15;
	//Time of flight constant
	const double KTETA = 1000.0;
	//RDOT to start into HUNTEST
	const double VRCONTROL = 700.0*0.3048;
	//Factors in KA computation
	const double KA1 = 1.3;
	const double KA2 = Q7F / (32.2*0.3048);
	//Factors in D0 computation
	const double KA3 = 90.0*0.3048;
	const double KA4 = 40.0*0.3048;
	//Velocity to start final phase on INITENTRY
	const double VFINAL1 = 27000.0*0.3048;
	//Factor in initial attitude
	const double VFINAL = 26600.0*0.3048;
	//Initial attitude gain
	const double K44 = 19749550.0*0.3048;
	//Lateral switch bias term
	const double LATBIAS = 0.00012;
	//Maximum drag for down-lift
	const double C20 = 175.0*0.3048;
	//Factor in ALP computation
	const double C1 = 1.25;
	//Velocity to stop steering
	const double VQUIT = 1000.0*0.3048;
	//Minimum VL
	const double VLMIN = 18000.0*0.3048;
	//Factor in AHOOK computation
	const double CHOOK = 0.25;
	//Factor in GAMMAL computation
	const double CH1 = 1.0;
	//Constant in GAMMAL1
	const double Q19 = 0.5;
	//Final phase D range/DV (meters per meters per second)
	const double Q3 = 0.07*1852.0 / 0.3048;
	//Final phase D range/D GAMMA (meters per rad)
	const double Q5 = 7050.0*1852.0;
	//Final phase initial flight path angle
	const double Q6 = 0.0349;
	//Bias velocity for final phase start
	const double C18 = 500.0*0.3048;
	//Optimized upcontrol gains
	const double KB1 = 3.4;
	const double KB2 = 0.0034 / 0.3048;
	//Factor to reduce upcontrol gain
	const double POINT1 = 0.1;
	//Increment of Q7 to detect end of Kepler phase
	const double KDMIN = 0.5*0.3048;
	//Constant in FINAL PHASE
	const double K13P = 4.0;
	//Maximum acceleration
	const double GMAX = 257.6*0.3048;
	double VREF_TAB[13];
	double RDTRF_TAB[13];
	double AREF_TAB[13];
	double FRDT_TAB[13];
	double FA_TAB[13];
	double RTOGO_TAB[13];
	double PP_TAB[13];

	//Initial Switches:
	//Final phase switch
	bool EGSW = false;
	//Indicates overshoot of target
	bool GONEPAST = true;
	//Overshoot indicator
	bool GONEBY = false;
	//Indicates iteration in HUNTEST
	bool HIND = false;
	//Indicates initial roll attitude set
	bool INRLSW = false;
	//Relative velocity switch
	bool RELVELSW = false;
	//Inhibit downlift switch in DAP if set=0
	bool LATSW = true;
	//0.05g switch
	bool K05GSW = false;
	//Inhibits roll switch during upcontrol
	bool NOSWITCH = false;
	//Indicates program has started utilizing guidance commands
	bool ROLLSW = false;
	//Indicate a bad trajectory
	bool NOGOSW = false;

	//Variables:
	//Initial unit target vector
	VECTOR3 URTO;
	//Vector east at initial target
	VECTOR3 RTE;
	//Vector normal to RTE and UZ
	VECTOR3 UTR;
	//Target vector
	VECTOR3 URT;
	//Earth rate times time
	double WT;
	//Desired great circle range (radians)
	double THETA;
	//Desired great circle range (meters)
	double THETNM;
	//Parameter used in calculation of roll command
	double K2ROLL;
	//Lateral switch gain
	double KLAT;
	double LD_CMINR;
	//Used in upcontrol
	double FACTOR;
	//Minimum drag for upcontrol
	double Q7;
	//Initial CM bank angle
	double C10;
	//Desired lift-to-drag ratio (osculating plane)
	double LD;
	//Change in LEWD
	double DLEWD;
	//Upcontrol reference L/D
	double LEWD;
	//Final phase range, function of LAD (meters)
	double Q2;
	//Mode selector (1 = initial roll, 2 = Huntest, 3 = Upcontrol, 4 = Ballistic, 5 = Final)
	int SELECTOR;
	//Huntest counter?
	int HUNTCN;
	//Lateral range
	double LATANG;
	//Excess centrifugal force over gravity: = (VSQ-1)*GS
	double LEQ;
	//Total aerodynamic acceleration
	double D;
	//Altitude rate
	double RDOT;
	//Velocity magnitude
	double v;
	//DRAG TO LIFT UP IF DOWN
	double KA;
	//Controlled constant drag
	double D0;
	//Temporary storage for LD in lateral logic
	double LD1;
	//Normalized velocity squared
	double VSQ;
	//Lateral miss limit
	double Y;
	//Initial velocity for upcontrol
	double V1;
	//Initial drag for upcontrol
	double A0;
	//Drag value in factor calculation
	double A1;
	//Exit velocity for upcontrol
	double VL;
	//Flight-path angle at VL
	double GAMMAL;
	//Simple form of GAMMAL
	double GAMMAL1;
	//Previous value of DIFF
	double DIFFOLD;
	//Constant for HUNTEST
	double ALP;
	//Constants for upcontrol
	double FACT1, FACT2;
	//VSAT or V1, whichever is smaller
	double VS1;
	//Terms in GAMMAL computation
	double AHOOK, DHOOK;

	//DAP gains and constants:
	double SWTCH2 = 0.0;
};

class ReentryNumericalIntegrator : public RTCCModule
{
public:
	ReentryNumericalIntegrator(RTCC *r);
	void Main(const RMMYNIInputTable &in, RMMYNIOutputTable &out);
protected:
	void RungeKuttaIntegrationRoutine(VECTOR3 R_N, VECTOR3 V_N, double dt, VECTOR3 &R_N1, VECTOR3 &V_N1);
	VECTOR3 SecondDerivativeRoutine(VECTOR3 R, VECTOR3 V);
	VECTOR3 GravityAcceleration(VECTOR3 R);
	VECTOR3 LiftDragAcceleration(VECTOR3 R, VECTOR3 V, double &AOA);
	void CalculateLiftDrag(double mach, double &CL, double &CD, double &AOA);
	void GuidanceRoutine(VECTOR3 R, VECTOR3 V);
	void CalculateDragAcceleration(VECTOR3 R, VECTOR3 V);
	void Limit02PI(double &val);
	double ConstantGLogic(VECTOR3 unitR, VECTOR3 VI, double D);
	bool IsInSBandBlackout(double v_r, double h);
	bool IsInVHFBlackout(double v_r, double h);

	//G&N
	void GNInitialization();
	void GNTargeting();
	void GNModeSelector();
	void GNInitialRoll();
	void GNHuntest();
	void GNRangePrediction();
	void GNConstD();
	void GNNegTest();
	void GNUpcontrol();
	void GNBallistic();
	void GNFinalPhase();
	void GNGLimiter();
	void GNRoutine310();
	void GNRoutine380();

	//Previous and current state vector
	VECTOR3 R_cur, V_cur, R_prev, V_prev;
	//GMT of initial state vector
	double GMT0;
	double T, T_prev;
	//Time difference
	double DT;
	//Ratio of area to mass of spacecraft
	double N;
	double Bank;
	double DRAG;
	double A_X;
	//Initial bank angle
	double K1;
	//Final bank angle
	double K2;
	int LiftMode;
	//Has 0.05g been passed?
	bool K05G;
	//Has specified G level been passed?
	bool KGC;
	double g_c_BU, g_c_GN;
	//Time to reverse bank angle
	double t_RB;
	//Bank angle rate
	double BRATE;
	//EMS initialization altitude
	double H_EMS;
	//Constant g level
	double D0;
	VECTOR3 U_Z;
	double LOD, LAD;
	double RLDIR;
	int IEND;
	//Roll command
	double ROLLC;
	bool ISGNInit;
	//Maximum g level encountered during the reentry
	double gmax;
	//Time of above
	double t_gmax;
	//Time when 0.05g was passed
	double t_05G;
	//Time when 0.2g was passed
	double t_2G;
	//Time when specific g level was passed
	double t_gc;
	//Altitude
	double alt;
	//Wind relative velocity
	VECTOR3 V_R;
	//Wind relative speed
	double v_R;
	//Drogue and main chute deplyed?
	bool droguedeployed, maindeployed;
	//Time of drogue and main chute deployment
	double t_drogue, t_main;
	//Ephemeris build indicator
	bool EphemerisBuildInd;
	//Next ephemeris storage
	double TNEXT;
	//End of current step
	double TE;
	//Step size
	double STEP;
	//Reverse bank angle implemented
	bool IREVBANK;
	double EPS;

	//Parameters for constant G and G&N
	double VSAT;
	double VMIN;
	double C16, C17;

	//Constants
	double HS;
	double GS;
	double KWE;
	double RE;
	double ATK; //Angle in RAD to meters (meters/rad)

	//G&N
	GNDataTab GNData;
};