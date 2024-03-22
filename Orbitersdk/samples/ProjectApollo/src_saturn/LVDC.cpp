/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Launch Vehicle Digital Computer (C++ Implementation)

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
#include <string>
#include <iostream>
#include <fstream>

#include "Orbitersdk.h"

#include "nasspdefs.h"

#include "papi.h"
#include "../src_rtccmfd/OrbMech.h"
#include "LVDC.h"
#include "LVDA.h"

//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

LVDC::LVDC(LVDA &lvd) : lvda(lvd)
{
	RealTimeClock = 0.0;
	ReadyToLaunch = false;
	TI = 0.0;
}

double LVDC::log(double a)
{
	if (a < 0.125)
	{
		fprintf(lvlog, "Logarithm outside limit! %lf\r\n", a);
		return std::log(0.125);
	}
	if (a > 8.0)
	{
		fprintf(lvlog, "Logarithm outside limit! %lf\r\n", a);
		return std::log(8.0);
	}
	return std::log(a);
}

MATRIX3 LVDC::tmat(MATRIX3 a)
{
	MATRIX3 b;

	b = _M(a.m11, a.m21, a.m31, a.m12, a.m22, a.m32, a.m13, a.m23, a.m33);
	return b;
}

void LVDC::PrepareToLaunch()
{
	if (ReadyToLaunch == false)
	{
		double iptr;
		RealTimeClock = modf(oapiGetSimMJD(), &iptr)*24.0*3600.0;
		ReadyToLaunch = true;
	}
}

// Constructor
LVDC1B::LVDC1B(LVDA &lvd) : LVDC(lvd)
{
	int x=0;
	Initialized = false;					// Reset cloberness flag
	// Zeroize
	// chars
	FSPFileName[0] = '\0';
	// bool
	alpha_D_op = false;
	BOOST = false;
	CountPIPA = false;
	GATE = false;
	GATE5 = false;
	GRR_init = false;
	HSL = false;
	LVDC_GRR = false;
	LVDC_Stop = false;
	poweredflight = false;
	S1B_Engine_Out = false;
	S1B_CECO_Commanded = false;
	S4B_IGN = false;
	TerminalConditions = false;
	// int
	IGMCycle = 0;
	LVDC_Timebase = 0;
	T_EO1 = 0;
	T_EO2 = 0;
	UP = 0;
	// double
	a = 0;
	a_1 = 0;
	a_2 = 0;
	A1 = 0;
	A2 = 0;
	A3 = 0;
	A4 = 0;
	A5 = 0;
	alpha_D = 0;
	for (x = 0;x < 4;x++)
	{
		Ax[x] = 0;
	}
	Azimuth = 0;
	A_zL = 0;
	B_11 = 0;
	B_21 = 0;
	B_12 = 0;
	B_22 = 0;
	C_2 = 0;
	C_4 = 0;
	C_3 = 0;
	CG = 0;
	Chi_xp_apo = 0.0;
	Chi_zp_apo = 0.0;
	cos_chi_Yit = 0;
	cos_chi_Zit = 0;
	d2 = 0;
	ddot_xi_G = 0;
	ddot_eta_G = 0;
	ddot_zeta_G = 0;
	ddot_zeta_GT = 0;
	ddot_xi_GT = 0;
	DescNodeAngle = 0;
	deta = 0;
	dxi = 0;
	dot_dxi = 0;
	dot_deta = 0;
	dot_dzeta = 0;
	dot_dxit = 0;
	dot_detat = 0;
	dot_dzetat = 0;
	dotM_1 = 0;
	dotM_2 = 0;
	dot_phi_1 = 0;
	dot_phi_T = 0;
	dot_zeta_T = 0;
	dot_xi_T = 0;
	dot_eta_T = 0;
	dt = 0;
	dT_2 = 0;
	dt_c = 0;
	dT_F = 0;
	dt_LET = 0;
	dtt_1 = 0;
	dtt_2 = 0;
	DT_N = 0.0;
	DT_N1 = DT_N2 = DT_N3 = DT_N4 = DT_N5 = DT_N6 = 0.0;
	for (x = 0; x < 5; x++) {
		Drag_Area[x] = 0;
	}
	MS25DT = 0.0;
	MS04DT = 0.0;
	dV = 0;
	dV_B = 0;
	e = 0;
	eps_2 = 0;
	eps_3 = 0;
	eps_4 = 0;
	f = 0;
	F = 0;
	Fm = 0;
	for(x=0; x < 5; x++){
		int y=0;
		for(y=0; y < 5; y++){
			Fx[x][y] = 0;
		}
	}
	G_T = 0;
	Inclination = 0;
	J = 0;
	D = 0;
	H = 0;
	h = 0;
	h_1 = 0;
	h_2 = 0;
	J_1 = 0;
	J_2 = 0;
	Jt_2 = 0;
	J_Y = 0;
	J_P = 0;
	K_1 = 0;
	K_2 = 0;
	K_3 = 0;
	K_4 = 0;
	K_5 = 0;
	K_D = 0;
	K_p = 0;
	K_y = 0;
	K_r = 0;
	K_Y = 0;
	K_P = 0;
	KSCLNG = 0;
	D_P = 0;
	D_Y = 0;
	L_1 = 0;
	L_2 = 0;
	dL_2 = 0;
	Lt_2 = 0;
	Lambda_0 = 0;
	lambda_dot = 0;
	L_P = 0;
	L_Y = 0;
	Lt_Y = 0;
	LVDC_TB_ETime = 0;
	MFS = 0;
	for (x = 0;x < 8;x++)
	{
		MFK[x] = 0;
	}
	for (x = 0;x < 8;x++)
	{
		MFSArr[x] = 0;
		MF[x] = 0;
	}
	MF0 = 0;
	MSK16 = 0;
	MSK5 = 0;
	MSK6 = 0;
	MSLIM1 = MSLIM2 = 0.0;
	mu = 0;
	NUPTIM = 0;
	omega_E = 0;
	p = 0;
	P_1 = 0;
	P_2 = 0;
	PHI = 0;
	PHIP = 0;
	phi_T = 0;
	Q_1 = 0;
	Q_2 = 0;
	Q_Y = 0;
	Q_P = 0;
	R = 0;
	ROV = 0;
	R_T = 0;
	for (x = 0; x < 6; x++) {
		Rho[x] = 0;
	}
	rho_c = 0;
	S_1 = 0;
	S_2 = 0;
	S_P = 0;
	S_Y = 0;
	sinceLastIGM = 0;
	sin_chi_Yit = 0;
	sin_chi_Zit = 0;
	sin_ups = 0;
	cos_ups = 0;
	SMCG = 0;
	S = 0;
	P = 0;
	t = 0;
	t_1 = 0;
	T_1 = 0;
	t_2 = 0;
	T_2 = 0;
	t_3 = 0;
	t_3i = 0;
	t_4 = 0;
	t_5 = 0;
	t_6 = 0;
	T_ar = 0;
	TAS = 0;
	tau1 = 0;
	tau2 = 0;
	t_B1 = 0;
	t_B3 = 0;
	tchi_y_last = 0;
	tchi_p_last = 0;
	tchi_y = 0;
	tchi_p = 0;
	t_clock = 0;
	T_CO = 0;
	t_D = 0;
	T_d = 0;
	t_fail = 0;
	T_GO = 0;
	T_GRR = 0;
	T_GRR0 = 0;
	theta_xa = 0.0;
	theta_za = 0.0;
	OrbitalGuidanceCycle = 0;
	T_L_apo = 0;
	T_LET = 0;
	T_S1 = 0;
	T_S2 = 0;
	T_S3 = 0;
	TSMC1 = 0;
	TSMC2 = 0;
	T_SON = 0.0;
	T_SOM = 0.0;
	Y_ref = Z_ref = X_ref = 0.0;
	T_T = 0;
	Tt_2 = 0;
	Tt_T = 0;
	U_1 = 0;
	U_2 = 0;
	gamma_T = 0;
	V = 0;
	V_ex1 = 0;
	V_ex2 = 0;
	V_i = 0;
	V_0 = 0;
	V_1 = 0;
	V_2 = 0;
	V_T = 0;
	V_TC = 0;
	xi_T = 0;
	X_S1 = 0;
	X_S2 = 0;
	X_S3 = 0;
	Xtt_y = 0;
	Xtt_p = 0;
	X_Zi = 0;
	X_Yi = 0;
	Y_u = 0;
	// VECTOR3 
	AttitudeError = _V(0,0,0);
	AttitudeErrorOld = _V(0, 0, 0);
	CommandedAttitude = _V(0,0,0);
	CurrentAttitude = _V(0,0,0);
	DChi_apo = _V(0, 0, 0);
	ddotG_act = _V(0,0,0);
	ddotG_last = _V(0,0,0);
	ddotM_act = _V(0,0,0);
	ddotM_last = _V(0,0,0);
	DDotXEZ_G = _V(0,0,0);
	DeltaAtt = _V(0,0,0);
	Dot0 = _V(0,0,0);
	DotG_act = _V(0,0,0);
	DotG_last = _V(0,0,0);
	DotM_act = _V(0,0,0);
	DotM_last = _V(0,0,0);
	DotS = _V(0,0,0);
	DotXEZ = _V(0,0,0);
	PCommandedAttitude = _V(0,0,0);
	Pos4 = _V(0,0,0);
	PosS = _V(0,0,0);
	PosXEZ = _V(0,0,0);
	Pos_Nav = _V(0, 0, 0);
	R_OG = _V(0, 0, 0);
	Vel_Nav = _V(0, 0, 0);
	W_ES = _V(0, 0, 0);
	ddotS = _V(0, 0, 0);
	// MATRIX3
	MX_A = _M(0,0,0,0,0,0,0,0,0);
	MX_B = _M(0,0,0,0,0,0,0,0,0);
	MX_G = _M(0,0,0,0,0,0,0,0,0);
	MX_K = _M(0,0,0,0,0,0,0,0,0);
	MX_phi_T = _M(0,0,0,0,0,0,0,0,0);

	CommandSequence = 0;
	OrbitManeuverCounter = 0;

	for (int i = 0;i < 10;i++)
	{
		SP_M[i] = 0.0;
		CP_M[i] = 0.0;
		SY_M[i] = 0.0;
		CY_M[i] = 0.0;
		RA_M[i] = 0.0;
		t_Maneuver[i] = 0.0;
		TYP_M[i] = 0;
	}
	SPITCH = 0.0;
	CPITCH = 0.0;
	SYAW = 0.0;
	CYAW = 0.0;
	ROLLA = 0.0;
	FSLR = false;
	FMANT = 0;
	FSCAH = false;
	FSCIC = false;
	FFPIH = false;
}

void LVDC1B::Init(){
	if(Initialized == true){ 
		fprintf(lvlog,"init called after init, ignored\r\n");
		fflush(lvlog);
		return;
	}

	sprintf(FSPFileName, "Config\\ProjectApollo\\Saturn IB Default Flight Sequence Program.txt");
	
	//presettings in order of boeing listing for easier maintainece
	//GENERAL
	e = 0.00417135950879776;
	f = 359.611584444894;
	C_3 = -60139891.8062616; // Stored as twice the etc etc.
	Inclination = 31.605;
	DescNodeAngle = 119.0;
	A_zL = 100.0;
	Azimuth = 72.0;
	Lambda_0 = 119.0;
	GATE = false;							// 'chi-freeze-gate': freezes steering commands when true
	GATE5 = false;							// allows single pass through HSL initialization when false
	T_LET = 25;								// LET Jettison Time, i.e. the time IGM starts after start of TB3
	UP = 0;									// switching variable for Tt_t parameter update
	TerminalConditions = true;
	//PRE_IGM GUIDANCE
	B_11 = -0.62;							// Coefficients for determining freeze time after S1C engine failure
	B_12 = 40.9;							// dto.
	B_21 = -0.3611;							// dto.
	B_22 = 29.25;							// dto.
	// PITCH POLYNOMIAL (AS-205)
	Fx[1][0] = -0.8014017976;
	Fx[1][1] = 0.1834943176;
	Fx[1][2] = -0.0127672185;
	Fx[1][3] = 0.000065735448;
	Fx[1][4] = 0.0;
	Fx[2][0] = 19.31641263;
	Fx[2][1] = -0.6190923568;
	Fx[2][2] = -0.0022020716;
	Fx[2][3] = 0.000020108813;
	Fx[2][4] = 0.0;
	Fx[3][0] = 12.08184643;
	Fx[3][1] = -0.5606283163;
	Fx[3][2] = -0.0005797187;
	Fx[3][3] = 0.00000592461278;
	Fx[3][4] = 0.0;
	Fx[4][0] = -59.31545;
	Fx[4][1] = 0.0;
	Fx[4][2] = 0.0;
	Fx[4][3] = 0.0;
	Fx[4][4] = 0.0;
	T_d = 1.2375;
	//Azimuth polynomial (AS-205)
	Ax[0] = 72.0;
	Ax[1] = 0.0;
	Ax[2] = 0.0;
	Ax[3] = 0.0;

	// Air density polynomial
	Rho[0] = 0.179142e-6;	Rho[1] = -0.37213949e-11;	Rho[2] = 0.31057886e-16;
	Rho[3] = -0.12962178e-21;	Rho[4] = 0.2698641e-27;	Rho[5] = -0.2238826e-33;
	Drag_Area[0] = 11.079923;
	Drag_Area[1] = 0.17954281;
	Drag_Area[2] = -2.1771971;
	Drag_Area[3] = -0.28074902;
	Drag_Area[4] = -5.8764139;

	t_1 = 10;								// Backup timer for Pre-IGM pitch maneuver
	t_2 = 25;								// Time to initiate pitch freeze for S1C engine failure
	t_3 = 36;								// Constant pitch freeze for S1C engine failure prior to t_2
	t_3i=0;									// Clock time at S4B ignition
	t_4 = 45;								// Upper bound of validity for first segment of pitch freeze
	t_5 = 81;								// Upper bound of validity for second segment of pitch freeze
	t_6 = 0;								// Time to terminate pitch freeze after S1C engine failure
	T_ar = 134.3;							// S1C Tilt Arrest Time	   
	T_S1 = 63.8;							// Pitch Polynomial Segment Times
	T_S2 = 109.8;							// dto.
	T_S3 = 134.3;							// dto.
	T_EO1 = 0;								// switching constant for SIC engine-out handling
	T_EO2 = 0;								// allows single pass through IGM engine-out presettings when 0
	dt = 1.7;								// Nominal powered-flight or coast-guidance computation-cycle interval
	dT_F=0;									// Period of frozen pitch in S1C
	dt_LET = 25;							// Nominal time between SII ign and LET jet
	t_fail =0;								// S1C Engine Failure time
	//IGM BOOST TO ORBIT	
	dotM_1 = 242.7976615;						// Mass flowrate of SIVB from approximately LET jettison to second MRS
	dotM_2 = 183.3909139;						// Mass flowrate of SIVB after second MRS
	dV_B = 6.22; // AP11// dV_B = 2.0275; // AP9// Velocity cutoff bias for orbital insertion
	ROV = 1.8;
	PHI = 28.5217969*RAD;
	PHIP = 28.5217969*RAD;
	R_L = 6373407.3;
	omega_E = 0.729211e-4;
	KSCLNG = -80.5612465*RAD;
	SMCG = 0.03;
	TSMC1 = 20; TSMC2 = 5; // AP9
	// TSMC1 = 60.6 TSMC2 = 15 // AP11

	// (M/F)S filter coeffcients
	//Values from ASTP LVOT. Commented values from EDD:
	MFK[0] = 0.32157638; //0.0120948;
	MFK[1] = -0.2687668; //0.0213339;
	MFK[2] = 0.0; //-0.0261577;
	MFK[3] = 0.0; //-0.00526684;
	MFK[4] = 1.6504718; //2.80483;
	MFK[5] = -0.70328021; //-2.8588;
	MFK[6] = 0.0; //1.24933;
	MFK[7] = 0.0; //-0.197360;
	MF0 = 0.135;

	T_1 = 286;								// Time left in first-stage IGM
	T_2 = 0;								// Time left in second and fourth stage IGM
	Tt_2 = 150;								// Estimated third or fifth stage burn time
	Tt_T = T_1 + Tt_2;						// Time-To-Go computed using Tt_3
	t = 1.7;									// Time from accelerometer reading to next steering command
	t_B1 = 2;								// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	t_B3 = 0;								// Time from second S2 MRS signal
	V_ex1 = 4159.44;
	V_ex2 = 4198.68;
	V_TC = 300;
	
	eps_2 = 35;								// Time to begin chi bar steering
	eps_3 = 10000;							// Terminal condition freeze time
	eps_4 = 3;								// Time to enable HSL loop & chi freeze
	mu =398603200000000;					// Product of G and Earth's mass
	tau2 = 324; //tau2 = 722.67;		// Time to consume all fuel between MRS and S2 Cutoff

	//Not in boeing doc, but needed for nav:
	a = 6.373338e6;							//earth's equatorial radius
	J = 1.62345e-3;							//first coefficient of earth's gravity
	D = 0.7875e-5;
	H = 0.575e-5;

	h_1 = 1.5e5;
	h_2 = 3.0e5;
	rho_c = 0.5e-7;
	K_D = 5.7089e-04;//Apollo 7 weight at S-IVB cutoff //0.164311287e-2; Skylab EDD

	//'real' software variable, i.e. those are computed at runtime
	// Software flags
	GRR_init = false;
	poweredflight = false;
	HSL=false;								// High-Speed Loop flag
	BOOST=false;							// Boost To Orbit
	S4B_IGN=false;							// SIVB Ignition
	LVDC_GRR = false;
	tau1=0;									// Time to consume all fuel before S2 MRS
	Fm=0;									// sensed total accel
	CommandedAttitude=_V(0,0,0);			// Commanded Attitude (RADIANS)
	PCommandedAttitude=_V(0,0,0);			// Previous Commanded Attitude (RADIANS)
	CurrentAttitude=_V(0,0,0);				// Current Attitude   (RADIANS)
	F=0;									// Force in Newtons, I assume.	
	K_Y=0; K_P=0; D_P=0; D_Y=0;				// Intermediate variables in IGM
	P_1=0; P_2=0; 
	L_1=0; L_2=0; L_P=0; L_Y=0; Lt_Y=0;
	J_1=0; J_2=0; J_Y=0; J_P=0; 
	S_1=0; S_2=0; S_P=0; S_Y=0; 
	U_1=0; U_2=0;  
	Q_1=0; Q_2=0; Q_Y=0; Q_P=0; 
	d2=0;
	//f=0;									// True anomaly of the predicted cutoff radius vector
	//e=0;									// Eccentricity of the transfer ellipse
	C_2=0; C_4=0;							// IGM coupling terms for pitch steering
	p=0;									// semilatus rectum of terminal ellipse
	K_1=0; K_2=0; K_3=0; K_4=0;				// Correction to chi-tilde steering angles, K_i
	K_5=0;									// IGM terminal velocity constant
	R=0;									// Instantaneous Radius Magnitude
	R_T = 6563366.0;						// Desired terminal radius
	V = 0;									// Instantaneous vehicle velocity
	V_T = 7793.0429;						// Desired terminal velocity
	V_i=0; V_0=0; V_1=0; V_2=0;				// Parameters for cutoff velocity computation
	gamma_T=0;								// Desired terminal flight-path angle
	MX_A=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to equatorial
	MX_B=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from equatorial to orbital coordinates
	MX_G=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to orbital
	MX_K=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to terminal
	MX_phi_T=_M(0,0,0,0,0,0,0,0,0);			// Matrix made from phi_T
	phi_T=0;								// Angle used to estimate location of terminal radius in orbital plane
	Pos4=_V(0,0,0);							// Position in the orbital reference system
	PosS=_V(0,0,0);							// Position in the earth-centered plumbline system. SPACE-FIXED.
	DotS=_V(0,0,0);							// VELOCITY in the earth-centered plumbline system
	DotM_act=_V(0,0,0);						// actual sensed velocity from platform
	ddotM_act =_V(0,0,0);
	ddotM_last =_V(0,0,0);
	ddotG_act=_V(0,0,0);					// actual computed acceleration from gravity
	DotG_act=_V(0,0,0);						// actual computed velocity from gravity
	DotM_last=_V(0,0,0);					// last sensed velocity from platform
	ddotG_last=_V(0,0,0);					// last computed acceleration from gravity
	DotG_last=_V(0,0,0);					// last computed velocity from gravity
	alpha_D=0;								// Angle from perigee to DN vector
	alpha_D_op=true;						// Option to determine alpha_D or load it
	G_T= -9.255;							// Magnitude of desired terminal gravitational acceleration
	xi_T=0;									// Desired position component in the terminal reference system
	PosXEZ=_V(0,0,0);						// Position components in the terminal reference system
	DotXEZ=_V(0,0,0);						// Instantaneous something
	deta=0; dxi=0;							// Position components to be gained in this axis
	T_T=0;									// Time-To-Go computed using T_2
	tchi_y=0; tchi_p=0;						// Angles to null velocity deficiencies without regard to terminal data
	dot_zeta_T=0; dot_xi_T=0; dot_eta_T=0;	// I don't know.
	ddot_zeta_GT=0; ddot_xi_GT=0;
	DDotXEZ_G=_V(0,0,0);					// ???
	ddot_xi_G=0; ddot_eta_G=0; ddot_zeta_G=0;								
	dot_dxit=0; dot_detat=0; dot_dzetat=0; 	// Intermediate velocity deficiency used in time-to-go computation
	dot_dxi=0; dot_deta=0; dot_dzeta=0; 	// More Deltas
	Xtt_y=0; Xtt_p=0; 						// IGM computed steering angles in terminal system
	X_S1=0; X_S2=0; X_S3=0; 				// Direction cosines of the thrust vector
	sin_ups=0; cos_ups=0;					// Sine and cosine of upsilon (flight-path angle)
	dot_phi_1=0; dot_phi_T=0; 				// ???
	dtt_1=0; dtt_2=0;						// Used in TGO determination
	a_1=0; a_2=0;							// Acceleration terms used to determine TGO
	T_GO=0;									// Time before S4B shutdown
	T_CO=0;									// Predicted time of S4B shutdown, from GRR
	dV=0;
	TAS=0;
	t_clock = 0;
	DT_N = 1.0;
	DT_N1 = 0.77;
	DT_N2 = 0.75;
	DT_N3 = 1.6;
	DT_N4 = 1.3;
	DT_N5 = 0.98;
	DT_N6 = 1.0;
	OrbitalGuidanceCycle = 0;
	MLR = 25.0;
	MSLIM1 = 0.06*RAD;
	MSLIM2 = 0.04*RAD;
	MinorLoopCycles = 10;
	MSK5 = 0.48*RAD;
	MSK6 = MSK16 = 15.3*RAD;
	MinorLoopCounter = 0;
	MinorLoopCycles = 10; //Default to 10 for old scenarios in orbit

	// Set up remainder
	LVDC_Timebase = -1;										// Start up halted in pre-PTL wait
	LVDC_TB_ETime = 0;
	LVDC_Stop = false;
	IGMCycle = 0;
	sinceLastIGM = 0;
	// INTERNAL (NON-REAL-LVDC) FLAGS
	CountPIPA = false;

	CommandSequence = 0;

	if(!Initialized){ lvlog = fopen("lvlog1b.txt","w+"); } // Don't reopen the log if it's already open
	fprintf(lvlog,"init complete\r\n");
	fflush(lvlog);
	Initialized = true;
}
	
void LVDC1B::SwitchSelectorProcessing(const std::vector<SwitchSelectorSet> &table)
{
	while (CommandSequence < (int)table.size() && LVDC_TB_ETime > table[CommandSequence].time)
	{
		lvda.SwitchSelector(table[CommandSequence].stage, table[CommandSequence].channel);
		fprintf(lvlog, "[TB%d+%f] Switch Selector command issued: Stage %d Channel %d\r\n", LVDC_Timebase, LVDC_TB_ETime, table[CommandSequence].stage, table[CommandSequence].channel);
		CommandSequence++;
	}
}

bool LVDC1B::SwitchSelectorSequenceComplete(std::vector<SwitchSelectorSet> &table)
{
	if (CommandSequence >= (int)table.size())
		return true;

	return false;
}

// DS20070205 LVDC++ EXECUTION
void LVDC1B::TimeStep(double simdt) {
	// Bail if uninitialized
	if (Initialized == false) { return; }
	
	if(LVDC_Stop == false){

		//Update real time clock
		RealTimeClock += simdt;
		// Update timebase ET
		LVDC_TB_ETime += simdt;

		/* **** LVDC GUIDANCE PROGRAM **** */
		switch(LVDC_Timebase){
			case -1: // LOOP WAITING FOR PTL
				// Lock time accel to 100x
				if (oapiGetTimeAcceleration() > 100){ oapiSetTimeAcceleration(100); } 

				// BEFORE PTL COMMAND (T-00:20:00) STOPS HERE
				if (ReadyToLaunch == false){
						//sprintf(oapiDebugString(),"LVDC: T - %d:%d:%.2f | AWAITING PTL INTERRUPT",(int)Hours,(int)Minutes,Seconds);
						lvda.ZeroLVIMUCDUs();					// Zero IMU CDUs
						break;
				}
				else
				{
					//sprintf(oapiDebugString(),"LVDC: T - %d:%d:%.2f | AWAITING GRR",(int)Hours,(int)Minutes,Seconds);
				}
			
				// WAIT FOR GRR

				// Between PTL signal and GRR, we monitor the IMU for any failure signals and do vehicle self-tests.
				// At GRR we transfer control to the flight program and start TB0.

				// BEFORE GRR (T-00:00:17) STOPS HERE
				if (lvda.GetGuidanceReferenceRelease()){
					BOOST = true;
					LVDC_GRR = true;								// Mark event
					poweredflight = true;
					oapiSetTimeAcceleration (1);					// Set time acceleration to 1
					LVDC_Timebase = 0;								// Start TB0
					LVDC_TB_ETime = 0;
					MinorLoopCycles = 4;
					MinorLoopCounter = 0;
					MLR = 25.0;
					DT_N = DT_N1;
					MS25DT = 25.0*DT_N;
					MS04DT = 0.04 / DT_N;
					MSK5 = 0.48*RAD;
					MSK6 = MSK16 = 15.3*RAD;
					MSLIM1 = 0.04*RAD;
					MSLIM2 = 0.04*RAD;
					ModeCode25[MC25_BeginTB0] = true;
				}
				break;
			case 0: // MORE TB0
				break;

			case 1: // LIFTOFF TIME
				
				SwitchSelectorProcessing(SSTTB[1]);

				// Below here are timed events that must not be dependent on the iteration delay.

				//Timebase 2 initiated at certain fuel level

				if (LVDC_TB_ETime > 132.0 && lvda.SIBLowLevelSensorsDry() && DotS.z > 500.0) {

					// Begin timebase 2
					LVDC_Timebase = 2;
					LVDC_TB_ETime = 0;
					CommandSequence = 0;
				}

				if (lvda.SpacecraftSeparationIndication())
				{
					//LVDC_Stop = true;
				}

				break;

			case 2:

				SwitchSelectorProcessing(SSTTB[2]);

				if (S1B_CECO_Commanded == false && LVDC_TB_ETime > 3.1)
				{
					S1B_Engine_Out = true;
					S1B_CECO_Commanded = true;
				}
				
				// S1B OECO TRIGGER
				// Done by low-level sensor.
				if (lvda.GetSIPropellantDepletionEngineCutoff()){
					// For S1C thruster calibration
					fprintf(lvlog,"[T+%f] S1C OECO\r\n", t_clock);
					// Begin timebase 3
					LVDC_Timebase = 3;
					LVDC_TB_ETime = 0;
					CommandSequence = 0;
				}

				if (lvda.SpacecraftSeparationIndication())
				{
					//LVDC_Stop = true;
				}

				break;

			case 3:

				SwitchSelectorProcessing(SSTTB[3]);

				if(LVDC_TB_ETime >= 8.6 && S4B_IGN == false){
					S4B_IGN=true;
				}

				//IGM
				if (ModeCode25[MC25_SIVBIGMBeforeEMRC] == false && LVDC_TB_ETime > T_LET)
				{
					DT_N = DT_N3;
					ModeCode25[MC25_SIVBIGMBeforeEMRC] = true;
				}

				if (T_1 <= 0.0 && ModeCode25[MC25_SIVBIGMAfterEMRC] == false)
				{
					ModeCode25[MC25_SIVBIGMAfterEMRC] = true;
				}

				//Manual S-IVB Shutdown
				if (S4B_IGN == true && lvda.GetSIVBEngineOutA())
				{
					S4B_IGN = false;
					LVDC_Timebase = 4;
					LVDC_TB_ETime = 0;
					CommandSequence = 0;
					DT_N = DT_N5;
					MSK5 = 0.48*RAD;
					MSK6 = MSK16 = 15.3*RAD;
					MSLIM1 = 0.024*RAD;
					MSLIM2 = 0.016*RAD;

					//HSL Exit settings
					GATE = false;
					GATE5 = false;
					Tt_T = 1000;
					HSL = false;
					BOOST = false;

					fprintf(lvlog, "SIVB BACKUP CUTOFF! TAS = %f \r\n", TAS);
				}

				if (lvda.SpacecraftSeparationIndication())
				{
					//LVDC_Stop = true;
				}

				break;

			case 4:
				// TB4 timed events
				SwitchSelectorProcessing(SSTTB[4]);

				if (LVDC_TB_ETime > 10.0 && lvda.GetStage() == LAUNCH_STAGE_SIVB)
				{
					lvda.SetStage(STAGE_ORBIT_SIVB);
				}

				//For now, disable LVDC at TB4+57,600 seconds (Apollo 7 power loss)
				if (LVDC_TB_ETime > 57600.0)
				{
					LVDC_Stop = true;
					return;
				}

				break;

			case 10:
				//LM Abort Sequence

				SwitchSelectorProcessing(SSTALT1);

				if (BOOST == true)
				{
					S4B_IGN = false;

					//HSL Exit settings
					GATE = false;
					GATE5 = false;
					Tt_T = 1000;
					HSL = false;
					BOOST = false;

					fprintf(lvlog, "ALT SEQUENCE: LM ABORT\r\n");
				}

				break;
		}

		//No need to run the code below before GRR
		if (LVDC_Timebase < 0) return;

		/*
		if (lvimu.Operate) { fprintf(lvlog, "IMU: Operate\r\n"); }else{ fprintf(lvlog, "ERROR: IMU: NO-Operate\r\n"); }
		if (lvimu.TurnedOn) { fprintf(lvlog, "IMU: Turned On\r\n"); }else{ fprintf(lvlog, "ERROR: IMU: Turned OFF\r\n"); }
		if (lvimu.ZeroIMUCDUFlag) { fprintf(lvlog, "IMU: ERROR: Zero-IMU-CDU-Flag\r\n"); }
		if (lvimu.CoarseAlignEnableFlag) { fprintf(lvlog, "IMU: ERROR: Coarse-Align-Enable-Flag\r\n"); }
		if (lvimu.Caged) { fprintf(lvlog, "IMU: ERROR: Caged\r\n"); }
		*/

		//This is the actual LVDC code & logic; has to be independent from any of the above events
		if(LVDC_GRR && GRR_init == false){			
			fprintf(lvlog,"[GMT%+f] GRR received!\r\n", RealTimeClock);

			// Time into launch window = launch time from midnight - reference time of launch from midnight

			fprintf(lvlog, "Inclination = %f\r\n", Inclination);

			T_GRR = RealTimeClock;
			t_D = T_GRR - T_GRR0;
			fprintf(lvlog, "Time into launch window (if applicable) = %f\r\n", t_D);

			DescNodeAngle = Lambda_0 + lambda_dot * t_D;
			fprintf(lvlog, "Lambda_0 = %f, lambda_dot = %f, DescNodeAngle = %f\r\n", Lambda_0, lambda_dot, DescNodeAngle);

			Azimuth = Ax[0] + Ax[1] * Inclination + Ax[2] * DescNodeAngle + Ax[3] * Inclination*DescNodeAngle;
			fprintf(lvlog, "Azimuth = %f\r\n", Azimuth);

			// Need to make those into radians
			Azimuth *= RAD;
			Inclination *= RAD;
			DescNodeAngle *= RAD;

			fprintf(lvlog, "Rad Convert: Az / Inc / DNA = %f %f %f\r\n", Azimuth, Inclination, DescNodeAngle);

			// Initial Position & Velocity
			PosS = _V(cos(PHI - PHIP), sin(PHI - PHIP)*sin(Azimuth), -sin(PHI - PHIP)*cos(Azimuth))*R_L;
			DotS = _V(0, cos(PHIP)*cos(Azimuth), cos(PHIP)*sin(Azimuth))*R_L*omega_E;
			Dot0 = DotS;

			fprintf(lvlog, "Initial Position = %f %f %f\r\n", PosS.x, PosS.y, PosS.z);
			fprintf(lvlog, "Initial Velocity = %f %f %f\r\n", DotS.x, DotS.y, DotS.z);

			if (TerminalConditions == false)
			{
				// p is the semi-latus rectum of the desired terminal ellipse.
				p = (mu / C_3)*(pow(e, 2) - 1);
				fprintf(lvlog, "p = %f, mu = %f, e2 = %f, mu/C_3 = %f\r\n", p, mu, pow(e, 2), mu / C_3);

				// K_5 is the IGM terminal velocity constant
				K_5 = sqrt(mu / p);
				fprintf(lvlog, "K_5 = %f\r\n", K_5);

				R_T = p / (1 + (e*(cos(f))));
				V_T = K_5*sqrt((1 + ((2 * e)*(cos(f))) + pow(e, 2)));
				gamma_T = atan((e*(sin(f))) / (1 + (e*(cos(f)))));
				G_T = -mu / pow(R_T, 2);
			}
			fprintf(lvlog,"R_T = %f, V_T = %f, gamma_T = %f\r\n",R_T,V_T,gamma_T);

			// G MATRIX CALCULATION
			MX_A.m11 = cos(PHI);  MX_A.m12 = sin(PHI)*sin(Azimuth); MX_A.m13 = -(sin(PHI)*cos(Azimuth));
			MX_A.m21 = -sin(PHI); MX_A.m22 = cos(PHI)*sin(Azimuth); MX_A.m23 = -(cos(PHI)*cos(Azimuth));
			MX_A.m31 = 0;  MX_A.m32 = cos(Azimuth);  MX_A.m33 = sin(Azimuth);

			MX_B.m11 = cos(DescNodeAngle); MX_B.m12 = 0; MX_B.m13 = sin(DescNodeAngle);
			MX_B.m21 = sin(DescNodeAngle)*sin(Inclination); MX_B.m22 = cos(Inclination); 
			MX_B.m23 = -cos(DescNodeAngle)*sin(Inclination);
			MX_B.m31 = -sin(DescNodeAngle)*cos(Inclination); MX_B.m32 = sin(Inclination);
			MX_B.m33 = cos(DescNodeAngle)*cos(Inclination);

			MX_G = mul(MX_B,MX_A); // Matrix Multiply
			W_ES = -_V(MX_A.m21, MX_A.m22, MX_A.m23)*omega_E;

			ddotG_last = GravitationSubroutine(PosS, true);
			lvda.ZeroLVIMUPIPACounters();
			sinceLastIGM = 0;

			lvda.ReleaseLVIMUCDUs();						// Release IMU CDUs
			VECTOR3 align = lvda.GetTheodoliteAlignment(Azimuth);
			lvda.DriveLVIMUGimbals(align.x, align.y, align.z); // Now bring to alignment
			fprintf(lvlog, "Initial Attitude: %f %f %f \r\n", align.x*DEG, align.y*DEG, align.z*DEG);
			lvda.ReleaseLVIMU();							// Release IMU
			CountPIPA = true;								// Enable PIPA storage

			MF[0] = MF[1] = MF[2] = MF[3] = MFSArr[0] = MFSArr[1] = MFSArr[2] = MFSArr[3] = MFS = MF0;

			PCommandedAttitude.x = align.x;
			PCommandedAttitude.y = 0;
			PCommandedAttitude.z = 0;
			CommandedAttitude = PCommandedAttitude;
			DChi_apo = _V(0, 0, 0);

			GRR_init = true;
			lvda.SetOutputRegisterBit(FiringCommitEnable, true);
			fprintf(lvlog,"Initialization completed.\r\n\r\n");
			goto minorloop;
		}
		
		//Orbital Processing
		if (poweredflight == false)
		{
			goto minorloop;
		}
		// various clocks the LVDC needs...
		if(ModeCode25[MC25_BeginTB1] == true){ t_clock += simdt; } //time since liftoff
		//powered flight nav
		if(LVDC_GRR == true){
			sinceLastIGM += simdt;
			if (sinceLastIGM < DT_N) { goto minorloop; }
			dt_c = sinceLastIGM;
			sinceLastIGM = 0;
			IGMCycle++;				// For debugging
			fprintf(lvlog, "[%d+%f] *** Major Loop %d ***\r\n", LVDC_Timebase, LVDC_TB_ETime, IGMCycle);

			//ACCELEROMETER READ
			AccelerometerRead();
			//FM CALCULATIONS
			FMCalculations();			
			//BOOST NAVIGATION
			BoostNavigation();
		}

		if(ModeCode25[MC25_BeginTB1] == false){//liftoff not received; initial roll command for FCC
			goto limittest;
		}
		if(BOOST == false){//i.e. we're either in orbit or boosting out of orbit
			if(LVDC_Timebase == 4 || LVDC_Timebase == 10){
				R_OG = PosS;
				goto orbitalguidance;
			}else{
				goto minorloop;
			}
		} //TBD: 
		if(ModeCode25[MC25_SIVBIGMBeforeEMRC] == false){
			//Pre-IGM guidance:
			if(S1B_Engine_Out == true && T_EO1 == 0){
				//S1B engine out interrupt handling
				T_EO1 = 1;
				t_fail = t_clock;
				fprintf(lvlog,"[%d+%f] S1B engine out interrupt received! t_fail = %f\r\n",LVDC_Timebase,LVDC_TB_ETime,t_fail);
			}
			if(t_clock > t_1 || (PosS.x - R_L) > 138.0){
				//roll/pitch program
				if(t_clock >= t_2 && T_EO1 > 0){
					//time to re-calculate freeze time?
					T_EO1 = -1; //allow only one pass thru re-calculation
					if(t_fail <= t_2){ dT_F = t_3; }
					if(t_2 < t_clock && t_clock <= t_4){ dT_F = (B_11 * t_fail) + B_12; }
					if(t_4 < t_clock && t_clock <= t_5){ dT_F = (B_21 * t_fail) + B_22; }
					if(t_5 < t_clock){ dT_F = 0; }
					t_6 = t_clock + dT_F;
					T_ar = T_ar + (0.25*(T_ar - t_fail));
					fprintf(lvlog,"[%d+%f] Freeze time recalculated! t_6 = %f T_ar = %f\r\n",LVDC_Timebase,LVDC_TB_ETime,t_6,T_ar);
				}
				if(t_clock >= t_6){
					if (t_clock > T_ar){
						//time for pitch freeze?
						fprintf(lvlog,"[%d+%f] Pitch freeze! \r\n",LVDC_Timebase,LVDC_TB_ETime);
						CommandedAttitude.y = PCommandedAttitude.y;
						CommandedAttitude.x = 0 * RAD;
						CommandedAttitude.z = 0;
						goto limittest;
					}else{
						//Pre-IGM steering
						int x=0,y=0;						
						double Tc = t_clock - dT_F + T_d, cmd = 0;  
						if(Tc < T_S1){               x = 1; }
						if(T_S1 <= Tc && Tc < T_S2){ x = 2; }
						if(T_S2 <= Tc && Tc < T_S3){ x = 3; }
						if(T_S3 <= Tc){              x = 4; }
						while(y <= 4){
							cmd += (Fx[x][y] * ((double)pow(Tc,y)));
							y++;
						}
						CommandedAttitude.y = cmd * RAD;
						CommandedAttitude.x = 0;
						CommandedAttitude.z = 0;
						fprintf(lvlog,"[%d+%f] Roll/pitch programm %f \r\n",LVDC_Timebase,LVDC_TB_ETime,cmd);
						goto limittest;
					}
				}else{
					CommandedAttitude.y = PCommandedAttitude.y;
					CommandedAttitude.x = 0 * RAD;
					CommandedAttitude.z = 0;
					goto limittest;
				}
			}else{
				//Pre tower clear
				goto limittest;
			}
			//end of pre igm
		}
		if(HSL == false){		
			// If we are not in the high-speed loop
			fprintf(lvlog,"HSL False\r\n");
			// IGM STAGE LOGIC
			if(ModeCode25[MC25_SIVBIGMAfterEMRC] == true){
				fprintf(lvlog,"Post-MRS\n");
				if(t_B1 <= t_B3){
					tau2 = V_ex2 * MFS;
					fprintf(lvlog, "Normal Tau: tau2 = %f, F/m = %f\r\n", tau2, 1.0 / MFS);
				}else{
					// This is the "ARTIFICIAL TAU" code.
					t_B3 += dt_c; 
					tau2 = tau2+(T_1*(dotM_1/dotM_2));
					fprintf(lvlog,"Art. Tau: tau2 = %f, T_1 = %f, dotM_1 = %f dotM_2 = %f \r\n",tau2,T_1,dotM_1,dotM_2);
					fprintf(lvlog,"Diff: %f \r\n",(tau2-(V_ex2*MFS)));
					T_2 = T_2+T_1*(dotM_1/dotM_2);
					T_1 = 0;
					fprintf(lvlog,"T_1 = 0\r\nT_2 = %f, dotM_1 = %f, dotM_2 = %f \r\n",T_2,dotM_1,dotM_2);
				}					
			}else{
				fprintf(lvlog,"Pre-MRS\n");
				if(T_1 < 0){	// If we're out of first-stage IGM time
					// Artificial Tau
					tau2 = tau2+(T_1*(dotM_1/dotM_2));
					fprintf(lvlog,"Art. Tau: tau2 = %f, T_1 = %f, dotM_1 = %f, dotM_2 = %f \r\n",tau2,T_1,dotM_1,dotM_2);
					T_2 = T_2+T_1*(dotM_1/dotM_2);
					T_1 = 0;
					fprintf(lvlog,"T_2 = %f, T_1 = %f, dotM_1 = %f, dotM_2 = %f \r\n",T_2,T_1,dotM_1,dotM_2);		
				}else{															
					tau1 = V_ex1 * MFS;
					fprintf(lvlog, "Normal Tau: tau1 = %f, F/m = %f\r\n", tau1, 1.0 / MFS);
				}
			}
			fprintf(lvlog,"--- STAGE INTEGRAL LOGIC ---\r\n");
			// CHI-TILDE LOGIC
			// STAGE INTEGRAL CALCULATIONS				
			Pos4 = mul(MX_G,PosS);
			fprintf(lvlog,"Pos4 = %f, %f, %f\r\n",Pos4.x,Pos4.y,Pos4.z);
			fprintf(lvlog,"T_1 = %f,T_2 = %f\r\n",T_1,T_2);
			L_1 = V_ex1 * log(tau1 / (tau1-T_1));
			J_1 = (L_1 * tau1) - (V_ex1 * T_1);
			S_1 = (L_1 * T_1) - J_1;
			Q_1 = (S_1 * tau1) - ((V_ex1 * pow(T_1,2)) / 2);
			P_1 = (J_1 * tau1) - ((V_ex1 * pow(T_1,2)) / 2);
			U_1 = (Q_1 * tau1) - ((V_ex1 * pow(T_1,3)) / 6);
			fprintf(lvlog,"L_1 = %f, J_1 = %f, S_1 = %f, Q_1 = %f, P_1 = %f, U_1 = %f\r\n",L_1,J_1,S_1,Q_1,P_1,U_1);

			Lt_2 = V_ex2 * log(tau2 / (tau2-Tt_2));
			fprintf(lvlog,"Lt_2 = %f, tau2 = %f, Tt_2 = %f\r\n",Lt_2,tau2,Tt_2);

			Jt_2 = (Lt_2 * tau2) - (V_ex2 * Tt_2);
			fprintf(lvlog,"Jt_2 = %f",Jt_2);
			Lt_Y = (L_1 + Lt_2);
			fprintf(lvlog,", Lt_Y = %f\r\n",Lt_Y);

			// SELECT RANGE OPTION				
gtupdate:	// Target of jump from further down
			fprintf(lvlog,"--- GT UPDATE ---\r\n");

			// RANGE ANGLE 1
			fprintf(lvlog,"RANGE ANGLE\r\n");
			d2 = (V * Tt_T) - Jt_2 + (Lt_Y * Tt_2) - (ROV / V_ex2) * 
				((tau1 - T_1) * L_1 + (tau2 - Tt_2) * Lt_2) *
				(Lt_Y + V - V_T);
			phi_T = ((atan2(Pos4.z,Pos4.x))+(((1/R_T)*(S_1+d2))*(cos(gamma_T))));
			fprintf(lvlog,"V = %f, d2 = %f, phi_T = %f\r\n",V,d2,phi_T);				

			// FREEZE TERMINAL CONDITIONS TEST
			if(!(Tt_T <= eps_3)){
				// UPDATE TERMINAL CONDITIONS
				fprintf(lvlog,"UPDATE TERMINAL CONDITIONS\r\n");
				f = phi_T + alpha_D;
				R_T = p/(1+((e*(cos(f)))));
				fprintf(lvlog,"f = %f, R_T = %f\r\n",f,R_T);
				V_T = K_5 * pow(1+((2*e)*(cos(f)))+pow(e,2),0.5);
				gamma_T = atan((e*(sin(f)))/(1+(e*(cos(f)))));
				G_T = -mu/pow(R_T,2);
				fprintf(lvlog,"V_T = %f, gamma_T = %f, G_T = %f\r\n",V_T,gamma_T,G_T);
			}

			// UNROTATED TERMINAL CONDITIONS
			fprintf(lvlog,"UNROTATED TERMINAL CONDITIONS\r\n");
			xi_T = R_T;					
			dot_zeta_T = V_T * (cos(gamma_T));
			dot_xi_T = V_T * (sin(gamma_T));
			ddot_zeta_GT = 0;
			ddot_xi_GT = G_T;
			fprintf(lvlog,"xi_T = %f, dot_zeta_T = %f, dot_xi_T = %f\r\n",xi_T,dot_zeta_T,dot_xi_T);
			fprintf(lvlog,"ddot_zeta_GT = %f, ddot_xi_GT = %f\r\n",ddot_zeta_GT,ddot_xi_GT);
				
			// ROTATION TO TERMINAL COORDINATES
			fprintf(lvlog,"--- ROTATION TO TERMINAL COORDINATES ---\r\n");
			// This is the last time PosS is referred to.
			MX_phi_T.m11 = (cos(phi_T));    MX_phi_T.m12 = 0; MX_phi_T.m13 = ((sin(phi_T)));
			MX_phi_T.m21 = 0;               MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
			MX_phi_T.m31 = (-sin(phi_T)); MX_phi_T.m32 = 0; MX_phi_T.m33 = (cos(phi_T));
			fprintf(lvlog,"MX_phi_T R1 = %f %f %f\r\n",MX_phi_T.m11,MX_phi_T.m12,MX_phi_T.m13);
			fprintf(lvlog,"MX_phi_T R2 = %f %f %f\r\n",MX_phi_T.m21,MX_phi_T.m22,MX_phi_T.m23);
			fprintf(lvlog,"MX_phi_T R3 = %f %f %f\r\n",MX_phi_T.m31,MX_phi_T.m32,MX_phi_T.m33);

			MX_K = mul(MX_phi_T,MX_G);
			fprintf(lvlog,"MX_K R1 = %f %f %f\r\n",MX_K.m11,MX_K.m12,MX_K.m13);
			fprintf(lvlog,"MX_K R2 = %f %f %f\r\n",MX_K.m21,MX_K.m22,MX_K.m23);
			fprintf(lvlog,"MX_K R3 = %f %f %f\r\n",MX_K.m31,MX_K.m32,MX_K.m33);

			PosXEZ = mul(MX_K,PosS);
			DotXEZ = mul(MX_K,DotS);	
			fprintf(lvlog,"PosXEZ = %f %f %f\r\n",PosXEZ.x,PosXEZ.y,PosXEZ.z);
			fprintf(lvlog,"DotXEZ = %f %f %f\r\n",DotXEZ.x,DotXEZ.y,DotXEZ.z);

			VECTOR3 RTT_T1,RTT_T2;
			RTT_T1.x = ddot_xi_GT; RTT_T1.y = 0;        RTT_T1.z = ddot_zeta_GT;
			RTT_T2 = ddotG_act;
			fprintf(lvlog,"RTT_T1 = %f %f %f\r\n",RTT_T1.x,RTT_T1.y,RTT_T1.z);
			fprintf(lvlog,"RTT_T2 = %f %f %f\r\n",RTT_T2.x,RTT_T2.y,RTT_T2.z);

			RTT_T2 = mul(MX_K,RTT_T2);
			fprintf(lvlog,"RTT_T2 (mul) = %f %f %f\r\n",RTT_T2.x,RTT_T2.y,RTT_T2.z);

			RTT_T1 = RTT_T1+RTT_T2;	  
			fprintf(lvlog,"RTT_T1 (add) = %f %f %f\r\n",RTT_T1.x,RTT_T1.y,RTT_T1.z);

			ddot_xi_G   = 0.5*RTT_T1.x;
			ddot_eta_G  = 0.5*RTT_T1.y;
			ddot_zeta_G = 0.5*RTT_T1.z;
			fprintf(lvlog,"ddot_XEZ_G = %f %f %f\r\n",ddot_xi_G,ddot_eta_G,ddot_zeta_G);

			// ESTIMATED TIME-TO-GO
			fprintf(lvlog,"--- ESTIMATED TIME-TO-GO ---\r\n");

			dot_dxit   = dot_xi_T - DotXEZ.x - (ddot_xi_G*Tt_T);
			dot_detat  = -DotXEZ.y - (ddot_eta_G * Tt_T);
			dot_dzetat = dot_zeta_T - DotXEZ.z - (ddot_zeta_G * Tt_T);
			fprintf(lvlog,"dot_XEZt = %f %f %f\r\n",dot_dxit,dot_detat,dot_dzetat);
			dV = pow((pow(dot_dxit,2)+pow(dot_detat,2)+pow(dot_dzetat,2)),0.5);
			dL_2 = (((pow(dot_dxit,2)+pow(dot_detat,2)+pow(dot_dzetat,2))/Lt_Y)-Lt_Y)/2;
			// if(dL_3 < 0){ sprintf(oapiDebugString(),"Est TTG: dL_3 %f (X/E/Z %f %f %f) @ Cycle %d (TB%d+%f)",dL_3,dot_dxit,dot_detat,dot_dzetat,IGMCycle,LVDC_Timebase,LVDC_TB_ETime);
			//	LVDC_GP_PC = 30; break; } 

			dT_2 = (dL_2*(tau2-Tt_2))/V_ex2;
			T_2 = Tt_2 + dT_2;
			T_T = Tt_T + dT_2;
			fprintf(lvlog,"dV = %f, dL_2 = %f, dT_2 = %f, T_2 = %f, T_T = %f\r\n",dV,dL_2,dT_2,T_2,T_T);

			// TARGET PARAMETER UPDATE
			if(!(UP > 0)){	
				fprintf(lvlog,"--- TARGET PARAMETER UPDATE ---\r\n");
				UP = 1; 
				Tt_2 = T_2;
				Tt_T = T_T;
				fprintf(lvlog,"UP = 1, Tt_2 = %f, Tt_T = %f\r\n",Tt_2,Tt_T);
				Lt_2 = Lt_2 + dL_2;
				Lt_Y = Lt_Y + dL_2;
				Jt_2 = Jt_2 + (dL_2*T_2);
				fprintf(lvlog,"Lt_2 = %f, Lt_Y = %f, Jt_2 = %f\r\n",Lt_2,Lt_Y,Jt_2);

				// NOTE: This is perfectly valid. Just because Dijkstra and Wirth think otherwise
				// does not mean it's gospel. I shouldn't have to defend my choice of instructions
				// because a bunch of people read the title of the paper with no context and take
				// it as a direct revelation from God with no further study into the issue.
				fprintf(lvlog,"RECYCLE\r\n");
				goto gtupdate; // Recycle. 
			}

			// tchi_y AND tchi_p CALCULATIONS
			fprintf(lvlog,"--- tchi_y/p CALCULATION ---\r\n");

			L_2 = Lt_2 + dL_2;
			J_2 = Jt_2 + (dL_2*T_2);
			S_2 = (L_2*T_2)-J_2;
			Q_2 = (S_2*tau2)-((V_ex2*pow(T_2,2))/2);
			P_2 = (J_2*(tau2+(2*T_1)))-((V_ex2*pow(T_2,2))/2);
			U_2 = (Q_2*(tau2+(2*T_1)))-((V_ex2*pow(T_2,3))/6);
			fprintf(lvlog,"L_2 = %f, J_2 = %f, S_2 = %f, Q_2 = %f, P_2 = %f, U_2 = %f\r\n",L_2,J_2,S_2,Q_2,P_2,U_2);

			// This is where velocity-to-be-gained is generated.

			dot_dxi   = dot_dxit   - (ddot_xi_G   * dT_2);
			dot_deta  = dot_detat  - (ddot_eta_G  * dT_2);
			dot_dzeta = dot_dzetat - (ddot_zeta_G * dT_2);
			fprintf(lvlog,"dot_dXEZ = %f %f %f\r\n",dot_dxi,dot_deta,dot_dzeta);

				
	//		sprintf(oapiDebugString(),".dxi = %f | .deta %f | .dzeta %f | dT3 %f",
	//			dot_dxi,dot_deta,dot_dzeta,dT_3);

			L_Y = L_1 + L_2;
			tchi_y_last = tchi_y;
			tchi_p_last = tchi_p;
			tchi_y = atan2(dot_deta,pow(pow(dot_dxi,2)+pow(dot_dzeta,2),0.5));
			tchi_p = atan2(dot_dxi,dot_dzeta);				
			UP = -1;
			fprintf(lvlog,"L_Y = %f, tchi_y = %f, tchi_p = %f, UP = -1\r\n",L_Y,tchi_y,tchi_p);

			// *** END OF CHI-TILDE LOGIC ***
			// Is it time for chi-tilde mode?
			if (ModeCode25[MC25_TerminalGuidance] == false && Tt_T <= eps_2)
			{
				ModeCode25[MC25_TerminalGuidance] = true;
				DT_N = DT_N4;
			}

			if(ModeCode25[MC25_TerminalGuidance]){
				fprintf(lvlog,"CHI BAR STEERING ON, REMOVE ALTITUDE CONSTRAINS (K_1-4 = 0)\r\n");
				// Yes
				// Go to the test that we would be testing if HSL was true
				K_1 = 0; K_2 = 0; K_3 = 0; K_4 = 0;
				// See the note above if the presence of this goto bothers you.
				// sprintf(oapiDebugString(),"LVDC: HISPEED LOOP ENTRY: Tt_T %f eps_2 %f", Tt_T,eps_2); LVDC_GP_PC = 30; break; // STOP
				goto hsl;
			}else{
				// No.
				// YAW STEERING PARAMETERS
				fprintf(lvlog,"--- YAW STEERING PARAMETERS ---\r\n");

				J_Y = J_1 + J_2 + (L_2*T_1);
				S_Y = S_1 - J_2 + (L_Y*T_2);
				Q_Y = Q_1 + Q_2 + (S_2*T_1) + ((T_2)*J_1);
				K_Y = L_Y/J_Y;
				D_Y = S_Y - (K_Y*Q_Y);
				fprintf(lvlog,"J_Y = %f, S_Y = %f, Q_Y = %f, K_Y = %f, D_Y = %f\r\n",J_Y,S_Y,Q_Y,K_Y,D_Y);

				deta = PosXEZ.y + (DotXEZ.y*T_T) + ((ddot_eta_G*pow(T_T,2))/2) + (S_Y*(sin(tchi_y)));
				K_3 = deta/(D_Y*(cos(tchi_y)));
				K_4 = K_Y*K_3;
				fprintf(lvlog,"deta = %f, K_3 = %f, K_4 = %f\r\n",deta,K_3,K_4);

				// PITCH STEERING PARAMETERS
				fprintf(lvlog,"--- PITCH STEERING PARAMETERS ---\r\n");

				L_P = L_Y*cos(tchi_y);
				C_2 = cos(tchi_y)+(K_3*sin(tchi_y));
				C_4 = K_4*sin(tchi_y);
				J_P = (J_Y*C_2) - (C_4*(P_1+P_2+(pow(T_1,2)*L_2)));
				fprintf(lvlog,"L_P = %f, C_2 = %f, C_4 = %f, J_P = %f\r\n",L_P,C_2,C_4,J_P);

				S_P = (S_Y*C_2) - (C_4*Q_Y);
				Q_P = (Q_Y*C_2) - (C_4*(U_1+U_2+(pow(T_1,2)*S_2)+((T_2)*P_1)));
				K_P = L_P/J_P;
				D_P = S_P - (K_P*Q_P);
				fprintf(lvlog,"S_P = %f, Q_P = %f, K_P = %f, D_P = %f\r\n",S_P,Q_P,K_P,D_P);

				dxi = PosXEZ.x - xi_T + (DotXEZ.x*T_T) + ((ddot_xi_G*pow(T_T,2))/2) + (S_P*(sin(tchi_p)));
				K_1 = dxi/(D_P*cos(tchi_p));
				K_2 = K_P*K_1;
				fprintf(lvlog,"dxi = %f, K_1 = %f, K_2 = %f, cos(tchi_p) = %f\r\n",dxi,K_1,K_2,cos(tchi_p));
			}
		}
		else {
		hsl:	// HIGH-SPEED LOOP ENTRY	
			if (Tt_T <= 14.4)
			{
				// CUTOFF VELOCITY EQUATIONS
				fprintf(lvlog, "--- CUTOFF VELOCITY EQUATIONS ---\r\n");
				V_0 = V_1;
				V_1 = V_2;
				V_2 = V;
				dtt_1 = dtt_2;
				dtt_2 = dt_c;
				DT_B = dV_B * dt_c*1.0 / (V_2 - V_1);
				fprintf(lvlog, "V = %f, Tt_t = %f\r\n", V, Tt_T);
				fprintf(lvlog, "V = %f, V_0 = %f, V_1 = %f, V_2 = %f, dtt_1 = %f, dtt_2 = %f DT_B = %f\r\n", V, V_0, V_1, V_2, dtt_1, dtt_2, DT_B);
				if (Tt_T <= (eps_4 + DT_B) && V + V_TC >= V_T) {
					fprintf(lvlog, "--- HI SPEED LOOP ---\r\n");
					// TGO CALCULATION
					fprintf(lvlog, "--- TGO CALCULATION ---\r\n");
					if (GATE5 == false && ModeCode27[MC27_GRFDiscretesSet] == false) {
						fprintf(lvlog, "CHI FREEZE\r\n");
						// CHI FREEZE
						tchi_y = tchi_y_last;
						tchi_p = tchi_p_last;
						HSL = true;
						GATE5 = true;
						T_GO = T_2;
						Tt_T = Tt_T - DT_B;
						fprintf(lvlog, "HSL = true, GATE5 = true, T_GO = %f\r\n", T_GO);
					}

					// TGO DETERMINATION
					fprintf(lvlog, "--- TGO DETERMINATION ---\r\n");
					Tt_T = Tt_T - dt_c;
					a_2 = (((V_2 - V_1)*dtt_1) - ((V_1 - V_0)*dtt_2)) / (dtt_2*dtt_1*(dtt_2 + dtt_1));
					a_1 = ((V_2 - V_1) / dtt_2) + (a_2*dtt_2);
					T_GO = ((V_T - dV_B) - V_2) / (a_1 + a_2 * T_GO);
					T_CO = TAS + T_GO;
					fprintf(lvlog, "a_2 = %f, a_1 = %f, T_GO = %f, T_CO = %f, V_T = %f\r\n", a_2, a_1, T_GO, T_CO, V_T);

					// Done, go to navigation
					//sprintf(oapiDebugString(),"TB%d+%f | CP/Y %f %f | -HSL- TGO %f",
					//	LVDC_Timebase,LVDC_TB_ETime,PITCH,YAW,T_GO);

					goto minorloop;
				}
				// End of HSL
			}
		}
		// GUIDANCE TIME UPDATE
		fprintf(lvlog,"--- GUIDANCE TIME UPDATE ---\r\n");

		if(BOOST){
			if(ModeCode25[MC25_SIVBIGMAfterEMRC] == false){
				T_1 = T_1 - dt_c; 
			}else{
				if(t_B1 <= t_B3){
					T_2 = T_2 - dt_c;
				}else{
					// Here if t_B1 is bigger.
					fprintf(lvlog,"t_B1 = %f, t_B3 = %f\r\n",t_B1,t_B3);
					T_1 = (((dotM_1*(t_B3-t_B1))-(dotM_2*t_B3))*dt)/(dotM_1*t_B1);
				}
			}
		}
		fprintf(lvlog,"T_1 = %f, T_2 = %f, dt_c = %f\r\n",T_1,T_2,dt_c);
		Tt_2 = T_2;
		Tt_T = T_1+Tt_2;
		fprintf(lvlog,"Tt_2 = %f, Tt_T = %f\r\n",Tt_2,Tt_T);
	
		// IGM STEERING ANGLES
		fprintf(lvlog,"--- IGM STEERING ANGLES ---\r\n");

		//sprintf(oapiDebugString(),"IGM: K_1 %f K_2 %f K_3 %f K_4 %f",K_1,K_2,K_3,K_4);
		Xtt_y = ((tchi_y) - K_3 + (K_4 * DT_N));
		Xtt_p = ((tchi_p) - K_1 + (K_2 * DT_N)) - phi_T - PI05;
		fprintf(lvlog,"Xtt_y = %f, Xtt_p = %f\r\n",Xtt_y,Xtt_p);

		sin_chi_Yit = sin(Xtt_p);
		cos_chi_Yit = cos(Xtt_p);
		sin_chi_Zit = sin(Xtt_y);
		cos_chi_Zit = cos(Xtt_y);
		ChiComputations(0);
	limittest:
		MinorLoopSupport();
		//DISCRETE BACKUPS
		//fprintf(lvlog, "[%d+%f] DISCRETE BACKUPS\r\n", LVDC_Timebase, LVDC_TB_ETime);
		if (ModeCode25[MC25_BeginTB1] == false)
		{
			if (LVDC_Timebase == 0 && LVDC_TB_ETime >= 17.5 && LVDC_TB_ETime <= 150.0)
			{
				//TBD: Backup liftoff
			}
		}
		//Engine failure code
		if (!S1B_Engine_Out && ((LVDC_Timebase == 1 && LVDC_TB_ETime > 5.8) || LVDC_Timebase == 2))
		{
			S1B_Engine_Out = lvda.GetSIInboardEngineOut() || lvda.GetSIOutboardEngineOut();
		}

		if (LVDC_Timebase == 4)
		{
			if (poweredflight && LVDC_TB_ETime >= 15.0) {
				fprintf(lvlog, "Orbit Initialization\r\n");
				poweredflight = false; //powered flight nav off
				MinorLoopCycles = 10;
				MLR = 10.0;
				//MinorLoopCounter = 0;
				DT_N = DT_N6;
				MSLIM1 = 0.06*RAD;
				MSLIM2 = 0.04*RAD;
				MS25DT = 10.0*DT_N;
				MS04DT = 0.1 / DT_N;
				if (ModeCode27[MC27_GRFDiscretesSet] == false)
				{
					MSK5 = 0.24*RAD;
				}

				//Orbit Init
				ddotS = ddotG_act;
				fprintf(lvlog, "EarthRel Acceleration: %f %f %f \r\n", ddotS.x, ddotS.y, ddotS.z);
			}
		}

		DiscreteProcessor();

		if (poweredflight == false)
		{
			return;
		}

	minorloop: //minor loop;

		//Only run minor loop 25 times per second during boost, 10 times per second during orbit phase
		MinorLoopCounter++;
		if (MinorLoopCounter < MinorLoopCycles)
		{
			return;
		}
		MinorLoopCounter = 0;
		//fprintf(lvlog, "[%d+%f] *** Minor Loop ***\r\n", LVDC_Timebase, LVDC_TB_ETime);

		//Cutoff Logic
		if (HSL == true && S4B_IGN == true && RealTimeClock > T_GRR + T_CO - 0.02) {
			//Time for S4B cutoff? We need to check that here -IGM runs every 2 sec only, but cutoff has to be on the second			
			S4B_IGN = false;
			HSL = false;
			BOOST = false;
			LVDC_Timebase = 4;
			LVDC_TB_ETime = 0;
			CommandSequence = 0;
			DT_N = DT_N5;
			MSLIM1 = 0.024*RAD;
			MSLIM2 = 0.016*RAD;
			fprintf(lvlog, "SIVB VELOCITY CUTOFF! TAS = %f \r\n", TAS);
		};

		//Liftoff search
		if (LVDC_Timebase == 0 && LVDC_TB_ETime > 16.0 && LVDC_TB_ETime < 150.0 && lvda.GetLiftoff()) {
			LVDC_Timebase = 1;
			LVDC_TB_ETime = 0;
			CommandSequence = 0;
			DT_N = DT_N2;
			lvda.SwitchSelector(SWITCH_SELECTOR_SI, 0);
			lvda.SetOutputRegisterBit(FiringCommitEnable, false);
			ModeCode25[MC25_BeginTB1] = true;
		}

		if (ModeCode27[MC27_GRFDiscretesSet] == false)
		{
			if (LVDC_Timebase > 0 && lvda.GetLVIMUFailure())
			{
				ModeCode27[MC27_GRFDiscretesSet] = true;
				lvda.SetOutputRegisterBit(GuidanceReferenceFailureA, true);
				lvda.SetOutputRegisterBit(GuidanceReferenceFailureB, true);
			}

			if (!ModeCode27[MC27_GRFDiscretesSet])
			{
				CurrentAttitude = lvda.GetLVIMUAttitude();
			}
		}

		if (ModeCode27[MC27_SCInControl] || ModeCode27[MC27_SCControlAfterGRF])
		{
			PCommandedAttitude = CommandedAttitude = CurrentAttitude;
			AttitudeError = AttitudeErrorOld = _V(0.0, 0.0, 0.0);
		}
		else if (!ModeCode27[MC27_GRFDiscretesSet])
		{
			//Attitude error commands
			PCommandedAttitude = PCommandedAttitude + DChi_apo;
			for (int i = 0;i < 3;i++)
			{
				if (PCommandedAttitude.data[i] >= PI2)
				{
					PCommandedAttitude.data[i] -= PI2;
				}
				if (PCommandedAttitude.data[i] < 0)
				{
					PCommandedAttitude.data[i] += PI2;
				}
			}

			//calculate delta attitude
			DeltaAtt.x = fmod((CurrentAttitude.x - PCommandedAttitude.x + TWO_PI), TWO_PI);
			if (DeltaAtt.x > PI) { DeltaAtt.x -= TWO_PI; }
			DeltaAtt.y = fmod((CurrentAttitude.y - PCommandedAttitude.y + TWO_PI), TWO_PI);
			if (DeltaAtt.y > PI) { DeltaAtt.y -= TWO_PI; }
			DeltaAtt.z = fmod((CurrentAttitude.z - PCommandedAttitude.z + TWO_PI), TWO_PI);
			if (DeltaAtt.z > PI) { DeltaAtt.z -= TWO_PI; }

			// ROLL ERROR
			AttitudeError.x = -(DeltaAtt.x + A3 * DeltaAtt.y);
			// PITCH ERROR
			AttitudeError.y = -(A1 * DeltaAtt.y + A2 * DeltaAtt.z);
			// YAW ERROR
			AttitudeError.z = -(-A4 * DeltaAtt.y + A5 * DeltaAtt.z);

			//Magnitude limit
			if (abs(AttitudeError.x) > MSK16)
			{
				fprintf(lvlog, "Minor Loop: Roll magnitude limited. Magnitude = %f Limit = %f\r\n", AttitudeError.x*DEG, MSK16*DEG);
				if (AttitudeError.x > 0)
				{
					AttitudeError.x = MSK16;
				}
				else
				{
					AttitudeError.x = -MSK16;
				}
			}
			if (abs(AttitudeError.y) > MSK6)
			{
				fprintf(lvlog, "Minor Loop: Pitch magnitude limited. Magnitude = %f Limit = %f\r\n", AttitudeError.y*DEG, MSK6*DEG);
				if (AttitudeError.y > 0)
				{
					AttitudeError.y = MSK6;
				}
				else
				{
					AttitudeError.y = -MSK6;
				}
			}
			if (abs(AttitudeError.z) > MSK6)
			{
				fprintf(lvlog, "Minor Loop: Yaw magnitude limited. Magnitude = %f Limit = %f\r\n", AttitudeError.z*DEG, MSK6*DEG);
				if (AttitudeError.z > 0)
				{
					AttitudeError.z = MSK6;
				}
				else
				{
					AttitudeError.z = -MSK6;
				}
			}
			//Rate limit
			for (int i = 0;i < 3;i++)
			{
				if (AttitudeError.data[i] - AttitudeErrorOld.data[i] > MSK5)
				{
					fprintf(lvlog, "Minor Loop: Rate limited. Axis = %d Magnitude = %f Limit = %f\r\n", i + 1, (AttitudeError.data[i] - AttitudeErrorOld.data[i])*DEG, MSK5*DEG);
					AttitudeError.data[i] = AttitudeErrorOld.data[i] + MSK5;
				}
				else if (AttitudeError.data[i] - AttitudeErrorOld.data[i] < -MSK5)
				{
					fprintf(lvlog, "Minor Loop: Rate limited. Axis = %d Magnitude = %f Limit = %f\r\n", i + 1, (AttitudeError.data[i] - AttitudeErrorOld.data[i])*DEG, MSK5*DEG);
					AttitudeError.data[i] = AttitudeErrorOld.data[i] - MSK5;
				}
			}

			AttitudeErrorOld = AttitudeError;
		}

		if (LVDC_Timebase > 0)
		{
			lvda.SetFCCAttitudeError(AttitudeError);
		}
		else
		{
			lvda.SetFCCAttitudeError(_V(0, 0, 0));
		}

		// Debug if we're launched
		/*if(LVDC_Timebase > -1){
			if(LVDC_Timebase < 4){
				sprintf(oapiDebugString(),"TB%d+%f | T1 = %f | T2 = %f | Tt_T = %f | ERR %f %f %f | V = %f R = %f",
					LVDC_Timebase,LVDC_TB_ETime,
					T_1,Tt_2,Tt_T,
					AttitudeError.x*DEG,AttitudeError.y*DEG,AttitudeError.z*DEG, V,R/1000);
			}else{
				// On-Orbit
				sprintf(oapiDebugString(),"TB%d+%.2f | ATT %.3f %.3f %.3f | CMD %.3f %.3f %.3f | ERR %.3f %.3f %.3f | V = %.2f R= %.2f",
					LVDC_Timebase,LVDC_TB_ETime,
					CurrentAttitude.x*DEG,CurrentAttitude.y*DEG,CurrentAttitude.z*DEG,
					CommandedAttitude.x*DEG,CommandedAttitude.y*DEG,CommandedAttitude.z*DEG,
					AttitudeError.x*DEG,AttitudeError.y*DEG,AttitudeError.z*DEG, V,R/1000);
			}
		}*/

		if (poweredflight)
		{
			return;
		}

		//ORBITAL PROCESSING
		
		//Navigation Update Implementation
		if (ModeCode27[MC27_DCSNavigationUpdateAccepted])
		{
			if (RealTimeClock - T_GRR > NUPTIM)
			{
				fprintf(lvlog, "[%d+%f] Navigation Update implemented!\r\n", LVDC_Timebase, LVDC_TB_ETime);
				ModeCode27[MC27_DCSNavigationUpdateAccepted] = false;
				TAS = NUPTIM;
				PosS = Pos_Nav;
				DotS = Vel_Nav;
				ddotS = GravitationSubroutine(PosS, false) + DragSubroutine(PosS, DotS, CurrentAttitude);
				R = length(PosS);
				V = length(DotS);
				CG = length(ddotS);
			}
		}

		T_SON = RealTimeClock - TAS - T_GRR;
		if (T_SON >= 8.0)
		{
			//Orbital Navigation
			fprintf(lvlog, "[%d+%f] ORBITAL NAVIGATION\r\n", LVDC_Timebase, LVDC_TB_ETime);
			RTEMP1 = PosS + DotS * 4.0 + ddotS * 8.0;
			VTEMP1 = DotS + ddotS * 4.0;
			ATEMP1 = GravitationSubroutine(RTEMP1, false) + DragSubroutine(RTEMP1, VTEMP1, CurrentAttitude);
			RPT = PosS + DotS * 8.0 + ATEMP1 * 32.0;
			VPT = VTEMP1 + ATEMP1 * 8.0;
			MidPointAttitude = CurrentAttitude + DChi_apo * MLR*4.0;
			APT = GravitationSubroutine(RPT, false) + DragSubroutine(RPT, VPT, MidPointAttitude);
			DRT = DotS * 8.0 + (ddotS + ATEMP1*2.0)*32.0 / 3.0;
			PosS = PosS + DRT;
			DVT = (ddotS + ATEMP1 * 4.0 + APT)*4.0 / 3.0;
			DotS = DotS + DVT;
			ddotS = GravitationSubroutine(PosS, false) + DragSubroutine(PosS, DotS, CurrentAttitude);
			TAS = TAS + 8.0;
			R = length(PosS);
			V = length(DotS);
			CG = length(ddotS);

			fprintf(lvlog, "Inertial Attitude: %f %f %f \r\n", CurrentAttitude.x*DEG, CurrentAttitude.y*DEG, CurrentAttitude.z*DEG);
			fprintf(lvlog, "EarthRel Position: %f %f %f \r\n", PosS.x, PosS.y, PosS.z);
			fprintf(lvlog, "EarthRel Velocity: %f %f %f \r\n", DotS.x, DotS.y, DotS.z);
			fprintf(lvlog, "EarthRel Acceleration: %f %f %f \r\n", ddotS.x, ddotS.y, ddotS.z);
			fprintf(lvlog, "State vector time: %f \r\n", TAS);
			fprintf(lvlog, "Gravity Acceleration: %f \r\n", CG);
			fprintf(lvlog, "Total Velocity: %f \r\n", V);
			fprintf(lvlog, "Dist. from Earth's Center: %f \r\n", R);
			fprintf(lvlog, "S: %f \r\n", S);
			fprintf(lvlog, "P: %f \r\n", P);
			VECTOR3 drtest = SVCompare();
			fprintf(lvlog, "SV Accuracy: %f %f %f\r\n", drtest.x, drtest.y, drtest.z);
		}
		OrbitalGuidanceCycle++;
		if (OrbitalGuidanceCycle >= 10)
		{
			OrbitalGuidanceCycle = 0;
			//Orbital Guidance
			R_OG = PosS + DotS * T_SON;
		orbitalguidance:
			OrbitalGuidance();
			goto limittest;
		}
	}
	else
	{
		lvda.SetFCCAttitudeError(_V(0, 0, 0));
	}

	/*
		sprintf(oapiDebugString(),"LVDC: TB%d + %f | PS %f %f %f | VS %f %f %f",
			LVDC_Timebase,LVDC_TB_ETime,
			PosS.x,PosS.y,PosS.z,
			DotS.x,DotS.y,DotS.z);
			*/
//	CurrentAttitude.x*DEG,CurrentAttitude.y*DEG,CurrentAttitude.z*DEG,V);								
}

void LVDC1B::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, LVDC_START_STRING);
	// Thank heaven for text processing.
	oapiWriteScenario_string(scn, "LVDC_FSPFileName", FSPFileName);
	// bool
	oapiWriteScenario_int(scn, "LVDC_alpha_D_op", alpha_D_op);
	oapiWriteScenario_int(scn, "LVDC_BOOST", BOOST);
	oapiWriteScenario_int(scn, "LVDC_CountPIPA", CountPIPA);
	oapiWriteScenario_int(scn, "LVDC_GATE", GATE);
	oapiWriteScenario_int(scn, "LVDC_GATE5", GATE5);
	oapiWriteScenario_int(scn, "LVDC_GRR_init", GRR_init);
	oapiWriteScenario_int(scn, "LVDC_HSL", HSL);
	oapiWriteScenario_int(scn, "LVDC_LVDC_GRR", LVDC_GRR);
	oapiWriteScenario_int(scn, "LVDC_LVDC_Stop", LVDC_Stop);
	oapiWriteScenario_int(scn, "LVDC_poweredflight", poweredflight);
	oapiWriteScenario_int(scn, "LVDC_S1B_CECO_Commanded", S1B_CECO_Commanded);
	oapiWriteScenario_int(scn, "LVDC_S1B_Engine_Out", S1B_Engine_Out);
	oapiWriteScenario_int(scn, "LVDC_S4B_IGN", S4B_IGN);
	oapiWriteScenario_int(scn, "LVDC_TerminalConditions", TerminalConditions);
	oapiWriteScenario_int(scn, "LVDC_ReadyToLaunch", ReadyToLaunch);
	oapiWriteScenario_int(scn, "LVDC_FSCAH", FSCAH);
	oapiWriteScenario_int(scn, "LVDC_FSCIC", FSCIC);
	oapiWriteScenario_int(scn, "LVDC_FFPIH", FFPIH);
	// int
	oapiWriteScenario_int(scn, "LVDC_CommandSequence", CommandSequence);
	oapiWriteScenario_int(scn, "LVDC_IGMCycle", IGMCycle);
	oapiWriteScenario_int(scn, "LVDC_MinorLoopCounter", MinorLoopCounter);
	oapiWriteScenario_int(scn, "LVDC_MinorLoopCycles", MinorLoopCycles);
	oapiWriteScenario_int(scn, "LVDC_ModeCode24", ModeCode24.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_ModeCode25", ModeCode25.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_ModeCode27", ModeCode27.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_LVDC_Timebase", LVDC_Timebase);
	oapiWriteScenario_int(scn, "LVDC_T_EO1", T_EO1);
	oapiWriteScenario_int(scn, "LVDC_T_EO2", T_EO2);
	oapiWriteScenario_int(scn, "LVDC_UP", UP);
	oapiWriteScenario_int(scn, "LVDC_FMANT", FMANT);
	// double
	papiWriteScenario_double(scn, "LVDC_a", a);
	papiWriteScenario_double(scn, "LVDC_a_1", a_1);
	papiWriteScenario_double(scn, "LVDC_a_2", a_2);
	papiWriteScenario_double(scn, "LVDC_A1", A1);
	papiWriteScenario_double(scn, "LVDC_A2", A2);
	papiWriteScenario_double(scn, "LVDC_A3", A3);
	papiWriteScenario_double(scn, "LVDC_A4", A4);
	papiWriteScenario_double(scn, "LVDC_A5", A5);
	papiWriteScenario_double(scn, "LVDC_alpha_D", alpha_D);
	papiWriteScenario_double(scn, "LVDC_Ax[0]", Ax[0]);
	papiWriteScenario_double(scn, "LVDC_Ax[1]", Ax[1]);
	papiWriteScenario_double(scn, "LVDC_Ax[2]", Ax[2]);
	papiWriteScenario_double(scn, "LVDC_Ax[3]", Ax[3]);
	papiWriteScenario_double(scn, "LVDC_Azimuth", Azimuth);
	papiWriteScenario_double(scn, "LVDC_A_zL", A_zL);
	papiWriteScenario_double(scn, "LVDC_B_11", B_11);
	papiWriteScenario_double(scn, "LVDC_B_21", B_21);
	papiWriteScenario_double(scn, "LVDC_B_12", B_12);
	papiWriteScenario_double(scn, "LVDC_B_22", B_22);
	papiWriteScenario_double(scn, "LVDC_C_2", C_2);
	papiWriteScenario_double(scn, "LVDC_C_4", C_4);
	papiWriteScenario_double(scn, "LVDC_C_3", C_3);
	papiWriteScenario_double(scn, "LVDC_CG", CG);
	papiWriteScenario_double(scn, "LVDC_cos_chi_Yit", cos_chi_Yit);
	papiWriteScenario_double(scn, "LVDC_cos_chi_Zit", cos_chi_Zit);
	papiWriteScenario_double(scn, "LVDC_d2", d2);
	papiWriteScenario_double(scn, "LVDC_ddot_xi_G", ddot_xi_G);
	papiWriteScenario_double(scn, "LVDC_ddot_eta_G", ddot_eta_G);
	papiWriteScenario_double(scn, "LVDC_ddot_zeta_G", ddot_zeta_G);
	papiWriteScenario_double(scn, "LVDC_ddot_zeta_GT", ddot_zeta_GT);
	papiWriteScenario_double(scn, "LVDC_ddot_xi_GT", ddot_xi_GT);
	papiWriteScenario_double(scn, "LVDC_DescNodeAngle", DescNodeAngle);
	papiWriteScenario_double(scn, "LVDC_deta", deta);
	papiWriteScenario_double(scn, "LVDC_dxi", dxi);
	papiWriteScenario_double(scn, "LVDC_dot_dxi", dot_dxi);
	papiWriteScenario_double(scn, "LVDC_dot_deta", dot_deta);
	papiWriteScenario_double(scn, "LVDC_dot_dzeta", dot_dzeta);
	papiWriteScenario_double(scn, "LVDC_dot_dxit", dot_dxit);
	papiWriteScenario_double(scn, "LVDC_dot_detat", dot_detat);
	papiWriteScenario_double(scn, "LVDC_dot_dzetat", dot_dzetat);
	papiWriteScenario_double(scn, "LVDC_dotM_1", dotM_1);
	papiWriteScenario_double(scn, "LVDC_dotM_2", dotM_2);
	papiWriteScenario_double(scn, "LVDC_dot_phi_1", dot_phi_1);
	papiWriteScenario_double(scn, "LVDC_dot_phi_T", dot_phi_T);
	papiWriteScenario_double(scn, "LVDC_dot_zeta_T", dot_zeta_T);
	papiWriteScenario_double(scn, "LVDC_dot_xi_T", dot_xi_T);
	papiWriteScenario_double(scn, "LVDC_dot_eta_T", dot_eta_T);
	papiWriteScenario_double(scn, "LVDC_dt", dt);
	papiWriteScenario_double(scn, "LVDC_dT_2", dT_2);
	papiWriteScenario_double(scn, "LVDC_dt_c", dt_c);
	papiWriteScenario_double(scn, "LVDC_dT_F", dT_F);
	papiWriteScenario_double(scn, "LVDC_dt_LET", dt_LET);
	papiWriteScenario_double(scn, "LVDC_DT_N", DT_N);
	papiWriteScenario_double(scn, "LVDC_dtt_1", dtt_1);
	papiWriteScenario_double(scn, "LVDC_dtt_2", dtt_2);
	papiWriteScenario_double(scn, "LVDC_dV", dV);
	papiWriteScenario_double(scn, "LVDC_dV_B", dV_B);
	papiWriteScenario_double(scn, "LVDC_e", e);
	papiWriteScenario_double(scn, "LVDC_eps_2", eps_2);
	papiWriteScenario_double(scn, "LVDC_eps_3", eps_3);
	papiWriteScenario_double(scn, "LVDC_eps_4", eps_4);
	papiWriteScenario_double(scn, "LVDC_f", f);
	papiWriteScenario_double(scn, "LVDC_F", F);
	papiWriteScenario_double(scn, "LVDC_Fm", Fm);
	papiWriteScenario_double(scn, "LVDC_Fx[0][0]", Fx[0][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][1]", Fx[0][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][2]", Fx[0][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][3]", Fx[0][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][4]", Fx[0][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][0]", Fx[1][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][1]", Fx[1][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][2]", Fx[1][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][3]", Fx[1][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][4]", Fx[1][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][0]", Fx[2][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][1]", Fx[2][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][2]", Fx[2][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][3]", Fx[2][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][4]", Fx[2][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][0]", Fx[3][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][1]", Fx[3][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][2]", Fx[3][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][3]", Fx[3][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][4]", Fx[3][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][0]", Fx[4][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][1]", Fx[4][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][2]", Fx[4][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][3]", Fx[4][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][4]", Fx[4][4]);
	papiWriteScenario_double(scn, "LVDC_G_T", G_T);
	papiWriteScenario_double(scn, "LVDC_Inclination", Inclination);
	papiWriteScenario_double(scn, "LVDC_J", J);
	papiWriteScenario_double(scn, "LVDC_J_1", J_1);
	papiWriteScenario_double(scn, "LVDC_J_2", J_2);
	papiWriteScenario_double(scn, "LVDC_Jt_2", Jt_2);
	papiWriteScenario_double(scn, "LVDC_J_Y", J_Y);
	papiWriteScenario_double(scn, "LVDC_J_P", J_P);
	papiWriteScenario_double(scn, "LVDC_K_1", K_1);
	papiWriteScenario_double(scn, "LVDC_K_2", K_2);
	papiWriteScenario_double(scn, "LVDC_K_3", K_3);
	papiWriteScenario_double(scn, "LVDC_K_4", K_4);
	papiWriteScenario_double(scn, "LVDC_K_5", K_5);
	papiWriteScenario_double(scn, "LVDC_K_D", K_D);
	papiWriteScenario_double(scn, "LVDC_K_p", K_p);
	papiWriteScenario_double(scn, "LVDC_K_y", K_y);
	papiWriteScenario_double(scn, "LVDC_K_r", K_r);
	papiWriteScenario_double(scn, "LVDC_K_Y", K_Y);
	papiWriteScenario_double(scn, "LVDC_K_P", K_P);
	papiWriteScenario_double(scn, "LVDC_D_P", D_P);
	papiWriteScenario_double(scn, "LVDC_D_Y", D_Y);
	papiWriteScenario_double(scn, "LVDC_KSCLNG", KSCLNG);
	papiWriteScenario_double(scn, "LVDC_L_1", L_1);
	papiWriteScenario_double(scn, "LVDC_L_2", L_2);
	papiWriteScenario_double(scn, "LVDC_dL_2", dL_2);
	papiWriteScenario_double(scn, "LVDC_Lt_2", Lt_2);
	papiWriteScenario_double(scn, "LVDC_Lambda_0", Lambda_0);
	papiWriteScenario_double(scn, "LVDC_lambda_dot", lambda_dot);
	papiWriteScenario_double(scn, "LVDC_L_P", L_P);
	papiWriteScenario_double(scn, "LVDC_L_Y", L_Y);
	papiWriteScenario_double(scn, "LVDC_Lt_Y", Lt_Y);
	papiWriteScenario_double(scn, "LVDC_LVDC_TB_ETime", LVDC_TB_ETime);
	papiWriteScenario_doublearr(scn, "LVDC_MF", MF, 4);
	papiWriteScenario_double(scn, "LVDC_MFS", MFS);
	papiWriteScenario_doublearr(scn, "LVDC_MFSArr", MFSArr, 4);
	papiWriteScenario_double(scn, "LVDC_MLR", MLR);
	papiWriteScenario_double(scn, "LVDC_MS04DT", MS04DT);
	papiWriteScenario_double(scn, "LVDC_MS25DT", MS25DT);
	papiWriteScenario_double(scn, "LVDC_MSK5", MSK5);
	papiWriteScenario_double(scn, "LVDC_MSK6", MSK6);
	papiWriteScenario_double(scn, "LVDC_MSK16", MSK16);
	papiWriteScenario_double(scn, "LVDC_MSLIM1", MSLIM1);
	papiWriteScenario_double(scn, "LVDC_MSLIM2", MSLIM2);
	papiWriteScenario_double(scn, "LVDC_mu", mu);
	papiWriteScenario_double(scn, "LVDC_NUPTIM", NUPTIM);
	papiWriteScenario_double(scn, "LVDC_omega_E", omega_E);
	papiWriteScenario_double(scn, "LVDC_p", p);
	papiWriteScenario_double(scn, "LVDC_P_1", P_1);
	papiWriteScenario_double(scn, "LVDC_P_2", P_2);
	papiWriteScenario_double(scn, "LVDC_PHI", PHI);
	papiWriteScenario_double(scn, "LVDC_PHIP", PHIP);
	papiWriteScenario_double(scn, "LVDC_phi_T", phi_T);
	papiWriteScenario_double(scn, "LVDC_Q_1", Q_1);
	papiWriteScenario_double(scn, "LVDC_Q_2", Q_2);
	papiWriteScenario_double(scn, "LVDC_Q_Y", Q_Y);
	papiWriteScenario_double(scn, "LVDC_Q_P", Q_P);
	papiWriteScenario_double(scn, "LVDC_R", R);
	papiWriteScenario_double(scn, "LVDC_RealTimeClock", RealTimeClock);
	papiWriteScenario_double(scn, "LVDC_R_L", R_L);
	papiWriteScenario_double(scn, "LVDC_ROV", ROV);
	papiWriteScenario_double(scn, "LVDC_R_T", R_T);
	papiWriteScenario_double(scn, "LVDC_S_1", S_1);
	papiWriteScenario_double(scn, "LVDC_S_2", S_2);
	papiWriteScenario_double(scn, "LVDC_S_P", S_P);
	papiWriteScenario_double(scn, "LVDC_S_Y", S_Y);
	papiWriteScenario_double(scn, "LVDC_sinceLastIGM", sinceLastIGM);
	papiWriteScenario_double(scn, "LVDC_sin_chi_Yit", sin_chi_Yit);
	papiWriteScenario_double(scn, "LVDC_sin_chi_Zit", sin_chi_Zit);
	papiWriteScenario_double(scn, "LVDC_sin_ups", sin_ups);
	papiWriteScenario_double(scn, "LVDC_cos_ups", cos_ups);
	papiWriteScenario_double(scn, "LVDC_SMCG", SMCG);
	papiWriteScenario_double(scn, "LVDC_S", S);
	papiWriteScenario_double(scn, "LVDC_P", P);
	papiWriteScenario_double(scn, "LVDC_t", t);
	papiWriteScenario_double(scn, "LVDC_t_1", t_1);
	papiWriteScenario_double(scn, "LVDC_T_1", T_1);
	papiWriteScenario_double(scn, "LVDC_t_2", t_2);
	papiWriteScenario_double(scn, "LVDC_T_2", T_2);
	papiWriteScenario_double(scn, "LVDC_t_3", t_3);
	papiWriteScenario_double(scn, "LVDC_t_3i", t_3i);
	papiWriteScenario_double(scn, "LVDC_t_4", t_4);
	papiWriteScenario_double(scn, "LVDC_t_5", t_5);
	papiWriteScenario_double(scn, "LVDC_t_6", t_6);
	papiWriteScenario_double(scn, "LVDC_T_ar", T_ar);
	papiWriteScenario_double(scn, "LVDC_TAS", TAS);
	papiWriteScenario_double(scn, "LVDC_tau1", tau1);
	papiWriteScenario_double(scn, "LVDC_tau2", tau2);
	papiWriteScenario_double(scn, "LVDC_t_B1", t_B1);
	papiWriteScenario_double(scn, "LVDC_t_B3", t_B3);
	papiWriteScenario_double(scn, "LVDC_tchi_y_last", tchi_y_last);
	papiWriteScenario_double(scn, "LVDC_tchi_p_last", tchi_p_last);
	papiWriteScenario_double(scn, "LVDC_tchi_y", tchi_y);
	papiWriteScenario_double(scn, "LVDC_tchi_p", tchi_p);
	papiWriteScenario_double(scn, "LVDC_t_clock", t_clock);
	papiWriteScenario_double(scn, "LVDC_T_CO", T_CO);
	papiWriteScenario_double(scn, "LVDC_t_D", t_D);
	papiWriteScenario_double(scn, "LVDC_t_fail", t_fail);
	papiWriteScenario_double(scn, "LVDC_T_GO", T_GO);
	papiWriteScenario_double(scn, "LVDC_T_GRR", T_GRR);
	papiWriteScenario_double(scn, "LVDC_T_GRR0", T_GRR0);
	papiWriteScenario_double(scn, "LVDC_TI", TI);
	papiWriteScenario_double(scn, "LVDC_T_L_apo", T_L_apo);
	papiWriteScenario_double(scn, "LVDC_T_LET", T_LET);
	papiWriteScenario_double(scn, "LVDC_T_S1", T_S1);
	papiWriteScenario_double(scn, "LVDC_T_S2", T_S2);
	papiWriteScenario_double(scn, "LVDC_T_S3", T_S3);
	papiWriteScenario_double(scn, "LVDC_TSMC1", TSMC1);
	papiWriteScenario_double(scn, "LVDC_TSMC2", TSMC2);
	papiWriteScenario_double(scn, "LVDC_T_T", T_T);
	papiWriteScenario_double(scn, "LVDC_Tt_2", Tt_2);
	papiWriteScenario_double(scn, "LVDC_Tt_T", Tt_T);
	papiWriteScenario_double(scn, "LVDC_U_1", U_1);
	papiWriteScenario_double(scn, "LVDC_U_2", U_2);
	papiWriteScenario_double(scn, "LVDC_gamma_T", gamma_T);
	papiWriteScenario_double(scn, "LVDC_V", V);
	papiWriteScenario_double(scn, "LVDC_V_ex1", V_ex1);
	papiWriteScenario_double(scn, "LVDC_V_ex2", V_ex2);
	papiWriteScenario_double(scn, "LVDC_V_i", V_i);
	papiWriteScenario_double(scn, "LVDC_V_0", V_0);
	papiWriteScenario_double(scn, "LVDC_V_1", V_1);
	papiWriteScenario_double(scn, "LVDC_V_2", V_2);
	papiWriteScenario_double(scn, "LVDC_V_T", V_T);
	papiWriteScenario_double(scn, "LVDC_V_TC", V_TC);
	papiWriteScenario_double(scn, "LVDC_xi_T", xi_T);
	papiWriteScenario_double(scn, "LVDC_X_S1", X_S1);
	papiWriteScenario_double(scn, "LVDC_X_S2", X_S2);
	papiWriteScenario_double(scn, "LVDC_X_S3", X_S3);
	papiWriteScenario_double(scn, "LVDC_Xtt_y", Xtt_y);
	papiWriteScenario_double(scn, "LVDC_Xtt_p", Xtt_p);
	papiWriteScenario_double(scn, "LVDC_X_Zi", X_Zi);
	papiWriteScenario_double(scn, "LVDC_X_Yi", X_Yi);
	papiWriteScenario_doublearr(scn, "LVDC_SP_M", SP_M, 10);
	papiWriteScenario_doublearr(scn, "LVDC_CP_M", CP_M, 10);
	papiWriteScenario_doublearr(scn, "LVDC_SY_M", SY_M, 10);
	papiWriteScenario_doublearr(scn, "LVDC_CY_M", CY_M, 10);
	papiWriteScenario_doublearr(scn, "LVDC_RA_M", RA_M, 10);
	papiWriteScenario_doublearr(scn, "LVDC_t_Maneuver", t_Maneuver, 10);
	papiWriteScenario_intarr(scn, "LVDC_TYP_M", TYP_M, 10);
	papiWriteScenario_double(scn, "LVDC_SPITCH", SPITCH);
	papiWriteScenario_double(scn, "LVDC_CPITCH", CPITCH);
	papiWriteScenario_double(scn, "LVDC_SYAW", SYAW);
	papiWriteScenario_double(scn, "LVDC_CYAW", CYAW);
	papiWriteScenario_double(scn, "LVDC_ROLLA", ROLLA);
	oapiWriteScenario_int(scn, "LVDC_OrbitManeuverCounter", OrbitManeuverCounter);
	oapiWriteScenario_int(scn, "LVDC_OrbitalGuidanceCycle", OrbitalGuidanceCycle);
	// VECTOR3
	papiWriteScenario_vec(scn, "LVDC_AttitudeError", AttitudeError);
	papiWriteScenario_vec(scn, "LVDC_AttitudeErrorOld", AttitudeErrorOld);
	papiWriteScenario_vec(scn, "LVDC_CommandedAttitude", CommandedAttitude);
	papiWriteScenario_vec(scn, "LVDC_CurrentAttitude", CurrentAttitude);
	papiWriteScenario_vec(scn, "LVDC_DChi_apo", DChi_apo);
	papiWriteScenario_vec(scn, "LVDC_ddotG_act", ddotG_act);
	papiWriteScenario_vec(scn, "LVDC_ddotG_last", ddotG_last);
	papiWriteScenario_vec(scn, "LVDC_ddotM_act", ddotM_act);
	papiWriteScenario_vec(scn, "LVDC_ddotM_last", ddotM_last);
	papiWriteScenario_vec(scn, "LVDC_ddotS", ddotS);
	papiWriteScenario_vec(scn, "LVDC_DDotXEZ_G", DDotXEZ_G);
	papiWriteScenario_vec(scn, "LVDC_DeltaAtt", DeltaAtt);
	papiWriteScenario_vec(scn, "LVDC_Dot0", Dot0);
	papiWriteScenario_vec(scn, "LVDC_DotG_act", DotG_act);
	papiWriteScenario_vec(scn, "LVDC_DotG_last", DotG_last);
	papiWriteScenario_vec(scn, "LVDC_DotM_act", DotM_act);
	papiWriteScenario_vec(scn, "LVDC_DotM_last", DotM_last);
	papiWriteScenario_vec(scn, "LVDC_DotS", DotS);
	papiWriteScenario_vec(scn, "LVDC_DotXEZ", DotXEZ);
	papiWriteScenario_vec(scn, "LVDC_PCommandedAttitude", PCommandedAttitude);
	papiWriteScenario_vec(scn, "LVDC_Pos4", Pos4);
	papiWriteScenario_vec(scn, "LVDC_Pos_Nav", Pos_Nav);
	papiWriteScenario_vec(scn, "LVDC_PosS", PosS);
	papiWriteScenario_vec(scn, "LVDC_PosXEZ", PosXEZ);
	papiWriteScenario_vec(scn, "LVDC_Vel_Nav", Vel_Nav);
	papiWriteScenario_vec(scn, "LVDC_W_ES", W_ES);
	// MATRIX3
	papiWriteScenario_mx(scn, "LVDC_MX_A", MX_A);
	papiWriteScenario_mx(scn, "LVDC_MX_B", MX_B);
	papiWriteScenario_mx(scn, "LVDC_MX_G", MX_G);
	papiWriteScenario_mx(scn, "LVDC_MX_K", MX_K);
	papiWriteScenario_mx(scn, "LVDC_MX_phi_T", MX_phi_T);
	// Done
	oapiWriteLine(scn, LVDC_END_STRING);
}

void LVDC1B::LoadState(FILEHANDLE scn){
	char *line;	
	int tmp = 0; // Used in boolean type loader

	if(Initialized){
		fprintf(lvlog,"LoadState() called\r\n");
		fflush(lvlog);
	}
	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, LVDC_END_STRING, sizeof(LVDC_END_STRING))){
			break;
		}

		// OLD STUFF
		/*
		if (!strnicmp (line, "phi_lat", 7)) {
			sscanf(line + 7, "%lf", &phi_lat);
		} else if (!strnicmp (line, "phi_lng", 7)) {
			sscanf(line + 7, "%lf", &phi_lng);
		}else if (!strnicmp (line, "Azimuth", 7)) {
			sscanf(line + 7, "%lf", &Azimuth);
		}else if (!strnicmp (line, "DescNodeAngle", 13)) {
			sscanf(line + 13, "%lf", &DescNodeAngle);
		}else if (!strnicmp (line, "Inclination", 11)) {
			sscanf(line + 11, "%lf", &Inclination);
		}else if (!strnicmp (line, "TB", 2)) {
			sscanf(line + 2, "%i", &LVDC_Timebase);
		}else if (!strnicmp (line, "TB_ETime", 8)) {
			sscanf(line + 8, "%lf", &LVDC_TB_ETime);
		}else if (!strnicmp (line, "TAS", 3)) {
			sscanf(line + 3, "%lf", &TAS);
		}else if (!strnicmp (line, "t_clock", 7)) {
			sscanf(line + 7, "%lf", &t_clock);
		}else if (!strnicmp (line, "sinceLastIGM", 12)) {
			sscanf(line + 12, "%lf", &sinceLastIGM);
		}else if (!strnicmp (line, "DotM_act", 8)) {
			sscanf(line + 8, "%lf %lf %lf", &DotM_act.x,&DotM_act.y,&DotM_act.z);
		}else if (!strnicmp (line, "DotM_last", 9)) {
			sscanf(line + 9, "%lf %lf %lf", &DotM_last.x,&DotM_last.y,&DotM_last.z);
		}else if (!strnicmp (line, "PsnS", 4)) {
			sscanf(line + 4, "%lf %lf %lf", &PosS.x,&PosS.y,&PosS.z);
		}else if (!strnicmp (line, "Dot0", 4)) {
			sscanf(line + 4, "%lf %lf %lf", &Dot0.x,&Dot0.y,&Dot0.z);
		}else if (!strnicmp (line, "DotG_last", 9)) {
			sscanf(line + 9, "%lf %lf %lf", &DotG_last.x,&DotG_last.y,&DotG_last.z);
		}else if (!strnicmp (line, "ddotG_last", 10)) {
			sscanf(line + 10, "%lf %lf %lf", &ddotG_last.x,&ddotG_last.y,&ddotG_last.z);
		}else if (!strnicmp (line, "PCommandedAttitude", 18)) {
			sscanf(line + 18, "%lf %lf %lf", &PCommandedAttitude.x,&PCommandedAttitude.y,&PCommandedAttitude.z);
		}
		*/
		// NEW STUFF
		// Doing all this in one go makes the MS compiler barf.
		// Doing it in long chains makes the MS compiler silently optimize away the tail of the chain.
		// So we do it in small groups.
		papiReadScenario_string(line, "LVDC_FSPFileName", FSPFileName);
		// INT
		papiReadScenario_int(line, "LVDC_CommandSequence", CommandSequence);
		papiReadScenario_int(line, "LVDC_IGMCycle", IGMCycle);
		papiReadScenario_int(line, "LVDC_MinorLoopCounter", MinorLoopCounter);
		papiReadScenario_int(line, "LVDC_MinorLoopCycles", MinorLoopCycles);
		if (papiReadScenario_int(line, "LVDC_ModeCode24", tmp))
		{
			ModeCode24 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_ModeCode25", tmp))
		{
			ModeCode25 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_ModeCode27", tmp))
		{
			ModeCode27 = tmp;
		}
		papiReadScenario_int(line, "LVDC_LVDC_Timebase", LVDC_Timebase);
		papiReadScenario_int(line, "LVDC_T_EO1", T_EO1);
		papiReadScenario_int(line, "LVDC_T_EO2", T_EO2);
		papiReadScenario_int(line, "LVDC_UP", UP);
		papiReadScenario_int(line, "LVDC_FMANT", FMANT);
		// BOOL
		papiReadScenario_bool(line, "LVDC_alpha_D_op", alpha_D_op);
		papiReadScenario_bool(line, "LVDC_BOOST", BOOST);
		papiReadScenario_bool(line, "LVDC_CountPIPA", CountPIPA);
		papiReadScenario_bool(line, "LVDC_GATE", GATE);
		papiReadScenario_bool(line, "LVDC_GATE5", GATE5);
		papiReadScenario_bool(line, "LVDC_GRR_init", GRR_init);
		papiReadScenario_bool(line, "LVDC_HSL", HSL);
		papiReadScenario_bool(line, "LVDC_LVDC_GRR", LVDC_GRR);
		papiReadScenario_bool(line, "LVDC_LVDC_Stop", LVDC_Stop);
		papiReadScenario_bool(line, "LVDC_poweredflight", poweredflight);
		papiReadScenario_bool(line, "LVDC_S1B_CECO_Commanded", S1B_CECO_Commanded);
		papiReadScenario_bool(line, "LVDC_S1B_Engine_Out", S1B_Engine_Out);
		papiReadScenario_bool(line, "LVDC_S4B_IGN", S4B_IGN);
		papiReadScenario_bool(line, "LVDC_TerminalConditions", TerminalConditions);
		papiReadScenario_bool(line, "LVDC_ReadyToLaunch", ReadyToLaunch);
		papiReadScenario_bool(line, "LVDC_FSCAH", FSCAH);
		papiReadScenario_bool(line, "LVDC_FSCIC", FSCIC);
		papiReadScenario_bool(line, "LVDC_FFPIH", FFPIH);

		// DOUBLE
		papiReadScenario_double(line, "LVDC_a", a);
		papiReadScenario_double(line, "LVDC_a_1", a_1);
		papiReadScenario_double(line, "LVDC_a_2", a_2);
		papiReadScenario_double(line, "LVDC_A1", A1);
		papiReadScenario_double(line, "LVDC_A2", A2);
		papiReadScenario_double(line, "LVDC_A3", A3);
		papiReadScenario_double(line, "LVDC_A4", A4);
		papiReadScenario_double(line, "LVDC_A5", A5);
		papiReadScenario_double(line, "LVDC_alpha_D", alpha_D);
		papiReadScenario_double(line, "LVDC_Ax[0]", Ax[0]);
		papiReadScenario_double(line, "LVDC_Ax[1]", Ax[1]);
		papiReadScenario_double(line, "LVDC_Ax[2]", Ax[2]);
		papiReadScenario_double(line, "LVDC_Ax[3]", Ax[3]);
		papiReadScenario_double(line, "LVDC_Azimuth", Azimuth);
		papiReadScenario_double(line, "LVDC_A_zL", A_zL);
		papiReadScenario_double(line, "LVDC_B_11", B_11);
		papiReadScenario_double(line, "LVDC_B_12", B_12);
		papiReadScenario_double(line, "LVDC_B_21", B_21);
		papiReadScenario_double(line, "LVDC_B_22", B_22);
		papiReadScenario_double(line, "LVDC_C_2", C_2);
		papiReadScenario_double(line, "LVDC_C_3", C_3);
		papiReadScenario_double(line, "LVDC_C_4", C_4);
		papiReadScenario_double(line, "LVDC_CG", CG);
		papiReadScenario_double(line, "LVDC_cos_chi_Yit", cos_chi_Yit);
		papiReadScenario_double(line, "LVDC_cos_chi_Zit", cos_chi_Zit);
		papiReadScenario_double(line, "LVDC_d2", d2);
		papiReadScenario_double(line, "LVDC_ddot_xi_G", ddot_xi_G);
		papiReadScenario_double(line, "LVDC_ddot_eta_G", ddot_eta_G);
		papiReadScenario_double(line, "LVDC_ddot_zeta_G", ddot_zeta_G);
		papiReadScenario_double(line, "LVDC_ddot_zeta_GT", ddot_zeta_GT);
		papiReadScenario_double(line, "LVDC_ddot_xi_GT", ddot_xi_GT);
		papiReadScenario_double(line, "LVDC_DescNodeAngle", DescNodeAngle);
		papiReadScenario_double(line, "LVDC_deta", deta);
		papiReadScenario_double(line, "LVDC_dxi", dxi);
		papiReadScenario_double(line, "LVDC_dot_dxi", dot_dxi);
		papiReadScenario_double(line, "LVDC_dot_dzeta", dot_dzeta);
		papiReadScenario_double(line, "LVDC_dot_dxit", dot_dxit);
		papiReadScenario_double(line, "LVDC_dot_detat", dot_detat);
		papiReadScenario_double(line, "LVDC_dot_dzetat", dot_dzetat);
		papiReadScenario_double(line, "LVDC_dotM_1", dotM_1);
		papiReadScenario_double(line, "LVDC_dotM_2", dotM_2);
		papiReadScenario_double(line, "LVDC_dot_phi_1", dot_phi_1);
		papiReadScenario_double(line, "LVDC_dot_phi_T", dot_phi_T);
		papiReadScenario_double(line, "LVDC_dot_zeta_T", dot_zeta_T);
		papiReadScenario_double(line, "LVDC_dot_xi_T", dot_xi_T);
		papiReadScenario_double(line, "LVDC_dot_eta_T", dot_eta_T);
		papiReadScenario_double(line, "LVDC_dt", dt);
		papiReadScenario_double(line, "LVDC_dT_2", dT_2);
		papiReadScenario_double(line, "LVDC_dt_c", dt_c);
		papiReadScenario_double(line, "LVDC_dT_F", dT_F);
		papiReadScenario_double(line, "LVDC_dt_LET", dt_LET);
		papiReadScenario_double(line, "LVDC_DT_N", DT_N);
		papiReadScenario_double(line, "LVDC_dtt_1", dtt_1);
		papiReadScenario_double(line, "LVDC_dtt_2", dtt_2);
		papiReadScenario_double(line, "LVDC_dV", dV);
		papiReadScenario_double(line, "LVDC_dV_B", dV_B);
		papiReadScenario_double(line, "LVDC_e", e);
		papiReadScenario_double(line, "LVDC_eps_2", eps_2);
		papiReadScenario_double(line, "LVDC_eps_3", eps_3);
		papiReadScenario_double(line, "LVDC_eps_4", eps_4);
		papiReadScenario_double(line, "LVDC_e", e);
		papiReadScenario_double(line, "LVDC_F", F);
		papiReadScenario_double(line, "LVDC_Fm", Fm);
		papiReadScenario_double(line, "LVDC_Fx[0][0]", Fx[0][0]);
		papiReadScenario_double(line, "LVDC_Fx[0][1]", Fx[0][1]);
		papiReadScenario_double(line, "LVDC_Fx[0][2]", Fx[0][2]);
		papiReadScenario_double(line, "LVDC_Fx[0][3]", Fx[0][3]);
		papiReadScenario_double(line, "LVDC_Fx[0][4]", Fx[0][4]);
		papiReadScenario_double(line, "LVDC_Fx[1][0]", Fx[1][0]);
		papiReadScenario_double(line, "LVDC_Fx[1][1]", Fx[1][1]);
		papiReadScenario_double(line, "LVDC_Fx[1][2]", Fx[1][2]);
		papiReadScenario_double(line, "LVDC_Fx[1][3]", Fx[1][3]);
		papiReadScenario_double(line, "LVDC_Fx[1][4]", Fx[1][4]);
		papiReadScenario_double(line, "LVDC_Fx[2][0]", Fx[2][0]);
		papiReadScenario_double(line, "LVDC_Fx[2][1]", Fx[2][1]);
		papiReadScenario_double(line, "LVDC_Fx[2][2]", Fx[2][2]);
		papiReadScenario_double(line, "LVDC_Fx[2][3]", Fx[2][3]);
		papiReadScenario_double(line, "LVDC_Fx[2][4]", Fx[2][4]);
		papiReadScenario_double(line, "LVDC_Fx[3][0]", Fx[3][0]);
		papiReadScenario_double(line, "LVDC_Fx[3][1]", Fx[3][1]);
		papiReadScenario_double(line, "LVDC_Fx[3][2]", Fx[3][2]);
		papiReadScenario_double(line, "LVDC_Fx[3][3]", Fx[3][3]);
		papiReadScenario_double(line, "LVDC_Fx[3][4]", Fx[3][4]);
		papiReadScenario_double(line, "LVDC_Fx[4][0]", Fx[4][0]);
		papiReadScenario_double(line, "LVDC_Fx[4][1]", Fx[4][1]);
		papiReadScenario_double(line, "LVDC_Fx[4][2]", Fx[4][2]);
		papiReadScenario_double(line, "LVDC_Fx[4][3]", Fx[4][3]);
		papiReadScenario_double(line, "LVDC_Fx[4][4]", Fx[4][4]);
		papiReadScenario_double(line, "LVDC_G_T", G_T);
		papiReadScenario_double(line, "LVDC_Inclination", Inclination);
		papiReadScenario_double(line, "LVDC_J", J);
		papiReadScenario_double(line, "LVDC_J_1", J_1);
		papiReadScenario_double(line, "LVDC_J_2", J_2);
		papiReadScenario_double(line, "LVDC_Jt_2", Jt_2);
		papiReadScenario_double(line, "LVDC_J_Y", J_Y);
		papiReadScenario_double(line, "LVDC_J_P", J_P);
		papiReadScenario_double(line, "LVDC_K_1", K_1);
		papiReadScenario_double(line, "LVDC_K_2", K_2);
		papiReadScenario_double(line, "LVDC_K_3", K_3);
		papiReadScenario_double(line, "LVDC_K_4", K_4);
		papiReadScenario_double(line, "LVDC_K_5", K_5);
		papiReadScenario_double(line, "LVDC_K_D", K_D);
		papiReadScenario_double(line, "LVDC_K_p", K_p);
		papiReadScenario_double(line, "LVDC_K_y", K_y);
		papiReadScenario_double(line, "LVDC_K_r", K_r);
		papiReadScenario_double(line, "LVDC_K_Y", K_Y);
		papiReadScenario_double(line, "LVDC_K_P", K_P);
		papiReadScenario_double(line, "LVDC_D_P", D_P);
		papiReadScenario_double(line, "LVDC_D_Y", D_Y);
		papiReadScenario_double(line, "LVDC_KSCLNG", KSCLNG);
		papiReadScenario_double(line, "LVDC_L_1", L_1);
		papiReadScenario_double(line, "LVDC_L_2", L_2);
		papiReadScenario_double(line, "LVDC_dL_2", dL_2);
		papiReadScenario_double(line, "LVDC_Lt_2", Lt_2);
		papiReadScenario_double(line, "LVDC_Lambda_0", Lambda_0);
		papiReadScenario_double(line, "LVDC_lambda_dot", lambda_dot);
		papiReadScenario_double(line, "LVDC_L_P", L_P);
		papiReadScenario_double(line, "LVDC_L_Y", L_Y);
		papiReadScenario_double(line, "LVDC_Lt_Y", Lt_Y);
		papiReadScenario_double(line, "LVDC_LVDC_TB_ETime", LVDC_TB_ETime);
		papiReadScenario_doublearr(line, "LVDC_MF", MF, 5);
		papiReadScenario_double(line, "LVDC_MFS", MFS);
		papiReadScenario_doublearr(line, "LVDC_MFSArr", MFSArr, 4);
		papiReadScenario_double(line, "LVDC_MLR", MLR);
		papiReadScenario_double(line, "LVDC_MS04DT", MS04DT);
		papiReadScenario_double(line, "LVDC_MS25DT", MS25DT);
		papiReadScenario_double(line, "LVDC_MSK5", MSK5);
		papiReadScenario_double(line, "LVDC_MSK6", MSK6);
		papiReadScenario_double(line, "LVDC_MSK16", MSK16);
		papiReadScenario_double(line, "LVDC_MSLIM1", MSLIM1);
		papiReadScenario_double(line, "LVDC_MSLIM2", MSLIM2);
		papiReadScenario_double(line, "LVDC_mu", mu);
		papiReadScenario_double(line, "LVDC_NUPTIM", NUPTIM);
		papiReadScenario_double(line, "LVDC_omega_E", omega_E);
		papiReadScenario_double(line, "LVDC_p", p);
		papiReadScenario_double(line, "LVDC_P_1", P_1);
		papiReadScenario_double(line, "LVDC_P_2", P_2);
		papiReadScenario_double(line, "LVDC_PHI", PHI);
		papiReadScenario_double(line, "LVDC_PHIP", PHIP);
		papiReadScenario_double(line, "LVDC_phi_T", phi_T);
		papiReadScenario_double(line, "LVDC_Q_1", Q_1);
		papiReadScenario_double(line, "LVDC_Q_2", Q_2);
		papiReadScenario_double(line, "LVDC_Q_Y", Q_Y);
		papiReadScenario_double(line, "LVDC_Q_P", Q_P);
		papiReadScenario_double(line, "LVDC_R", R);
		papiReadScenario_double(line, "LVDC_RealTimeClock", RealTimeClock);
		papiReadScenario_double(line, "LVDC_R_L", R_L);
		papiReadScenario_double(line, "LVDC_ROV", ROV);
		papiReadScenario_double(line, "LVDC_R_T", R_T);
		papiReadScenario_double(line, "LVDC_S_1", S_1);
		papiReadScenario_double(line, "LVDC_S_2", S_2);
		papiReadScenario_double(line, "LVDC_S_P", S_P);
		papiReadScenario_double(line, "LVDC_S_Y", S_Y);
		papiReadScenario_double(line, "LVDC_sinceLastIGM", sinceLastIGM);
		papiReadScenario_double(line, "LVDC_sin_chi_Yit", sin_chi_Yit);
		papiReadScenario_double(line, "LVDC_sin_chi_Zit", sin_chi_Zit);
		papiReadScenario_double(line, "LVDC_sin_ups", sin_ups);
		papiReadScenario_double(line, "LVDC_cos_ups", cos_ups);
		papiReadScenario_double(line, "LVDC_SMCG", SMCG);
		papiReadScenario_double(line, "LVDC_S", S);
		papiReadScenario_double(line, "LVDC_P", P);
		papiReadScenario_double(line, "LVDC_t", t);
		papiReadScenario_double(line, "LVDC_t_1", t_1);
		papiReadScenario_double(line, "LVDC_T_1", T_1);
		papiReadScenario_double(line, "LVDC_t_2", t_2);
		papiReadScenario_double(line, "LVDC_T_2", T_2);
		papiReadScenario_double(line, "LVDC_t_3", t_3);
		papiReadScenario_double(line, "LVDC_t_3i", t_3i);
		papiReadScenario_double(line, "LVDC_t_4", t_4);
		papiReadScenario_double(line, "LVDC_t_5", t_5);
		papiReadScenario_double(line, "LVDC_t_6", t_6);
		papiReadScenario_double(line, "LVDC_T_ar", T_ar);
		papiReadScenario_double(line, "LVDC_TAS", TAS);
		papiReadScenario_double(line, "LVDC_tau1", tau1);
		papiReadScenario_double(line, "LVDC_tau2", tau2);
		papiReadScenario_double(line, "LVDC_t_B1", t_B1);
		papiReadScenario_double(line, "LVDC_t_B3", t_B3);
		papiReadScenario_double(line, "LVDC_tchi_y_last", tchi_y_last);
		papiReadScenario_double(line, "LVDC_tchi_p_last", tchi_p_last);
		papiReadScenario_double(line, "LVDC_tchi_y", tchi_y);
		papiReadScenario_double(line, "LVDC_tchi_p", tchi_p);
		papiReadScenario_double(line, "LVDC_t_clock", t_clock);
		papiReadScenario_double(line, "LVDC_T_CO", T_CO);
		papiReadScenario_double(line, "LVDC_t_D", t_D);
		papiReadScenario_double(line, "LVDC_t_fail", t_fail);
		papiReadScenario_double(line, "LVDC_T_GO", T_GO);
		papiReadScenario_double(line, "LVDC_T_GRR", T_GRR);
		papiReadScenario_double(line, "LVDC_T_GRR0", T_GRR0);
		papiReadScenario_double(line, "LVDC_TI", TI);
		papiReadScenario_double(line, "LVDC_T_L_apo", T_L_apo);
		papiReadScenario_double(line, "LVDC_T_LET", T_LET);
		papiReadScenario_double(line, "LVDC_T_S1", T_S1);
		papiReadScenario_double(line, "LVDC_T_S2", T_S2);
		papiReadScenario_double(line, "LVDC_T_S3", T_S3);
		papiReadScenario_double(line, "LVDC_TSMC1", TSMC1);
		papiReadScenario_double(line, "LVDC_TSMC2", TSMC2);
		papiReadScenario_double(line, "LVDC_T_T", T_T);
		papiReadScenario_double(line, "LVDC_Tt_2", Tt_2);
		papiReadScenario_double(line, "LVDC_Tt_T", Tt_T);
		papiReadScenario_double(line, "LVDC_U_1", U_1);
		papiReadScenario_double(line, "LVDC_U_2", U_2);
		papiReadScenario_double(line, "LVDC_gamma_T", gamma_T);
		papiReadScenario_double(line, "LVDC_V", V);
		papiReadScenario_double(line, "LVDC_V_ex1", V_ex1);
		papiReadScenario_double(line, "LVDC_V_ex2", V_ex2);
		papiReadScenario_double(line, "LVDC_V_i", V_i);
		papiReadScenario_double(line, "LVDC_V_0", V_0);
		papiReadScenario_double(line, "LVDC_V_1", V_1);
		papiReadScenario_double(line, "LVDC_V_2", V_2);
		papiReadScenario_double(line, "LVDC_V_T", V_T);
		papiReadScenario_double(line, "LVDC_V_TC", V_TC);
		papiReadScenario_double(line, "LVDC_xi_T", xi_T);
		papiReadScenario_double(line, "LVDC_X_S1", X_S1);
		papiReadScenario_double(line, "LVDC_X_S2", X_S2);
		papiReadScenario_double(line, "LVDC_X_S3", X_S3);
		papiReadScenario_double(line, "LVDC_Xtt_y", Xtt_y);
		papiReadScenario_double(line, "LVDC_Xtt_p", Xtt_p);
		papiReadScenario_double(line, "LVDC_X_Zi", X_Zi);
		papiReadScenario_double(line, "LVDC_X_Yi", X_Yi);
		papiReadScenario_doublearr(line, "LVDC_SP_M", SP_M, 10);
		papiReadScenario_doublearr(line, "LVDC_CP_M", CP_M, 10);
		papiReadScenario_doublearr(line, "LVDC_SY_M", SY_M, 10);
		papiReadScenario_doublearr(line, "LVDC_CY_M", CY_M, 10);
		papiReadScenario_doublearr(line, "LVDC_RA_M", RA_M, 10);
		papiReadScenario_doublearr(line, "LVDC_t_Maneuver", t_Maneuver, 10);
		papiReadScenario_intarr(line, "LVDC_TYP_M", TYP_M, 10);
		papiReadScenario_double(line, "LVDC_SPITCH", SPITCH);
		papiReadScenario_double(line, "LVDC_CPITCH", CPITCH);
		papiReadScenario_double(line, "LVDC_SYAW", SYAW);
		papiReadScenario_double(line, "LVDC_CYAW", CYAW);
		papiReadScenario_double(line, "LVDC_ROLLA", ROLLA);
		papiReadScenario_int(line, "LVDC_OrbitManeuverCounter", OrbitManeuverCounter);
		papiReadScenario_int(line, "LVDC_OrbitalGuidanceCycle", OrbitalGuidanceCycle);

		// VECTOR3
		papiReadScenario_vec(line, "LVDC_AttitudeError", AttitudeError);
		papiReadScenario_vec(line, "LVDC_AttitudeErrorOld", AttitudeErrorOld);
		papiReadScenario_vec(line, "LVDC_CommandedAttitude", CommandedAttitude);
		papiReadScenario_vec(line, "LVDC_CurrentAttitude", CurrentAttitude);
		papiReadScenario_vec(line, "LVDC_DChi_apo", DChi_apo);
		papiReadScenario_vec(line, "LVDC_ddotG_act", ddotG_act);
		papiReadScenario_vec(line, "LVDC_ddotG_last", ddotG_last);
		papiReadScenario_vec(line, "LVDC_ddotM_act", ddotM_act);
		papiReadScenario_vec(line, "LVDC_ddotM_last", ddotM_last);
		papiReadScenario_vec(line, "LVDC_ddotS", ddotS);
		papiReadScenario_vec(line, "LVDC_DDotXEZ_G", DDotXEZ_G);
		papiReadScenario_vec(line, "LVDC_DeltaAtt", DeltaAtt);
		papiReadScenario_vec(line, "LVDC_Dot0", Dot0);
		papiReadScenario_vec(line, "LVDC_DotG_act", DotG_act);
		papiReadScenario_vec(line, "LVDC_DotG_last", DotG_last);
		papiReadScenario_vec(line, "LVDC_DotM_act", DotM_act);
		papiReadScenario_vec(line, "LVDC_DotM_last", DotM_last);
		papiReadScenario_vec(line, "LVDC_DotS", DotS);
		papiReadScenario_vec(line, "LVDC_DotXEZ", DotXEZ);
		papiReadScenario_vec(line, "LVDC_PCommandedAttitude", PCommandedAttitude);
		papiReadScenario_vec(line, "LVDC_Pos4", Pos4);
		papiReadScenario_vec(line, "LVDC_Pos_Nav", Pos_Nav);
		papiReadScenario_vec(line, "LVDC_PosS", PosS);
		papiReadScenario_vec(line, "LVDC_PosXEZ", PosXEZ);
		papiReadScenario_vec(line, "LVDC_Vel_Nav", Vel_Nav);
		papiReadScenario_vec(line, "LVDC_W_ES", W_ES);

		// MATRIX3
		papiReadScenario_mat(line, "LVDC_MX_A", MX_A);
		papiReadScenario_mat(line, "LVDC_MX_B", MX_B);
		papiReadScenario_mat(line, "LVDC_MX_G", MX_G);
		papiReadScenario_mat(line, "LVDC_MX_K", MX_K);
		papiReadScenario_mat(line, "LVDC_MX_phi_T", MX_phi_T);
	}

	ReadFlightSequenceProgram(FSPFileName);

	return;
}

void LVDC1B::ReadFlightSequenceProgram(char *fspfile)
{
	using namespace std;

	std::vector<SwitchSelectorSet> v;

	SwitchSelectorSet ssset;

	bool first = true;
	int tb, tbtemp;

	string line;
	ifstream file(fspfile);
	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (sscanf(line.c_str(), "TB%d", &tbtemp) == 1 || line.compare("END") == 0)
			{
				if (first == false)
				{
					if (tb >= 1 && tb <= 4)
					{
						SSTTB[tb] = v;
					}
					else if (tb == 10)
					{
						SSTALT1 = v;
					}
				}

				v.clear();
				tb = tbtemp;
				first = false;
			}
			else if (sscanf(line.c_str(), "%lf,%d,%d", &ssset.time, &ssset.stage, &ssset.channel) == 3)
			{
				v.push_back(ssset);
			}
		}
	}

	file.close();
}

VECTOR3 LVDC1B::SVCompare()
{
	VECTOR3 pos, vel, newpos, newvel;
	MATRIX3 mat;
	double MJD_L, MJD_cur, MJD_LD, xtemp;
	MJD_cur = oapiGetSimMJD();
	xtemp = modf(MJD_cur - (RealTimeClock - T_GRR) / 3600.0 / 24.0, &MJD_LD);
	MJD_L = MJD_LD + T_GRR / 3600.0 / 24.0;
	mat = OrbMech::Orbiter2PACSS13(MJD_L, PHI, KSCLNG, Azimuth);
	lvda.GetRelativePos(pos);
	lvda.GetRelativeVel(vel);
	newpos = mul(mat, pos);
	newvel = mul(mat, vel);

	VECTOR3 i, j, k;
	j = unit(crossp(newvel, newpos));
	k = unit(-newpos);
	i = crossp(j, k);
	MATRIX3 Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z); //rotation matrix to LVLH

	return mul(Q_Xx, PosS - newpos);
}

bool LVDC1B::TimebaseUpdate(double dt)
{
	if (LVDC_Timebase == 4)
	{
		LVDC_TB_ETime += dt;
		return true;
	}

	return false;
}

bool LVDC1B::GeneralizedSwitchSelector(int stage, int channel)
{
	if (LVDC_Timebase == 3 || LVDC_Timebase == 4)
	{
		if (stage >= 0 && stage < 4)
		{
			if (channel > 0 && channel < 113)
			{
				lvda.SwitchSelector(stage, channel);
				return true;
			}
		}
	}

	return false;
}

bool LVDC1B::LMAbort()
{
	if (lvda.GetVehicleNo() == 204)
	{
		if (LVDC_Timebase == 4 && LVDC_TB_ETime > 5.0)
		{
			//Issue S-IVB cutoff
			LVDC_Timebase = 10;
			LVDC_TB_ETime = 0;
			CommandSequence = 0;
			//Program attitude maneuver
			GOMTYP[1] = true;
			T_SOM = 0.0;
			return true;
		}
	}

	return false;
}

bool LVDC1B::InhibitAttitudeManeuver()
{
	return false;
}

bool LVDC1B::LaunchTargetingUpdate(double v_t, double r_t, double theta_t, double inc, double dsc, double dsc_dot, double t_grr0)
{
	if (LVDC_Timebase < 0)
	{
		V_T = v_t;
		R_T = r_t;
		gamma_T = theta_t;
		T_GRR0 = t_grr0;
		Inclination = inc;
		Lambda_0 = dsc;
		lambda_dot = dsc_dot;
		fprintf(lvlog, "Targeting update received! R_T %f V_T %f gamma_T %f T_GRR0 %f\r\n", R_T, V_T, gamma_T, T_GRR0);
		fprintf(lvlog, "Inclination %f Lambda_0 %f lambda_dot %f\r\n", Inclination, Lambda_0, lambda_dot);

		return true;
	}

	return false;
}

bool LVDC1B::DiscreteOutputTest(int bit, bool on)
{
	if (bit >= 0 && bit <= 12)
	{
		lvda.SetOutputRegisterBit(bit, on);
		return true;
	}
	return false;
}

bool LVDC1B::NavigationUpdate(VECTOR3 DCSRVEC, VECTOR3 DCSVVEC, double DCSNUPTIM)
{
	if (DCSNUPTIM > RealTimeClock - T_GRR + 10.0)
	{
		NUPTIM = DCSNUPTIM;
		Pos_Nav = DCSRVEC;
		Vel_Nav = DCSVVEC;
		ModeCode27[MC27_DCSNavigationUpdateAccepted] = true;
		fprintf(lvlog, "Navigation update received! R %f %f %f V %f %f %f T %f \r\n", DCSRVEC.x, DCSRVEC.y, DCSRVEC.z, DCSRVEC.x, DCSRVEC.y, DCSRVEC.z, DCSNUPTIM);
		return true;
	}
	return false;
}

bool LVDC1B::GeneralizedManeuver(double T, double X, double Y, double Z, int type)
{
	if (LVDC_Timebase == 4)
	{
		T_SOM = T;
		X_ref = X;
		Y_ref = Y;
		Z_ref = Z;
		GOMTYP = type;

		fprintf(lvlog, "Generalized maneuver command received! T %f X_ref %f Y_ref %f Z_ref %f Type %d \r\n", T_SOM, X_ref*DEG, Y_ref*DEG, Z_ref*DEG, type);
		return true;
	}
	return false;
}

VECTOR3 LVDC1B::GravitationSubroutine(VECTOR3 Rvec, bool J2only)
{
	VECTOR3 GTEMP;
	double P_34, S_34;
	Y_u = -(Rvec.x*MX_A.m21 + Rvec.y*MX_A.m22 + Rvec.z*MX_A.m23); //position component south of equator
	double rtemp = length(Rvec); //instantaneous distance from earth's center
	double AoverR = a / rtemp;
	double YoverR = Y_u / rtemp;
	if (J2only)
	{
		S_34 = 0.0;
		P_34 = 0.0;
	}
	else
	{
		S_34 = H * pow(AoverR, 3)*(YoverR)*(3.0 - 7.0*pow(YoverR, 2)) + D / 7.0*pow(AoverR, 4)*(3.0 - 42.0*pow(YoverR, 2) + 63.0*pow(YoverR, 4));
		P_34 = H / 5.0*(AoverR)*(15.0*pow(YoverR, 2) - 3.0) + D / 7.0*pow(AoverR, 2)*YoverR*(12.0 - 28.0*pow(YoverR, 2));
	}
	S = (-mu / pow(rtemp, 3))*(1.0 + J * pow(AoverR, 2)*(1.0 - 5.0 * pow(YoverR, 2)) + S_34);
	P = (-mu / pow(rtemp, 2))*pow(AoverR, 2) *(2.0*J*(YoverR) + P_34);
	GTEMP.x = Rvec.x*S - MX_A.m21*P; //gravity acceleration vector
	GTEMP.y = Rvec.y*S - MX_A.m22*P;
	GTEMP.z = Rvec.z*S - MX_A.m23*P;
	return GTEMP;
}

VECTOR3 LVDC1B::DragSubroutine(VECTOR3 Rvec, VECTOR3 Vvec, VECTOR3 Att)
{
	//fprintf(lvlog, "DRAG SUBROUTINE\r\n");
	//fprintf(lvlog, "Rvec = %f %f %f Vvec = %f %f %f\r\n", Rvec.x, Rvec.y, Rvec.z, Vvec.x, Vvec.y, Vvec.z);
	//Hack for old scenarios. Remove at some point
	if (length(W_ES) == 0.0)
	{
		W_ES = -_V(MX_A.m21, MX_A.m22, MX_A.m23)*omega_E;
	}
	h = length(Rvec) - a;
	if (h > h_2)
	{
		rho = 0.0;
	}
	else if (h < h_1)
	{
		rho = rho_c;
	}
	else
	{
		rho = Rho[0] + Rho[1] * h + Rho[2] * pow(h, 2) + Rho[3] * pow(h, 3) + Rho[4] * pow(h, 4) + Rho[5] * pow(h, 5);
	}
	//fprintf(lvlog, "h = %f rho = %e\r\n", h, rho);
	V_R = Vvec - crossp(W_ES, Rvec);
	v_R = length(V_R);
	//fprintf(lvlog, "V_R = %f %f %f v_R = %f\r\n", V_R.x, V_R.y, V_R.z, v_R);
	cos_alpha = 1.0 / v_R * (V_R.x*cos(Att.y)*cos(Att.z) + V_R.y*sin(Att.z) - V_R.z*sin(Att.y)*cos(Att.z));
	drag_area = Drag_Area[0] + Drag_Area[1] * cos_alpha + Drag_Area[2] * pow(cos_alpha, 2) + Drag_Area[3] * pow(cos_alpha, 3) + Drag_Area[4] * pow(cos_alpha, 4);
	//fprintf(lvlog, "cos_alpha = %f drag_area = %f\r\n", cos_alpha, drag_area);
	//TBD: Remove factor 0.05 when RTCC takes drag into account properly
	VECTOR3 A_DS = -V_R * rho * drag_area*K_D*v_R*0.05;
	//fprintf(lvlog, "A_DS = %e %e %e\r\n", A_DS.x, A_DS.y, A_DS.z);
	return A_DS;
}

void LVDC1B::AccelerometerRead()
{
	//fprintf(lvlog, "[%d+%f] ACCELEROMETER READ\r\n", LVDC_Timebase, LVDC_TB_ETime);

	DotM_act += lvda.GetLVIMUPIPARegisters(); //read the PIPA CDUs
	//TAS is time of last accelerometer sampling
	TAS = RealTimeClock - T_GRR;
}

void LVDC1B::FMCalculations()
{
	//fprintf(lvlog, "[%d+%f] F/M CALCULATIONS\r\n", LVDC_Timebase, LVDC_TB_ETime);

	//dt_c is the LVDC guidance cycle time. The LV IMU currently runs once per Orbiter timestep, so to get a stable Fm reading we need to take the time difference of the LV IMU measurements
	ddotM_act = (DotM_act - DotM_last) / dt_c;
	Fm = length(ddotM_act);
	fprintf(lvlog, "Sensed Acceleration: %f \r\n", Fm);

	if (LVDC_Timebase == 3 && LVDC_TB_ETime > 7.48)
	{
		MF[0] = 1.0 / Fm;
		double MFSTEMP = MFK[0] * MF[0] + MFK[1] * MF[1] + MFK[2] * MF[2] + MFK[3] * MF[3] + MFK[4] * MFSArr[0] + MFK[5] * MFSArr[1] + MFK[6] * MFSArr[2] + MFK[7] * MFSArr[3];

		//MFS can only change by 0.005 sec^2/m per major loop
		if (MFSTEMP - MFS > 0.005)
		{
			MFS = MFS + 0.005;
		}
		else if (MFSTEMP - MFS < -0.005)
		{
			MFS = MFS - 0.005;
		}
		else
		{
			MFS = MFSTEMP;
		}

		//Limit magnitude to 0.25 (= a minimum of 4 m/s^2 acceleration)
		if (MFS > 0.25)
		{
			MFS = 0.25;
		}

		MFSArr[3] = MFSArr[2];
		MFSArr[2] = MFSArr[1];
		MFSArr[1] = MFSArr[0];
		MFSArr[0] = MFS;
		MF[3] = MF[2];
		MF[2] = MF[1];
		MF[1] = MF[0];

		//sprintf(oapiDebugString(), "FM[0] %lf 1/MFS %lf", Fm, 1.0 / MFS);
		fprintf(lvlog, "Smoothed Acceleration: %f \r\n", 1.0 / MFS);
	}
}

void LVDC1B::BoostNavigation()
{
	//fprintf(lvlog, "[%d+%f] BOOST NAVIGATION\r\n", LVDC_Timebase, LVDC_TB_ETime);
	PosS.x += (DotM_act.x + DotM_last.x) * dt_c / 2.0 + (DotG_last.x + ddotG_last.x * dt_c / 2.0)*dt_c + Dot0.x * dt_c; //position vector
	PosS.y += (DotM_act.y + DotM_last.y) * dt_c / 2.0 + (DotG_last.y + ddotG_last.y * dt_c / 2.0)*dt_c + Dot0.y * dt_c;
	PosS.z += (DotM_act.z + DotM_last.z) * dt_c / 2.0 + (DotG_last.z + ddotG_last.z * dt_c / 2.0)*dt_c + Dot0.z * dt_c;
	R = length(PosS);
	ddotG_act = GravitationSubroutine(PosS, true);
	CG = pow((pow(ddotG_act.x, 2) + pow(ddotG_act.y, 2) + pow(ddotG_act.z, 2)), 0.5);
	DotG_act.x = DotG_last.x + (ddotG_act.x + ddotG_last.x) * dt_c / 2; //gravity velocity vector
	DotG_act.y = DotG_last.y + (ddotG_act.y + ddotG_last.y) * dt_c / 2;
	DotG_act.z = DotG_last.z + (ddotG_act.z + ddotG_last.z) * dt_c / 2;
	DotS.x = DotM_act.x + DotG_act.x + Dot0.x; //total velocity vector 
	DotS.y = DotM_act.y + DotG_act.y + Dot0.y;
	DotS.z = DotM_act.z + DotG_act.z + Dot0.z;
	V = pow(pow(DotS.x, 2) + pow(DotS.y, 2) + pow(DotS.z, 2), 0.5);
	//save the 'actual' variables as 'last' variables for the next step
	DotM_last = DotM_act;
	DotG_last = DotG_act;
	ddotG_last = ddotG_act;
	fprintf(lvlog, "Navigation \r\n");
	fprintf(lvlog, "Inertial Attitude: %f %f %f \r\n", CurrentAttitude.x*DEG, CurrentAttitude.y*DEG, CurrentAttitude.z*DEG);
	fprintf(lvlog, "DotM: %f %f %f \r\n", DotM_act.x, DotM_act.y, DotM_act.z);
	fprintf(lvlog, "Gravity velocity: %f %f %f \r\n", DotG_act.x, DotG_act.y, DotG_act.z);
	fprintf(lvlog, "EarthRel Position: %f %f %f \r\n", PosS.x, PosS.y, PosS.z);
	VECTOR3 drtest = SVCompare();
	fprintf(lvlog, "SV Accuracy: %f %f %f\r\n", drtest.x, drtest.y, drtest.z);
	fprintf(lvlog, "EarthRel Velocity: %f %f %f \r\n", DotS.x, DotS.y, DotS.z);
	fprintf(lvlog, "Gravity Acceleration: %f \r\n", CG);
	fprintf(lvlog, "Total Velocity: %f \r\n", V);
	fprintf(lvlog, "Dist. from Earth's Center: %f \r\n", R);
	fprintf(lvlog, "S: %f \r\n", S);
	fprintf(lvlog, "P: %f \r\n", P);
	lvda.ZeroLVIMUPIPACounters();
}

void LVDC1B::ChiComputations(int entry)
{
	//Entries:
	//0 = Entered once per computation cycle from IGM while IGM is active during Phase 1. Entered 1/sec from the Orbit Guidance module during phase 2.
	//1 = Entered from Time Tilt Guidance module from liftoff until start of IGM. Entered once per computation cycle from Orbit Guidance during an attitude freeze.

	if (entry == 0)
	{
		VECTOR3 VT;
		VT = tmul(MX_G, _V(cos_chi_Yit*cos_chi_Zit, sin_chi_Zit, -sin_chi_Yit * cos_chi_Zit));
		fprintf(lvlog, "VT (mul) = %f %f %f\r\n", VT.x, VT.y, VT.z);

		X_S1 = VT.x;
		X_S2 = VT.y;
		X_S3 = VT.z;
		fprintf(lvlog, "X_S1-3 = %f %f %f\r\n", X_S1, X_S2, X_S3);

		// FINALLY - COMMANDS!
		X_Zi = atan2(X_S2, sqrt(1.0 - pow(X_S2, 2)));	// Yaw

		X_Yi = atan2(-X_S3, X_S1);						// Pitch
		if (X_Yi < 0)
		{
			X_Yi += PI2;
		}
		fprintf(lvlog, "*** COMMAND ISSUED ***\r\n");
		fprintf(lvlog, "PITCH = %f, YAW = %f\r\n", X_Yi*DEG, X_Zi*DEG);

		// IGM is supposed to generate attitude directly.
		CommandedAttitude.y = X_Yi; // PITCH
		CommandedAttitude.z = X_Zi; // YAW;
	}

	//TBD: Telemeter guidance chi X, Y, Z
}

void LVDC1B::DiscreteProcessor()
{
	//Discrete Input 9 Check (S&C Control of Saturn)		
	if (ModeCode27[MC27_GRFDiscretesSet] && lvda.GetCMCSIVBTakeover() && lvda.GetSCControlPoweredFlight() && ModeCode27[MC27_SCControlAfterGRF] == false)
	{
		lvda.SwitchSelector(SWITCH_SELECTOR_IU, 18);
		ModeCode27[MC27_SCControlAfterGRF] = true;
		fprintf(lvlog, "[%d+%f] Permanent SC Control bit set\r\n", LVDC_Timebase, LVDC_TB_ETime);
	}

	if (ModeCode27[MC27_SCInControl] == false && lvda.GetCMCSIVBTakeover())
	{
		if (LVDC_Timebase == 4 && LVDC_TB_ETime > 5.0)
		{
			ModeCode27[MC27_SCInControl] = true;
			fprintf(lvlog, "[%d+%f] SC has taken control of Saturn (coasting flight)\r\n", LVDC_Timebase, LVDC_TB_ETime);
		}
		else if (ModeCode27[MC27_GRFDiscretesSet] && lvda.GetSCControlPoweredFlight())
		{
			ModeCode27[MC27_SCInControl] = true;
			fprintf(lvlog, "[%d+%f] SC has taken control of Saturn (GRF)\r\n", LVDC_Timebase, LVDC_TB_ETime);
		}
	}
	else if (ModeCode27[MC27_SCInControl] && ModeCode27[MC27_SCControlAfterGRF] == false && !lvda.GetCMCSIVBTakeover())
	{
		ModeCode27[MC27_SCInControl] = false;
		CommandedAttitude = PCommandedAttitude = CurrentAttitude;
		DChi_apo = _V(0, 0, 0);
		FSCAH = true;
		FSCIC = true;
		fprintf(lvlog, "[%d+%f] Saturn control returned to LVDC\r\n", LVDC_Timebase, LVDC_TB_ETime);
	}
}

void LVDC1B::AttitudeFreze(int entry)
{
	//Entries:
	//0 = Entered by the Orbit Guidance module to perform an absolute freeze
	//1 = Entered from Non-Interrupt Sequencer modules to perform a command freeze

	if (entry == 0)
	{
		//Absolute freeze

		//Set command chi's to MI theta's
		PCommandedAttitude = CurrentAttitude;
	}
	else
	{
		//Command freeze
	}

	//Set guidance chi's to command chi's
	CommandedAttitude = PCommandedAttitude;
	//Zero delta chi's
	DChi_apo = _V(0, 0, 0);
	//Setup to bypass setting delta chi's
	//DFDCO = true;
}

void LVDC1B::OrbitalGuidanceManeuverProcessor(bool timefordcs)
{
	if (timefordcs)
	{
		//Local Reference Maneuver
		if (GOMTYP[4])
		{
			fprintf(lvlog, "Orbital Guidance: Initiate DCS Local Reference Maneuver\r\n");

			ModeCode27[MC27_DCSExecuteGeneralizedManeuverAccepted] = true;
			ModeCode27[MC27_DCSSpecialManeuverAImplemented] = false;
			ModeCode27[MC27_DCSSpecialManeuverBImplemented] = false;

			FMANT = -1;

			ROLLA = X_ref;
			SPITCH = sin(Y_ref);
			CPITCH = cos(Y_ref);
			SYAW = sin(Z_ref);
			CYAW = cos(Z_ref);
		}
		//Inertial Reference Maneuver
		else if (GOMTYP[3])
		{
			fprintf(lvlog, "Orbital Guidance: Initiate DCS Inertial Attitude Hold\r\n");

			ModeCode27[MC27_DCSExecuteGeneralizedManeuverAccepted] = true;
			ModeCode27[MC27_DCSSpecialManeuverAImplemented] = false;
			ModeCode27[MC27_DCSSpecialManeuverBImplemented] = false;

			FMANT = 0;

			ROLLA = X_ref;
			SPITCH = Y_ref;
			SYAW = Z_ref;
		}
		//Return to Nominal Timeline
		else if (GOMTYP[2])
		{
			fprintf(lvlog, "Orbital Guidance: DCS Return to Nominal Timeline\r\n");

			ModeCode27[MC27_DCSExecuteGeneralizedManeuverAccepted] = false;
			ModeCode27[MC27_DCSSpecialManeuverAImplemented] = false;
			ModeCode27[MC27_DCSSpecialManeuverBImplemented] = false;

			//Restart the currently desired scheduled maneuver
			if (OrbitManeuverCounter > 0)
			{
				OrbitManeuverCounter--;
			}

		}
		//Special Maneuver A
		else if (GOMTYP[1])
		{
			fprintf(lvlog, "Orbital Guidance: Initiate DCS Special Maneuver A\r\n");

			ModeCode27[MC27_DCSExecuteGeneralizedManeuverAccepted] = false;
			ModeCode27[MC27_DCSSpecialManeuverAImplemented] = true;
			ModeCode27[MC27_DCSSpecialManeuverBImplemented] = false;

			FMANT = 0;

			//Pitch up 10�
			ROLLA = CurrentAttitude.x;
			SPITCH = CurrentAttitude.y + 10.0*RAD;
			if (SPITCH >= PI2)
			{
				SPITCH -= PI2;
			}
			SYAW = CurrentAttitude.z;
		}
		//Special Maneuver B
		else if (GOMTYP[0])
		{
			fprintf(lvlog, "Orbital Guidance: Initiate DCS Special Maneuver A\r\n");

			ModeCode27[MC27_DCSExecuteGeneralizedManeuverAccepted] = false;
			ModeCode27[MC27_DCSSpecialManeuverAImplemented] = false;
			ModeCode27[MC27_DCSSpecialManeuverBImplemented] = true;

			//TBD		
		}

		GOMTYP.reset();
	}
	else
	{
		//Save commanded maneuver type if it is a valid one
		if (TYP_M[OrbitManeuverCounter] != 9)
		{
			FMANT = TYP_M[OrbitManeuverCounter];
			//Reset maneuver flags
			FSCAH = false;
			FSCIC = false;
		}

		//Chi freeze
		if (TYP_M[OrbitManeuverCounter] == 0)
		{
			fprintf(lvlog, "Orbital Guidance: Initiate Chi Freeze\r\n");

			PCommandedAttitude = CurrentAttitude;
			ROLLA = PCommandedAttitude.x;
			SPITCH = PCommandedAttitude.y;
			SYAW = PCommandedAttitude.z;

			fprintf(lvlog, "Attitude: %f %f %f\r\n", PCommandedAttitude.x*DEG, PCommandedAttitude.y*DEG, PCommandedAttitude.z*DEG);
		}
		//Inertial attitude hold
		else if (TYP_M[OrbitManeuverCounter] == 1)
		{
			fprintf(lvlog, "Orbital Guidance: Initiate Attitude Hold\r\n");

			SPITCH = SP_M[OrbitManeuverCounter];
			SYAW = SY_M[OrbitManeuverCounter];
			ROLLA = RA_M[OrbitManeuverCounter];

			fprintf(lvlog, "Attitude: %f %f %f\r\n", ROLLA*DEG, SPITCH*DEG, SYAW*DEG);
		}
		//Track or hold local reference maneuver
		else if (TYP_M[OrbitManeuverCounter] == -1 || TYP_M[OrbitManeuverCounter] == -2)
		{
			if (TYP_M[OrbitManeuverCounter] == -1)
			{
				fprintf(lvlog, "Orbital Guidance: Initiate Local Reference Track\r\n");
				FSLR = false;
			}
			else
			{
				fprintf(lvlog, "Orbital Guidance: Initiate Local Reference Inertial Hold\r\n");
				FSLR = true;
			}

			SPITCH = SP_M[OrbitManeuverCounter];
			CPITCH = CP_M[OrbitManeuverCounter];
			SYAW = SY_M[OrbitManeuverCounter];
			CYAW = CY_M[OrbitManeuverCounter];
			ROLLA = RA_M[OrbitManeuverCounter];

			fprintf(lvlog, "Local Reference: SP %f CP %f SY %f CY %f RA %f\r\n", SPITCH, CPITCH, SYAW, CYAW, ROLLA*DEG);
		}
		else
		{
			//Unknown type, displayed orbital guidance
			OrbitManeuverCounter = 10;
		}
		OrbitManeuverCounter++;
	}

	FFPIH = true;
}

void LVDC1B::OrbitalGuidance()
{
	fprintf(lvlog, "[%d+%f] ORBITAL GUIDANCE\r\n", LVDC_Timebase, LVDC_TB_ETime);

	//Check if it is time to execute DCS maneuver
	if (GOMTYP.to_ulong() != 0)
	{
		if (LVDC_TB_ETime > T_SOM)
		{
			OrbitalGuidanceManeuverProcessor(true);
		}
	}
	//Start next scheduled maneuver
	bool bypass = (ModeCode27[MC27_DCSExecuteGeneralizedManeuverAccepted] == true || ModeCode27[MC27_DCSSpecialManeuverAImplemented] == true || ModeCode27[MC27_DCSSpecialManeuverBImplemented] == true);
	if (bypass == false)
	{
		bool tempflag = false;
		if (LVDC_Timebase == 4)
		{
			if (OrbitManeuverCounter < 10)
			{
				//Time since GRR
				if (t_Maneuver[OrbitManeuverCounter] < 0)
				{
					if (RealTimeClock - T_GRR > abs(t_Maneuver[OrbitManeuverCounter]))
					{
						tempflag = true;
					}
				}
				//Time since TB4
				else
				{
					if (LVDC_TB_ETime > t_Maneuver[OrbitManeuverCounter])
					{
						tempflag = true;
					}
				}
			}
		}

		if (tempflag)
		{
			OrbitalGuidanceManeuverProcessor(false);
		}
	}

	//Compute positions in orbit plane
	OrbitalGuidance040();

	//Reset maneuver bits for mode code 27
	ModeCode27[MC27_InertialAttHoldInProgress] = false;
	ModeCode27[MC27_LocalReferenceManeuverInProgress] = false;
	ModeCode27[MC27_TrackLocalReferenceAfterSCControl] = false;
	ModeCode27[MC27_InertialAttHoldManeuverAfterSCControl] = false;

	if (ModeCode27[MC27_SCInControl] == true)
	{
		//Absolute attitude freeze
		AttitudeFreze(0);
		//Telemeter guidance chi's
		ChiComputations(1);
	}
	else if (FSCAH)
	{
		//Spacecraft has been in control
		OrbitalGuidance280();
	}
	else if (FMANT == -1 || FMANT == -2)
	{
		//Local reference
		if (FMANT == -1)
		{
			//Nominal local reference maneuver
			ModeCode27[MC27_LocalReferenceManeuverInProgress] = true;
			//Track local reference
			OrbitalGuidance680();
		}
		else
		{
			//Special local reference maneuver
			ModeCode27[MC27_InertialAttHoldInProgress] = true;
			//Execute special local reference maneuver
			OrbitalGuidance940();
		}
	}
	else
	{
		if (FMANT == 1)
		{
			//Inertial hold
			ModeCode27[MC27_InertialAttHoldInProgress] = true;
			//Execute inertial hold maneuver
			OrbitalGuidance860();
		}
		else
		{
			//Chi freeze
			ModeCode27[MC27_InertialAttHoldInProgress] = true;
			//Attitude freeze maneuver
			OrbitalGuidance340();
		}
	}
}

void LVDC1B::OrbitalGuidance040()
{
	Pos4 = mul(MX_G, R_OG);
	//fprintf(lvlog, "Pos4: %f %f %f\r\n", Pos4.x, Pos4.y, Pos4.z);
	R4 = sqrt(Pos4.x*Pos4.x + Pos4.z*Pos4.z);
}

void LVDC1B::OrbitalGuidance280()
{
	if (FSCIC)
	{
		//First pass
		FSCIC = false;

		if (FMANT == -1)
		{
			VECTOR3 P_S;

			//Compute unit thrust vector
			P_S = OrbitalGuidance480();
			//Compute maneuver parameters for spacecraft
			OrbitalGuidance580(P_S);
			//Local reference maneuver
			ModeCode27[MC27_TrackLocalReferenceAfterSCControl] = true;
			//Execute track local reference maneuver
			OrbitalGuidance680();
		}
		else
		{
			//Inertial hold
			ModeCode27[MC27_InertialAttHoldManeuverAfterSCControl] = true;
			//Process attitude freeze
			OrbitalGuidance340();
		}
	}
	else
	{
		if (FMANT == -1)
		{
			//Local reference maneuver
			ModeCode27[MC27_TrackLocalReferenceAfterSCControl] = true;
			//Execute track local reference maneuver
			OrbitalGuidance680();
		}
		else
		{
			//Inertial hold
			ModeCode27[MC27_InertialAttHoldManeuverAfterSCControl] = true;
			//Process attitude freeze
			OrbitalGuidance340();
		}
	}
}

void LVDC1B::OrbitalGuidance340()
{
	//Attitude freeze maneuver

	if (FFPIH)
	{
		//Perform attitude freeze
		AttitudeFreze(0);
		FFPIH = false;
	}
	//Telemeter guidance chi's
	ChiComputations(1);
}

VECTOR3 LVDC1B::OrbitalGuidance480()
{
	CommandedAttitude.x = CurrentAttitude.x;

	return _V(cos(CurrentAttitude.y)*cos(CurrentAttitude.z), sin(CurrentAttitude.z), -sin(CurrentAttitude.y)*cos(CurrentAttitude.z));
}

void LVDC1B::OrbitalGuidance580(VECTOR3 P_S)
{
	VECTOR3 P_4;
	double phi_SO, phi_P, alpha_LH;

	//Rotate unit thrust vector to 4-system (equation 5.5.1)
	P_4 = mul(MX_G, P_S);
	sin_chi_Zit = P_4.y;
	//Calculate cosine of target plane yaw angle (equation 5.5.11)
	cos_chi_Zit = sqrt(1.0-pow(sin_chi_Zit,2));
	//Calculate sine of angle between spacecraft and target vector (equation 5.5.12)
	phi_SO = atan2(Pos4.z, Pos4.x);
	//Calculate pitch angle in the target plane (equation 5.5.12)
	phi_P = atan2(-P_4.z, P_4.x);
	//Calculate angle from local horizontal to vehicle longitudinal axis (equation 5.5.12)
	alpha_LH = phi_SO + phi_P + PI05;
	//Sine and cosines from local horizontal to vehicle longitudinal axis (equation 5.5.13)
	SPITCH = sin(alpha_LH);
	CPITCH = cos(alpha_LH);
	SYAW = sin_chi_Zit;
	CYAW = cos_chi_Zit;
	ROLLA = CurrentAttitude.x;
}

void LVDC1B::OrbitalGuidance680()
{
	//Track local reference maneuver

	//Calculate sine and cosine of yaw and pitch in target plane
	OrbitalGuidance700();
	//Set up for roll guidance chi
	CommandedAttitude.x = ROLLA;
	//Compute guidance chi's
	ChiComputations(0);
}

void LVDC1B::OrbitalGuidance700()
{
	//Calculate sine of pitch angle in target plane (equation 5.5.4)
	sin_chi_Yit = SPITCH * -Pos4.z / R4 + CPITCH * -Pos4.x / R4;
	//Calculate cosine of pitch angle in target plane (equation 5.5.4)
	cos_chi_Yit = CPITCH * -Pos4.z / R4 - SPITCH * -Pos4.x / R4;
	//Calculate sine of yaw angle in target plane (equation 5.5.5)
	sin_chi_Zit = SYAW;
	//Calculate cosine of yaw angle in target plane (equation 5.5.5)
	cos_chi_Zit = CYAW;
}

void LVDC1B::OrbitalGuidance860()
{
	//Inertial hold maneuver

	//Set guidance chi's equal to inertial attitude
	CommandedAttitude.x = ROLLA;
	CommandedAttitude.y = SPITCH;
	CommandedAttitude.z = SYAW;

	//Telemeter guidance chi's
	ChiComputations(1);
}

void LVDC1B::OrbitalGuidance940()
{
	//Special local reference track

	//First pass?
	if (FSLR)
	{
		FSLR = false;
		OrbitalGuidance700();
	}
	else
	{
		//Telemeter guidance chi's
		ChiComputations(1);
	}
}

void LVDC1B::MinorLoopSupport()
{
	//fprintf(lvlog, "MINOR LOOP SUPPORT\r\n");

	ModeCode25[MC25_SMCActive] = true;

	//Limit commanded yaw gimbal
	if (CommandedAttitude.z < -45 * RAD && CommandedAttitude.z >= -180 * RAD)
	{
		CommandedAttitude.z = -45 * RAD;
		ModeCode25[MC25_SMCActive] = false;
	}
	if (CommandedAttitude.z > 45 * RAD && CommandedAttitude.z <= 180 * RAD)
	{
		CommandedAttitude.z = 45 * RAD;
		ModeCode25[MC25_SMCActive] = false;
	}

	//Compute delta chi values
	DChi = CommandedAttitude - PCommandedAttitude;
	for (int i = 0; i < 3; i++)
	{
		if (DChi.data[i] > PI)
		{
			DChi.data[i] -= PI2;
		}
		else if (DChi.data[i] < -PI)
		{
			DChi.data[i] += PI2;
		}
	}
	//fprintf(lvlog, "DChi = %f %f %f\r\n", DChi.x*DEG, DChi.y*DEG, DChi.z*DEG);

	DChi_apo.x = (DChi.x) / (MLR*DT_N);
	DChi_apo.y = (DChi.y) / (MLR*DT_N);
	DChi_apo.z = (DChi.z) / (MLR*DT_N);
	//fprintf(lvlog, "DChi_apo = %f %f %f\r\n", DChi_apo.x*DEG, DChi_apo.y*DEG, DChi_apo.z*DEG);

	//Limit delta chi's

	//Magnitude limit
	if (abs(DChi_apo.x) > MSLIM1)
	{
		fprintf(lvlog, "Roll magnitude limit. Magnitude = %f Limit = %f\r\n", DChi_apo.x*DEG, MSLIM1*DEG);
		ModeCode25[MC25_SMCActive] = false;
		if (DChi_apo.x > 0)
		{
			DChi_apo.x = MSLIM1;
		}
		else
		{
			DChi_apo.x = -MSLIM1;
		}
	}
	if (abs(DChi_apo.y) > MSLIM2)
	{
		fprintf(lvlog, "Pitch magnitude limit. Magnitude = %f Limit = %f\r\n", DChi_apo.y*DEG, MSLIM2*DEG);
		ModeCode25[MC25_SMCActive] = false;
		if (DChi_apo.y > 0)
		{
			DChi_apo.y = MSLIM2;
		}
		else
		{
			DChi_apo.y = -MSLIM2;
		}
	}
	if (abs(DChi_apo.z) > MSLIM2)
	{
		fprintf(lvlog, "Yaw magnitude limit. Magnitude = %f Limit = %f\r\n", DChi_apo.z*DEG, MSLIM2*DEG);
		ModeCode25[MC25_SMCActive] = false;
		if (DChi_apo.z > 0)
		{
			DChi_apo.z = MSLIM2;
		}
		else
		{
			DChi_apo.z = -MSLIM2;
		}
	}

	//Compute A1-A5
	//Gimbal-to-Body Transformation
	Chi_xp_apo = PCommandedAttitude.x + DChi_apo.x*MLR*DT_N;
	Chi_zp_apo = PCommandedAttitude.z + DChi_apo.z*MLR*DT_N;
	//fprintf(lvlog, "Gimbal-to-Body Transformation. Chi_xp_apo = %f Chi_zp_apo = %f\r\n", Chi_xp_apo*DEG, Chi_zp_apo*DEG);
	theta_xa = 0.5*(Chi_xp_apo + CurrentAttitude.x);
	if (abs(Chi_xp_apo - CurrentAttitude.x) > PI05)
	{
		theta_xa += PI;
	}
	if (theta_xa >= PI2)
	{
		theta_xa -= PI2;
	}
	theta_za = 0.5*(Chi_zp_apo + CurrentAttitude.z);
	if (abs(Chi_zp_apo - CurrentAttitude.z) > PI05)
	{
		theta_za += PI;
	}
	if (theta_za >= PI2)
	{
		theta_za -= PI2;
	}
	//fprintf(lvlog, "Gimbal-to-Body Transformation. theta_xa = %f theta_za = %f\r\n", theta_xa*DEG, theta_za*DEG);
	A1 = cos(theta_xa) * cos(theta_za);
	A2 = sin(theta_xa);
	A3 = sin(theta_za);
	A4 = sin(theta_xa) * cos(theta_za);
	A5 = cos(theta_xa);
	//fprintf(lvlog, "Euler Correction. A1 = %f A2 = %f A3 = %f A4 = %f A5 = %f\r\n", A1, A2, A3, A4, A5);
}

// ***************************
// DS20150720 LVDC++ ON WHEELS
// ***************************

// Constructor
LVDCSV::LVDCSV(LVDA &lvd) : LVDC(lvd)
{
	int x=0;
	Initialized = false;					// Reset cloberness flag

	// Zeroize
	// chars
	FSPFileName[0] = '\0';
	// booleans
	alpha_D_op = false;
	BOOST = false;
	CHIBARSTEER = false;
	CountPIPA = false;
	DFIL1 = false;
	DFIL3 = false;
	DFSMC = false;
	DFTBCEP = false;
	DFTUP = false;
	Direct_Ascent = false;
	directstagereset = false;
	FASE = false;
	first_op = false;
	FCLS4 = false;
	FSSAC = false;
	FTADV = false;
	TerminalConditions = false;
	ImpactBurnEnabled = false;
	ImpactBurnInProgress = false;
	GATE = false;
	GATE0 = false;
	GATE1 = false;
	GATE2 = false;
	GATE5 = false;
	HSL = false;
	init = false;
	i_op = false;
	KSSINDXTB7A = KSSINDXTB6A = KSSINDXTB6B = KSSINDXTB6C = KSSINDXTB3A = KSSINDXTB5A = KSSINDXTB5B = KSSINDXSIVA = KSSINDXSIVB = KSSINDXS4C1 = KSSINDXGSS = 0;
	KSSINDXSBLO = KSSINDXSBHI = KSSINDXSBOM = KSSINDXECSV = KSSINDXECS1 = KSSINDXGAIN = KSSINDXTB6D = KSSINDXALU = 0;
	MRS = false;
	for (int i = 0;i < 21;i++)
	{
		NISTAT[i] = false;
	}
	poweredflight = false;
	ROT = false;
	S2_BURNOUT = false;
	S2_IGNITION = false;
	S4B_IGN = false;
	for (int i = 0;i < 11;i++)
	{
		T2STAT[i] = false;
	}
	theta_N_op = false;
	TU = false;
	TU10 = false;
	// Integers
	DGST2 = 0;
	DVASW = 0;
	DVP = 0;
	DVSST = 0;
	GST1M = 0;
	IGMCycle = 0;
	LVDC_Stop = 0;
	LVDC_Timebase = 0;
	MLM = 0;
	SSM = 0;
	SST1PTR = 0;
	SST2PTR = 0;
	Timer1Counter = 0;
	Timer2Counter = 0;
	T_EO2 = 0;
	for (int i = 0;i < 9;i++)
	{
		SSTTBPTR[i] = 0;
	}
	UP = 0;
	VASPI = 0;
	VHSTW1 = 0;
	VHSTW2 = 0;
	VSC30 = VSC31 = VSC10 = VSC11 = 0;
	VSNA1 = 0;
	VSNA2 = 0;
	AttitudeManeuverState = 0;
	// doubles
	a = 0;
	a_1 = 0;
	a_2 = 0;
	A1 = 0;
	A2 = 0;
	A3 = 0;
	A4 = 0;
	A5 = 0;
	alpha_1 = 0;
	alpha_2 = 0;
	alpha_D = 0;
	ART = 0;
	Azimuth = 0;
	Azo = 0;
	Azs = 0;
	B_11 = 0;
	B_21 = 0;
	B_12 = 0;
	B_22 = 0;
	C_2 = 0;
	C_4 = 0;
	C_3 = 0;
	Cf = 0;
	CG = 0;
	Chi_xp_apo = 0.0;
	Chi_zp_apo = 0.0;
	cos_chi_Yit = 0;
	cos_chi_Zit = 0;
	cos_sigma = 0.0;
	Ct = 0;
	Ct_o = 0;
	D = 0;
	d2 = 0;
	d_A = 0;
	ddot_zeta_GT = 0;
	ddot_xi_GT = 0;
	DEC = 0;
	deta = 0;
	for (int i = 0;i < 11;i++)
	{
		DLTTL[i] = 0.0;
	}
	dxi = 0;
	dot_dxi = 0;
	dot_deta = 0;
	dot_dzeta = 0;
	dot_dxit = 0;
	dot_detat = 0;
	dot_dzetat = 0;
	dotM_1 = 0;
	dotM_2 = 0;
	dotM_2R = 0;
	dotM_3 = 0;
	dotM_3R = 0;
	dot_phi_1 = 0;
	dot_phi_T = 0;
	dot_zeta_T = 0;
	dot_xi_T = 0;
	dot_eta_T = 0;
	dT_3 = 0;
	dT_4 = 0;
	dt_c = 0;
	dT_cost = 0;
	dT_F = 0;
	dt_g = 0;
	dt_LET = 0;
	dT_LIM = 0;
	dtt_1 = 0;
	dtt_2 = 0;
	dTt_4 = 0;
	dV = 0;
	dV_B = 0;
	DV2TG = 0;
	DVACT = 0;
	DVCA[0] = DVCA[1] = 0.0;
	DVRTC = 0;
	DVTGB = 0;
	e = 0;
	e_N = 0;
	eps_1 = 0;
	eps_2 = 0;
	eps_3 = 0;
	eps_4 = 0;
	eps_1R = 0;
	eps_2R = 0;
	eps_3R = 0;
	eps_4R = 0;
	EPTINDX = 0;
	for (int i = 0;i < 131;i++)
	{
		EPTTIM[i] = 0.0;
		EPTABLE[i] = 0;
	}
	f = 0;
	F = 0;
	Fm = 0;
	for(x=0; x < 5; x++){
		int y=0;
		for(y=0; y < 5; y++){
			Fx[x][y] = 0;
		}
		hx[0][y] = 0;
		hx[1][y] = 0;
		hx[2][y] = 0;
		Drag_Area[x] = 0;
	}
	for(x=0; x < 7; x++){
		fx[x] = 0;
		fxt[x] = 0;
		gx[x] = 0;
		gxt[x] = 0;
	}
	for (x = 0; x < 6; x++) {
		Rho[x] = 0;
	}
	for (x = 0; x < 2; x++)
	{
		int y;
		TABLE15[x].alphaS_TS = 0;
		TABLE15[x].beta = 0;
		TABLE15[x].f = 0;
		TABLE15[x].T_ST = 0;
		TABLE15[x].R_N = 0;
		TABLE15[x].T2IR = 0;
		TABLE15[x].T3PR = 0;
		TABLE15[x].TAU3R = 0;
		TABLE15[x].dV_BR = 0;
		for (y = 0; y < 15; y++)
		{
			TABLE15[x].target[y].alpha_D = 0;
			TABLE15[x].target[y].cos_sigma = 0;
			TABLE15[x].target[y].C_3 = 0;
			TABLE15[x].target[y].DEC = 0;
			TABLE15[x].target[y].e_N = 0;
			TABLE15[x].target[y].RAS = 0;
			TABLE15[x].target[y].t_D = 0;
		}
	}
	G_T = 0;
	H = 0;
	h = 0;
	h_1 = 0;
	h_2 = 0;
	Inclination = 0;
	J = 0;
	J_1 = 0;
	J_2 = 0;
	J_3 = 0;
	J_12 = 0;
	Jt_3 = 0;
	J_Y = 0;
	J_P = 0;
	K_1 = 0;
	K_2 = 0;
	K_3 = 0;
	K_4 = 0;
	K_5 = 0;
	KCSSK = 0;
	K_D = 0;
	K_P1 = 0;
	K_P2 = 0;
	KSSB1 = KSSB2 = KSSB3 = KSSB5 = KSSB8 = 0;
	K_T3 = 0;
	K_Y1 = 0;
	K_Y2 = 0;
	K_Y = 0;
	K_P = 0;
	KSCLNG = 0;
	D_P = 0;
	D_Y = 0;
	L_1 = 0;
	L_2 = 0;
	L_3 = 0;
	dL_3 = 0;
	Lt_3 = 0;
	L_12 = 0;
	L_P = 0;
	L_Y = 0;
	Lt_Y = 0;
	LVDC_TB_ETime = 0;
	DT_N = 0.0;
	DT_N1 = DT_N2 = DT_N3 = DT_N4 = DT_N5 = DT_N6 = 0.0;
	for (x = 0;x < 9;x++)
	{
		MFK[x] = 0.0;
		S4MFK[x] = 0.0;
	}
	for (x = 0;x < 5;x++)
	{
		MF[x] = 0.0;
	}
	for (x = 0;x < 4;x++)
	{
		MFSArr[x] = 0.0;
	}
	MFS = 0.0;
	MF0 = 0.0;
	MF1 = 0.0;
	MIR = 0.0;
	MLD = 0.0;
	MLR = 0.0;
	MLT = 0.0;
	MOR = 0.0;
	MS25DT = 0.0;
	MS04DT = 0.0;
	MSK16 = 0;
	MSK5 = 0;
	MSK6 = 0;
	MSLIM1 = MSLIM2 = 0.0;
	NUPTIM = 0.0;
	mu = 0;
	omega_E = 0;
	p = 0;
	P_1 = 0;
	P_2 = 0;
	P_3 = 0;
	P_12 = 0;
	PHI = 0;
	PHIP = 0;
	R_L = 0;
	phi_T = 0;
	Q_1 = 0;
	Q_2 = 0;
	Q_3 = 0;
	Q_12 = 0;
	Q_Y = 0;
	Q_P = 0;
	R = 0;
	RAS = 0;
	rho = 0;
	rho_c = 0;
	ROV = 0;
	ROVR = 0;
	ROVs = 0;
	R_STA = 0;
	R_T = 0;
	S_1 = 0;
	S_2 = 0;
	S_3 = 0;
	S_12 = 0;
	S_P = 0;
	S_Y = 0;
	sinceLastCycle = 0;
	sin_chi_Yit = 0;
	sin_chi_Zit = 0;
	sin_gam = 0;
	cos_gam = 0;
	SMCG = 0;
	SMCY = 0.0;
	SMCZ = 0.0;
	S = 0;
	S_34 = 0;
	P = 0;
	P_34 = 0;
	T_0 = 0;
	t_1 = 0;
	T_1 = 0;
	T_1c = 0;
	t_2 = 0;
	T_2 = 0;
	T_2R = 0;
	t_21 = 0;
	t_3 = 0;
	T_3 = 0;
	t_3i = 0;
	t_4 = 0;
	T_4N = 0;
	t_5 = 0;
	TMEFRZ = 0;
	T_ar = 0;
	TAS = 0;
	tau1 = 0;
	tau2 = 0;
	tau2N = 0;
	tau3 = 0;
	tau3N = 0;
	tau3R = 0;
	t_B1 = 0;
	TB1 = 0;
	t_B2 = 0;
	TB2 = 0;
	t_B3 = 0;
	TB3 = 0;
	t_B4 = 0;
	TB4 = 0;
	TB4a = 0;
	TB5 = 0;
	TB6 = 0;
	TB7 = 0;
	TB8 = 0;
	TBA = 0;
	TBL = 0;
	T_c = 0;
	tchi_y_last = 0;
	tchi_p_last = 0;
	tchi_y = 0;
	tchi_p = 0;
	t_clock = 0;
	T_CO = 0;
	T_EO1 = 0;
	T_GO = 0;
	theta_N = 0;
	theta_R = 0;
	theta_xa = 0.0;
	theta_za = 0.0;
	TI = 0.0;
	TI5F3 = 0.0;
	TI5F4 = 0.0;
	TI5F5 = 0.0;
	T7M10 = 0.0;
	T7M11 = 0.0;
	T_ImpactBurn = 0;
	dT_ImpactBurn = 0;
	T_L = 0.0;
	TMM = 0;
	TMR = 0;
	T_RP = 0;
	T_S1 = 0;
	T_S2 = 0;
	T_S3 = 0;
	TS4BS = 0;
	T_SON = 0.0;
	T_ST = 0;
	T_T = 0;
	t_TB8Start = 0;
	Tt_3 = 0;
	Tt_3R = 0;
	Tt_T = 0;
	TVRATE = 0;
	U_1 = 0;
	U_2 = 0;
	U_3 = 0;
	U_12 = 0;
	gamma_T = 0;
	V = 0;
	V_ex1 = 0;
	V_ex2 = 0;
	V_ex2R = 0;
	V_ex3 = 0;
	V_ex3R = 0;
	V_i = 0;
	V_0 = 0;
	V_1 = 0;
	V_2 = 0;
	VATR4 = 0;
	VATRR = 0;
	for (x = 0;x < 7;x++)
	{
		VENTA[x] = 0.0;
	}
	VCCYA = VCCZA = 0.0;
	VGBIA = 0;
	VOLD = 0;
	V_S2T = 0;
	VSC32 = VSC12 = 0;
	VSSRT = 0;
	VSSTM = 0;
	VSSW = 0;
	VSTGO = 0;
	V_T = 0;
	V_TC = 0;
	for (x = 0;x < 6;x++)
	{
		VTIM[x] = 0.0;
	}
	VTOLD = 0.0;
	xi_T = 0;
	X_1 = 0;
	X_2 = 0;
	X_S1 = 0;
	X_S2 = 0;
	X_S3 = 0;
	Xtt_y = 0;
	Xtt_p = 0;
	X_Zi = 0;
	X_Yi = 0;
	Y_u = 0;
	// VECTOR3
	AttitudeError = _V(0,0,0);
	AttitudeErrorOld = _V(0, 0, 0);
	for (int i = 0;i < 11;i++)
	{
		C_A[i] = _V(0, 0, 0);
	}
	CommandedAttitude = _V(0,0,0);
	CurrentAttitude = _V(0,0,0);
	DChi = DChi_apo = _V(0, 0, 0);
	DeltaAtt = _V(0, 0, 0);
	ddotG_act = _V(0,0,0);
	ddotG_last = _V(0,0,0);
	ddotS = _V(0, 0, 0);
	DDotXEZ_G = _V(0,0,0);
	DotG_act = _V(0,0,0);
	DotG_last = _V(0,0,0);
	DotM_act = _V(0,0,0);
	DotM_last = _V(0,0,0);
	DotS = _V(0,0,0);
	DotXEZ = _V(0,0,0);
	ddotM_act = _V(0, 0, 0);
	DVAC = _V(0, 0, 0);
	PCommandedAttitude = _V(0,0,0);
	PosA = _V(0, 0, 0);
	Pos4 = _V(0,0,0);
	Pos_Nav = _V(0, 0, 0);
	PosS = _V(0,0,0);
	PosXEZ = _V(0,0,0);
	R_OG = _V(0, 0, 0);
	Sbar = _V(0, 0, 0);
	TargetVector = _V(0, 0, 0);
	T_P = _V(0, 0, 0);
	Vel_Nav = _V(0, 0, 0);
	W_ES = _V(0, 0, 0);
	VOAC = _V(0, 0, 0);
	XLunarAttitude = _V(0,0,0);
	XLunarSlingshotAttitude = _V(0, 0, 0);
	XLunarCommAttitude = _V(0, 0, 0);
	// MATRIX3
	MX_A = _M(0,0,0,0,0,0,0,0,0);
	MX_B = _M(0,0,0,0,0,0,0,0,0);
	MX_G = _M(0,0,0,0,0,0,0,0,0);
	MX_K = _M(0,0,0,0,0,0,0,0,0);
	MX_phi_T = _M(0,0,0,0,0,0,0,0,0);
	MGA = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	MSA = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	MEG = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	//TABLE15 and TABLE 25
	TABLE15[0] = {};
	TABLE15[1] = {};
	tgt_index = 0;
	SIICenterEngineCutoff = false;
	FixedAttitudeBurn = false;
}

// Setup
void LVDCSV::Init(){
	if(Initialized == true){ 
		fprintf(lvlog,"init called after init, ignored\r\n");
		fflush(lvlog);
		return;
	}

	sprintf(FSPFileName, "Config\\ProjectApollo\\Saturn V Default Flight Sequence Program.txt");

	//presettings in order of boeing listing for easier maintainece
	//GENERAL
	C_3 = -60731530.2; // Stored as twice the etc etc.
	  //C_3 = -60903382.7388059;
	Direct_Ascent = false;					// flag for selecting direct ascent targeting; not used normally
	GATE = false;							// 'chi-freeze-gate': freezes steering commands when true
	GATE0 = false;							// allows single pass through restart preparations (TB6 start) when false
	GATE1 = false;							// allows single pass through out-of-orbit targeting calculations when false
	GATE2 = false;							// logic switch for first or second opportunity out-of-orbit targeting; false for first
	GATE5 = false;							// allows single pass through HSL initialization when false
	TB1 = TB2 = TB3 = TB4 = TB4a = TB5 = TB6 = TB7 = TB8 = 100000; //LVDC's elapsed timebase times; set to 0 when resp. TB starts
	TU = false;								// flag indicating target update has been received from ground
	TU10 = false;							// flag indicating 10-parameter target update has been received
	UP = 0;									// switching variable for Tt_t parameter update
	alpha_D_op = true;
	i_op = true;							// flag for selecting method of EPO inclination calculation
	theta_N_op = true;						// flag for selecting method of EPO descending node calculation
	TerminalConditions = true;
	ImpactBurnEnabled = false;
	ImpactBurnInProgress = false;

	//PRE_IGM GUIDANCE
	B_11 = -1.2;							// Coefficients for determining freeze time after S1C engine failure
	B_12 = 67.2;							// dto.
	B_21 = -0.77;							// dto.
	B_22 = 50.9;							// dto.

	Drag_Area[0] = 11.079923;
	Drag_Area[1] = 0.17954281;
	Drag_Area[2] = -2.1771971;
	Drag_Area[3] = -0.28074902;
	Drag_Area[4] = -5.8764139;

	//Apollo 17 vent accelerations
	VTIM[0] = 700.0;
	VTIM[1] = 2300.0;
	VTIM[2] = 3800.0;
	VTIM[3] = 7800.0;
	VTIM[4] = 10800.0;
	VTIM[5] = 14300.0;
	VENTA[0] = 0.00117;
	VENTA[1] = 0.000778;
	VENTA[2] = 0.000605;
	VENTA[3] = 0.00047;
	VENTA[4] = 0.00035;
	VENTA[5] = 0.000361;
	VENTA[6] = 0.000325;

	// PITCH POLYNOMIAL (Apollo 11)
	Fx[1][0] = 0.104707442e1;
	Fx[1][1] = -0.147669484;
	Fx[1][2] = 0.824109168e-2;
	Fx[1][3] = -0.403816898e-3;
	Fx[1][4] = 0.338751980e-5;
	Fx[2][0] = -0.687116080e1;
	Fx[2][1] = 0.664009046;
	Fx[2][2] = -0.231195092e-1;
	Fx[2][3] = 0.137310354e-3;
	Fx[2][4] = -0.124330390e-6;
	Fx[3][0] = -0.663318417e1;
	Fx[3][1] = 0.810101295;
	Fx[3][2] = -0.299134054e-1;
	Fx[3][3] = 0.239665250e-3;
	Fx[3][4] = -0.635979551e-6;
	Fx[4][0] = 0.425287982e2;
	Fx[4][1] = -0.125455452e1,
	Fx[4][2] = 0.237693395e-2,
	Fx[4][3] = 0.163508094e-4;
	Fx[4][4] = -0.587090259e-7;
	t_1 = 13;								// Backup timer for Pre-IGM pitch maneuver
	t_2 = 6;								// Time to initiate pitch freeze for S1C engine failure
	t_3 = 60;								// Constant pitch freeze for S1C engine failure prior to t_2
	t_3i = 0;								// Clock time at S4B ignition
	t_4 = 38;								// Upper bound of validity for first segment of pitch freeze
	t_5 = 66;								// Upper bound of validity for second segment of pitch freeze
	T_ar = 160.0;							// S1C Tilt Arrest Time
	T_S1 = 33.6;							// Pitch Polynomial Segment Times
	T_S2 = 68.6;							// dto.
	T_S3 = 95.6;							// dto.
	T_EO1 = 0;								// switching constant for SIC engine-out handling
	T_EO2 = 0;								// allows single pass through IGM engine-out presettings when 0
	dT_F=0;									// Period of frozen pitch in S1C
	dt_LET = 35.1;							// Nominal time between SII ign and LET jet
	t_TB8Start = 3600.0;
	//IGM BOOST TO ORBIT
	Ct = 0;
	Ct_o = 10;
	Cf = 0.087996;
	// Inclination from azimuth polynomial
	fx[0] = 32.55754;  fx[1] = -15.84615; fx[2] = 11.64780; fx[3] = 9.890970;
	fx[4] = -5.111430; fx[5] = 0;         fx[6] = 0;
	// Descending Node Angle from azimuth polynomial
	gx[0] = 123.1935; gx[1] = -55.06485; gx[2] = -35.26208; gx[3] = 26.01324;
	gx[4] = -1.47591; gx[5] = 0;         gx[6] = 0;	
	// Launch azimuth from time polynomial
	hx[0][0] = 72.0;	hx[0][1] = 0.0;	hx[0][2] = 0.0;	hx[0][3] = 0.0;	hx[0][4] = 0.0;
	hx[1][0] = 72.0;	hx[1][1] = 0.0;	hx[1][2] = 0.0;	hx[1][3] = 0.0;	hx[1][4] = 0.0;
	hx[2][0] = 72.0;	hx[2][1] = 0.0;	hx[2][2] = 0.0;	hx[2][3] = 0.0;	hx[2][4] = 0.0;
	// Air density polynomial
	Rho[0] = 0.179142e-6;	Rho[1] = -0.37213949e-11;	Rho[2] = 0.31057886e-16;
	Rho[3] = -0.12962178e-21;	Rho[4] = 0.2698641e-27;	Rho[5] = -0.2238826e-33;

	// Out-of-orbit parameters
	TABLE15[0].alphaS_TS = 14.2691472;
	TABLE15[0].beta = 61.89975;
	TABLE15[0].f = 14.26968;
	TABLE15[0].R_N = 6575100;
	TABLE15[0].T_ST = 15000.0;
	TABLE15[0].target[0].cos_sigma = 9.958662e-1;
	TABLE15[0].target[1].cos_sigma = 9.958662e-1;
	TABLE15[0].target[0].C_3 = -1.418676e6;
	TABLE15[0].target[1].C_3 = -1.418676e6;
	TABLE15[0].target[0].DEC = -26.646912;
	TABLE15[0].target[1].DEC = -26.646912;
	TABLE15[0].target[0].e_N = 0.9765500;
	TABLE15[0].target[1].e_N = 0.9765500;
	TABLE15[0].target[0].RAS = -114.382494;
	TABLE15[0].target[1].RAS = -114.382494;
	TABLE15[0].target[0].t_D = 0.0;
	TABLE15[0].target[1].t_D = 1000.0;
	TABLE15[0].T2IR = 10.0;
	TABLE15[1].T2IR = 10.0;
	TABLE15[0].T3PR = 310.8243;
	TABLE15[1].T3PR = 308.6854;
	TABLE15[0].TAU3R = 684.5038;
	TABLE15[1].TAU3R = 682.1127;
	TABLE15[0].dV_BR = 3.7; //Estimated value for S-IVB in NASSP
	TABLE15[1].dV_BR = 3.7;

	MRS = false;						// MR Shift
	dotM_1 = 1221.1489;					// Mass flowrate of S2 from approximately LET jettison to second MRS
	dotM_2 = 950.4154;					// Mass flowrate of S2 after second MRS
	dotM_2R = 215.2241;
	dotM_3 = 208.7674;					// Mass flowrate of S4B during first burn
	dotM_3R = 215.2241;
	ROT = false;
	ROTR = true;
	dV_B = 1.7; // Velocity cutoff bias for orbital insertion
	dV_BR = 3.7;
	MSLIM1 = 0.06*RAD;
	MSLIM2 = 0.04*RAD;
	MSK6 = MSK16 = 15.3*RAD;
	MSK5 = 0.48*RAD;
	rho = 0;
	rho_c = 0.5e-7;
	ROV = 1.5;
	ROVs = 1.5;
	ROVR = -0.4;
	PHI = 28.6082888*RAD;
	PHIP = 28.6082888*RAD;
	R_L = 6373418.5;
	KSCLNG = -80.6041140*RAD;
	R_N = 6575100;
	SMCG = 0.05;
	TS4BS = 13.5;
	T_c = 8; // T_c = 6.5; 					// Coast time between S2 burnout and S4B ignition
	T_1 = 286.2;							// Time left in first-stage IGM
	T_2 = 53.5;								// Time left in second and fourth stage IGM
	T_2R = 10.0;
	T_3 = 0;								// Time left in third and fifth stage IGM
	T_1c = T_1 + T_2 + T_c;					// Sum of the burn times of IGM first, second, and coast guidance stages
	T_4N = 135.6;							// Nominal time of S4B first burn
	Tt_3 = T_4N;							// Estimated third or fifth stage burn time
	Tt_3R = 315.0;//340.0;
	Tt_T = T_1c + Tt_3;						// Time-To-Go computed using Tt_3
	t_B1 = 4;								// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	t_B2 = 0;
	t_B3 = 0;								// Time from second S2 MRS signal
	t_B4 = 0;
	t_D = 0;
	t_D0 = 0.0;
	t_D1 = 0.0;
	t_D2 = 10984.2;
	t_D3 = 16503.1;
	t_DS0 = 0.0;
	t_DS1 = 10984.2;
	t_DS2 = 16503.1;
	t_DS3 = 0.0;
	theta_EO = 0.0;
	T_ImpactBurn = 0;
	dT_ImpactBurn = 0;
	T_LO = 0.0;
	t_SD1 = 10984.2;
	t_SD2 = 5518.9;
	t_SD3 = 1233.6;
	//dt: not set; dependend on cycle time
	dt_g = 1.0;
	dT_LIM = 90;							// Limit to dT_4;
	V_ex1 = 4153.941218;//4148.668555;
	V_ex2 = 4221.827032;//4158.852692;
	V_ex2R = 4221.827032;//4228.02;
	V_ex3 = 4184.41823;//4130.010682;
	V_ex3R = 4184.41823;//4130.010682;//4193.05;
	V_S2T = 7007.18;
	V_TC = 300;
	eps_1 = 0;								// IGM range angle calculation selection
	eps_1R = 500.0;
	eps_2 = 32;								// Time to begin chi bar steering
	eps_2R = 30.0;
	eps_3 = 10000;							// Terminal condition freeze time
	eps_3R = 30.0;
	eps_4 = 3;								// Time to enable HSL loop & chi freeze
	eps_4R = 3.59;
	mu = 398603200000000;					// Product of G and Earth's mass
	tau2 = 308.95;							// Time to consume all fuel between MRS and S2 Cutoff
	tau2N = 721;
	tau3 = 709.7853;//748.7;							// Time to consume all fuel of SIVB
	tau3R = tau3 - T_4N;//576;
	tau3N = tau3;							// artificial tau3
	omega_E = TVRATE = 2.32115e-5*PI;
	//rate limits: set in pre-igm
	alpha_1 = 0;							// orbital guidance pitch
	alpha_2 = 0;							// orbital guidance yaw
	K_D = 0.131764e-3;
	K_P1 = 0.0;//4.3 * RAD;					// restart attitude coefficients
	K_P2 = 0;
	K_Y1 = 0;
	K_Y2 = 0;
	K_T3 = -.274;
	K_pc = 75.0;
	first_op = true;
	//IGM out-of-orbit
	T_RP = 0;
	T_ST = 0;
	alpha_TS = 0;
	beta = 0;
	ART = 0;

	//Not in boeing doc, but needed for nav:
	a = 6.373338e6;							// earth's equatorial radius
	J = 1.62345e-3;							// first coefficient of earth's gravity
	D = 0.7875e-5;
	H = 0.575e-5;

	//'real' software variable, i.e. those are computed at runtime
	// Software flags
	poweredflight = false;
	HSL=false;								// High-Speed Loop flag
	BOOST=false;							// Boost To Orbit
	S4B_IGN=false;							// SIVB Ignition
	S4B_REIGN = false;						// SIVB Reignition
	S2_IGNITION=false;						// S2 Ignition
	S2_BURNOUT=false;						// SII Burn Out
	tau1 = 1.0;								// Time to consume all fuel before S2 MRS
	Fm=0;									// sensed total accel
	Inclination=0;							// Inclination
	theta_N = 0;
	cos_sigma = 0.0;
	Azo=72.0; Azs=36.0;							// Variables for scaling the -from-azimuth polynomials
	CommandedAttitude=_V(0,0,0);			// Commanded Attitude (RADIANS)
	PCommandedAttitude=_V(0,0,0);			// Previous Commanded Attitude (RADIANS)
	CurrentAttitude=_V(0,0,0);				// Current Attitude   (RADIANS)
	F=0;									// Force in Newtons, I assume.	
	K_Y=0; K_P=0; D_P=0; D_Y=0;				// Intermediate variables in IGM
	P_1=0; P_2=0; P_3=0; P_12=0;	
	L_1=0; L_2=0; L_3=0; dL_3=0; Lt_3=0; L_12=0; L_P=0; L_Y=0; Lt_Y=0;
	J_1=0; J_2=0; J_3=0; J_12=0; Jt_3=0; J_Y=0; J_P=0; 
	S_1=0; S_2=0; S_3=0; S_12=0; S_P=0; S_Y=0; 
	U_1=0; U_2=0; U_3=0; U_12=0; 
	Q_1=0; Q_2=0; Q_3=0; Q_12=0; Q_Y=0; Q_P=0; 
	d2=0;
	f=0;									// True anomaly of the predicted cutoff radius vector
	e=0;									// Eccentricity of the transfer ellipse
	e_N = 0;
	C_2=0; C_4=0;							// IGM coupling terms for pitch steering
	p=0;									// semilatus rectum of terminal ellipse
	K_1=0; K_2=0; K_3=0; K_4=0;				// Correction to chi-tilde steering angles, K_i
	K_5=0;									// IGM terminal velocity constant
	R=0;									// Instantaneous Radius Magnitude
	R_T=6563366.0;							// Desired terminal radius
	V=0;									// Instantaneous vehicle velocity
	V_T=7793.0429;							// Desired terminal velocity
	V_i=0; V_0=0; V_1=0; V_2=0;				// Parameters for cutoff velocity computation
	gamma_T=0;								// Desired terminal flight-path angle
	MX_A=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to equatorial
	MX_B=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from equatorial to orbital coordinates
	MX_G=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to orbital
	MX_K=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to terminal
	MX_phi_T=_M(0,0,0,0,0,0,0,0,0);			// Matrix made from phi_T
	phi_T=0;								// Angle used to estimate location of terminal radius in orbital plane
	Pos4=_V(0,0,0);							// Position in the orbital reference system
	PosS=_V(0,0,0);							// Position in the earth-centered plumbline system. SPACE-FIXED.
	DotS=_V(0,0,0);							// VELOCITY in the earth-centered plumbline system
	DotM_act=_V(0,0,0);						// actual sensed velocity from platform
	ddotG_act=_V(0,0,0);					// actual computed acceleration from gravity
	DotG_act=_V(0,0,0);						// actual computed velocity from gravity
	DotM_last=_V(0,0,0);					// last sensed velocity from platform
	ddotG_last=_V(0,0,0);					// last computed acceleration from gravity
	DotG_last=_V(0,0,0);					// last computed velocity from gravity
	alpha_D=0;								// Angle from perigee to DN vector
	alpha_D_op=true;						// Option to determine alpha_D or load it
	G_T= -9.255;							// Magnitude of desired terminal gravitational acceleration
	xi_T=0;									// Desired position component in the terminal reference system
	PosXEZ=_V(0,0,0);						// Position components in the terminal reference system
	DotXEZ=_V(0,0,0);						// Instantaneous something
	deta=0; dxi=0;							// Position components to be gained in this axis
	dT_3=0;									// Correction to third or fifth stage burn time
	dT_4=0;									// Difference between nominal and actual 1st S4B burn time
	dTt_4=0;								// Limited value of above
	T_T=0;									// Time-To-Go computed using T_3
	X_1 = 0;
	X_2 = 0;
	h = 0;
	h_1 = 1.5e5;
	h_2 = 3.0e5;
	
	tchi_y=0; tchi_p=0;						// Angles to null velocity deficiencies without regard to terminal data
	dot_zeta_T=0; dot_xi_T=0; dot_eta_T=0;	// I don't know.
	ddot_zeta_GT=0; ddot_xi_GT=0;
	DDotXEZ_G=_V(0,0,0);					// ???							
	dot_dxit=0; dot_detat=0; dot_dzetat=0; 	// Intermediate velocity deficiency used in time-to-go computation
	dot_dxi=0; dot_deta=0; dot_dzeta=0; 	// More Deltas
	Xtt_y=0; Xtt_p=0; 						// IGM computed steering angles in terminal system
	X_S1=0; X_S2=0; X_S3=0; 				// Direction cosines of the thrust vector
	sin_gam=0; cos_gam=0;					// Sine and cosine of upsilon (flight-path angle)
	dot_phi_1=0; dot_phi_T=0; 				// ???
	dtt_1=0; dtt_2=0;						// Used in TGO determination
	a_1=0; a_2=0;							// Acceleration terms used to determine TGO
	T_GO=0;									// Time before S4B shutdown
	T_CO=0;									// Predicted time of S4B shutdown, from GRR
	dV=0;
	t_21 = 0;
	TAS=0;
	t_clock = 0;
	DT_N = 1.0;
	DT_N1 = 0.77;
	DT_N2 = 0.75;
	DT_N3 = 1.7;
	DT_N4 = 1.3;
	DT_N5 = 0.98;
	DT_N6 = 1.0;
	MLR = 25.0;
	AttitudeManeuverState = 2;
	MIR = 0.04;
	MOR = 0.1;
	MLD = 0.1;

	XLunarSlingshotAttitude = _V(PI, PI, 0.0);
	XLunarCommAttitude = _V(PI, PI, 0);
	TI5F3 = 2.0*3600.0 + 42.0*60.0;
	TI5F4 = TI5F3 + 100.0*60.0;
	TI5F5 = 99999999.9;
	T7M10 = 900.0;
	T7M11 = 99999999.9;

	//S-II Filter
	MFK[0] = 0.39254895;
	MFK[1] = -0.3135895;
	MFK[2] = 0.0;
	MFK[3] = 0.0;
	MFK[4] = 0.0;
	MFK[5] = 1.56507857;
	MFK[6] = -0.64403642;
	MFK[7] = 0.0;
	MFK[8] = 0.0;

	//S-IVB Filter
	S4MFK[0] = 0.39254895;
	S4MFK[1] = -0.3135895;
	S4MFK[2] = 0.0;
	S4MFK[3] = 0.0;
	S4MFK[4] = 0.0;
	S4MFK[5] = 1.56507857;
	S4MFK[6] = -0.64403642;
	S4MFK[7] = 0.0;
	S4MFK[8] = 0.0;

	MF0 = 0.1521;
	MF1 = 0.1899;

	//For backwards compatibility for TLI
	MF[0] = MF[1] = MF[2] = MF[3] = MF[4] = MFSArr[0] = MFSArr[1] = MFSArr[2] = MFSArr[3] = MFS = MF1;

	//Default Event Table
	EPTABLE[0] = 1;	EPTTIM[0] = 0.0; //Start of Time Base 0 Table
	EPTABLE[1] = 0;	EPTTIM[1] = 16.0;
	EPTABLE[2] = 0; EPTTIM[2] = 17.0;
	EPTABLE[3] = 0; EPTTIM[3] = 17.5;
	EPTABLE[4] = 0; EPTTIM[4] = 0.0;
	EPTABLE[5] = 5; EPTTIM[5] = 0.0; //Start of Time Base 1 Table
	EPTABLE[6] = 6; EPTTIM[6] = 1.0;
	EPTABLE[7] = 0; EPTTIM[7] = 6.0;
	EPTABLE[8] = 8; EPTTIM[8] = 9.0;
	EPTABLE[9] = 9;	EPTTIM[9] = 10.0;
	EPTABLE[10] = 0; EPTTIM[10] = 14.0;
	EPTABLE[11] = 11; EPTTIM[11] = 134.7;
	EPTABLE[12] = 0; EPTTIM[12] = 0.0;
	EPTABLE[13] = 12; EPTTIM[13] = 0.0; //Start of Time Base 2 Table
	EPTABLE[14] = 0; EPTTIM[14] = 0.0;
	EPTABLE[15] = 0; EPTTIM[15] = 18.4;
	EPTABLE[16] = 0; EPTTIM[16] = 27.5;
	EPTABLE[17] = 0; EPTTIM[17] = 0.0;
	EPTABLE[18] = 16; EPTTIM[18] = 0.0; //Start of Time Base 3 Table
	EPTABLE[19] = 17; EPTTIM[19] = 0.0;
	EPTABLE[20] = 18; EPTTIM[20] = 0.0;
	EPTABLE[21] = 19; EPTTIM[21] = 0.0;
	EPTABLE[22] = 20; EPTTIM[22] = 0.0;
	EPTABLE[23] = 21; EPTTIM[23] = 0.0;
	EPTABLE[24] = 22; EPTTIM[24] = 1.4;
	EPTABLE[25] = 23; EPTTIM[25] = 4.4;
	EPTABLE[26] = 24; EPTTIM[26] = 4.4;
	EPTABLE[27] = 25; EPTTIM[27] = 6.7;
	EPTABLE[28] = 26; EPTTIM[28] = 6.7;
	EPTABLE[29] = 27; EPTTIM[29] = 6.7;
	EPTABLE[30] = 28; EPTTIM[30] = 40.6;
	EPTABLE[31] = 29; EPTTIM[31] = 40.6;
	EPTABLE[32] = 30; EPTTIM[32] = 58.6;
	EPTABLE[33] = 31; EPTTIM[33] = 60.6;
	EPTABLE[34] = 32; EPTTIM[34] = 299.0;
	EPTABLE[35] = 33; EPTTIM[35] = 355.0;
	EPTABLE[36] = 34; EPTTIM[36] = 388.5;
	EPTABLE[37] = 0; EPTTIM[37] = 0.0;

	EPTABLE[38] = 35; EPTTIM[38] = 0.0; //Start of Time Base 4 Table
	EPTABLE[39] = 36; EPTTIM[39] = 6.5;
	EPTABLE[40] = 37; EPTTIM[40] = 8.6;
	EPTABLE[41] = 38; EPTTIM[41] = 10.0;
	EPTABLE[42] = 39; EPTTIM[42] = 12.0;
	EPTABLE[43] = 40; EPTTIM[43] = 15.0;
	EPTABLE[44] = 0; EPTTIM[44] = 0.0;
	//TBD
	EPTABLE[55] = 51; EPTTIM[55] = 0.0; //Start of Time Base 5 Table
	EPTABLE[56] = 52; EPTTIM[56] = 10.0;
	EPTABLE[57] = 53; EPTTIM[57] = 20.0;
	EPTABLE[58] = 54; EPTTIM[58] = 100.0;
	EPTABLE[59] = 0; EPTTIM[59] = 0.0;
	//TBD
	EPTABLE[71] = 66; EPTTIM[71] = 0.0; //Start of Time Base 6 Table
	EPTABLE[72] = 67; EPTTIM[72] = 41.0;
	EPTABLE[73] = 68; EPTTIM[73] = 48.0;
	EPTABLE[74] = 69; EPTTIM[74] = 341.3;
	EPTABLE[75] = 70; EPTTIM[75] = 496.7;
	EPTABLE[76] = 71; EPTTIM[76] = 497.3;
	EPTABLE[77] = 72; EPTTIM[77] = 560.0;
	EPTABLE[78] = 73; EPTTIM[78] = 578.6;
	EPTABLE[79] = 74; EPTTIM[79] = 580.3;
	EPTABLE[80] = 75; EPTTIM[80] = 583.0;
	EPTABLE[81] = 76; EPTTIM[81] = 584.0;
	EPTABLE[82] = 77; EPTTIM[82] = 590.0;
	EPTABLE[83] = 0; EPTTIM[83] = 0.0;
	//TBD
	EPTABLE[93] = 87; EPTTIM[93] = 0.0; //Start of Time Base 7 Table
	EPTABLE[94] = 88; EPTTIM[94] = 20.0;
	EPTABLE[95] = 89; EPTTIM[95] = 20.0;
	EPTABLE[96] = 0; EPTTIM[96] = 0.0;
	EPTABLE[97] = 90; EPTTIM[97] = 0.0;
	EPTABLE[98] = 91; EPTTIM[98] = 0.0;
	EPTABLE[99] = 0; EPTTIM[99] = 0.0;
	EPTABLE[100] = 92; EPTTIM[100] = 0.0;
	EPTABLE[101] = 0; EPTTIM[101] = 0.0;
	//TBD
	EPTABLE[107] = 100; EPTTIM[107] = 0.0; //Start of Time Base 8 Table
	EPTABLE[108] = 101; EPTTIM[108] = 3705.0;
	EPTABLE[109] = 0; EPTTIM[109] = 0.0;
	EPTABLE[110] = 102; EPTTIM[110] = 0.0; //Start of Time Base 4A Table
	EPTABLE[111] = 103; EPTTIM[111] = 11.5;
	EPTABLE[112] = 104; EPTTIM[112] = 13.3;
	EPTABLE[113] = 105; EPTTIM[113] = 15.0;
	EPTABLE[114] = 0; EPTTIM[114] = 0.0;
	//TBD

	//By default, inhibit Earth orbit attitude maneuvers
	ModeCode27[MC27_InhibitManeuver3] = true;
	ModeCode27[MC27_InhibitManeuver4] = true;
	ModeCode27[MC27_InhibitManeuver5] = true;

	//Switch selector
	KSSB1 = 0.018;
	KSSB2 = 0.026;
	KSSB3 = 0.017;
	KSSB5 = 0.0256;
	KSSB8 = 0.0108;
	KCSSK = 0.2;

	//Telemetry stations
	C_A[0] = _V(0.87981, -0.47532, -0.00134);
	C_A[1] = _V(0.81395, -0.53253, 0.23214);
	C_A[2] = _V(0.37533, -0.46339, 0.80275);
	C_A[3] = _V(0.39895, 0.13748, 0.90661);
	C_A[4] = _V(0.17931, -0.64631, 0.74171);
	C_A[5] = _V(-0.87993, 0.41883, -0.2243);
	C_A[6] = _V(-0.68462, -0.22874, -0.69208);
	C_A[7] = _V(-0.52869, 0.57948, -0.62023);
	C_A[8] = _V(0.17552, -0.37445, -0.91048);
	C_A[9] = _V(0.65891, -0.57586, -0.48396);
	C_A[10] = _V(0.84918, -0.46168, -0.25641);
	R_STA = 6374932.0;

	// Set up remainder
	LVDC_Timebase = -1;						// Start up halted in pre-launch pre-GRR loop
	LVDC_TB_ETime = 0;
	LVDC_Stop = 0;
	IGMCycle = 0;
	sinceLastCycle = 0;
	DVIH.set();
	// INTERNAL (NON-REAL-LVDC) FLAGS
	CountPIPA = false;
	SIICenterEngineCutoff = false;
	FixedAttitudeBurn = false;
	if(!Initialized){ lvlog = fopen("lvlog.txt","w+"); }
	fprintf(lvlog,"init complete\r\n");
	fflush(lvlog);
	Initialized = true;
}

void LVDCSV::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, LVDC_START_STRING);
	// Here we go
	oapiWriteScenario_string(scn, "LVDC_FSPFileName", FSPFileName);
	oapiWriteScenario_int(scn, "LVDC_alpha_D_op", alpha_D_op);
	oapiWriteScenario_int(scn, "LVDC_BOOST", BOOST);
	oapiWriteScenario_int(scn, "LVDC_CHIBARSTEER", CHIBARSTEER);
	oapiWriteScenario_int(scn, "LVDC_CountPIPA", CountPIPA);
	oapiWriteScenario_int(scn, "LVDC_DFSMC", DFSMC);
	oapiWriteScenario_int(scn, "LVDC_DFTUP", DFTUP);
	oapiWriteScenario_int(scn, "LVDC_Direct_Ascent", Direct_Ascent);
	oapiWriteScenario_int(scn, "LVDC_directstagereset", directstagereset);
	oapiWriteScenario_int(scn, "LVDC_first_op", first_op);
	oapiWriteScenario_int(scn, "LVDC_FCLS4", FCLS4);
	oapiWriteScenario_int(scn, "LVDC_FSSAC", FSSAC);
	oapiWriteScenario_int(scn, "LVDC_FTADV", FTADV);
	oapiWriteScenario_int(scn, "LVDC_FixedAttitudeBurn", FixedAttitudeBurn);
	oapiWriteScenario_int(scn, "LVDC_GATE", GATE);
	oapiWriteScenario_int(scn, "LVDC_GATE0", GATE0);
	oapiWriteScenario_int(scn, "LVDC_GATE1", GATE1);
	oapiWriteScenario_int(scn, "LVDC_GATE2", GATE2);
	oapiWriteScenario_int(scn, "LVDC_GATE5", GATE5);
	oapiWriteScenario_int(scn, "LVDC_HSL", HSL);
	oapiWriteScenario_int(scn, "LVDC_ImpactBurnEnabled", ImpactBurnEnabled);
	oapiWriteScenario_int(scn, "LVDC_ImpactBurnInProgress", ImpactBurnInProgress);
	oapiWriteScenario_int(scn, "LVDC_init", init);
	oapiWriteScenario_int(scn, "LVDC_i_op", i_op);
	oapiWriteScenario_int(scn, "LVDC_MRS", MRS);
	papiWriteScenario_boolarr(scn, "LVDC_NISTAT", NISTAT, 21);
	oapiWriteScenario_int(scn, "LVDC_poweredflight", poweredflight);
	oapiWriteScenario_int(scn, "LVDC_ReadyToLaunch", ReadyToLaunch);
	oapiWriteScenario_int(scn, "LVDC_ROT", ROT);
	oapiWriteScenario_int(scn, "LVDC_ROTR", ROTR);
	oapiWriteScenario_int(scn, "LVDC_S2_BURNOUT", S2_BURNOUT);
	oapiWriteScenario_int(scn, "LVDC_S2_IGNITION", S2_IGNITION);
	oapiWriteScenario_int(scn, "LVDC_S4B_IGN", S4B_IGN);
	oapiWriteScenario_int(scn, "LVDC_S4B_REIGN", S4B_REIGN);
	oapiWriteScenario_int(scn, "LVDC_SIICenterEngineCutoff", SIICenterEngineCutoff);
	papiWriteScenario_boolarr(scn, "LVDC_T2STAT", T2STAT, 11);
	oapiWriteScenario_int(scn, "LVDC_TerminalConditions", TerminalConditions);
	oapiWriteScenario_int(scn, "LVDC_theta_N_op", theta_N_op);
	oapiWriteScenario_int(scn, "LVDC_TU", TU);
	oapiWriteScenario_int(scn, "LVDC_TU10", TU10);
	oapiWriteScenario_int(scn, "LVDC_AttitudeManeuverState", AttitudeManeuverState);
	oapiWriteScenario_int(scn, "LVDC_DGST2", DGST2);
	oapiWriteScenario_int(scn, "LVDC_DPM", DPM.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_DVASW", DVASW);
	oapiWriteScenario_int(scn, "LVDC_DVIH", DVIH.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_DVP", DVP);
	oapiWriteScenario_int(scn, "LVDC_EPTINDX", EPTINDX);
	oapiWriteScenario_int(scn, "LVDC_GST1M", GST1M);
	oapiWriteScenario_int(scn, "LVDC_IGMCycle", IGMCycle);
	oapiWriteScenario_int(scn, "LVDC_InterruptState", InterruptState.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_MLM", MLM);
	oapiWriteScenario_int(scn, "LVDC_ModeCode24", ModeCode24.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_ModeCode25", ModeCode25.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_ModeCode26", ModeCode26.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_ModeCode27", ModeCode27.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_LVDC_Stop", LVDC_Stop);
	oapiWriteScenario_int(scn, "LVDC_LVDC_Timebase", LVDC_Timebase);
	oapiWriteScenario_int(scn, "LVDC_OGStatusWord", OGStatusWord.to_ulong());
	oapiWriteScenario_int(scn, "LVDC_SSM", SSM);
	oapiWriteScenario_int(scn, "LVDC_SST1PTR", SST1PTR);
	oapiWriteScenario_int(scn, "LVDC_SST2PTR", SST2PTR);
	oapiWriteScenario_int(scn, "LVDC_Timer1Counter", Timer1Counter);
	oapiWriteScenario_int(scn, "LVDC_Timer2Counter", Timer2Counter);
	oapiWriteScenario_int(scn, "LVDC_tgt_index", tgt_index);
	oapiWriteScenario_int(scn, "LVDC_T_EO2", T_EO2);
	oapiWriteScenario_int(scn, "LVDC_UP", UP);
	oapiWriteScenario_int(scn, "LVDC_VASPI", VASPI);
	oapiWriteScenario_int(scn, "LVDC_VSC10", VSC10);
	oapiWriteScenario_int(scn, "LVDC_VSC11", VSC11);
	oapiWriteScenario_int(scn, "LVDC_VSC30", VSC30);
	oapiWriteScenario_int(scn, "LVDC_VSC31", VSC31);
	oapiWriteScenario_int(scn, "LVDC_VSNA1", VSNA1);
	oapiWriteScenario_int(scn, "LVDC_VSNA2", VSNA2);
	papiWriteScenario_intarr(scn, "LVDC_EPTABLE0", EPTABLE, 20);
	papiWriteScenario_intarr(scn, "LVDC_EPTABLE1", EPTABLE + 20, 20);
	papiWriteScenario_intarr(scn, "LVDC_EPTABLE2", EPTABLE + 40, 20);
	papiWriteScenario_intarr(scn, "LVDC_EPTABLE3", EPTABLE + 60, 20);
	papiWriteScenario_intarr(scn, "LVDC_EPTABLE4", EPTABLE + 80, 20);
	papiWriteScenario_intarr(scn, "LVDC_EPTABLE5", EPTABLE + 100, 20);
	papiWriteScenario_intarr(scn, "LVDC_EPTABLE6", EPTABLE + 120, 11);
	papiWriteScenario_doublearr(scn, "LVDC_EPTTIM0", EPTTIM, 20); //Save EPTTIM before any other double, in case we want something in the load function to overwrite it
	papiWriteScenario_doublearr(scn, "LVDC_EPTTIM1", EPTTIM + 20, 20);
	papiWriteScenario_doublearr(scn, "LVDC_EPTTIM2", EPTTIM + 40, 20);
	papiWriteScenario_doublearr(scn, "LVDC_EPTTIM3", EPTTIM + 60, 20);
	papiWriteScenario_doublearr(scn, "LVDC_EPTTIM4", EPTTIM + 80, 20);
	papiWriteScenario_doublearr(scn, "LVDC_EPTTIM5", EPTTIM + 100, 20);
	papiWriteScenario_doublearr(scn, "LVDC_EPTTIM6", EPTTIM + 120, 11);
	papiWriteScenario_double(scn, "LVDC_a", a);
	papiWriteScenario_double(scn, "LVDC_a_1", a_1);
	papiWriteScenario_double(scn, "LVDC_a_2", a_2);
	papiWriteScenario_double(scn, "LVDC_A1", A1);
	papiWriteScenario_double(scn, "LVDC_A2", A2);
	papiWriteScenario_double(scn, "LVDC_A3", A3);
	papiWriteScenario_double(scn, "LVDC_A4", A4);
	papiWriteScenario_double(scn, "LVDC_A5", A5);
	papiWriteScenario_double(scn, "LVDC_alpha_1", alpha_1);
	papiWriteScenario_double(scn, "LVDC_alpha_2", alpha_2);
	papiWriteScenario_double(scn, "LVDC_alpha_D", alpha_D);
	papiWriteScenario_double(scn, "LVDC_alpha_TS", alpha_TS);
	papiWriteScenario_double(scn, "LVDC_ALFTSA", TABLE15[0].alphaS_TS);
	papiWriteScenario_double(scn, "LVDC_ALFTSB", TABLE15[1].alphaS_TS);
	papiWriteScenario_double(scn, "LVDC_ART", ART);
	papiWriteScenario_double(scn, "LVDC_Azimuth", Azimuth);
	papiWriteScenario_double(scn, "LVDC_AZO", Azo);
	papiWriteScenario_double(scn, "LVDC_AZS", Azs);
	papiWriteScenario_double(scn, "LVDC_B_11", B_11);
	papiWriteScenario_double(scn, "LVDC_B_21", B_21);
	papiWriteScenario_double(scn, "LVDC_B_12", B_12);
	papiWriteScenario_double(scn, "LVDC_B_22", B_22);
	papiWriteScenario_double(scn, "LVDC_beta", beta);
	papiWriteScenario_double(scn, "LVDC_BETAA", TABLE15[0].beta);
	papiWriteScenario_double(scn, "LVDC_BETAB", TABLE15[1].beta);
	papiWriteScenario_double(scn, "LVDC_C_2", C_2);
	papiWriteScenario_double(scn, "LVDC_C_3", C_3);
	papiWriteScenario_double(scn, "LVDC_C_4", C_4);
	papiWriteScenario_double(scn, "LVDC_C3A0", TABLE15[0].target[0].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A1", TABLE15[0].target[1].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A2", TABLE15[0].target[2].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A3", TABLE15[0].target[3].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A4", TABLE15[0].target[4].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A5", TABLE15[0].target[5].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A6", TABLE15[0].target[6].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A7", TABLE15[0].target[7].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A8", TABLE15[0].target[8].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A9", TABLE15[0].target[9].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A10", TABLE15[0].target[10].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A11", TABLE15[0].target[11].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A12", TABLE15[0].target[12].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A13", TABLE15[0].target[13].C_3);
	papiWriteScenario_double(scn, "LVDC_C3A14", TABLE15[0].target[14].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B0", TABLE15[1].target[0].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B1", TABLE15[1].target[1].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B2", TABLE15[1].target[2].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B3", TABLE15[1].target[3].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B4", TABLE15[1].target[4].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B5", TABLE15[1].target[5].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B6", TABLE15[1].target[6].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B7", TABLE15[1].target[7].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B8", TABLE15[1].target[8].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B9", TABLE15[1].target[9].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B10", TABLE15[1].target[10].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B11", TABLE15[1].target[11].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B12", TABLE15[1].target[12].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B13", TABLE15[1].target[13].C_3);
	papiWriteScenario_double(scn, "LVDC_C3B14", TABLE15[1].target[14].C_3);
	papiWriteScenario_double(scn, "LVDC_COSA0", TABLE15[0].target[0].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA1", TABLE15[0].target[1].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA2", TABLE15[0].target[2].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA3", TABLE15[0].target[3].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA4", TABLE15[0].target[4].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA5", TABLE15[0].target[5].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA6", TABLE15[0].target[6].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA7", TABLE15[0].target[7].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA8", TABLE15[0].target[8].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA9", TABLE15[0].target[9].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA10", TABLE15[0].target[10].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA11", TABLE15[0].target[11].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA12", TABLE15[0].target[12].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA13", TABLE15[0].target[13].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSA14", TABLE15[0].target[14].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB0", TABLE15[1].target[0].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB1", TABLE15[1].target[1].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB2", TABLE15[1].target[2].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB3", TABLE15[1].target[3].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB4", TABLE15[1].target[4].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB5", TABLE15[1].target[5].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB6", TABLE15[1].target[6].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB7", TABLE15[1].target[7].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB8", TABLE15[1].target[8].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB9", TABLE15[1].target[9].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB10", TABLE15[1].target[10].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB11", TABLE15[1].target[11].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB12", TABLE15[1].target[12].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB13", TABLE15[1].target[13].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_COSB14", TABLE15[1].target[14].cos_sigma);
	papiWriteScenario_double(scn, "LVDC_Cf", Cf);
	papiWriteScenario_double(scn, "LVDC_CG", CG);
	papiWriteScenario_double(scn, "LVDC_cos_chi_Yit", cos_chi_Yit);
	papiWriteScenario_double(scn, "LVDC_cos_chi_Zit", cos_chi_Zit);
	papiWriteScenario_double(scn, "LVDC_cos_sigma", cos_sigma);
	papiWriteScenario_double(scn, "LVDC_Ct", Ct);
	papiWriteScenario_double(scn, "LVDC_Ct_o", Ct_o);
	papiWriteScenario_double(scn, "LVDC_D", D);
	papiWriteScenario_double(scn, "LVDC_d2", d2);
	papiWriteScenario_double(scn, "LVDC_ddot_zeta_GT", ddot_zeta_GT);
	papiWriteScenario_double(scn, "LVDC_ddot_xi_GT", ddot_xi_GT);
	papiWriteScenario_double(scn, "LVDC_DEC", DEC);
	papiWriteScenario_double(scn, "LVDC_DECA0", TABLE15[0].target[0].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA1", TABLE15[0].target[1].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA2", TABLE15[0].target[2].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA3", TABLE15[0].target[3].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA4", TABLE15[0].target[4].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA5", TABLE15[0].target[5].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA6", TABLE15[0].target[6].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA7", TABLE15[0].target[7].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA8", TABLE15[0].target[8].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA9", TABLE15[0].target[9].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA10", TABLE15[0].target[10].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA11", TABLE15[0].target[11].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA12", TABLE15[0].target[12].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA13", TABLE15[0].target[13].DEC);
	papiWriteScenario_double(scn, "LVDC_DECA14", TABLE15[0].target[14].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB0", TABLE15[1].target[0].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB1", TABLE15[1].target[1].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB2", TABLE15[1].target[2].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB3", TABLE15[1].target[3].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB4", TABLE15[1].target[4].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB5", TABLE15[1].target[5].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB6", TABLE15[1].target[6].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB7", TABLE15[1].target[7].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB8", TABLE15[1].target[8].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB9", TABLE15[1].target[9].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB10", TABLE15[1].target[10].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB11", TABLE15[1].target[11].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB12", TABLE15[1].target[12].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB13", TABLE15[1].target[13].DEC);
	papiWriteScenario_double(scn, "LVDC_DECB14", TABLE15[1].target[14].DEC);
	papiWriteScenario_double(scn, "LVDC_deta", deta);
	papiWriteScenario_doublearr(scn, "LVDC_DLTTL", DLTTL, 11);
	papiWriteScenario_double(scn, "LVDC_dxi", dxi);
	papiWriteScenario_double(scn, "LVDC_dot_dxi", dot_dxi);
	papiWriteScenario_double(scn, "LVDC_dot_deta", dot_deta);
	papiWriteScenario_double(scn, "LVDC_dot_dzeta", dot_dzeta);
	papiWriteScenario_double(scn, "LVDC_dot_dxit", dot_dxit);
	papiWriteScenario_double(scn, "LVDC_dot_detat", dot_detat);
	papiWriteScenario_double(scn, "LVDC_dot_dzetat", dot_dzetat);
	papiWriteScenario_double(scn, "LVDC_dotM_1", dotM_1);
	papiWriteScenario_double(scn, "LVDC_dotM_2", dotM_2);
	papiWriteScenario_double(scn, "LVDC_dotM_2R", dotM_2R);
	papiWriteScenario_double(scn, "LVDC_dotM_3", dotM_3);
	papiWriteScenario_double(scn, "LVDC_dotM_3R", dotM_3R);
	papiWriteScenario_double(scn, "LVDC_dot_phi_1", dot_phi_1);
	papiWriteScenario_double(scn, "LVDC_dot_phi_T", dot_phi_T);
	papiWriteScenario_double(scn, "LVDC_dot_zeta_T", dot_zeta_T);
	papiWriteScenario_double(scn, "LVDC_dot_xi_T", dot_xi_T);
	papiWriteScenario_double(scn, "LVDC_dot_eta_T", dot_eta_T);
	papiWriteScenario_double(scn, "LVDC_dT_3", dT_3);
	papiWriteScenario_double(scn, "LVDC_dT_4", dT_4);
	papiWriteScenario_double(scn, "LVDC_dt_c", dt_c);
	papiWriteScenario_double(scn, "LVDC_dT_cost", dT_cost);
	papiWriteScenario_double(scn, "LVDC_dT_F", dT_F);
	papiWriteScenario_double(scn, "LVDC_dt_g", dt_g);
	papiWriteScenario_double(scn, "LVDC_dT_ImpactBurn", dT_ImpactBurn);
	papiWriteScenario_double(scn, "LVDC_dt_LET", dt_LET);
	papiWriteScenario_double(scn, "LVDC_dT_LIM", dT_LIM);
	papiWriteScenario_double(scn, "LVDC_DT_N", DT_N);
	papiWriteScenario_double(scn, "LVDC_dtt_1", dtt_1);
	papiWriteScenario_double(scn, "LVDC_dtt_2", dtt_2);
	papiWriteScenario_double(scn, "LVDC_dTt_4", dTt_4);
	papiWriteScenario_double(scn, "LVDC_dV", dV);
	papiWriteScenario_double(scn, "LVDC_dV_B", dV_B);
	papiWriteScenario_double(scn, "LVDC_dV_BR", dV_BR);
	papiWriteScenario_double(scn, "LVDC_DVBRA", TABLE15[0].dV_BR);
	papiWriteScenario_double(scn, "LVDC_DVBRB", TABLE15[1].dV_BR);
	papiWriteScenario_doublearr(scn, "LVDC_DVCA", DVCA, 2);
	papiWriteScenario_double(scn, "LVDC_DVSST", DVSST);
	papiWriteScenario_double(scn, "LVDC_DVTGB", DVTGB);
	papiWriteScenario_double(scn, "LVDC_e", e);
	papiWriteScenario_double(scn, "LVDC_e_N", e_N);
	papiWriteScenario_double(scn, "LVDC_ENA0", TABLE15[0].target[0].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA1", TABLE15[0].target[1].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA2", TABLE15[0].target[2].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA3", TABLE15[0].target[3].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA4", TABLE15[0].target[4].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA5", TABLE15[0].target[5].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA6", TABLE15[0].target[6].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA7", TABLE15[0].target[7].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA8", TABLE15[0].target[8].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA9", TABLE15[0].target[9].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA10", TABLE15[0].target[10].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA11", TABLE15[0].target[11].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA12", TABLE15[0].target[12].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA13", TABLE15[0].target[13].e_N);
	papiWriteScenario_double(scn, "LVDC_ENA14", TABLE15[0].target[14].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB0", TABLE15[1].target[0].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB1", TABLE15[1].target[1].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB2", TABLE15[1].target[2].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB3", TABLE15[1].target[3].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB4", TABLE15[1].target[4].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB5", TABLE15[1].target[5].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB6", TABLE15[1].target[6].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB7", TABLE15[1].target[7].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB8", TABLE15[1].target[8].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB9", TABLE15[1].target[9].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB10", TABLE15[1].target[10].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB11", TABLE15[1].target[11].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB12", TABLE15[1].target[12].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB13", TABLE15[1].target[13].e_N);
	papiWriteScenario_double(scn, "LVDC_ENB14", TABLE15[1].target[14].e_N);
	papiWriteScenario_double(scn, "LVDC_eps_1", eps_1);
	papiWriteScenario_double(scn, "LVDC_eps_1R", eps_1R);
	papiWriteScenario_double(scn, "LVDC_eps_2", eps_2);
	papiWriteScenario_double(scn, "LVDC_eps_2R", eps_2R);
	papiWriteScenario_double(scn, "LVDC_eps_3", eps_3);
	papiWriteScenario_double(scn, "LVDC_eps_3R", eps_3R);
	papiWriteScenario_double(scn, "LVDC_eps_4", eps_4);
	papiWriteScenario_double(scn, "LVDC_eps_4R", eps_4R);
	papiWriteScenario_double(scn, "LVDC_f", f);
	papiWriteScenario_double(scn, "LVDC_F", F);
	papiWriteScenario_double(scn, "LVDC_FA", TABLE15[0].f);
	papiWriteScenario_double(scn, "LVDC_FB", TABLE15[1].f);
	papiWriteScenario_double(scn, "LVDC_Fm", Fm);
	papiWriteScenario_double(scn, "LVDC_Fx[0][0]", Fx[0][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][1]", Fx[0][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][2]", Fx[0][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][3]", Fx[0][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[0][4]", Fx[0][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][0]", Fx[1][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][1]", Fx[1][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][2]", Fx[1][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][3]", Fx[1][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[1][4]", Fx[1][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][0]", Fx[2][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][1]", Fx[2][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][2]", Fx[2][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][3]", Fx[2][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[2][4]", Fx[2][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][0]", Fx[3][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][1]", Fx[3][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][2]", Fx[3][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][3]", Fx[3][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[3][4]", Fx[3][4]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][0]", Fx[4][0]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][1]", Fx[4][1]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][2]", Fx[4][2]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][3]", Fx[4][3]);
	papiWriteScenario_double(scn, "LVDC_Fx[4][4]", Fx[4][4]);
	papiWriteScenario_double(scn, "LVDC_fx[0]", fx[0]);
	papiWriteScenario_double(scn, "LVDC_fx[1]", fx[1]);
	papiWriteScenario_double(scn, "LVDC_fx[2]", fx[2]);
	papiWriteScenario_double(scn, "LVDC_fx[3]", fx[3]);
	papiWriteScenario_double(scn, "LVDC_fx[4]", fx[4]);
	papiWriteScenario_double(scn, "LVDC_fx[5]", fx[5]);
	papiWriteScenario_double(scn, "LVDC_fx[6]", fx[6]);
	papiWriteScenario_double(scn, "LVDC_fxt[0]", fxt[0]);
	papiWriteScenario_double(scn, "LVDC_fxt[1]", fxt[1]);
	papiWriteScenario_double(scn, "LVDC_fxt[2]", fxt[2]);
	papiWriteScenario_double(scn, "LVDC_fxt[3]", fxt[3]);
	papiWriteScenario_double(scn, "LVDC_fxt[4]", fxt[4]);
	papiWriteScenario_double(scn, "LVDC_fxt[5]", fxt[5]);
	papiWriteScenario_double(scn, "LVDC_fxt[6]", fxt[6]);
	papiWriteScenario_double(scn, "LVDC_G_T", G_T);
	papiWriteScenario_double(scn, "LVDC_gx[0]", gx[0]);
	papiWriteScenario_double(scn, "LVDC_gx[1]", gx[1]);
	papiWriteScenario_double(scn, "LVDC_gx[2]", gx[2]);
	papiWriteScenario_double(scn, "LVDC_gx[3]", gx[3]);
	papiWriteScenario_double(scn, "LVDC_gx[4]", gx[4]);
	papiWriteScenario_double(scn, "LVDC_gx[5]", gx[5]);
	papiWriteScenario_double(scn, "LVDC_gx[6]", gx[6]);
	papiWriteScenario_double(scn, "LVDC_gxt[0]", gxt[0]);
	papiWriteScenario_double(scn, "LVDC_gxt[1]", gxt[1]);
	papiWriteScenario_double(scn, "LVDC_gxt[2]", gxt[2]);
	papiWriteScenario_double(scn, "LVDC_gxt[3]", gxt[3]);
	papiWriteScenario_double(scn, "LVDC_gxt[4]", gxt[4]);
	papiWriteScenario_double(scn, "LVDC_gxt[5]", gxt[5]);
	papiWriteScenario_double(scn, "LVDC_gxt[6]", gxt[6]);
	papiWriteScenario_double(scn, "LVDC_H", H);
	papiWriteScenario_double(scn, "LVDC_h_1", h_1);
	papiWriteScenario_double(scn, "LVDC_h_2", h_2);
	papiWriteScenario_double(scn, "LVDC_hx[0][0]", hx[0][0]);
	papiWriteScenario_double(scn, "LVDC_hx[0][1]", hx[0][1]);
	papiWriteScenario_double(scn, "LVDC_hx[0][2]", hx[0][2]);
	papiWriteScenario_double(scn, "LVDC_hx[0][3]", hx[0][3]);
	papiWriteScenario_double(scn, "LVDC_hx[0][4]", hx[0][4]);
	papiWriteScenario_double(scn, "LVDC_hx[1][0]", hx[1][0]);
	papiWriteScenario_double(scn, "LVDC_hx[1][1]", hx[1][1]);
	papiWriteScenario_double(scn, "LVDC_hx[1][2]", hx[1][2]);
	papiWriteScenario_double(scn, "LVDC_hx[1][3]", hx[1][3]);
	papiWriteScenario_double(scn, "LVDC_hx[1][4]", hx[1][4]);
	papiWriteScenario_double(scn, "LVDC_hx[2][0]", hx[2][0]);
	papiWriteScenario_double(scn, "LVDC_hx[2][1]", hx[2][1]);
	papiWriteScenario_double(scn, "LVDC_hx[2][2]", hx[2][2]);
	papiWriteScenario_double(scn, "LVDC_hx[2][3]", hx[2][3]);
	papiWriteScenario_double(scn, "LVDC_hx[2][4]", hx[2][4]);
	papiWriteScenario_double(scn, "LVDC_Inclination", Inclination);
	papiWriteScenario_double(scn, "LVDC_J", J);
	papiWriteScenario_double(scn, "LVDC_J_1", J_1);
	papiWriteScenario_double(scn, "LVDC_J_2", J_2);
	papiWriteScenario_double(scn, "LVDC_J_3", J_3);
	papiWriteScenario_double(scn, "LVDC_J_12", J_12);
	papiWriteScenario_double(scn, "LVDC_Jt_3", Jt_3);
	papiWriteScenario_double(scn, "LVDC_J_Y", J_Y);
	papiWriteScenario_double(scn, "LVDC_J_P", J_P);
	papiWriteScenario_double(scn, "LVDC_K_1", K_1);
	papiWriteScenario_double(scn, "LVDC_K_2", K_2);
	papiWriteScenario_double(scn, "LVDC_K_3", K_3);
	papiWriteScenario_double(scn, "LVDC_K_4", K_4);
	papiWriteScenario_double(scn, "LVDC_K_5", K_5);
	papiWriteScenario_double(scn, "LVDC_K_D", K_D);
	papiWriteScenario_double(scn, "LVDC_K_P1", K_P1);
	papiWriteScenario_double(scn, "LVDC_K_P2", K_P2);
	papiWriteScenario_double(scn, "LVDC_KSCLNG", KSCLNG);
	papiWriteScenario_double(scn, "LVDC_K_T3", K_T3);
	papiWriteScenario_double(scn, "LVDC_K_Y1", K_Y1);
	papiWriteScenario_double(scn, "LVDC_K_Y2", K_Y2);
	papiWriteScenario_double(scn, "LVDC_K_Y", K_Y);
	papiWriteScenario_double(scn, "LVDC_K_P", K_P);
	papiWriteScenario_double(scn, "LVDC_D_P", D_P);
	papiWriteScenario_double(scn, "LVDC_D_Y", D_Y);
	papiWriteScenario_double(scn, "LVDC_L_1", L_1);
	papiWriteScenario_double(scn, "LVDC_L_2", L_2);
	papiWriteScenario_double(scn, "LVDC_L_3", L_3);
	papiWriteScenario_double(scn, "LVDC_dL_3", dL_3);
	papiWriteScenario_double(scn, "LVDC_Lt_3", Lt_3);
	papiWriteScenario_double(scn, "LVDC_L_12", L_12);
	papiWriteScenario_double(scn, "LVDC_L_P", L_P);
	papiWriteScenario_double(scn, "LVDC_L_Y", L_Y);
	papiWriteScenario_double(scn, "LVDC_Lt_Y", Lt_Y);
	papiWriteScenario_double(scn, "LVDC_LVDC_TB_ETime", LVDC_TB_ETime);
	papiWriteScenario_doublearr(scn, "LVDC_MF", MF, 5);
	papiWriteScenario_double(scn, "LVDC_MFS", MFS);
	papiWriteScenario_doublearr(scn, "LVDC_MFSArr", MFSArr, 4);
	papiWriteScenario_double(scn, "LVDC_MLD", MLD);
	papiWriteScenario_double(scn, "LVDC_MLR", MLR);
	papiWriteScenario_double(scn, "LVDC_MLT", MLT);
	papiWriteScenario_double(scn, "LVDC_MS04DT", MS04DT);
	papiWriteScenario_double(scn, "LVDC_MS25DT", MS25DT);
	papiWriteScenario_double(scn, "LVDC_MSK5", MSK5);
	papiWriteScenario_double(scn, "LVDC_MSK6", MSK6);
	papiWriteScenario_double(scn, "LVDC_MSK16", MSK16);
	papiWriteScenario_double(scn, "LVDC_MSLIM1", MSLIM1);
	papiWriteScenario_double(scn, "LVDC_MSLIM2", MSLIM2);
	papiWriteScenario_double(scn, "LVDC_mu", mu);
	papiWriteScenario_double(scn, "LVDC_NUPTIM", NUPTIM);
	papiWriteScenario_double(scn, "LVDC_omega_E", omega_E);
	papiWriteScenario_double(scn, "LVDC_p", p);
	papiWriteScenario_double(scn, "LVDC_P_1", P_1);
	papiWriteScenario_double(scn, "LVDC_P_2", P_2);
	papiWriteScenario_double(scn, "LVDC_P_3", P_3);
	papiWriteScenario_double(scn, "LVDC_P_12", P_12);
	papiWriteScenario_double(scn, "LVDC_PHI", PHI);
	papiWriteScenario_double(scn, "LVDC_PHIP", PHIP);
	papiWriteScenario_double(scn, "LVDC_phi_T", phi_T);
	papiWriteScenario_double(scn, "LVDC_Q_1", Q_1);
	papiWriteScenario_double(scn, "LVDC_Q_2", Q_2);
	papiWriteScenario_double(scn, "LVDC_Q_3", Q_3);
	papiWriteScenario_double(scn, "LVDC_Q_12", Q_12);
	papiWriteScenario_double(scn, "LVDC_Q_Y", Q_Y);
	papiWriteScenario_double(scn, "LVDC_Q_P", Q_P);
	papiWriteScenario_double(scn, "LVDC_R", R);
	papiWriteScenario_double(scn, "LVDC_RAS", RAS);
	papiWriteScenario_double(scn, "LVDC_RASA0", TABLE15[0].target[0].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA1", TABLE15[0].target[1].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA2", TABLE15[0].target[2].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA3", TABLE15[0].target[3].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA4", TABLE15[0].target[4].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA5", TABLE15[0].target[5].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA6", TABLE15[0].target[6].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA7", TABLE15[0].target[7].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA8", TABLE15[0].target[8].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA9", TABLE15[0].target[9].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA10", TABLE15[0].target[10].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA11", TABLE15[0].target[11].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA12", TABLE15[0].target[12].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA13", TABLE15[0].target[13].RAS);
	papiWriteScenario_double(scn, "LVDC_RASA14", TABLE15[0].target[14].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB0", TABLE15[1].target[0].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB1", TABLE15[1].target[1].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB2", TABLE15[1].target[2].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB3", TABLE15[1].target[3].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB4", TABLE15[1].target[4].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB5", TABLE15[1].target[5].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB6", TABLE15[1].target[6].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB7", TABLE15[1].target[7].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB8", TABLE15[1].target[8].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB9", TABLE15[1].target[9].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB10", TABLE15[1].target[10].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB11", TABLE15[1].target[11].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB12", TABLE15[1].target[12].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB13", TABLE15[1].target[13].RAS);
	papiWriteScenario_double(scn, "LVDC_RASB14", TABLE15[1].target[14].RAS);
	papiWriteScenario_double(scn, "LVDC_RealTimeClock", RealTimeClock);
	papiWriteScenario_double(scn, "LVDC_R_L", R_L);
	papiWriteScenario_double(scn, "LVDC_R_N", R_N);
	papiWriteScenario_double(scn, "LVDC_RNA", TABLE15[0].R_N);
	papiWriteScenario_double(scn, "LVDC_RNB", TABLE15[1].R_N);
	papiWriteScenario_double(scn, "LVDC_ROV", ROV);
	papiWriteScenario_double(scn, "LVDC_ROVR", ROVR);
	papiWriteScenario_double(scn, "LVDC_ROVs", ROVs);
	papiWriteScenario_double(scn, "LVDC_R_T", R_T);
	papiWriteScenario_double(scn, "LVDC_S_1", S_1);
	papiWriteScenario_double(scn, "LVDC_S_2", S_2);
	papiWriteScenario_double(scn, "LVDC_S_3", S_3);
	papiWriteScenario_double(scn, "LVDC_S_12", S_12);
	papiWriteScenario_double(scn, "LVDC_S_P", S_P);
	papiWriteScenario_double(scn, "LVDC_S_Y", S_Y);
	papiWriteScenario_double(scn, "LVDC_sinceLastCycle", sinceLastCycle);
	papiWriteScenario_double(scn, "LVDC_sin_chi_Yit", sin_chi_Yit);
	papiWriteScenario_double(scn, "LVDC_sin_chi_Zit", sin_chi_Zit);
	papiWriteScenario_double(scn, "LVDC_sin_gam", sin_gam);
	papiWriteScenario_double(scn, "LVDC_cos_gam", cos_gam);
	papiWriteScenario_double(scn, "LVDC_SMCY", SMCY);
	papiWriteScenario_double(scn, "LVDC_SMCZ", SMCZ);
	papiWriteScenario_double(scn, "LVDC_S", S);
	papiWriteScenario_double(scn, "LVDC_P", P);
	papiWriteScenario_double(scn, "LVDC_S_34", S_34);
	papiWriteScenario_double(scn, "LVDC_P_34", P_34);
	papiWriteScenario_double(scn, "LVDC_T_0", T_0);
	papiWriteScenario_double(scn, "LVDC_t_1", t_1);
	papiWriteScenario_double(scn, "LVDC_T_1", T_1);
	papiWriteScenario_double(scn, "LVDC_T_1c", T_1c);
	papiWriteScenario_double(scn, "LVDC_t_2", t_2);
	papiWriteScenario_double(scn, "LVDC_T_2", T_2);
	papiWriteScenario_double(scn, "LVDC_t_21", t_21);
	papiWriteScenario_double(scn, "LVDC_T_2R", T_2R);
	papiWriteScenario_double(scn, "LVDC_t_3", t_3);
	papiWriteScenario_double(scn, "LVDC_T_3", T_3);
	papiWriteScenario_double(scn, "LVDC_t_3i", t_3i);
	papiWriteScenario_double(scn, "LVDC_t_4", t_4);
	papiWriteScenario_double(scn, "LVDC_T_4N", T_4N);
	papiWriteScenario_double(scn, "LVDC_t_5", t_5);
	papiWriteScenario_double(scn, "LVDC_T2IR", TABLE15[0].T2IR);
	papiWriteScenario_double(scn, "LVDC_T2IRB", TABLE15[1].T2IR);
	papiWriteScenario_double(scn, "LVDC_T3PRA", TABLE15[0].T3PR);
	papiWriteScenario_double(scn, "LVDC_T3PRB", TABLE15[1].T3PR);
	papiWriteScenario_double(scn, "LVDC_T_ar", T_ar);
	papiWriteScenario_double(scn, "LVDC_TAS", TAS);
	papiWriteScenario_double(scn, "LVDC_tau1", tau1);
	papiWriteScenario_double(scn, "LVDC_tau2", tau2);
	papiWriteScenario_double(scn, "LVDC_tau2N", tau2N);
	papiWriteScenario_double(scn, "LVDC_tau3", tau3);
	papiWriteScenario_double(scn, "LVDC_tau3N", tau3N);
	papiWriteScenario_double(scn, "LVDC_tau3R", tau3R);
	papiWriteScenario_double(scn, "LVDC_TAU3RA", TABLE15[0].TAU3R);
	papiWriteScenario_double(scn, "LVDC_TAU3RB", TABLE15[1].TAU3R);
	papiWriteScenario_double(scn, "LVDC_t_B1", t_B1);
	papiWriteScenario_double(scn, "LVDC_TB1", TB1);
	papiWriteScenario_double(scn, "LVDC_t_B2", t_B2);
	papiWriteScenario_double(scn, "LVDC_TB2", TB2);
	papiWriteScenario_double(scn, "LVDC_t_B3", t_B3);
	papiWriteScenario_double(scn, "LVDC_TB3", TB3);
	papiWriteScenario_double(scn, "LVDC_t_B4", t_B4);
	papiWriteScenario_double(scn, "LVDC_TB4", TB4);
	papiWriteScenario_double(scn, "LVDC_TB4a", TB4a);
	papiWriteScenario_double(scn, "LVDC_TB5", TB5);
	papiWriteScenario_double(scn, "LVDC_TB6", TB6);
	papiWriteScenario_double(scn, "LVDC_TB7", TB7);
	papiWriteScenario_double(scn, "LVDC_TB8", TB8);
	papiWriteScenario_double(scn, "LVDC_TBA", TBA);
	papiWriteScenario_double(scn, "LVDC_TBL", TBL);
	papiWriteScenario_double(scn, "LVDC_T_c", T_c);
	papiWriteScenario_double(scn, "LVDC_tchi_y_last", tchi_y_last);
	papiWriteScenario_double(scn, "LVDC_tchi_p_last", tchi_p_last);
	papiWriteScenario_double(scn, "LVDC_tchi_y", tchi_y);
	papiWriteScenario_double(scn, "LVDC_tchi_p", tchi_p);
	papiWriteScenario_double(scn, "LVDC_t_clock", t_clock);
	papiWriteScenario_double(scn, "LVDC_T_CO", T_CO);
	papiWriteScenario_double(scn, "LVDC_t_D", t_D);
	papiWriteScenario_double(scn, "LVDC_t_D0", t_D0);
	papiWriteScenario_double(scn, "LVDC_t_D1", t_D1);
	papiWriteScenario_double(scn, "LVDC_t_D2", t_D2);
	papiWriteScenario_double(scn, "LVDC_t_D3", t_D3);
	papiWriteScenario_double(scn, "LVDC_t_DS0", t_DS0);
	papiWriteScenario_double(scn, "LVDC_t_DS1", t_DS1);
	papiWriteScenario_double(scn, "LVDC_t_DS2", t_DS2);
	papiWriteScenario_double(scn, "LVDC_t_DS3", t_DS3);
	papiWriteScenario_double(scn, "LVDC_T_EO1", T_EO1);
	papiWriteScenario_double(scn, "LVDC_T_GO", T_GO);
	papiWriteScenario_double(scn, "LVDC_THTEO", theta_EO);
	papiWriteScenario_double(scn, "LVDC_theta_N", theta_N);
	papiWriteScenario_double(scn, "LVDC_TI", TI);
	papiWriteScenario_double(scn, "LVDC_T_ImpactBurn", T_ImpactBurn);
	papiWriteScenario_double(scn, "LVDC_TI5F3", TI5F3);
	papiWriteScenario_double(scn, "LVDC_TI5F4", TI5F4);
	papiWriteScenario_double(scn, "LVDC_TI5F5", TI5F5);
	papiWriteScenario_double(scn, "LVDC_T7M10", T7M10);
	papiWriteScenario_double(scn, "LVDC_T7M11", T7M11);
	papiWriteScenario_double(scn, "LVDC_T_L", T_L);
	papiWriteScenario_double(scn, "LVDC_T_LO", T_LO);
	papiWriteScenario_double(scn, "LVDC_TMEFRZ", TMEFRZ);
	papiWriteScenario_double(scn, "LVDC_TMM", TMM);
	papiWriteScenario_double(scn, "LVDC_TMR", TMR);
	papiWriteScenario_double(scn, "LVDC_TPA0", TABLE15[0].target[0].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA1", TABLE15[0].target[1].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA2", TABLE15[0].target[2].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA3", TABLE15[0].target[3].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA4", TABLE15[0].target[4].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA5", TABLE15[0].target[5].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA6", TABLE15[0].target[6].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA7", TABLE15[0].target[7].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA8", TABLE15[0].target[8].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA9", TABLE15[0].target[9].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA10", TABLE15[0].target[10].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA11", TABLE15[0].target[11].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA12", TABLE15[0].target[12].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA13", TABLE15[0].target[13].t_D);
	papiWriteScenario_double(scn, "LVDC_TPA14", TABLE15[0].target[14].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB0", TABLE15[1].target[0].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB1", TABLE15[1].target[1].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB2", TABLE15[1].target[2].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB3", TABLE15[1].target[3].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB4", TABLE15[1].target[4].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB5", TABLE15[1].target[5].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB6", TABLE15[1].target[6].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB7", TABLE15[1].target[7].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB8", TABLE15[1].target[8].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB9", TABLE15[1].target[9].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB10", TABLE15[1].target[10].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB11", TABLE15[1].target[11].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB12", TABLE15[1].target[12].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB13", TABLE15[1].target[13].t_D);
	papiWriteScenario_double(scn, "LVDC_TPB14", TABLE15[1].target[14].t_D);
	papiWriteScenario_double(scn, "LVDC_T_RP", T_RP);
	papiWriteScenario_double(scn, "LVDC_T_S1", T_S1);
	papiWriteScenario_double(scn, "LVDC_T_S2", T_S2);
	papiWriteScenario_double(scn, "LVDC_T_S3", T_S3);
	papiWriteScenario_double(scn, "LVDC_TS4BS", TS4BS);
	papiWriteScenario_double(scn, "LVDC_t_SD1", t_SD1);
	papiWriteScenario_double(scn, "LVDC_t_SD2", t_SD2);
	papiWriteScenario_double(scn, "LVDC_t_SD3", t_SD3);
	papiWriteScenario_double(scn, "LVDC_T_ST", T_ST);
	papiWriteScenario_double(scn, "LVDC_TSTA", TABLE15[0].T_ST);
	papiWriteScenario_double(scn, "LVDC_TSTB", TABLE15[1].T_ST);
	papiWriteScenario_double(scn, "LVDC_T_T", T_T);
	papiWriteScenario_double(scn, "LVDC_t_TB8Start", t_TB8Start);
	papiWriteScenario_double(scn, "LVDC_Tt_3", Tt_3);
	papiWriteScenario_double(scn, "LVDC_Tt_3R", Tt_3R);
	papiWriteScenario_double(scn, "LVDC_Tt_T", Tt_T);
	papiWriteScenario_double(scn, "LVDC_TVRATE", TVRATE);
	papiWriteScenario_double(scn, "LVDC_U_1", U_1);
	papiWriteScenario_double(scn, "LVDC_U_2", U_2);
	papiWriteScenario_double(scn, "LVDC_U_3", U_3);
	papiWriteScenario_double(scn, "LVDC_U_12", U_12);
	papiWriteScenario_double(scn, "LVDC_gamma_T", gamma_T);
	papiWriteScenario_double(scn, "LVDC_V", V);
	papiWriteScenario_double(scn, "LVDC_V_ex1", V_ex1);
	papiWriteScenario_double(scn, "LVDC_V_ex2", V_ex2);
	papiWriteScenario_double(scn, "LVDC_V_ex2R", V_ex2R);
	papiWriteScenario_double(scn, "LVDC_V_ex3", V_ex3);
	papiWriteScenario_double(scn, "LVDC_V_ex3R", V_ex3R);
	papiWriteScenario_double(scn, "LVDC_VGBIA", VGBIA);
	papiWriteScenario_double(scn, "LVDC_V_i", V_i);
	papiWriteScenario_double(scn, "LVDC_V_0", V_0);
	papiWriteScenario_double(scn, "LVDC_V_1", V_1);
	papiWriteScenario_double(scn, "LVDC_V_2", V_2);
	papiWriteScenario_double(scn, "LVDC_VATR4", VATR4);
	papiWriteScenario_double(scn, "LVDC_VATRR", VATRR);
	papiWriteScenario_double(scn, "LVDC_VCCYA", VCCYA);
	papiWriteScenario_double(scn, "LVDC_VCCZA", VCCZA);
	papiWriteScenario_doublearr(scn, "LVDC_VENTA", VENTA, 7);
	papiWriteScenario_double(scn, "LVDC_VOLD", VOLD);
	papiWriteScenario_double(scn, "LVDC_V_S2T", V_S2T);
	papiWriteScenario_double(scn, "LVDC_VSC12", VSC12);
	papiWriteScenario_double(scn, "LVDC_VSC32", VSC32);
	papiWriteScenario_double(scn, "LVDC_VSSRT", VSSRT);
	papiWriteScenario_double(scn, "LVDC_VSSW", VSSW);
	papiWriteScenario_double(scn, "LVDC_V_T", V_T);
	papiWriteScenario_double(scn, "LVDC_V_TC", V_TC);
	papiWriteScenario_doublearr(scn, "LVDC_VTIM", VTIM, 6);
	papiWriteScenario_double(scn, "LVDC_VTOLD", VTOLD);
	papiWriteScenario_double(scn, "LVDC_xi_T", xi_T);
	papiWriteScenario_vec(scn, "LVDC_AttitudeError", AttitudeError);
	papiWriteScenario_vec(scn, "LVDC_AttitudeErrorOld", AttitudeErrorOld);
	papiWriteScenario_vec(scn, "LVDC_CommandedAttitude", CommandedAttitude);
	papiWriteScenario_vec(scn, "LVDC_CurrentAttitude", CurrentAttitude);
	papiWriteScenario_vec(scn, "LVDC_DChi_apo", DChi_apo);
	papiWriteScenario_vec(scn, "LVDC_ddotG_act", ddotG_act);
	papiWriteScenario_vec(scn, "LVDC_ddotG_last", ddotG_last);
	papiWriteScenario_vec(scn, "LVDC_ddotS", ddotS);
	papiWriteScenario_vec(scn, "LVDC_DDotXEZ_G", DDotXEZ_G);
	papiWriteScenario_vec(scn, "LVDC_DotG_act", DotG_act);
	papiWriteScenario_vec(scn, "LVDC_DotG_last", DotG_last);
	papiWriteScenario_vec(scn, "LVDC_DotM_act", DotM_act);
	papiWriteScenario_vec(scn, "LVDC_DotM_last", DotM_last);
	papiWriteScenario_vec(scn, "LVDC_ddotM_act", ddotM_act);
	papiWriteScenario_vec(scn, "LVDC_DotS", DotS);
	papiWriteScenario_vec(scn, "LVDC_DotXEZ", DotXEZ);
	papiWriteScenario_vec(scn, "LVDC_PCommandedAttitude", PCommandedAttitude);
	papiWriteScenario_vec(scn, "LVDC_Pos4", Pos4);
	papiWriteScenario_vec(scn, "LVDC_Pos_Nav", Pos_Nav);
	papiWriteScenario_vec(scn, "LVDC_PosS", PosS);
	papiWriteScenario_vec(scn, "LVDC_PosXEZ", PosXEZ);
	papiWriteScenario_vec(scn, "LVDC_R_OG", R_OG);
	papiWriteScenario_vec(scn, "LVDC_Sbar", Sbar);
	papiWriteScenario_vec(scn, "LVDC_TargetVector", TargetVector);
	papiWriteScenario_vec(scn, "LVDC_T_P", T_P);
	papiWriteScenario_vec(scn, "LVDC_Vel_Nav", Vel_Nav);
	papiWriteScenario_vec(scn, "LVDC_W_ES", W_ES);
	papiWriteScenario_vec(scn, "LVDC_VOAC", VOAC);
	papiWriteScenario_vec(scn, "LVDC_XLunarAttitude", XLunarAttitude);
	papiWriteScenario_vec(scn, "LVDC_XLunarCommAttitude", XLunarCommAttitude);
	papiWriteScenario_vec(scn, "LVDC_XLunarSlingshotAttitude", XLunarSlingshotAttitude);
	papiWriteScenario_mx(scn, "LVDC_MX_A", MX_A);
	papiWriteScenario_mx(scn, "LVDC_MX_B", MX_B);
	papiWriteScenario_mx(scn, "LVDC_MX_G", MX_G);
	papiWriteScenario_mx(scn, "LVDC_MX_K", MX_K);
	papiWriteScenario_mx(scn, "LVDC_MX_phi_T", MX_phi_T);
	// All done
	oapiWriteLine(scn, LVDC_END_STRING);
}

void LVDCSV::LoadState(FILEHANDLE scn) {
	char *line;
	int tmp = 0; // for bool loader
	if (Initialized) {
		fprintf(lvlog, "LoadState() called\r\n");
		fflush(lvlog);
	}
	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, LVDC_END_STRING, sizeof(LVDC_END_STRING))) {
			break;
		}
		// are we there yet?
		// Doing all this in one go makes the MS compiler barf.
		// Doing it in long chains makes the MS compiler silently optimize away the tail of the chain.
		// So we do it in single lines.
		// strings
		papiReadScenario_string(line, "LVDC_FSPFileName", FSPFileName);
		// booleans
		papiReadScenario_bool(line, "LVDC_alpha_D_op", alpha_D_op);
		papiReadScenario_bool(line, "LVDC_BOOST", BOOST);
		papiReadScenario_bool(line, "LVDC_CHIBARSTEER", CHIBARSTEER);
		papiReadScenario_bool(line, "LVDC_CountPIPA", CountPIPA);
		papiReadScenario_bool(line, "LVDC_DFSMC", DFSMC);
		papiReadScenario_bool(line, "LVDC_DFTUP", DFTUP);
		papiReadScenario_bool(line, "LVDC_Direct_Ascent", Direct_Ascent);
		papiReadScenario_bool(line, "LVDC_directstagereset", directstagereset);
		papiReadScenario_bool(line, "LVDC_FCLS4", FCLS4);
		papiReadScenario_bool(line, "LVDC_first_op", first_op);
		papiReadScenario_bool(line, "LVDC_FixedAttitudeBurn", FixedAttitudeBurn);
		papiReadScenario_bool(line, "LVDC_FSSAC", FSSAC);
		papiReadScenario_bool(line, "LVDC_FTADV", FTADV);
		papiReadScenario_bool(line, "LVDC_GATE", GATE);
		papiReadScenario_bool(line, "LVDC_GATE0", GATE0);
		papiReadScenario_bool(line, "LVDC_GATE1", GATE1);
		papiReadScenario_bool(line, "LVDC_GATE2", GATE2);
		papiReadScenario_bool(line, "LVDC_GATE5", GATE5);
		papiReadScenario_bool(line, "LVDC_HSL", HSL);
		papiReadScenario_bool(line, "LVDC_ImpactBurnEnabled", ImpactBurnEnabled);
		papiReadScenario_bool(line, "LVDC_ImpactBurnInProgress", ImpactBurnInProgress);
		papiReadScenario_bool(line, "LVDC_init", init);
		papiReadScenario_bool(line, "LVDC_i_op", i_op);
		papiReadScenario_bool(line, "LVDC_MRS", MRS);
		papiReadScenario_boolarr(line, "LVDC_NISTAT", NISTAT, 21);
		papiReadScenario_bool(line, "LVDC_poweredflight", poweredflight);
		papiReadScenario_bool(line, "LVDC_ReadyToLaunch", ReadyToLaunch);
		papiReadScenario_bool(line, "LVDC_ROT", ROT);
		papiReadScenario_bool(line, "LVDC_ROTR", ROTR);
		papiReadScenario_bool(line, "LVDC_S2_BURNOUT", S2_BURNOUT);
		papiReadScenario_bool(line, "LVDC_S2_IGNITION", S2_IGNITION);
		papiReadScenario_bool(line, "LVDC_S4B_IGN", S4B_IGN);
		papiReadScenario_bool(line, "LVDC_S4B_REIGN", S4B_REIGN);
		papiReadScenario_bool(line, "LVDC_SIICenterEngineCutoff", SIICenterEngineCutoff);
		papiReadScenario_boolarr(line, "LVDC_T2STAT", T2STAT, 11);
		papiReadScenario_bool(line, "LVDC_TerminalConditions", TerminalConditions);
		papiReadScenario_bool(line, "LVDC_theta_N_op", theta_N_op);
		papiReadScenario_bool(line, "LVDC_TU", TU);
		papiReadScenario_bool(line, "LVDC_TU10", TU10);

		// integers
		papiReadScenario_int(line, "LVDC_AttitudeManeuverState", AttitudeManeuverState);
		papiReadScenario_int(line, "LVDC_DGST2", DGST2);
		if (papiReadScenario_int(line, "LVDC_DPM", tmp))
		{
			DPM = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_DVASW", tmp))
		{
			DVASW = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_DVIH", tmp))
		{
			DVIH = tmp;
		}
		papiReadScenario_int(line, "LVDC_DVP", DVP);
		papiReadScenario_int(line, "LVDC_EPTINDX", EPTINDX);
		papiReadScenario_int(line, "LVDC_GST1M", GST1M);
		papiReadScenario_int(line, "LVDC_IGMCycle", IGMCycle);
		if (papiReadScenario_int(line, "LVDC_InterruptState", tmp))
		{
			InterruptState = tmp;
		}
		papiReadScenario_int(line, "LVDC_MLM", MLM);
		if (papiReadScenario_int(line, "LVDC_ModeCode24", tmp))
		{
			ModeCode24 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_ModeCode25", tmp))
		{
			ModeCode25 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_ModeCode26", tmp))
		{
			ModeCode26 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_ModeCode27", tmp))
		{
			ModeCode27 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_OGStatusWord", tmp))
		{
			OGStatusWord = tmp;
		}
		papiReadScenario_int(line, "LVDC_SSM", SSM);
		if (papiReadScenario_int(line, "LVDC_SST1PTR", tmp))
		{
			SST1PTR = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_SST2PTR", tmp))
		{
			SST2PTR = tmp;
		}
		papiReadScenario_int(line, "LVDC_LVDC_Stop", LVDC_Stop);
		papiReadScenario_int(line, "LVDC_LVDC_Timebase", LVDC_Timebase);
		papiReadScenario_int(line, "LVDC_Timer1Counter", Timer1Counter);
		papiReadScenario_int(line, "LVDC_Timer2Counter", Timer2Counter);
		papiReadScenario_int(line, "LVDC_tgt_index", tgt_index);
		papiReadScenario_int(line, "LVDC_T_EO2", T_EO2);
		papiReadScenario_int(line, "LVDC_UP", UP);
		if (papiReadScenario_int(line, "LVDC_VASPI", tmp))
		{
			VASPI = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_VSC10", tmp))
		{
			VSC10 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_VSC11", tmp))
		{
			VSC11 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_VSC30", tmp))
		{
			VSC30 = tmp;
		}
		if (papiReadScenario_int(line, "LVDC_VSC31", tmp))
		{
			VSC31 = tmp;
		}
		papiReadScenario_int(line, "LVDC_VSNA1", VSNA1);
		papiReadScenario_int(line, "LVDC_VSNA2", VSNA2);
		papiReadScenario_intarr(line, "LVDC_EPTABLE0", EPTABLE, 20);
		papiReadScenario_intarr(line, "LVDC_EPTABLE1", EPTABLE + 20, 20);
		papiReadScenario_intarr(line, "LVDC_EPTABLE2", EPTABLE + 40, 20);
		papiReadScenario_intarr(line, "LVDC_EPTABLE3", EPTABLE + 60, 20);
		papiReadScenario_intarr(line, "LVDC_EPTABLE4", EPTABLE + 80, 20);
		papiReadScenario_intarr(line, "LVDC_EPTABLE5", EPTABLE + 100, 20);
		papiReadScenario_intarr(line, "LVDC_EPTABLE6", EPTABLE + 120, 11);
		// doubles
		papiReadScenario_doublearr(line, "LVDC_EPTTIM0", EPTTIM, 20);
		papiReadScenario_doublearr(line, "LVDC_EPTTIM1", EPTTIM + 20, 20);
		papiReadScenario_doublearr(line, "LVDC_EPTTIM2", EPTTIM + 40, 20);
		papiReadScenario_doublearr(line, "LVDC_EPTTIM3", EPTTIM + 60, 20);
		papiReadScenario_doublearr(line, "LVDC_EPTTIM4", EPTTIM + 80, 20);
		papiReadScenario_doublearr(line, "LVDC_EPTTIM5", EPTTIM + 100, 20);
		papiReadScenario_doublearr(line, "LVDC_EPTTIM6", EPTTIM + 120, 11);
		papiReadScenario_double(line, "LVDC_a", a);
		papiReadScenario_double(line, "LVDC_a_1", a_1);
		papiReadScenario_double(line, "LVDC_a_2", a_2);
		papiReadScenario_double(line, "LVDC_A1", A1);
		papiReadScenario_double(line, "LVDC_A2", A2);
		papiReadScenario_double(line, "LVDC_A3", A3);
		papiReadScenario_double(line, "LVDC_A4", A4);
		papiReadScenario_double(line, "LVDC_A5", A5);
		papiReadScenario_double(line, "LVDC_alpha_1", alpha_1);
		papiReadScenario_double(line, "LVDC_alpha_2", alpha_2);
		papiReadScenario_double(line, "LVDC_alpha_D", alpha_D);
		papiReadScenario_double(line, "LVDC_alpha_TS", alpha_TS);
		papiReadScenario_double(line, "LVDC_ALFTSA", TABLE15[0].alphaS_TS);
		papiReadScenario_double(line, "LVDC_ALFTSB", TABLE15[1].alphaS_TS);
		papiReadScenario_double(line, "LVDC_ART", ART);
		papiReadScenario_double(line, "LVDC_Azimuth", Azimuth);
		papiReadScenario_double(line, "LVDC_AZO", Azo);
		papiReadScenario_double(line, "LVDC_AZS", Azs);
		papiReadScenario_double(line, "LVDC_B_11", B_11);
		papiReadScenario_double(line, "LVDC_B_12", B_12);
		papiReadScenario_double(line, "LVDC_B_21", B_21);
		papiReadScenario_double(line, "LVDC_B_22", B_22);
		papiReadScenario_double(line, "LVDC_beta", beta);
		papiReadScenario_double(line, "LVDC_BETAA", TABLE15[0].beta);
		papiReadScenario_double(line, "LVDC_BETAB", TABLE15[1].beta);
		papiReadScenario_double(line, "LVDC_BN4", EPTTIM[95]); //TB7, task 3
		papiReadScenario_double(line, "LVDC_C_2", C_2);
		papiReadScenario_double(line, "LVDC_C_3", C_3);
		papiReadScenario_double(line, "LVDC_C_4", C_4);
		papiReadScenario_double(line, "LVDC_C3A0", TABLE15[0].target[0].C_3);
		papiReadScenario_double(line, "LVDC_C3A1", TABLE15[0].target[1].C_3);
		papiReadScenario_double(line, "LVDC_C3A2", TABLE15[0].target[2].C_3);
		papiReadScenario_double(line, "LVDC_C3A3", TABLE15[0].target[3].C_3);
		papiReadScenario_double(line, "LVDC_C3A4", TABLE15[0].target[4].C_3);
		papiReadScenario_double(line, "LVDC_C3A5", TABLE15[0].target[5].C_3);
		papiReadScenario_double(line, "LVDC_C3A6", TABLE15[0].target[6].C_3);
		papiReadScenario_double(line, "LVDC_C3A7", TABLE15[0].target[7].C_3);
		papiReadScenario_double(line, "LVDC_C3A8", TABLE15[0].target[8].C_3);
		papiReadScenario_double(line, "LVDC_C3A9", TABLE15[0].target[9].C_3);
		papiReadScenario_double(line, "LVDC_C3A10", TABLE15[0].target[10].C_3);
		papiReadScenario_double(line, "LVDC_C3A11", TABLE15[0].target[11].C_3);
		papiReadScenario_double(line, "LVDC_C3A12", TABLE15[0].target[12].C_3);
		papiReadScenario_double(line, "LVDC_C3A13", TABLE15[0].target[13].C_3);
		papiReadScenario_double(line, "LVDC_C3A14", TABLE15[0].target[14].C_3);
		papiReadScenario_double(line, "LVDC_C3B0", TABLE15[1].target[0].C_3);
		papiReadScenario_double(line, "LVDC_C3B1", TABLE15[1].target[1].C_3);
		papiReadScenario_double(line, "LVDC_C3B2", TABLE15[1].target[2].C_3);
		papiReadScenario_double(line, "LVDC_C3B3", TABLE15[1].target[3].C_3);
		papiReadScenario_double(line, "LVDC_C3B4", TABLE15[1].target[4].C_3);
		papiReadScenario_double(line, "LVDC_C3B5", TABLE15[1].target[5].C_3);
		papiReadScenario_double(line, "LVDC_C3B6", TABLE15[1].target[6].C_3);
		papiReadScenario_double(line, "LVDC_C3B7", TABLE15[1].target[7].C_3);
		papiReadScenario_double(line, "LVDC_C3B8", TABLE15[1].target[8].C_3);
		papiReadScenario_double(line, "LVDC_C3B9", TABLE15[1].target[9].C_3);
		papiReadScenario_double(line, "LVDC_C3B10", TABLE15[1].target[10].C_3);
		papiReadScenario_double(line, "LVDC_C3B11", TABLE15[1].target[11].C_3);
		papiReadScenario_double(line, "LVDC_C3B12", TABLE15[1].target[12].C_3);
		papiReadScenario_double(line, "LVDC_C3B13", TABLE15[1].target[13].C_3);
		papiReadScenario_double(line, "LVDC_C3B14", TABLE15[1].target[14].C_3);
		//Remove these below at some point
		papiReadScenario_double(line, "LVDC_CCSA0", TABLE15[0].target[0].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA1", TABLE15[0].target[1].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA2", TABLE15[0].target[2].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA3", TABLE15[0].target[3].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA4", TABLE15[0].target[4].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA5", TABLE15[0].target[5].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA6", TABLE15[0].target[6].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA7", TABLE15[0].target[7].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA8", TABLE15[0].target[8].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA9", TABLE15[0].target[9].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA10", TABLE15[0].target[10].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA11", TABLE15[0].target[11].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA12", TABLE15[0].target[12].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA13", TABLE15[0].target[13].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSA14", TABLE15[0].target[14].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB0", TABLE15[1].target[0].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB1", TABLE15[1].target[1].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB2", TABLE15[1].target[2].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB3", TABLE15[1].target[3].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB4", TABLE15[1].target[4].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB5", TABLE15[1].target[5].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB6", TABLE15[1].target[6].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB7", TABLE15[1].target[7].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB8", TABLE15[1].target[8].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB9", TABLE15[1].target[9].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB10", TABLE15[1].target[10].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB11", TABLE15[1].target[11].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB12", TABLE15[1].target[12].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB13", TABLE15[1].target[13].cos_sigma);
		papiReadScenario_double(line, "LVDC_CCSB14", TABLE15[1].target[14].cos_sigma);
		//Below are the correct names
		papiReadScenario_double(line, "LVDC_COSA0", TABLE15[0].target[0].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA1", TABLE15[0].target[1].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA2", TABLE15[0].target[2].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA3", TABLE15[0].target[3].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA4", TABLE15[0].target[4].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA5", TABLE15[0].target[5].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA6", TABLE15[0].target[6].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA7", TABLE15[0].target[7].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA8", TABLE15[0].target[8].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA9", TABLE15[0].target[9].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA10", TABLE15[0].target[10].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA11", TABLE15[0].target[11].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA12", TABLE15[0].target[12].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA13", TABLE15[0].target[13].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSA14", TABLE15[0].target[14].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB0", TABLE15[1].target[0].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB1", TABLE15[1].target[1].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB2", TABLE15[1].target[2].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB3", TABLE15[1].target[3].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB4", TABLE15[1].target[4].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB5", TABLE15[1].target[5].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB6", TABLE15[1].target[6].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB7", TABLE15[1].target[7].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB8", TABLE15[1].target[8].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB9", TABLE15[1].target[9].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB10", TABLE15[1].target[10].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB11", TABLE15[1].target[11].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB12", TABLE15[1].target[12].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB13", TABLE15[1].target[13].cos_sigma);
		papiReadScenario_double(line, "LVDC_COSB14", TABLE15[1].target[14].cos_sigma);

		papiReadScenario_double(line, "LVDC_Cf", Cf);
		papiReadScenario_double(line, "LVDC_CG", CG);
		papiReadScenario_double(line, "LVDC_cos_chi_Yit", cos_chi_Yit);
		papiReadScenario_double(line, "LVDC_cos_chi_Zit", cos_chi_Zit);
		papiReadScenario_double(line, "LVDC_cos_sigma", cos_sigma);
		papiReadScenario_double(line, "LVDC_Ct", Ct);
		papiReadScenario_double(line, "LVDC_Ct_o", Ct_o);
		papiReadScenario_double(line, "LVDC_D", D);
		papiReadScenario_double(line, "LVDC_d2", d2);
		papiReadScenario_double(line, "LVDC_ddot_xi_GT", ddot_xi_GT);
		papiReadScenario_double(line, "LVDC_ddot_zeta_GT", ddot_zeta_GT);
		papiReadScenario_double(line, "LVDC_DEC", DEC);
		papiReadScenario_double(line, "LVDC_DECA0", TABLE15[0].target[0].DEC);
		papiReadScenario_double(line, "LVDC_DECA1", TABLE15[0].target[1].DEC);
		papiReadScenario_double(line, "LVDC_DECA2", TABLE15[0].target[2].DEC);
		papiReadScenario_double(line, "LVDC_DECA3", TABLE15[0].target[3].DEC);
		papiReadScenario_double(line, "LVDC_DECA4", TABLE15[0].target[4].DEC);
		papiReadScenario_double(line, "LVDC_DECA5", TABLE15[0].target[5].DEC);
		papiReadScenario_double(line, "LVDC_DECA6", TABLE15[0].target[6].DEC);
		papiReadScenario_double(line, "LVDC_DECA7", TABLE15[0].target[7].DEC);
		papiReadScenario_double(line, "LVDC_DECA8", TABLE15[0].target[8].DEC);
		papiReadScenario_double(line, "LVDC_DECA9", TABLE15[0].target[9].DEC);
		papiReadScenario_double(line, "LVDC_DECA10", TABLE15[0].target[10].DEC);
		papiReadScenario_double(line, "LVDC_DECA11", TABLE15[0].target[11].DEC);
		papiReadScenario_double(line, "LVDC_DECA12", TABLE15[0].target[12].DEC);
		papiReadScenario_double(line, "LVDC_DECA13", TABLE15[0].target[13].DEC);
		papiReadScenario_double(line, "LVDC_DECA14", TABLE15[0].target[14].DEC);
		papiReadScenario_double(line, "LVDC_DECB0", TABLE15[1].target[0].DEC);
		papiReadScenario_double(line, "LVDC_DECB1", TABLE15[1].target[1].DEC);
		papiReadScenario_double(line, "LVDC_DECB2", TABLE15[1].target[2].DEC);
		papiReadScenario_double(line, "LVDC_DECB3", TABLE15[1].target[3].DEC);
		papiReadScenario_double(line, "LVDC_DECB4", TABLE15[1].target[4].DEC);
		papiReadScenario_double(line, "LVDC_DECB5", TABLE15[1].target[5].DEC);
		papiReadScenario_double(line, "LVDC_DECB6", TABLE15[1].target[6].DEC);
		papiReadScenario_double(line, "LVDC_DECB7", TABLE15[1].target[7].DEC);
		papiReadScenario_double(line, "LVDC_DECB8", TABLE15[1].target[8].DEC);
		papiReadScenario_double(line, "LVDC_DECB9", TABLE15[1].target[9].DEC);
		papiReadScenario_double(line, "LVDC_DECB10", TABLE15[1].target[10].DEC);
		papiReadScenario_double(line, "LVDC_DECB11", TABLE15[1].target[11].DEC);
		papiReadScenario_double(line, "LVDC_DECB12", TABLE15[1].target[12].DEC);
		papiReadScenario_double(line, "LVDC_DECB13", TABLE15[1].target[13].DEC);
		papiReadScenario_double(line, "LVDC_DECB14", TABLE15[1].target[14].DEC);
		papiReadScenario_double(line, "LVDC_deta", deta);
		papiReadScenario_doublearr(line, "LVDC_DLTTL", DLTTL, 11);
		papiReadScenario_double(line, "LVDC_dxi", dxi);
		papiReadScenario_double(line, "LVDC_dot_dxi", dot_dxi);
		papiReadScenario_double(line, "LVDC_dot_deta", dot_deta);
		papiReadScenario_double(line, "LVDC_dot_dzeta", dot_dzeta);
		papiReadScenario_double(line, "LVDC_dot_dxit", dot_dxit);
		papiReadScenario_double(line, "LVDC_dot_detat", dot_detat);
		papiReadScenario_double(line, "LVDC_dot_dzetat", dot_dzetat);
		papiReadScenario_double(line, "LVDC_dotM_1", dotM_1);
		papiReadScenario_double(line, "LVDC_dotM_2", dotM_2);
		papiReadScenario_double(line, "LVDC_dotM_2R", dotM_2R);
		papiReadScenario_double(line, "LVDC_dotM_3", dotM_3);
		papiReadScenario_double(line, "LVDC_dotM_3R", dotM_3R);
		papiReadScenario_double(line, "LVDC_dot_phi_1", dot_phi_1);
		papiReadScenario_double(line, "LVDC_dot_phi_T", dot_phi_T);
		papiReadScenario_double(line, "LVDC_dot_zeta_T", dot_zeta_T);
		papiReadScenario_double(line, "LVDC_dot_xi_T", dot_xi_T);
		papiReadScenario_double(line, "LVDC_dot_eta_T", dot_eta_T);
		papiReadScenario_double(line, "LVDC_dT_3", dT_3);
		papiReadScenario_double(line, "LVDC_dT_4", dT_4);
		papiReadScenario_double(line, "LVDC_dt_c", dt_c);
		papiReadScenario_double(line, "LVDC_dT_cost", dT_cost);
		papiReadScenario_double(line, "LVDC_dT_F", dT_F);
		papiReadScenario_double(line, "LVDC_dt_g", dt_g);
		papiReadScenario_double(line, "LVDC_dT_ImpactBurn", dT_ImpactBurn);
		papiReadScenario_double(line, "LVDC_dt_LET", dt_LET);
		papiReadScenario_double(line, "LVDC_dT_LIM", dT_LIM);
		papiReadScenario_double(line, "LVDC_DT_N", DT_N);
		papiReadScenario_double(line, "LVDC_dtt_1", dtt_1);
		papiReadScenario_double(line, "LVDC_dtt_2", dtt_2);
		papiReadScenario_double(line, "LVDC_dTt_4", dTt_4);
		papiReadScenario_double(line, "LVDC_dV", dV);
		papiReadScenario_double(line, "LVDC_dV_B", dV_B);
		papiReadScenario_double(line, "LVDC_dV_BR", dV_BR);
		papiReadScenario_double(line, "LVDC_DVBRA", TABLE15[0].dV_BR);
		papiReadScenario_double(line, "LVDC_DVBRB", TABLE15[1].dV_BR);
		papiReadScenario_doublearr(line, "LVDC_DVCA", DVCA, 2);
		papiReadScenario_double(line, "LVDC_DVSST", DVSST);
		papiReadScenario_double(line, "LVDC_DVTGB", DVTGB);
		papiReadScenario_double(line, "LVDC_e", e);
		papiReadScenario_double(line, "LVDC_e_N", e_N);
		papiReadScenario_double(line, "LVDC_ENA0", TABLE15[0].target[0].e_N);
		papiReadScenario_double(line, "LVDC_ENA1", TABLE15[0].target[1].e_N);
		papiReadScenario_double(line, "LVDC_ENA2", TABLE15[0].target[2].e_N);
		papiReadScenario_double(line, "LVDC_ENA3", TABLE15[0].target[3].e_N);
		papiReadScenario_double(line, "LVDC_ENA4", TABLE15[0].target[4].e_N);
		papiReadScenario_double(line, "LVDC_ENA5", TABLE15[0].target[5].e_N);
		papiReadScenario_double(line, "LVDC_ENA6", TABLE15[0].target[6].e_N);
		papiReadScenario_double(line, "LVDC_ENA7", TABLE15[0].target[7].e_N);
		papiReadScenario_double(line, "LVDC_ENA8", TABLE15[0].target[8].e_N);
		papiReadScenario_double(line, "LVDC_ENA9", TABLE15[0].target[9].e_N);
		papiReadScenario_double(line, "LVDC_ENA10", TABLE15[0].target[10].e_N);
		papiReadScenario_double(line, "LVDC_ENA11", TABLE15[0].target[11].e_N);
		papiReadScenario_double(line, "LVDC_ENA12", TABLE15[0].target[12].e_N);
		papiReadScenario_double(line, "LVDC_ENA13", TABLE15[0].target[13].e_N);
		papiReadScenario_double(line, "LVDC_ENA14", TABLE15[0].target[14].e_N);
		papiReadScenario_double(line, "LVDC_ENB0", TABLE15[1].target[0].e_N);
		papiReadScenario_double(line, "LVDC_ENB1", TABLE15[1].target[1].e_N);
		papiReadScenario_double(line, "LVDC_ENB2", TABLE15[1].target[2].e_N);
		papiReadScenario_double(line, "LVDC_ENB3", TABLE15[1].target[3].e_N);
		papiReadScenario_double(line, "LVDC_ENB4", TABLE15[1].target[4].e_N);
		papiReadScenario_double(line, "LVDC_ENB5", TABLE15[1].target[5].e_N);
		papiReadScenario_double(line, "LVDC_ENB6", TABLE15[1].target[6].e_N);
		papiReadScenario_double(line, "LVDC_ENB7", TABLE15[1].target[7].e_N);
		papiReadScenario_double(line, "LVDC_ENB8", TABLE15[1].target[8].e_N);
		papiReadScenario_double(line, "LVDC_ENB9", TABLE15[1].target[9].e_N);
		papiReadScenario_double(line, "LVDC_ENB10", TABLE15[1].target[10].e_N);
		papiReadScenario_double(line, "LVDC_ENB11", TABLE15[1].target[11].e_N);
		papiReadScenario_double(line, "LVDC_ENB12", TABLE15[1].target[12].e_N);
		papiReadScenario_double(line, "LVDC_ENB13", TABLE15[1].target[13].e_N);
		papiReadScenario_double(line, "LVDC_ENB14", TABLE15[1].target[14].e_N);
		papiReadScenario_double(line, "LVDC_eps_1", eps_1);
		papiReadScenario_double(line, "LVDC_eps_1R", eps_1R);
		papiReadScenario_double(line, "LVDC_eps_2", eps_2);
		papiReadScenario_double(line, "LVDC_eps_2R", eps_2R);
		papiReadScenario_double(line, "LVDC_eps_3", eps_3);
		papiReadScenario_double(line, "LVDC_eps_3R", eps_3R);
		papiReadScenario_double(line, "LVDC_eps_4", eps_4);
		papiReadScenario_double(line, "LVDC_eps_4R", eps_4R);
		papiReadScenario_double(line, "LVDC_EPTB2Start", EPTTIM[11]);
		papiReadScenario_double(line, "LVDC_f", f);
		papiReadScenario_double(line, "LVDC_F", F);
		papiReadScenario_double(line, "LVDC_FA", TABLE15[0].f);
		papiReadScenario_double(line, "LVDC_FB", TABLE15[1].f);
		papiReadScenario_double(line, "LVDC_Fm", Fm);
		papiReadScenario_double(line, "LVDC_fx[0]", fx[0]);
		papiReadScenario_double(line, "LVDC_fx[1]", fx[1]);
		papiReadScenario_double(line, "LVDC_fx[2]", fx[2]);
		papiReadScenario_double(line, "LVDC_fx[3]", fx[3]);
		papiReadScenario_double(line, "LVDC_fx[4]", fx[4]);
		papiReadScenario_double(line, "LVDC_fx[5]", fx[5]);
		papiReadScenario_double(line, "LVDC_fx[6]", fx[6]);
		papiReadScenario_double(line, "LVDC_fxt[0]", fxt[0]);
		papiReadScenario_double(line, "LVDC_fxt[1]", fxt[1]);
		papiReadScenario_double(line, "LVDC_fxt[2]", fxt[2]);
		papiReadScenario_double(line, "LVDC_fxt[3]", fxt[3]);
		papiReadScenario_double(line, "LVDC_fxt[4]", fxt[4]);
		papiReadScenario_double(line, "LVDC_fxt[5]", fxt[5]);
		papiReadScenario_double(line, "LVDC_fxt[6]", fxt[6]);
		papiReadScenario_double(line, "LVDC_Fx[0][0]", Fx[0][0]);
		papiReadScenario_double(line, "LVDC_Fx[0][1]", Fx[0][1]);
		papiReadScenario_double(line, "LVDC_Fx[0][2]", Fx[0][2]);
		papiReadScenario_double(line, "LVDC_Fx[0][3]", Fx[0][3]);
		papiReadScenario_double(line, "LVDC_Fx[0][4]", Fx[0][4]);
		papiReadScenario_double(line, "LVDC_Fx[1][0]", Fx[1][0]);
		papiReadScenario_double(line, "LVDC_Fx[1][1]", Fx[1][1]);
		papiReadScenario_double(line, "LVDC_Fx[1][2]", Fx[1][2]);
		papiReadScenario_double(line, "LVDC_Fx[1][3]", Fx[1][3]);
		papiReadScenario_double(line, "LVDC_Fx[1][4]", Fx[1][4]);
		papiReadScenario_double(line, "LVDC_Fx[2][0]", Fx[2][0]);
		papiReadScenario_double(line, "LVDC_Fx[2][1]", Fx[2][1]);
		papiReadScenario_double(line, "LVDC_Fx[2][2]", Fx[2][2]);
		papiReadScenario_double(line, "LVDC_Fx[2][3]", Fx[2][3]);
		papiReadScenario_double(line, "LVDC_Fx[2][4]", Fx[2][4]);
		papiReadScenario_double(line, "LVDC_Fx[3][0]", Fx[3][0]);
		papiReadScenario_double(line, "LVDC_Fx[3][1]", Fx[3][1]);
		papiReadScenario_double(line, "LVDC_Fx[3][2]", Fx[3][2]);
		papiReadScenario_double(line, "LVDC_Fx[3][3]", Fx[3][3]);
		papiReadScenario_double(line, "LVDC_Fx[3][4]", Fx[3][4]);
		papiReadScenario_double(line, "LVDC_Fx[4][0]", Fx[4][0]);
		papiReadScenario_double(line, "LVDC_Fx[4][1]", Fx[4][1]);
		papiReadScenario_double(line, "LVDC_Fx[4][2]", Fx[4][2]);
		papiReadScenario_double(line, "LVDC_Fx[4][3]", Fx[4][3]);
		papiReadScenario_double(line, "LVDC_Fx[4][4]", Fx[4][4]);
		papiReadScenario_double(line, "LVDC_G_T", G_T);
		papiReadScenario_double(line, "LVDC_gx[0]", gx[0]);
		papiReadScenario_double(line, "LVDC_gx[1]", gx[1]);
		papiReadScenario_double(line, "LVDC_gx[2]", gx[2]);
		papiReadScenario_double(line, "LVDC_gx[3]", gx[3]);
		papiReadScenario_double(line, "LVDC_gx[4]", gx[4]);
		papiReadScenario_double(line, "LVDC_gx[5]", gx[5]);
		papiReadScenario_double(line, "LVDC_gx[6]", gx[6]);
		papiReadScenario_double(line, "LVDC_gxt[0]", gxt[0]);
		papiReadScenario_double(line, "LVDC_gxt[1]", gxt[1]);
		papiReadScenario_double(line, "LVDC_gxt[2]", gxt[2]);
		papiReadScenario_double(line, "LVDC_gxt[3]", gxt[3]);
		papiReadScenario_double(line, "LVDC_gxt[4]", gxt[4]);
		papiReadScenario_double(line, "LVDC_gxt[5]", gxt[5]);
		papiReadScenario_double(line, "LVDC_gxt[6]", gxt[6]);
		papiReadScenario_double(line, "LVDC_H", H);
		papiReadScenario_double(line, "LVDC_h_1", h_1);
		papiReadScenario_double(line, "LVDC_h_2", h_2);
		papiReadScenario_double(line, "LVDC_hx[0][0]", hx[0][0]);
		papiReadScenario_double(line, "LVDC_hx[0][1]", hx[0][1]);
		papiReadScenario_double(line, "LVDC_hx[0][2]", hx[0][2]);
		papiReadScenario_double(line, "LVDC_hx[0][3]", hx[0][3]);
		papiReadScenario_double(line, "LVDC_hx[0][4]", hx[0][4]);
		papiReadScenario_double(line, "LVDC_hx[1][0]", hx[1][0]);
		papiReadScenario_double(line, "LVDC_hx[1][1]", hx[1][1]);
		papiReadScenario_double(line, "LVDC_hx[1][2]", hx[1][2]);
		papiReadScenario_double(line, "LVDC_hx[1][3]", hx[1][3]);
		papiReadScenario_double(line, "LVDC_hx[1][4]", hx[1][4]);
		papiReadScenario_double(line, "LVDC_hx[2][0]", hx[2][0]);
		papiReadScenario_double(line, "LVDC_hx[2][1]", hx[2][1]);
		papiReadScenario_double(line, "LVDC_hx[2][2]", hx[2][2]);
		papiReadScenario_double(line, "LVDC_hx[2][3]", hx[2][3]);
		papiReadScenario_double(line, "LVDC_hx[2][4]", hx[2][4]);
		papiReadScenario_double(line, "LVDC_Inclination", Inclination);
		papiReadScenario_double(line, "LVDC_J", J);
		papiReadScenario_double(line, "LVDC_J_1", J_1);
		papiReadScenario_double(line, "LVDC_J_2", J_2);
		papiReadScenario_double(line, "LVDC_J_3", J_3);
		papiReadScenario_double(line, "LVDC_J_12", J_12);
		papiReadScenario_double(line, "LVDC_Jt_3", Jt_3);
		papiReadScenario_double(line, "LVDC_J_P", J_P);
		papiReadScenario_double(line, "LVDC_J_Y", J_Y);
		papiReadScenario_double(line, "LVDC_K_1", K_1);
		papiReadScenario_double(line, "LVDC_K_2", K_2);
		papiReadScenario_double(line, "LVDC_K_3", K_3);
		papiReadScenario_double(line, "LVDC_K_4", K_4);
		papiReadScenario_double(line, "LVDC_K_5", K_5);
		papiReadScenario_double(line, "LVDC_K_D", K_D);
		papiReadScenario_double(line, "LVDC_K_P1", K_P1);
		papiReadScenario_double(line, "LVDC_K_P2", K_P2);
		papiReadScenario_double(line, "LVDC_KSCLNG", KSCLNG);
		papiReadScenario_double(line, "LVDC_K_T3", K_T3);
		papiReadScenario_double(line, "LVDC_K_Y1", K_Y1);
		papiReadScenario_double(line, "LVDC_K_Y2", K_Y2);
		papiReadScenario_double(line, "LVDC_K_P", K_P);
		papiReadScenario_double(line, "LVDC_K_Y", K_Y);
		papiReadScenario_double(line, "LVDC_D_P", D_P);
		papiReadScenario_double(line, "LVDC_D_Y", D_Y);
		papiReadScenario_double(line, "LVDC_L_1", L_1);
		papiReadScenario_double(line, "LVDC_L_2", L_2);
		papiReadScenario_double(line, "LVDC_L_3", L_3);
		papiReadScenario_double(line, "LVDC_dL_3", dL_3);
		papiReadScenario_double(line, "LVDC_Lt_3", Lt_3);
		papiReadScenario_double(line, "LVDC_L_12", L_12);
		papiReadScenario_double(line, "LVDC_L_P", L_P);
		papiReadScenario_double(line, "LVDC_L_Y", L_Y);
		papiReadScenario_double(line, "LVDC_Lt_Y", Lt_Y);
		papiReadScenario_double(line, "LVDC_LVDC_TB_ETime", LVDC_TB_ETime);
		papiReadScenario_doublearr(line, "LVDC_MF", MF, 5);
		papiReadScenario_double(line, "LVDC_MFS", MFS);
		papiReadScenario_doublearr(line, "LVDC_MFSArr", MFSArr, 4);
		papiReadScenario_double(line, "LVDC_MLD", MLD);
		papiReadScenario_double(line, "LVDC_MLR", MLR);
		papiReadScenario_double(line, "LVDC_MLT", MLT);
		papiReadScenario_double(line, "LVDC_MS04DT", MS04DT);
		papiReadScenario_double(line, "LVDC_MS25DT", MS25DT);
		papiReadScenario_double(line, "LVDC_MSK5", MSK5);
		papiReadScenario_double(line, "LVDC_MSK6", MSK6);
		papiReadScenario_double(line, "LVDC_MSK16", MSK16);
		papiReadScenario_double(line, "LVDC_MSLIM1", MSLIM1);
		papiReadScenario_double(line, "LVDC_MSLIM2", MSLIM2);
		papiReadScenario_double(line, "LVDC_mu", mu);
		papiReadScenario_double(line, "LVDC_NUPTIM", NUPTIM);
		papiReadScenario_double(line, "LVDC_omega_E", omega_E);
		papiReadScenario_double(line, "LVDC_p", p);
		papiReadScenario_double(line, "LVDC_P_1", P_1);
		papiReadScenario_double(line, "LVDC_P_2", P_2);
		papiReadScenario_double(line, "LVDC_P_3", P_3);
		papiReadScenario_double(line, "LVDC_P_12", P_12);
		papiReadScenario_double(line, "LVDC_PHI", PHI);
		papiReadScenario_double(line, "LVDC_PHIP", PHIP);
		papiReadScenario_double(line, "LVDC_phi_T", phi_T);
		papiReadScenario_double(line, "LVDC_Q_1", Q_1);
		papiReadScenario_double(line, "LVDC_Q_2", Q_2);
		papiReadScenario_double(line, "LVDC_Q_3", Q_3);
		papiReadScenario_double(line, "LVDC_Q_12", Q_12);
		papiReadScenario_double(line, "LVDC_Q_P", Q_P);
		papiReadScenario_double(line, "LVDC_Q_Y", Q_Y);
		papiReadScenario_double(line, "LVDC_R", R);
		papiReadScenario_double(line, "LVDC_RAS", RAS);
		papiReadScenario_double(line, "LVDC_RASA0", TABLE15[0].target[0].RAS);
		papiReadScenario_double(line, "LVDC_RASA1", TABLE15[0].target[1].RAS);
		papiReadScenario_double(line, "LVDC_RASA2", TABLE15[0].target[2].RAS);
		papiReadScenario_double(line, "LVDC_RASA3", TABLE15[0].target[3].RAS);
		papiReadScenario_double(line, "LVDC_RASA4", TABLE15[0].target[4].RAS);
		papiReadScenario_double(line, "LVDC_RASA5", TABLE15[0].target[5].RAS);
		papiReadScenario_double(line, "LVDC_RASA6", TABLE15[0].target[6].RAS);
		papiReadScenario_double(line, "LVDC_RASA7", TABLE15[0].target[7].RAS);
		papiReadScenario_double(line, "LVDC_RASA8", TABLE15[0].target[8].RAS);
		papiReadScenario_double(line, "LVDC_RASA9", TABLE15[0].target[9].RAS);
		papiReadScenario_double(line, "LVDC_RASA10", TABLE15[0].target[10].RAS);
		papiReadScenario_double(line, "LVDC_RASA11", TABLE15[0].target[11].RAS);
		papiReadScenario_double(line, "LVDC_RASA12", TABLE15[0].target[12].RAS);
		papiReadScenario_double(line, "LVDC_RASA13", TABLE15[0].target[13].RAS);
		papiReadScenario_double(line, "LVDC_RASA14", TABLE15[0].target[14].RAS);
		papiReadScenario_double(line, "LVDC_RASB0", TABLE15[1].target[0].RAS);
		papiReadScenario_double(line, "LVDC_RASB1", TABLE15[1].target[1].RAS);
		papiReadScenario_double(line, "LVDC_RASB2", TABLE15[1].target[2].RAS);
		papiReadScenario_double(line, "LVDC_RASB3", TABLE15[1].target[3].RAS);
		papiReadScenario_double(line, "LVDC_RASB4", TABLE15[1].target[4].RAS);
		papiReadScenario_double(line, "LVDC_RASB5", TABLE15[1].target[5].RAS);
		papiReadScenario_double(line, "LVDC_RASB6", TABLE15[1].target[6].RAS);
		papiReadScenario_double(line, "LVDC_RASB7", TABLE15[1].target[7].RAS);
		papiReadScenario_double(line, "LVDC_RASB8", TABLE15[1].target[8].RAS);
		papiReadScenario_double(line, "LVDC_RASB9", TABLE15[1].target[9].RAS);
		papiReadScenario_double(line, "LVDC_RASB10", TABLE15[1].target[10].RAS);
		papiReadScenario_double(line, "LVDC_RASB11", TABLE15[1].target[11].RAS);
		papiReadScenario_double(line, "LVDC_RASB12", TABLE15[1].target[12].RAS);
		papiReadScenario_double(line, "LVDC_RASB13", TABLE15[1].target[13].RAS);
		papiReadScenario_double(line, "LVDC_RASB14", TABLE15[1].target[14].RAS);
		papiReadScenario_double(line, "LVDC_RealTimeClock", RealTimeClock);
		papiReadScenario_double(line, "LVDC_R_L", R_L);
		papiReadScenario_double(line, "LVDC_R_N", R_N);
		papiReadScenario_double(line, "LVDC_RNA", TABLE15[0].R_N);
		papiReadScenario_double(line, "LVDC_RNB", TABLE15[1].R_N);
		papiReadScenario_double(line, "LVDC_ROV", ROV);
		papiReadScenario_double(line, "LVDC_ROVR", ROVR);
		papiReadScenario_double(line, "LVDC_ROVs", ROVs);
		papiReadScenario_double(line, "LVDC_R_T", R_T);
		papiReadScenario_double(line, "LVDC_S_1", S_1);
		papiReadScenario_double(line, "LVDC_S_2", S_2);
		papiReadScenario_double(line, "LVDC_S_3", S_3);
		papiReadScenario_double(line, "LVDC_S_12", S_12);
		papiReadScenario_double(line, "LVDC_S_P", S_P);
		papiReadScenario_double(line, "LVDC_S_Y", S_Y);
		papiReadScenario_double(line, "LVDC_sinceLastCycle", sinceLastCycle);
		papiReadScenario_double(line, "LVDC_sin_chi_Yit", sin_chi_Yit);
		papiReadScenario_double(line, "LVDC_sin_chi_Zit", sin_chi_Zit);
		papiReadScenario_double(line, "LVDC_sin_gam", sin_gam);
		papiReadScenario_double(line, "LVDC_cos_gam", cos_gam);
		papiReadScenario_double(line, "LVDC_SMCY", SMCY);
		papiReadScenario_double(line, "LVDC_SMCZ", SMCZ);
		papiReadScenario_double(line, "LVDC_S", S);
		papiReadScenario_double(line, "LVDC_P", P);
		papiReadScenario_double(line, "LVDC_S_34", S_34);
		papiReadScenario_double(line, "LVDC_P_34", P_34);
		papiReadScenario_double(line, "LVDC_T_0", T_0);
		papiReadScenario_double(line, "LVDC_t_1", t_1);
		papiReadScenario_double(line, "LVDC_T_1", T_1);
		papiReadScenario_double(line, "LVDC_T_1c", T_1c);
		papiReadScenario_double(line, "LVDC_t_2", t_2);
		papiReadScenario_double(line, "LVDC_T_2", T_2);
		papiReadScenario_double(line, "LVDC_t_21", t_21);
		papiReadScenario_double(line, "LVDC_T_2R", T_2R);
		papiReadScenario_double(line, "LVDC_t_3", t_3);
		papiReadScenario_double(line, "LVDC_T_3", T_3);
		papiReadScenario_double(line, "LVDC_t_3i", t_3i);
		papiReadScenario_double(line, "LVDC_t_4", t_4);
		papiReadScenario_double(line, "LVDC_T_4N", T_4N);
		papiReadScenario_double(line, "LVDC_t_5", t_5);
		papiReadScenario_double(line, "LVDC_T2IR", TABLE15[0].T2IR);
		papiReadScenario_double(line, "LVDC_T2IRB", TABLE15[1].T2IR);
		papiReadScenario_double(line, "LVDC_T3PRA", TABLE15[0].T3PR);
		papiReadScenario_double(line, "LVDC_T3PRB", TABLE15[1].T3PR);
		papiReadScenario_double(line, "LVDC_TI5F3", TI5F3);
		papiReadScenario_double(line, "LVDC_TI5F4", TI5F4);
		papiReadScenario_double(line, "LVDC_TI5F5", TI5F5);
		papiReadScenario_double(line, "LVDC_T_ar", T_ar);
		papiReadScenario_double(line, "LVDC_TAS", TAS);
		papiReadScenario_double(line, "LVDC_tau1", tau1);
		papiReadScenario_double(line, "LVDC_tau2", tau2);
		papiReadScenario_double(line, "LVDC_tau2N", tau2N);
		papiReadScenario_double(line, "LVDC_tau3", tau3);
		papiReadScenario_double(line, "LVDC_tau3N", tau3N);
		papiReadScenario_double(line, "LVDC_tau3R", tau3R);
		papiReadScenario_double(line, "LVDC_TAU3RA", TABLE15[0].TAU3R);
		papiReadScenario_double(line, "LVDC_TAU3RB", TABLE15[1].TAU3R);
		papiReadScenario_double(line, "LVDC_t_B1", t_B1);
		papiReadScenario_double(line, "LVDC_TB1", TB1);
		papiReadScenario_double(line, "LVDC_t_B2", t_B2);
		papiReadScenario_double(line, "LVDC_TB2", TB2);
		papiReadScenario_double(line, "LVDC_t_B3", t_B3);
		papiReadScenario_double(line, "LVDC_TB3", TB3);
		papiReadScenario_double(line, "LVDC_t_B4", t_B4);
		papiReadScenario_double(line, "LVDC_TB4", TB4);
		papiReadScenario_double(line, "LVDC_TB4a", TB4a);
		papiReadScenario_double(line, "LVDC_TB5", TB5);
		papiReadScenario_double(line, "LVDC_TB6", TB6);
		papiReadScenario_double(line, "LVDC_TB7", TB7);
		papiReadScenario_double(line, "LVDC_TB8", TB8);
		papiReadScenario_double(line, "LVDC_TBA", TBA);
		papiReadScenario_double(line, "LVDC_TBL", TBL);
		papiReadScenario_double(line, "LVDC_T_c", T_c);
		papiReadScenario_double(line, "LVDC_tchi_p_last", tchi_p_last);
		papiReadScenario_double(line, "LVDC_tchi_y_last", tchi_y_last);
		papiReadScenario_double(line, "LVDC_tchi_p", tchi_p);
		papiReadScenario_double(line, "LVDC_tchi_y", tchi_y);
		papiReadScenario_double(line, "LVDC_t_clock", t_clock);
		papiReadScenario_double(line, "LVDC_T_CO", T_CO);
		papiReadScenario_double(line, "LVDC_t_D", t_D);
		papiReadScenario_double(line, "LVDC_t_D0", t_D0);
		papiReadScenario_double(line, "LVDC_t_D1", t_D1);
		papiReadScenario_double(line, "LVDC_t_D2", t_D2);
		papiReadScenario_double(line, "LVDC_t_D3", t_D3);
		papiReadScenario_double(line, "LVDC_t_DS0", t_DS0);
		papiReadScenario_double(line, "LVDC_t_DS1", t_DS1);
		papiReadScenario_double(line, "LVDC_t_DS2", t_DS2);
		papiReadScenario_double(line, "LVDC_t_DS3", t_DS3);
		papiReadScenario_double(line, "LVDC_T_EO1", T_EO1);
		papiReadScenario_double(line, "LVDC_T_GO", T_GO);
		papiReadScenario_double(line, "LVDC_TETEO", theta_EO); //Remove this at some point, TETEO was a typo. THTEO is correct.
		papiReadScenario_double(line, "LVDC_THTEO", theta_EO);
		papiReadScenario_double(line, "LVDC_theta_N", theta_N);
		papiReadScenario_double(line, "LVDC_TI", TI);
		papiReadScenario_double(line, "LVDC_TI5F2", EPTTIM[57]); //Events Processor: TB5, event 3
		papiReadScenario_double(line, "LVDC_TI7AF1", EPTTIM[107]); //Events Processor: TB8, event 1
		papiReadScenario_double(line, "LVDC_TI7AF2", EPTTIM[108]); //Events Processor: TB8, event 2
		papiReadScenario_double(line, "LVDC_T7M9", EPTTIM[94]); //TB7, task 2
		papiReadScenario_double(line, "LVDC_T7M10", T7M10);
		papiReadScenario_double(line, "LVDC_T7M11", T7M11);
		papiReadScenario_double(line, "LVDC_T_ImpactBurn", T_ImpactBurn);
		papiReadScenario_double(line, "LVDC_T_L", T_L);
		papiReadScenario_double(line, "LVDC_T_LO", T_LO);
		papiReadScenario_double(line, "LVDC_TMEFRZ", TMEFRZ);
		papiReadScenario_double(line, "LVDC_TMM", TMM);
		papiReadScenario_double(line, "LVDC_TMR", TMR);
		papiReadScenario_double(line, "LVDC_TPA0", TABLE15[0].target[0].t_D);
		papiReadScenario_double(line, "LVDC_TPA1", TABLE15[0].target[1].t_D);
		papiReadScenario_double(line, "LVDC_TPA2", TABLE15[0].target[2].t_D);
		papiReadScenario_double(line, "LVDC_TPA3", TABLE15[0].target[3].t_D);
		papiReadScenario_double(line, "LVDC_TPA4", TABLE15[0].target[4].t_D);
		papiReadScenario_double(line, "LVDC_TPA5", TABLE15[0].target[5].t_D);
		papiReadScenario_double(line, "LVDC_TPA6", TABLE15[0].target[6].t_D);
		papiReadScenario_double(line, "LVDC_TPA7", TABLE15[0].target[7].t_D);
		papiReadScenario_double(line, "LVDC_TPA8", TABLE15[0].target[8].t_D);
		papiReadScenario_double(line, "LVDC_TPA9", TABLE15[0].target[9].t_D);
		papiReadScenario_double(line, "LVDC_TPA10", TABLE15[0].target[10].t_D);
		papiReadScenario_double(line, "LVDC_TPA11", TABLE15[0].target[11].t_D);
		papiReadScenario_double(line, "LVDC_TPA12", TABLE15[0].target[12].t_D);
		papiReadScenario_double(line, "LVDC_TPA13", TABLE15[0].target[13].t_D);
		papiReadScenario_double(line, "LVDC_TPA14", TABLE15[0].target[14].t_D);
		papiReadScenario_double(line, "LVDC_TPB0", TABLE15[1].target[0].t_D);
		papiReadScenario_double(line, "LVDC_TPB1", TABLE15[1].target[1].t_D);
		papiReadScenario_double(line, "LVDC_TPB2", TABLE15[1].target[2].t_D);
		papiReadScenario_double(line, "LVDC_TPB3", TABLE15[1].target[3].t_D);
		papiReadScenario_double(line, "LVDC_TPB4", TABLE15[1].target[4].t_D);
		papiReadScenario_double(line, "LVDC_TPB5", TABLE15[1].target[5].t_D);
		papiReadScenario_double(line, "LVDC_TPB6", TABLE15[1].target[6].t_D);
		papiReadScenario_double(line, "LVDC_TPB7", TABLE15[1].target[7].t_D);
		papiReadScenario_double(line, "LVDC_TPB8", TABLE15[1].target[8].t_D);
		papiReadScenario_double(line, "LVDC_TPB9", TABLE15[1].target[9].t_D);
		papiReadScenario_double(line, "LVDC_TPB10", TABLE15[1].target[10].t_D);
		papiReadScenario_double(line, "LVDC_TPB11", TABLE15[1].target[11].t_D);
		papiReadScenario_double(line, "LVDC_TPB12", TABLE15[1].target[12].t_D);
		papiReadScenario_double(line, "LVDC_TPB13", TABLE15[1].target[13].t_D);
		papiReadScenario_double(line, "LVDC_TPB14", TABLE15[1].target[14].t_D);
		papiReadScenario_double(line, "LVDC_T_RP", T_RP);
		papiReadScenario_double(line, "LVDC_T_S1", T_S1);
		papiReadScenario_double(line, "LVDC_T_S2", T_S2);
		papiReadScenario_double(line, "LVDC_T_S3", T_S3);
		papiReadScenario_double(line, "LVDC_TS4BS", TS4BS);
		papiReadScenario_double(line, "LVDC_t_SD1", t_SD1);
		papiReadScenario_double(line, "LVDC_t_SD2", t_SD2);
		papiReadScenario_double(line, "LVDC_t_SD3", t_SD3);
		papiReadScenario_double(line, "LVDC_T_ST", T_ST);
		papiReadScenario_double(line, "LVDC_TSTA", TABLE15[0].T_ST);
		papiReadScenario_double(line, "LVDC_TSTB", TABLE15[1].T_ST);
		papiReadScenario_double(line, "LVDC_T_T", T_T);
		papiReadScenario_double(line, "LVDC_t_TB8Start", t_TB8Start);
		papiReadScenario_double(line, "LVDC_Tt_3", Tt_3);
		papiReadScenario_double(line, "LVDC_Tt_3R", Tt_3R);
		papiReadScenario_double(line, "LVDC_Tt_T", Tt_T);
		papiReadScenario_double(line, "LVDC_TVRATE", TVRATE);
		papiReadScenario_double(line, "LVDC_U_1", U_1);
		papiReadScenario_double(line, "LVDC_U_2", U_2);
		papiReadScenario_double(line, "LVDC_U_3", U_3);
		papiReadScenario_double(line, "LVDC_U_12", U_12);
		papiReadScenario_double(line, "LVDC_gamma_T", gamma_T);
		papiReadScenario_double(line, "LVDC_V", V);
		papiReadScenario_double(line, "LVDC_V_ex1", V_ex1);
		papiReadScenario_double(line, "LVDC_V_ex2", V_ex2);
		papiReadScenario_double(line, "LVDC_V_ex2R", V_ex2R);
		papiReadScenario_double(line, "LVDC_V_ex3", V_ex3);
		papiReadScenario_double(line, "LVDC_V_ex3R", V_ex3R);
		papiReadScenario_double(line, "LVDC_V_i", V_i);
		papiReadScenario_double(line, "LVDC_V_0", V_0);
		papiReadScenario_double(line, "LVDC_V_1", V_1);
		papiReadScenario_double(line, "LVDC_V_2", V_2);
		papiReadScenario_double(line, "LVDC_VATR4", VATR4);
		papiReadScenario_double(line, "LVDC_VATRR", VATRR);
		papiReadScenario_double(line, "LVDC_VCCYA", VCCYA);
		papiReadScenario_double(line, "LVDC_VCCZA", VCCZA);
		papiReadScenario_doublearr(line, "LVDC_VENTA", VENTA, 7);
		papiReadScenario_double(line, "LVDC_VGBIA", VGBIA);
		papiReadScenario_double(line, "LVDC_VOLD", VOLD);
		papiReadScenario_double(line, "LVDC_V_S2T", V_S2T);
		papiReadScenario_double(line, "LVDC_VSC12", VSC12);
		papiReadScenario_double(line, "LVDC_VSC32", VSC32);
		papiReadScenario_double(line, "LVDC_VSSRT", VSSRT);
		papiReadScenario_double(line, "LVDC_VSSW", VSSW);
		papiReadScenario_double(line, "LVDC_V_T", V_T);
		papiReadScenario_double(line, "LVDC_V_TC", V_TC);
		papiReadScenario_doublearr(line, "LVDC_VTIM", VTIM, 6);
		papiReadScenario_double(line, "LVDC_VTOLD", VTOLD);
		papiReadScenario_double(line, "LVDC_xi_T", xi_T);

		// VECTOR3
		papiReadScenario_vec(line, "LVDC_AttitudeError", AttitudeError);
		papiReadScenario_vec(line, "LVDC_AttitudeErrorOld", AttitudeErrorOld);
		papiReadScenario_vec(line, "LVDC_CommandedAttitude", CommandedAttitude);
		papiReadScenario_vec(line, "LVDC_CurrentAttitude", CurrentAttitude);
		papiReadScenario_vec(line, "LVDC_DChi_apo", DChi_apo);
		papiReadScenario_vec(line, "LVDC_ddotG_act", ddotG_act);
		papiReadScenario_vec(line, "LVDC_ddotG_last", ddotG_last);
		papiReadScenario_vec(line, "LVDC_ddotS", ddotS);
		papiReadScenario_vec(line, "LVDC_DDotXEZ_G", DDotXEZ_G);
		papiReadScenario_vec(line, "LVDC_DotG_act", DotG_act);
		papiReadScenario_vec(line, "LVDC_DotG_last", DotG_last);
		papiReadScenario_vec(line, "LVDC_DotM_act", DotM_act);
		papiReadScenario_vec(line, "LVDC_DotM_last", DotM_last);
		papiReadScenario_vec(line, "LVDC_ddotM_act", ddotM_act);
		papiReadScenario_vec(line, "LVDC_DotS", DotS);
		papiReadScenario_vec(line, "LVDC_DotXEZ", DotXEZ);
		papiReadScenario_vec(line, "LVDC_PCommandedAttitude", PCommandedAttitude);
		papiReadScenario_vec(line, "LVDC_Pos4", Pos4);
		papiReadScenario_vec(line, "LVDC_Pos_Nav", Pos_Nav);
		papiReadScenario_vec(line, "LVDC_PosS", PosS);
		papiReadScenario_vec(line, "LVDC_PosXEZ", PosXEZ);
		papiReadScenario_vec(line, "LVDC_R_OG", R_OG);
		papiReadScenario_vec(line, "LVDC_Sbar", Sbar);
		papiReadScenario_vec(line, "LVDC_TargetVector", TargetVector);
		papiReadScenario_vec(line, "LVDC_T_P", T_P);
		papiReadScenario_vec(line, "LVDC_Vel_Nav", Vel_Nav);
		papiReadScenario_vec(line, "LVDC_W_ES", W_ES);
		papiReadScenario_vec(line, "LVDC_VOAC", VOAC);
		papiReadScenario_vec(line, "LVDC_XLunarAttitude", XLunarAttitude);
		papiReadScenario_vec(line, "LVDC_XLunarCommAttitude", XLunarCommAttitude);
		papiReadScenario_vec(line, "LVDC_XLunarSlingshotAttitude", XLunarSlingshotAttitude);

		// MATRIX3
		papiReadScenario_mat(line, "LVDC_MX_A", MX_A);
		papiReadScenario_mat(line, "LVDC_MX_B", MX_B);
		papiReadScenario_mat(line, "LVDC_MX_G", MX_G);
		papiReadScenario_mat(line, "LVDC_MX_K", MX_K);
		papiReadScenario_mat(line, "LVDC_MX_phi_T", MX_phi_T);
		// Done
	}

	ReadFlightSequenceProgram(FSPFileName);

	//For old scenarios
	if (RealTimeClock == 0.0)
	{
		double xtemp;
		RealTimeClock = modf(oapiGetSimMJD(), &xtemp)*24.0*3600.0;
	}

	return;
}

void LVDCSV::ReadFlightSequenceProgram(char *fspfile)
{
	using namespace std;

	SwitchSelectorSet ssset;
	string line;
	ifstream file(fspfile);

	SSTABLE.clear();

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (line.compare("END") == 0)
			{
				break;
			}
			else if ((line.substr(0, 3)).compare("SST") == 0)
			{
				if (line.compare("SSTTB1") == 0)
				{
					SSTTBPTR[0] = SSTABLE.size();
				}
				else if (line.compare("SSTTB2") == 0)
				{
					SSTTBPTR[1] = SSTABLE.size();
				}
				else if (line.compare("SSTTB3") == 0)
				{
					SSTTBPTR[2] = SSTABLE.size();
				}
				else if (line.compare("SSTTB4") == 0)
				{
					SSTTBPTR[3] = SSTABLE.size();
				}
				else if (line.compare("SSTTB5") == 0)
				{
					SSTTBPTR[4] = SSTABLE.size();
				}
				else if (line.compare("SSTTB6") == 0)
				{
					SSTTBPTR[5] = SSTABLE.size();
				}
				else if (line.compare("SSTTB7") == 0)
				{
					SSTTBPTR[6] = SSTABLE.size();
				}
				else if (line.compare("SSTTB8") == 0)
				{
					SSTTBPTR[7] = SSTABLE.size();
				}
				else if (line.compare("SSTTB3A") == 0)
				{
					KSSINDXTB3A = SSTABLE.size();
				}
				else if (line.compare("SSTS4C1") == 0)
				{
					KSSINDXS4C1 = SSTABLE.size();
				}
				else if (line.compare("SSTTB4A") == 0)
				{
					SSTTBPTR[8] = SSTABLE.size();
				}
				else if (line.compare("SSTTB6A") == 0)
				{
					KSSINDXTB6A = SSTABLE.size();
				}
				else if (line.compare("SSTTB6B") == 0)
				{
					KSSINDXTB6B = SSTABLE.size();
				}
				else if (line.compare("SSTTB6C") == 0)
				{
					KSSINDXTB6C = SSTABLE.size();
				}
				else if (line.compare("SSTTB6D") == 0)
				{
					KSSINDXTB6D = SSTABLE.size();
				}
				else if (line.compare("SSTGSS") == 0)
				{
					KSSINDXGSS = SSTABLE.size();
				}
				else if (line.compare("SSTSIVA") == 0)
				{
					KSSINDXSIVA = SSTABLE.size();
				}
				else if (line.compare("SSTSIVB") == 0)
				{
					KSSINDXSIVB = SSTABLE.size();
				}
				else if (line.compare("SSTECS1") == 0)
				{
					KSSINDXECS1 = SSTABLE.size();
				}
				else if (line.compare("SSTECSV") == 0)
				{
					KSSINDXECSV = SSTABLE.size();
				}
				else if (line.compare("SSTSBHI") == 0)
				{
					KSSINDXSBHI = SSTABLE.size();
				}
				else if (line.compare("SSTSBLO") == 0)
				{
					KSSINDXSBLO = SSTABLE.size();
				}
				else if (line.compare("SSTSBOM") == 0)
				{
					KSSINDXSBOM = SSTABLE.size();
				}
				else if (line.compare("SSTTB5A") == 0)
				{
					KSSINDXTB5A = SSTABLE.size();
				}
				else if (line.compare("SSTTB5B") == 0)
				{
					KSSINDXTB5B = SSTABLE.size();
				}
				else if (line.compare("SSTGAIN") == 0)
				{
					KSSINDXGAIN = SSTABLE.size();
				}
				else if (line.compare("SSTALU") == 0)
				{
					KSSINDXALU = SSTABLE.size();
				}
				else if (line.compare("SSTTB7A") == 0)
				{
					KSSINDXTB7A = SSTABLE.size();
				}
			}
			else if (sscanf(line.c_str(), "%lf,%d,%d", &ssset.time, &ssset.stage, &ssset.channel) == 3)
			{
				SSTABLE.push_back(ssset);
			}
		}
	}

	file.close();
}

void LVDCSV::TimeStep(double simdt) {
	if (Initialized == false) { return; }
	if (lvda.GetStage() < PRELAUNCH_STAGE) { return; }

	// Is the LVDC running?
	if(LVDC_Stop == 0){

		//Update real time clock
		RealTimeClock += simdt;

		//LVDC PREFLIGHT PROGRAM
		if (LVDC_Timebase == -1)
		{
			// Limit time accel to 100x
			if (oapiGetTimeAcceleration() > 100) { oapiSetTimeAcceleration(100); }

			// BEFORE PTL COMMAND (T-00:20:00) STOPS HERE
			if (ReadyToLaunch == false) {
				//sprintf(oapiDebugString(),"LVDC: T - %d:%d:%f | AWAITING PTL INTERRUPT",(int)Hours,(int)Minutes,Seconds);
				lvda.ZeroLVIMUCDUs();						// Zero IMU CDUs
				return;
			}

			// Between PTL signal and GRR, we monitor the IMU for any failure signals and do vehicle self-tests.
			// At GRR we transfer control to the flight program and start TB0.

			// BEFORE GRR (T-00:00:17) STOPS HERE
			if (!lvda.GetGuidanceReferenceRelease()) {
				//sprintf(oapiDebugString(),"LVDC: T %f | IMU XYZ %f %f %f PIPA %f %f %f | TV %f | AWAITING GRR",RealTimeClock,
					//lvimu.CDURegisters[LVRegCDUX],lvimu.CDURegisters[LVRegCDUY],lvimu.CDURegisters[LVRegCDUZ],
					//lvimu.CDURegisters[LVRegPIPAX],lvimu.CDURegisters[LVRegPIPAY],lvimu.CDURegisters[LVRegPIPAZ],atan((double)45));
				return;
			}
			else {
				//Flight Program Initialization
				PhaseActivator(true);
			}
		}

		//LVDC FLIGHT PROGRAM
		SystemTimeUpdateRoutine();

		//This would probably belong in the LVDA, hardware counters to cause interrupt
		Timer1Counter--;
		Timer2Counter--;

		//Interrupt processing
		for (int i = 0;i < 12;i++)
		{
			if (InterruptState[i] == false && DVIH[i] == false && GetInterrupt(i))
			{
				InterruptState[i] = true;
				ProcessInterrupt(i);
			}
			else if (InterruptState[i] && GetInterrupt(i) == false)
			{
				InterruptState[i] = false;
			}
		}

		/* **** LVDC GUIDANCE PROGRAM **** */		
		switch(LVDC_Timebase){
			case 1:
			case 2:
				if (lvda.SpacecraftSeparationIndication())
				{
					LVDC_Stop = true;
				}
				break;
			case 3:
				if (lvda.SpacecraftSeparationIndication())
				{
					LVDC_Stop = true;
				}
			
				// MR Shift
				if(T_1 <= 0.0 && MRS == false){
					fprintf(lvlog,"[TB%d+%f] MR Shift\r\n",LVDC_Timebase,LVDC_TB_ETime);
					DVASW = DVASW | MSKSST3A;
					SwitchSelectorProcessor(0);
					MRS = true;
				}
				
				break;

			case 4:	//Timebase 4
				if (lvda.SpacecraftSeparationIndication())
				{
					LVDC_Stop = true;
				}
				break;
			case 5:
				break;
			case 6:
				//TB6 timed events
				if (poweredflight == false)
				{
					//Powered nav initialization
					PhaseActivator(false);
					sinceLastCycle = (RealTimeClock - T_L) - TAS;
					DT_N = DT_N5;
					MS25DT = 10.0*DT_N;
					MS04DT = 0.1 / DT_N;
					MSK5 = 0.48*RAD;
					MSK6 = MSK16 = 15.3*RAD;
					MSLIM1 = 0.024*RAD;
					MSLIM2 = 0.016*RAD;
					ddotG_last = ddotS;
					DotM_act = _V(0.0, 0.0, 0.0);
					DotM_last = _V(0.0, 0.0, 0.0);
					DotG_last = DotS;
					lvda.ZeroLVIMUPIPACounters();
					lvda.TLIBegun();
				}

				if (S4B_REIGN && MRS == false && T_2 <= 0.0)
				{
					lvda.SwitchSelector(SWITCH_SELECTOR_SIVB, 18);
					MRS = true;
				}

				//TLI Inhibit check
				if (lvda.SIVBInjectionDelay())
				{
					if (LVDC_TB_ETime > 560.0)
					{
						//TLI commit, inhibit will be ignored
					}
					else if ((LVDC_TB_ETime >= 41.0 && LVDC_TB_ETime <= 41.1) || LVDC_TB_ETime > 497.3)
					{
						//Inhibit after 41 seconds will be accepted at 8min 17.3sec

						//Start TB6c
						if (ModeCode26[MC26_StartTB6C] == false)
						{
							//Disable events processor
							T2STAT[2] = false;
							Timer2Interrupt(true);

							DVASW = DVASW | MSKSSTB6C;
							SwitchSelectorProcessor(0);
						}
					}
				}

				break;
			case 7:
				// TB7 timed events

				if (ModeCode27[MC27_TB8EnableDCSCmdEnable] && LVDC_TB_ETime > t_TB8Start)
				{
					LVDC_Timebase = 8;
					TimeBaseChangeRoutine();
					TB8 = TI;
					ModeCode27[MC27_TB8_Started] = true;
				}

				//For now, disable LVDC at TB7+11,729 seconds
				if (LVDC_TB_ETime > 11729.0)
				{
					LVDC_Stop = true;
					return;
				}
				break;

			case 8:
				// T8B timed events
				//SwitchSelectorProcessing(SSTTB[8]);

				//Lunar Impact Burn
				if (ImpactBurnEnabled && !ImpactBurnInProgress && LVDC_TB_ETime > T_ImpactBurn)
				{
					lvda.SwitchSelector(SWITCH_SELECTOR_SIVB, 42);
					lvda.SwitchSelector(SWITCH_SELECTOR_SIVB, 101);
					ImpactBurnInProgress = true;
					fprintf(lvlog, "[TB%d+%f] Lunar impact burn started\r\n", LVDC_Timebase, LVDC_TB_ETime);
				}

				if (ImpactBurnEnabled && ImpactBurnInProgress && LVDC_TB_ETime > T_ImpactBurn + dT_ImpactBurn)
				{
					lvda.SwitchSelector(SWITCH_SELECTOR_SIVB, 43);
					lvda.SwitchSelector(SWITCH_SELECTOR_SIVB, 102);
					ImpactBurnEnabled = false;
					ImpactBurnInProgress = false;
					fprintf(lvlog, "[TB%d+%f] Lunar impact burn stopped\r\n", LVDC_Timebase, LVDC_TB_ETime);
				}

				//For now, disable LVDC at TB8+30,000 seconds to allow lunar impact burns
				if (LVDC_TB_ETime > 30000.0)
				{
					LVDC_Stop = true;
					return;
				}
				break;
		}

		//This is the actual LVDC code & logic; has to be independent from any of the above events
		if(ModeCode25[MC25_BeginTB0] && init == false)
		{
			fprintf(lvlog,"[GMT%+f] GRR received!\r\n", RealTimeClock);

			// Ground launch targeting
			VariableLaunchWindow();

			if (TerminalConditions == false)
			{
				if (Direct_Ascent) {
					C_3 = TABLE15[0].target[0].C_3;
					e = TABLE15[0].target[0].e_N;
					f = TABLE15[0].f;
					alpha_D = TABLE15[0].target[0].alpha_D;
					eps_3 = 0;
					//sprintf(oapiDebugString(), "LVDC: DIRECT-ASCENT"); // STOP
				}

				// p is the semi-latus rectum of the desired terminal ellipse.
				p = (mu / C_3)*(pow(e, 2) - 1);
				fprintf(lvlog, "p = %f, mu = %f, e2 = %f, mu/C_3 = %f\r\n", p, mu, pow(e, 2), mu / C_3);

				// K_5 is the IGM terminal velocity constant
				K_5 = sqrt(mu / p);
				fprintf(lvlog, "K_5 = %f\r\n", K_5);

				R_T = p / (1 + e*cos(f));
				V_T = K_5*sqrt((1 + 2 * e*cos(f) + pow(e, 2)));
				gamma_T = atan2((e*(sin(f))), (1 + (e*(cos(f)))));
				G_T = -mu / pow(R_T, 2);
			}
			fprintf(lvlog,"R_T = %f (Expecting 6,563,366), V_T = %f (Expecting 7793.0429), gamma_T = %f\r\n",R_T,V_T,gamma_T);

			// G MATRIX CALCULATION
			MX_A.m11 = cos(PHI);  MX_A.m12 = sin(PHI)*sin(Azimuth); MX_A.m13 = -(sin(PHI)*cos(Azimuth));
			MX_A.m21 = -sin(PHI); MX_A.m22 = cos(PHI)*sin(Azimuth); MX_A.m23 = -(cos(PHI)*cos(Azimuth));
			MX_A.m31 = 0;  MX_A.m32 = cos(Azimuth);  MX_A.m33 = sin(Azimuth);

			MX_B.m11 = cos(theta_N); MX_B.m12 = 0; MX_B.m13 = sin(theta_N);
			MX_B.m21 = sin(theta_N)*sin(Inclination); MX_B.m22 = cos(Inclination); MX_B.m23 = -cos(theta_N)*sin(Inclination);
			MX_B.m31 = -sin(theta_N)*cos(Inclination); MX_B.m32 = sin(Inclination);MX_B.m33 = cos(theta_N)*cos(Inclination);

			MX_G = mul(MX_B,MX_A); // Matrix Multiply

			theta_E = theta_EO + TVRATE * t_D;
			MEG = _M(cos(theta_E), sin(theta_E), 0.0, 0.0, 0.0, -1.0, -sin(theta_E), cos(theta_E), 0.0);
			MX_EPH = mul(tmat(MX_A), MEG);
			W_ES = -_V(MX_A.m21, MX_A.m22, MX_A.m23)*omega_E;

			PosS = _V(cos(PHI - PHIP), sin(PHI - PHIP)*sin(Azimuth), -sin(PHI - PHIP)*cos(Azimuth))*R_L;
			DotS = _V(0, cos(PHIP)*cos(Azimuth), cos(PHIP)*sin(Azimuth))*R_L*omega_E;
			
			fprintf(lvlog, "Initial Position = %f %f %f\r\n", PosS.x, PosS.y, PosS.z);
			fprintf(lvlog, "Initial Velocity = %f %f %f\r\n", DotS.x, DotS.y, DotS.z);
		
			ddotG_last = GravitationSubroutine(PosS, true);
			DotG_last = DotS;

			lvda.ReleaseLVIMUCDUs();						// Release IMU CDUs
			VECTOR3 align = lvda.GetTheodoliteAlignment(Azimuth);
			lvda.DriveLVIMUGimbals(align.x, align.y, align.z);	// Now bring to alignment 
			fprintf(lvlog, "Initial Attitude: %f %f %f \r\n", align.x*DEG, align.y*DEG, align.z*DEG);
			lvda.ReleaseLVIMU();							// Release IMU
			CountPIPA = true;								// Enable PIPA storage	

			PCommandedAttitude.x = align.x;
			PCommandedAttitude.y = 0;
			PCommandedAttitude.z = 0;
			CommandedAttitude = PCommandedAttitude;
			VCCYA = PCommandedAttitude.y;
			VCCZA = PCommandedAttitude.z;
			DChi_apo = _V(0, 0, 0);

			MF[0] = MF[1] = MF[2] = MF[3] = MF[4] = MFSArr[0] = MFSArr[1] = MFSArr[2] = MFSArr[3] = MFS = MF0;

			lvda.ZeroLVIMUPIPACounters();

			sinceLastCycle = 0;
			init = true;
			lvda.SetOutputRegisterBit(FiringCommitEnable, true);
			fprintf(lvlog, "Initialization completed.\r\n\r\n");
			return;//goto minorloop;
		}

		//Orbital Processing
		if (poweredflight == false)
		{
			return;
		}

		//This code causes the major loop to run at the correct rate
		sinceLastCycle += simdt;
		if (sinceLastCycle < DT_N)
		{
			return;
		}
		//Reset sinceLastCycle for boost major loop timing
		sinceLastCycle = 0;

		//BOOST MAJOR LOOP
		IGMCycle++;				// For debugging
		fprintf(lvlog, "[%d+%f] *** Major Loop (powered) %d ***\r\n", LVDC_Timebase, LVDC_TB_ETime, IGMCycle);
		//The Non Interrupt Sequencer contains everything in the boost major loop
		NonInterruptSequencer(true);

		// Debug if we're launched
		/*if(LVDC_Timebase > -1){
			if(LVDC_Timebase < 5 || (LVDC_Timebase == 6 && S4B_REIGN)){
				sprintf(oapiDebugString(),"TB%d+%f | T1 = %f | T2 = %f | T3 = %f | Tt_T = %f | ERR %f %f %f | V = %f R= %f",
					LVDC_Timebase,LVDC_TB_ETime,
					T_1,T_2,Tt_3,Tt_T,
					AttitudeError.x*DEG,AttitudeError.y*DEG,AttitudeError.z*DEG,
					V, R/1000);
			} else if (LVDC_Timebase == 40)	{
				sprintf(oapiDebugString(), "TB4A+%f | T1 = %f | T2 = %f | T3 = %f | Tt_T = %f | ERR %f %f %f | V = %f R= %f",
					LVDC_TB_ETime,
					T_1, T_2, Tt_3, Tt_T,
					AttitudeError.x*DEG, AttitudeError.y*DEG, AttitudeError.z*DEG,
					V, R / 1000);
			} else{
				sprintf(oapiDebugString(),"TB%d+%f |CMD %f %f %f | ERR %f %f %f | V = %f R= %f",
					LVDC_Timebase,LVDC_TB_ETime,
					CommandedAttitude.x*DEG,CommandedAttitude.y*DEG,CommandedAttitude.z*DEG,
					AttitudeError.x*DEG,AttitudeError.y*DEG,AttitudeError.z*DEG,
					V,R/1000);
			}
		}*/
		/*
		sprintf(oapiDebugString(),"LVDC: TB%d + %f | PS %f %f %f | VS %f %f %f",
			LVDC_Timebase,LVDC_TB_ETime,
			PosS.x,PosS.y,PosS.z,
			DotS.x,DotS.y,DotS.z);
			*/
		//	CurrentAttitude.x*DEG,CurrentAttitude.y*DEG,CurrentAttitude.z*DEG,V);								

		// End of test for LVDC_Stop
	}
	else
	{
		lvda.SetFCCAttitudeError(_V(0, 0, 0));
	}
}

void LVDCSV::PhaseActivator(bool init)
{
	//Mission Initialization
	if (init)
	{
		fprintf(lvlog, "MISSION INITIALIZATION\r\n");
		DVIH.set();
		DPM.set();
		VOAC = lvda.GetLVIMUPIPARegisters();
		lvda.ZeroLVIMUPIPACounters();
		DVACT = RealTimeClock;
		DVRTC = TEX = POT = DVACT;
		TMM = TMR = DVERT = 0.0;
		DFIL1 = DFIL3 = true;
		//Activate interrupt processor
		DVIH[INT11_Timer2Interrupt] = false;
		DVIH[INT12_Timer1Interrupt] = false;
		DVIH[INT9_TLCMemoryError] = false;
		//TBD: Set G&C steering switch off
		DVSST = 1.0e10;
		MLT = MLD = MIR;
		Timer1Interrupt(true);
		//Phase indicator to 1
		DVP = 1;

		BOOST = true;
		ModeCode25[MC25_BeginTB0] = true;								// Mark event
		LVDC_Timebase = 0;
		LVDC_TB_ETime = 0;
		DT_N = DT_N1;
		MS25DT = 25.0*DT_N;
		MS04DT = 0.04 / DT_N;
		MSK5 = 0.48*RAD;
		MSK6 = MSK16 = 15.3*RAD;
		MSLIM1 = 0.04*RAD;
		MSLIM2 = 0.04*RAD;
		T_L = RealTimeClock;
		TAS = TI = 0.0;
		oapiSetTimeAcceleration(1);					// Set time acceleration to 1
		ModeCode25[MC25_BeginTB0] = true;
		SystemTimeUpdateRoutine();
		DFIL1 = false;
		Timer2Counter = (int)(4.03 / LVDC_TIMESTEP);
		for (i = 0;i < 11;i++)
		{
			T2STAT[i] = false;
		}

		//Events Processor Task List
		if (true)
		{
			//Normal Mission

			EPTABLE[1] = 2; //Search for liftoff discrete (TB+16.0)
			EPTABLE[2] = -1;
			EPTABLE[3] = 4; //Start looking for backup liftoff acceleration (TB0+17.5)
			EPTABLE[7] = 7; //Start checking for engine failure (TB1+6.0)
			EPTABLE[10] = -1; //TBD (TB1+14.0)
			EPTABLE[14] = 13; //Enable INT5 and DIN 18 (TB2+0.0)
			EPTABLE[15] = 14; //(TB2+18.4)
			EPTABLE[16] = 0;
		}
		else
		{
			//Repeatable Flight Simulation

			EPTABLE[1] = -1;
			EPTABLE[2] = 3; //Liftoff on schedule (TB0+17.0)
			EPTABLE[3] = -1;
			EPTABLE[7] = -1;
			EPTABLE[10] = -1; //TBD
			EPTABLE[14] = -1;
			EPTABLE[15] = -1;
			EPTABLE[16] = 15; //TB3 on schedule (TB2+27.5)
		}


		//Events Processor On
		EventsProcessor(1);
	}
	else
	{
		//TBD: Set phase change in progress flag
		//Inhibit all interrupts: TBD, check if this is good to do
		//DVIH.set();
		//Load Timer 2 with a large value
		Timer2Counter = 0377776;
		//Reset any pending timer 2 interrupt
		for (i = 0;i < 11;i++)
		{
			T2STAT[i] = false;
		}
		//Set interrupt level 2 in prog. indicator
		DFIL1 = DFIL2 = true;
		//Release TLC and timer 1. interrupt
		DVIH[INT9_TLCMemoryError] = false;
		DVIH[INT12_Timer1Interrupt] = false;
		//Phase transition
		DVP = DVP + 1;
	}
	fprintf(lvlog, "PHASE ACTIVATOR FOR PHASE %d\r\n", DVP);
	switch (DVP)
	{
	case 1:
	case 3:
		//Set initial status of non-interrupt sequence tasks
		NISTAT[0] = true;
		NISTAT[1] = false;
		NISTAT[2] = true;
		NISTAT[3] = false;
		NISTAT[4] = true;
		NISTAT[5] = true;
		NISTAT[6] = false;
		NISTAT[7] = false;
		NISTAT[8] = false;
		NISTAT[9] = false; //TBD, should be true. What are Chi Computations needed for? Check on 45� yaw?
		NISTAT[10] = false;
		NISTAT[11] = false;
		NISTAT[12] = false;
		NISTAT[13] = false;
		NISTAT[14] = false;
		NISTAT[15] = false;
		NISTAT[16] = false;
		NISTAT[17] = false;
		NISTAT[18] = true;
		NISTAT[19] = false;
		NISTAT[20] = true;

		//TBD: Set initial time, status of periodic processor tasks
		//Set initial status of timer 2 tasks
		for (int i = 0;i < 11;i++)
		{
			T2STAT[i] = false;
		}
		//Enable events processor
		T2STAT[2] = true;
		Phase13ApplicationProgramInit();
		//Schedule next timer 2 function
		Timer2Interrupt(true);
		DFIL1 = DFIL2 = DFIL3 = false;
		//Release all previously enabled interrupts
		DVIH[INT11_Timer2Interrupt] = false;
		break;
	case 2:
	case 4:
		T2STAT[2] = true;
		T2STAT[6] = true;
		T2STAT[7] = true;
		Phase24ApplicationProgramInit();
		//Schedule next timer 2 function
		Timer2Interrupt(true);
		DFIL1 = DFIL2 = DFIL3 = false;
		//Release all previously enabled interrupts
		DVIH[INT11_Timer2Interrupt] = false;
		break;
	}
}

void LVDCSV::Phase13ApplicationProgramInit()
{
	poweredflight = true;
	MLR = 25.0;
	MLD = MIR;
	if (DVP == 1)
	{
		NISTAT[12] = false;
	}
	else
	{
		//Queue in orbital guidance in phase III
		NISTAT[12] = true;
	}
}

void LVDCSV::Phase24ApplicationProgramInit()
{
	poweredflight = false;
	MLR = 10.0;
	MLD = MOR;
	//Orbit Init
	ddotS = ddotG_act;
	//Schedule navigation update
	DLTTL[6] = TAS + 8.0;
	//Orbital guidance etc. as soon as possible
	DLTTL[7] = 0.0;
	if (DVP == 2)
	{
		//Some document was talking about an offset. Probably to not have all three timer 2 functions for phase II and IV at once
		T2STAT[8] = true;
		DLTTL[8] = TMM + 0.5;
	}
	else
	{
		T2STAT[8] = false;
	}
}

void LVDCSV::VariableLaunchWindow()
{
	t_D = T_L - T_LO;
	//t_D = TABLE15.target[tgt_index].t_D;

	fprintf(lvlog, "Time into launch window = %f\r\n", t_D);

	//Azimuth determination
	if (t_D < t_DS1)
	{
		Azimuth = hx[0][0] + hx[0][1] * ((t_D - t_D1) / t_SD1) + hx[0][2] * pow((t_D - t_D1) / t_SD1, 2) + hx[0][3] * pow((t_D - t_D1) / t_SD1, 3) + hx[0][4] * pow((t_D - t_D1) / t_SD1, 4);
	}
	else if (t_DS1 <= t_D && t_D < t_DS2)
	{
		Azimuth = hx[1][0] + hx[1][1] * ((t_D - t_D2) / t_SD2) + hx[1][2] * pow((t_D - t_D2) / t_SD2, 2) + hx[1][3] * pow((t_D - t_D2) / t_SD2, 3) + hx[1][4] * pow((t_D - t_D2) / t_SD2, 4);
	}
	else
	{
		Azimuth = hx[2][0] + hx[2][1] * ((t_D - t_D3) / t_SD3) + hx[2][2] * pow((t_D - t_D3) / t_SD3, 2) + hx[2][3] * pow((t_D - t_D3) / t_SD3, 3) + hx[2][4] * pow((t_D - t_D3) / t_SD3, 4);
	}

	if (i_op)
	{
		Inclination = fx[0] + fx[1] * (Azimuth - Azo) / Azs + fx[2] * pow((Azimuth - Azo) / Azs, 2) + fx[3] * pow((Azimuth - Azo) / Azs, 3)
			+ fx[4] * pow((Azimuth - Azo) / Azs, 4) + fx[5] * pow((Azimuth - Azo) / Azs, 5) + fx[6] * pow((Azimuth - Azo) / Azs, 6);
	}
	else
	{
		Inclination = fxt[0] + fxt[1] * (t_D - t_D0) / t_S + fxt[2] * pow((t_D - t_D0) / t_S, 2) + fxt[3] * pow((t_D - t_D0) / t_S, 3)
			+ fxt[4] * pow((t_D - t_D0) / t_S, 4) + fxt[5] * pow((t_D - t_D0) / t_S, 5) + fxt[6] * pow((t_D - t_D0) / t_S, 6);
	}

	if (theta_N_op)
	{
		theta_N = gx[0] + gx[1] * (Azimuth - Azo) / Azs + gx[2] * pow((Azimuth - Azo) / Azs, 2) + gx[3] * pow((Azimuth - Azo) / Azs, 3)
			+ gx[4] * pow((Azimuth - Azo) / Azs, 4) + gx[5] * pow((Azimuth - Azo) / Azs, 5) + gx[6] * pow((Azimuth - Azo) / Azs, 6);
	}
	else
	{
		theta_N = gxt[0] + gxt[1] * (t_D - t_D0) / t_S + gxt[2] * pow((t_D - t_D0) / t_S, 2) + gxt[3] * pow((t_D - t_D0) / t_S, 3)
			+ gxt[4] * pow((t_D - t_D0) / t_S, 4) + gxt[5] * pow((t_D - t_D0) / t_S, 5) + gxt[6] * pow((t_D - t_D0) / t_S, 6);
	}

	// preset to fixed value to be independent from any external stuff
	// Azimuth = 72.124;
	fprintf(lvlog, "Azimuth = %f\r\n", Azimuth);

	// Let's cheat a little. (Apollo 8)
	//Inclination = 32.5031;
	fprintf(lvlog, "Inclination = %f\r\n", Inclination);

	// Cheat a little more. (Apollo 8)
	// DescNodeAngle = 123.004; 
	fprintf(lvlog, "DescNodeAngle = %f\r\n", theta_N);

	// Need to make those into radians
	Azimuth *= RAD;
	Inclination *= RAD;
	theta_N *= RAD;

	fprintf(lvlog, "Rad Convert: Az / Inc / DNA = %f %f %f\r\n", Azimuth, Inclination, theta_N);
}

void LVDCSV::NonInterruptSequencer(bool phase13)
{
	if (phase13)
	{
		if (NISTAT[0]) { AccelerometerRead(); PeriodicProcessor(); }
		if (NISTAT[1]) { SimulatedAccelerometers(); PeriodicProcessor(); }
		if (NISTAT[2]) { AccelerometerProcessing(); PeriodicProcessor(); }
		if (NISTAT[3]) { FMCalculations(); PeriodicProcessor(); }
		if (NISTAT[4]) { DiscreteProcessor(); PeriodicProcessor(); }
		if (NISTAT[5]) { BoostNavigation(); PeriodicProcessor(); }
		if (NISTAT[6]) { RestartCalculations(); PeriodicProcessor(); }
		if (NISTAT[7]) { PhaseActivator(false); return; }
		if (NISTAT[8]) { TimeTiltGuidance(); PeriodicProcessor(); }
		if (NISTAT[9]) { ChiComputations(2); PeriodicProcessor(); }
		if (NISTAT[10]){ IterativeGuidanceMode(); PeriodicProcessor(); }
		//if (NISTAT[11]){ SIVBCutoffPrediction(); PeriodicProcessor(); }
		if (NISTAT[12]){ OrbitGuidance(); PeriodicProcessor(); }
		//if (NISTAT[13]) { TargetUpdate(); PeriodicProcessor(); }
		if (NISTAT[14]) { TimeToGoToRestartAndBetaTest(); PeriodicProcessor(); }
		//if (NISTAT[15]) { TimeBase6Check(); PeriodicProcessor(); }
		if (NISTAT[16]) { StartTimeBase1(1); PeriodicProcessor(); }
		if (NISTAT[17]) { CheckTimeBase57(); PeriodicProcessor(); }
		if (NISTAT[18]) { MinorLoopSupport(); PeriodicProcessor(); }
		//if (NISTAT[19]) { SimulatedPlatformGimbalAngles(); PeriodicProcessor(); }
		//if (NISTAT[20]) { EtcBtc(); PeriodicProcessor(); }
	}
	else
	{
		//TBD: Phase II and IV major loop
	}
}

void LVDCSV::SystemTimeUpdateRoutine()
{
	DVRTC = RealTimeClock;
	TMM = DVRTC - T_L;
	LVDC_TB_ETime = DVTRR = TMM - TMR; //Remove TB from here later
}

void LVDCSV::TimeBaseChangeRoutine()
{
	fprintf(lvlog, "TB%d started at TMM %.2lf\r\n", LVDC_Timebase, TMM);
	TMR = TI = RealTimeClock - T_L;
	SystemTimeUpdateRoutine();
	EventsProcessor(1);
	SwitchSelectorProcessor(2);
}

bool LVDCSV::GetInterrupt(int rupt)
{
	switch (rupt)
	{
	case INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA:
		return lvda.SCInitiationOfSIISIVBSeparation();
		break;
	case INT4_SIVBEngineOutB:
		return lvda.GetSIVBEngineOutB();
		break;
	case INT5_SICOutboardEnginesCutoffA:
		return lvda.GetSIPropellantDepletionEngineCutoff();
		break;
	case INT6_SIIEnginesCutoff:
		return lvda.GetSIIPropellantDepletionEngineCutoff();
		break;
	case INT7_GuidanceReferenceRelease:
		return lvda.GetCMCSIVBCutoff();
		break;
	case INT11_Timer2Interrupt:
		return (Timer2Counter <= 0);
		break;
	case INT12_Timer1Interrupt:
		return (Timer1Counter <= 0);
		break;
	}
	return false;
}

void LVDCSV::ProcessInterrupt(int rupt)
{
	switch (rupt)
	{
	case INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA:
		SCInitiationOfSIISIVBSeparationInterrupt();
		break;
	case INT4_SIVBEngineOutB:
		CheckTimeBase57();
		break;
	case INT5_SICOutboardEnginesCutoffA:
		StartTimeBase3();
		break;
	case INT6_SIIEnginesCutoff:
		StartTimeBase4();
		break;
	case INT7_GuidanceReferenceRelease:
		if (LVDC_Timebase == 6)
		{
			if (ModeCode26[MC26_SecondSIVBCutoffCommand] == false)
			{
				fprintf(lvlog, "SIVB BACKUP CUTOFF! TMM = %f \r\n", TMM);
				ModeCode26[MC26_SecondSIVBCutoffCommand] = true;

				DVASW = DVASW | MSKSSS4C1;
				SwitchSelectorProcessor(0);
			}
		}
		break;
	case INT11_Timer2Interrupt:
		Timer2Counter = 403; //Automatically generates an interrupt every 4.032 seconds if not under program control
		Timer2Interrupt(false);
		InterruptState[INT11_Timer2Interrupt] = false; //Reset interrupt here so that it can happen again on the next cycle, if needed
		break;
	case INT12_Timer1Interrupt:
		Timer1Counter = 403; //Automatically generates an interrupt every 4.032 seconds if not under program control
		Timer1Interrupt(false);
		InterruptState[INT12_Timer1Interrupt] = false; //Reset interrupt here so that it can happen again on the next cycle, if needed
		break;
	}
}

void LVDCSV::Timer1Interrupt(bool timer1schedule)
{
	if (timer1schedule == false)
	{
		DFIL3 = true;
		switch (GST1M)
		{
		case 0: //Minor Loop
		case 1: //Flight Simulation Minor Loop
		case 2: //Minor Loop with Liftoff Search
			//fprintf(lvlog, "T1 Interrupt (ML) at GRR+%lf\r\n", TMM);
			MinorLoop(MLM);
			break;
		default: //Switch Selector Processor
			//fprintf(lvlog, "T1 Interrupt (SS) at GRR+%lf\r\n", TMM);
			SwitchSelectorProcessor(SSM - 3);
			break;
		}
		DFIL3 = false;
	}

	if (MLT <= TMM)
	{
		//Are we already or nearly too late for a minor loop interrupt?
		Timer1Counter = 1;
		GST1M = MLM;
	}
	else
	{
		//+LVDC_TIMESTEP/2.0 because of rounding errors
		if (MLT <= DVSST + LVDC_TIMESTEP / 2.0)
		{
			//Minor loop takes precedence
			Timer1Counter = (int)(round((MLT - TMM) / LVDC_TIMESTEP));
			GST1M = MLM;
		}
		else
		{
			//Switch selector next
			GST1M = SSM;
			if (DVSST <= TMM)
			{
				Timer1Counter = 1;
			}
			else
			{
				Timer1Counter = (int)(round((DVSST - TMM) / LVDC_TIMESTEP));
			}
		}
	}

	//fprintf(lvlog, "T1 Interrupt. Counter %d\r\n", Timer1Counter);
}

void LVDCSV::Timer2Interrupt(bool timer2schedule)
{
	if (timer2schedule == false)
	{
		//fprintf(lvlog, "T2 Interrupt at GRR+%lf\r\n", TMM);
		//Lock T2 Interrupt
		DVIH[INT11_Timer2Interrupt] = true;
		//Set T2 interrupt in progress indicator
		DFIL1 = true;
		switch (DGST2)
		{
		case 0: //Time Update
			break;
		case 1: //Ladder Ramp Processor
			break;
		case 2: //Events Processor
			EventsProcessor(0);
			break;
		case 3: //Time Tilt Guidance
			break;
		case 4: //Navigation Update Implementation
			fprintf(lvlog, "[%d+%f] Navigation Update implemented!\r\n", LVDC_Timebase, LVDC_TB_ETime);
			ModeCode27[MC27_DCSNavigationUpdateAccepted] = false;
			TAS = NUPTIM;
			PosS = Pos_Nav;
			DotS = Vel_Nav;
			ddotS = OrbitalNavigationAcceleration(PosS, DotS, CurrentAttitude);
			R = length(PosS);
			V = length(DotS);
			CG = length(ddotS);
			T2STAT[4] = false;
			DLTTL[4] = 0.0;
			break;
		case 5: //Time Base 8 Enable
			break;
		case 6: //Phase 2/4 Control Module
			PhaseIIandIVControl(1);
			break;
		case 7: //Phase 2/4 Control Module
			PhaseIIandIVControl(2);
			break;
		case 8: //Phase 2/4 Control Module
			PhaseIIandIVControl(3);
			break;
		case 9: //Water Methanol Activate
			break;
		case 10: //Extra Accelerometer Read
			break;
		}
		//
		DFIL1 = false;
		DVIH[INT11_Timer2Interrupt] = false;
	}

	//None of the above
	DGST2 = 11;
	//Select T2 Scheduler to use Interrupt in 4 seconds
	DV2TG = TMM + 4.03;
	//Find the next module to execute
	for (int i = 0;i < 11;i++)
	{
		if (T2STAT[i])
		{
			if (DLTTL[i] < DV2TG)
			{
				DGST2 = i;
				DV2TG = DLTTL[i];
			}
		}
	}

	//Schedule next T2 interrupt
	Timer2Counter = (int)(round((DV2TG - TMM) / LVDC_TIMESTEP));
	if (Timer2Counter < 1)
	{
		Timer2Counter = 1;
	}

	if (timer2schedule)
	{
		//fprintf(lvlog, "T2 Scheduler. Next interrupt at GRR+%lf Task %d Counter %d\r\n", DV2TG, DGST2, Timer2Counter);
	}
	else
	{
		//fprintf(lvlog, "T2 Interrupt. Next interrupt at GRR+%lf Task %d Counter %d\r\n", DV2TG, DGST2, Timer2Counter);
	}
}

void LVDCSV::AccelerometerRead()
{
	//Read X, Y, Z accelerometers
	DVAC = lvda.GetLVIMUPIPARegisters();
	lvda.ZeroLVIMUPIPACounters();
	fprintf(lvlog, "ACCELEROMETER READ: %f %f %f\r\n", DVAC.x, DVAC.y, DVAC.z);	
	//TBD: Telemeter time from GRR current time base set
	//Save last time of accelerometer sampling
	double TEMP = TAS;
	//Compute time in mission at accelerometer read
	TAS = TMM;
	//Compute time in time base at accelerometer read
	LVDC_TB_ETime = TAS - TI;
	//Compute time since last accelerometer read
	dt_c = TAS - TEMP;
	//TBD: Telemeter time in time base at accelerometer read
	//Reset accelerometer bits for MC24
	ModeCode24 = 0;
	//TBD: Telemeter X and Y accelerometer reading
	//Is time base 1 set?
	/*if (ModeCode25[MC25_BeginTB1] == false)
	{
		//No, Calculate FMC as negative of X component of gravity acceleration
		DVFMC = -ddotG_last.x;
	}
	else
	{
		//Yes, Compute mass of the vehicle at accelerometer read
		DVMAS = DVMAS - DVEOF * FMR*dt_c;
		//Calculate FMC
		DVFMC = DVEOF * DVFOR / DVMAS;
	}*/
	//Compute Average Chi's for SMC Calculations
	DVCA[1] = (PCommandedAttitude.z + VCCZA) / 2.0;
	//TBD: This shouldn't be necessary for yaw
	if (abs(PCommandedAttitude.z - VCCZA) > PI05)
	{
		DVCA[1] = DVCA[1] + PI;
		if (DVCA[1] >= PI2)
		{
			DVCA[1] -= PI2;
		}
	}
	DVCA[0] = (PCommandedAttitude.y + VCCYA) / 2.0;
	if (abs(PCommandedAttitude.y - VCCYA) > PI05)
	{
		DVCA[0] = DVCA[0] + PI;
		if (DVCA[0] >= PI2)
		{
			DVCA[0] -= PI2;
		}
	}
	//sprintf(oapiDebugString(), "Cmd %lf %lf Past %lf %lf Average %lf %lf", PCommandedAttitude.y * DEG, PCommandedAttitude.z * DEG, VCCYA * DEG, VCCZA * DEG, DVCA[0] * DEG, DVCA[1] * DEG);
	VCCZA = PCommandedAttitude.z;
	VCCYA = PCommandedAttitude.y;
	//TBD: Telemeter Z accelerometer reading
	//Compute A&B acceleration change for X, Y, Z
	DVDA = DVDB = DVAC - VOAC;
	VOAC = DVAC;
	//TBD: Telemeter RTC reading at accelerometer reading
	//Compute expected velocity changes
	//SINTHZ = sin(CurrentAttitude.z);
	//COSTHZ = cos(CurrentAttitude.z);
	//TBD: Telemeter mission time at accelerometer read
	//SINTHY = sin(CurrentAttitude.y);
	//COSTHY = cos(CurrentAttitude.y);
	//DVD = _V(COSTHY*COSTHZ, SINTHZ, -SINTHY * COSTHZ)*dt_c;
	//DVF = DVD*Fm;
}

void LVDCSV::SimulatedAccelerometers()
{
	fprintf(lvlog, "SIMULATED ACCELEROMETERS \r\n");
	//TBD
}

void LVDCSV::AccelerometerProcessing()
{
	fprintf(lvlog, "ACCELEROMETER PROCESSING \r\n");
	/*DVVSQ = 0.0;
	VACZR = Fm * dt_c*KSN2D;
	for (i = 0;i < 3;i++)
	{
		if (abs(DVDA.data[i] - DVDB.data[i]) < 2.0*0.05) //2 Bits, 0.05 m/s each
		{

		}
	}*/
	DotM_act += DVAC;
}

void LVDCSV::FMCalculations()
{
	fprintf(lvlog, "FM CALCULATIONS\r\n");
	//Calculate acceleration vector
	ddotM_act = (DotM_act - DotM_last) / dt_c;
	//Calculate acceleration
	Fm = length(ddotM_act);
	fprintf(lvlog, "Sensed Acceleration: %f \r\n", Fm);

	//Reciprocal Acceleration Filter
	MF[0] = 1.0 / Fm;
	double *MFKP;
	if (LVDC_Timebase == 3)
	{
		//S-II
		MFKP = MFK;
	}
	else
	{
		//S-IVB
		MFKP = S4MFK;
	}
	double MFSTEMP = MFKP[0] * MF[0] + MFKP[1] * MF[1] + MFKP[2] * MF[2] + MFKP[3] * MF[3] + MFKP[4] * MF[4] + MFKP[5] * MFSArr[0] + MFKP[6] * MFSArr[1] + MFKP[7] * MFSArr[2] + MFKP[8] * MFSArr[3];

	//MFS can only change by 0.005 sec^2/m per major loop
	if (MFSTEMP - MFS > 0.005)
	{
		MFS = MFS + 0.005;
	}
	else if (MFSTEMP - MFS < -0.005)
	{
		MFS = MFS - 0.005;
	}
	else
	{
		MFS = MFSTEMP;
	}

	//Limit magnitude to 0.25 (= a minimum of 4 m/s^2 acceleration)
	if (MFS > 0.25)
	{
		MFS = 0.25;
	}

	MFSArr[3] = MFSArr[2];
	MFSArr[2] = MFSArr[1];
	MFSArr[1] = MFSArr[0];
	MFSArr[0] = MFS;
	MF[4] = MF[3];
	MF[3] = MF[2];
	MF[2] = MF[1];
	MF[1] = MF[0];

	//sprintf(oapiDebugString(), "FM[0] %lf 1/MFS %lf", Fm, 1.0 / MFS);
	fprintf(lvlog, "Smoothed Acceleration: %f \r\n", 1.0 / MFS);
}

void LVDCSV::DiscreteProcessor()
{
	//fprintf(lvlog, "DISCRETE PROCESSOR \r\n");
	if (DVP == 1)
	{
		DiscreteProcessor1();
		DiscreteProcessor2();
		//DiscreteProcessor4();
	}
	if (DVP == 1 || DVP == 3)
	{
		DiscreteProcessor3();
	}

	if (ModeCode26[MC26_Guidance_Reference_Failure] && lvda.GetCMCSIVBTakeover() && lvda.GetSCControlPoweredFlight() && !ModeCode26[MC26_SCControlAfterGRF])
	{
		ModeCode26[MC26_SCControlAfterGRF] = true;
		fprintf(lvlog, "[%d+%f] Permanent SC Control bit set\r\n", LVDC_Timebase, LVDC_TB_ETime);
	}

	if (!ModeCode27[MC27_SCInControl] && lvda.GetCMCSIVBTakeover())
	{
		if ((LVDC_Timebase == 5 || LVDC_Timebase == 7) && LVDC_TB_ETime > 5.0)
		{
			ModeCode27[MC27_SCInControl] = true;
			fprintf(lvlog, "[%d+%f] SC has taken control of Saturn (coasting flight)\r\n", LVDC_Timebase, LVDC_TB_ETime);
		}
		else if (ModeCode26[MC26_Guidance_Reference_Failure] && lvda.GetSCControlPoweredFlight())
		{
			ModeCode27[MC27_SCInControl] = true;
			fprintf(lvlog, "[%d+%f] SC has taken control of Saturn (GRF)\r\n", LVDC_Timebase, LVDC_TB_ETime);
		}
	}
	else if (ModeCode27[MC27_SCInControl] && !ModeCode26[MC26_SCControlAfterGRF] && !lvda.GetCMCSIVBTakeover())
	{
		ModeCode27[MC27_SCInControl] = false;
		fprintf(lvlog, "[%d+%f] Saturn control returned to LVDC\r\n", LVDC_Timebase, LVDC_TB_ETime);

		//We shouldn't get here if control was taken over during powered flight
		if (ModeCode27[MC27_TrackLocalHoriz] || ModeCode27[MC27_AttHoldWrtLocalContRetFromSC])
		{
			CommandedAttitude.x = CurrentAttitude.x;
			sin_chi_Yit = sin(CurrentAttitude.y);
			cos_chi_Yit = cos(CurrentAttitude.y);
			sin_chi_Zit = sin(CurrentAttitude.z);
			cos_chi_Zit = cos(CurrentAttitude.z);

			VECTOR3 P4;
			P4 = mul(MX_G, _V(cos_chi_Yit*cos_chi_Zit, sin_chi_Zit, -sin_chi_Yit * cos_chi_Zit));
			alpha_2 = atan2(P4.y, sqrt(1.0 - P4.y*P4.y));
			Pos4 = mul(MX_G, R_OG);
			alpha_1 = atan2(Pos4.z, Pos4.x) + atan2(-P4.z, P4.x) + PI05;
			fprintf(lvlog, "New local referenced attitude: R %.2lf P %.2lf Y %.2lf\r\n", CommandedAttitude.x*DEG, alpha_1*DEG, alpha_2*DEG);

			ModeCode27[MC27_AttHoldWrtInertialContRetFromSC] = false;
			ModeCode27[MC27_AttHoldWrtLocalContRetFromSC] = true;
		}
		else
		{
			CommandedAttitude = PCommandedAttitude = CurrentAttitude;
			ModeCode27[MC27_AttHoldWrtInertialContRetFromSC] = true;
			ModeCode27[MC27_AttHoldWrtLocalContRetFromSC] = false;
		}
		ModeCode27[MC27_InertialHoldInProgress] = false;
		ModeCode27[MC27_TrackLocalHoriz] = false;
	}

	//CMC controlled S-IVB Shutdown
	if (DPM[DIN23_SCInitiationOfSIVBEngineCutoff] == false && lvda.GetCMCSIVBCutoff())
	{
		if (ModeCode26[MC26_SecondSIVBCutoffCommand] == false)
		{
			fprintf(lvlog, "SIVB BACKUP CUTOFF! TMM = %f \r\n", TMM);
			ModeCode26[MC26_SecondSIVBCutoffCommand] = true;

			DVASW = DVASW | MSKSSS4C1;
			SwitchSelectorProcessor(0);
		}
	}

	//S-IVB O2/H2 Burner Malfunction
	if (DPM[DIN3_O2H2BurnerMalfunction] == false && lvda.GetSIVBO2H2BurnerMalfunction())
	{
		//TBD: Start TB6a or TB6b
	}

	//Discrete 6: Spacecraft separation sensed
	if (KSSINDXTB7A > 0 && DPM[DIN4_SpacecraftSeparation] == false && lvda.SpacecraftSeparationIndication())
	{
		DPM[DIN4_SpacecraftSeparation] = true;
		DVASW = DVASW | MSKSSTB5A;
		SwitchSelectorProcessor(0);
	}
}

void LVDCSV::DiscreteProcessor1()
{
	bool fail = false;
	if (DPM[DIN11_SICInboardEngineOutB] == false && lvda.GetSICInboardEngineCutoff())
	{
		fail = true;
		DPM[DIN11_SICInboardEngineOutB] = true;
		ModeCode25[MC25_SIC_InboardEngineCutoff] = true;
	}
	if (DPM[DIN14_SICOutboardEngineOut] == false && lvda.GetSIOutboardEngineOut())
	{
		fail = true;
		DPM[DIN14_SICOutboardEngineOut] = true;
		ModeCode25[MC25_SIC_OutboardEngineCutoff] = true;
	}

	if (fail)
	{
		if (T_EO1 == 0)
		{
			T_EO1 = TAS - TB1;
			fprintf(lvlog, "[%d+%f] S1C engine out discrete received! t_fail = %f\r\n", LVDC_Timebase, LVDC_TB_ETime, T_EO1);
		}
	}
}

void LVDCSV::DiscreteProcessor2()
{
	bool fail = false;
	//If we don't do a center engine cutoff treat center engine failure as normal. Otherwise ignore it
	if (SIICenterEngineCutoff == false && DPM[DIN13_SIIInboardEngineOut] == false && lvda.GetSIIInboardEngineOut())
	{
		fail = true;
		DPM[DIN13_SIIInboardEngineOut] = true;
		ModeCode25[MC25_SIIInboardEngineOut] = true;
	}
	//Outer engine failures are always treated normally
	if (DPM[DIN21_SIIOutboardEngineOut] == false && lvda.GetSIIOutboardEngineOut())
	{
		fail = true;
		DPM[DIN21_SIIOutboardEngineOut] = true;
		ModeCode25[MC25_SIIOutboardEngineOut] = true;
	}
	//If all S-II engines fail, start TB4. Also require the interrupt being enabled because DIN19 gets enabled early. But why?
	if (DPM[DIN19_SIIEnginesOut] == false && DVIH[INT6_SIIEnginesCutoff] == false && lvda.GetSIIEnginesOut())
	{
		StartTimeBase4();
	}

	if (fail)
	{
		if (T_EO2 == 0)
		{
			T_EO2 = 1;
			if (ModeCode25[MC25_SII_IGMBeforeEMRC] == false)
			{
				//pre-IGM S2 engine fail handling
				T_0 = t_21 + dt_LET - TMM;
				T_1 = (T_0 / 4) + ((5 * T_1) / 4);
				T_2 = 5 * T_2 / 4;
				tau2 = 5 * tau2 / 4;
				fprintf(lvlog, "[%d+%f] Pre-IGM SII engine out interrupt received!\r\n", LVDC_Timebase, LVDC_TB_ETime);
			}
			else
			{
				//IGM engine fail
				T_1 = 5.0 * T_1 / 4.0;
				T_2 = 5.0 * T_2 / 4.0;
				tau2 = 5.0 * tau2 / 4.0;
				fprintf(lvlog, "[%d+%f] SII engine out interrupt received!\r\n", LVDC_Timebase, LVDC_TB_ETime);
			}
		}
	}
}

void LVDCSV::DiscreteProcessor3()
{
	//Backup start of TB4a or S-IVB cutoff
	if (directstagereset && lvda.SCInitiationOfSIISIVBSeparation() == false)
	{
		directstagereset = false;
	}
	if (DPM[DIN22_SCInitSIISIVBSepB_SIVBEngineCutoffB] == false && directstagereset == false && lvda.SCInitiationOfSIISIVBSeparation())
	{
		SCInitiationOfSIISIVBSeparationInterrupt();
	}
}

void LVDCSV::BoostNavigation()
{
	fprintf(lvlog, "BOOST NAVIGATION \r\n");
	//Compute New Space Fixed Position
	PosS = PosS + (DotM_act + DotM_last) * dt_c / 2.0 + (DotG_last + ddotG_last * dt_c / 2.0)*dt_c; //position vector
	R = length(PosS); //instantaneous distance from earth's center
	//Gravitation Subroutine
	ddotG_act = GravitationSubroutine(PosS, true);
	CG = length(ddotG_act);
	//Compute Gravitational Velocity
	DotG_act = DotG_last + (ddotG_act + ddotG_last)*dt_c / 2.0;
	//Compute Space-Fixed Velocity
	DotS = DotM_act + DotG_act; //total velocity vector
	//Save old velocity for Timebase 5/7 check
	VOLD = V;
	V = length(DotS);
	//save the 'actual' variables as 'last' variables for the next step
	DotM_last = DotM_act;
	DotG_last = DotG_act;
	ddotG_last = ddotG_act;

	fprintf(lvlog, "Inertial Attitude: %f %f %f \r\n", CurrentAttitude.x*DEG, CurrentAttitude.y*DEG, CurrentAttitude.z*DEG);
	fprintf(lvlog, "DotM: %f %f %f \r\n", DotM_act.x, DotM_act.y, DotM_act.z);
	fprintf(lvlog, "Gravity velocity: %f %f %f \r\n", DotG_act.x, DotG_act.y, DotG_act.z);
	VECTOR3 drtest = SVCompare();
	fprintf(lvlog, "SV Accuracy: %f %f %f\r\n", drtest.x, drtest.y, drtest.z);
	fprintf(lvlog, "EarthRel Position: %f %f %f \r\n", PosS.x, PosS.y, PosS.z);
	fprintf(lvlog, "EarthRel Velocity: %f %f %f \r\n", DotS.x, DotS.y, DotS.z);
	fprintf(lvlog, "State vector time: %f \r\n", TAS);
	fprintf(lvlog, "Gravity Acceleration: %f \r\n", CG);
	fprintf(lvlog, "Total Velocity: %f \r\n", V);
	fprintf(lvlog, "Dist. from Earth's Center: %f \r\n", R);
	fprintf(lvlog, "S: %f \r\n", S);
	fprintf(lvlog, "P: %f \r\n", P);
}

void LVDCSV::TimeTiltGuidance()
{
	fprintf(lvlog, "TIME TILT GUIDANCE\r\n");
	t_clock = TAS - TB1;
	if (ModeCode25[MC25_StartPitchAndRollManeuver] == false && ((PosS.x - a) > 137 || t_clock > t_1))
	{
		ModeCode25[MC25_StartPitchAndRollManeuver] = true;
	}
	if (ModeCode25[MC25_StopRollManeuver] == false && (CurrentAttitude.x < 0.5*RAD || CurrentAttitude.x >359.5*RAD))
	{
		ModeCode25[MC25_StopRollManeuver] = true;
	}

	if (ModeCode25[MC25_StartPitchAndRollManeuver])
	{
		//After tower is cleared, set commanded roll and yaw to 0
		CommandedAttitude.x = 0;
		CommandedAttitude.z = 0;
		//Pitch program
		if (t_clock > TMEFRZ)
		{
			int x = 0, y = 0;
			double Tc = t_clock - dT_F, cmd = 0;
			if (Tc < T_S1) { x = 1; }
			if (T_S1 <= Tc && Tc < T_S2) { x = 2; }
			if (T_S2 <= Tc && Tc < T_S3) { x = 3; }
			if (T_S3 <= Tc) { x = 4; }
			while (y <= 4) {
				cmd += (Fx[x][y] * ((double)pow(Tc, y)));
				y++;
			}
			CommandedAttitude.y = cmd * RAD;
			fprintf(lvlog, "[%d+%f] Roll/pitch programm %f \r\n", LVDC_Timebase, LVDC_TB_ETime, cmd);

			//Engine-out freeze time
			if (t_clock > t_4 && T_EO1 > 0.0 && T_EO1 < t_5 && dT_F == 0.0)
			{
				if (T_EO1 <= t_2) { dT_F = t_3; }
				if (t_2 < T_EO1 && T_EO1 <= t_4) { dT_F = (B_11 * T_EO1) + B_12; }
				if (t_4 < T_EO1 && T_EO1 <= t_5) { dT_F = (B_21 * T_EO1) + B_22; }
				if (t_5 < T_EO1) { dT_F = 0; }

				TMEFRZ = t_clock + dT_F;
				T_ar = T_ar + dT_F;
				fprintf(lvlog, "[%d+%f] Freeze time recalculated! TMEFRZ = %f dT_F = %f T_ar = %f\r\n", LVDC_Timebase, LVDC_TB_ETime, TMEFRZ, dT_F, T_ar);
			}
		}
		if (t_clock >= T_ar)
		{
			NISTAT[8] = false;
			ModeCode25[MC25_StopPitchManeuver] = true;
		}
	}
}

void LVDCSV::IterativeGuidanceMode()
{
	//fprintf(lvlog, "ITERATIVE GUIDANCE MODE\r\n");
	if (HSL == false) {
		// We are not in the high-speed loop
		//fprintf(lvlog, "HSL False\r\n");
		// IGM STAGE LOGIC
		if (S4B_REIGN)
		{
			//fprintf(lvlog, "S-IVB 2nd BURN\n");
			if (MRS)
			{
				//fprintf(lvlog, "MRS\r\n");
				Tt_3 += T_2 * (dotM_2 / dotM_3);
				//fprintf(lvlog, "Tt_3 = %f\r\n", Tt_3);
				if (t_B2 <= t_B4)
				{
					goto relightentry1;
				}
				t_B4 += dt_c;
				//fprintf(lvlog, "t_B4 = %f\r\n", t_B4);
			}
			else
			{
				if (T_2 < 0)
				{
					P_c += dt_c;
					if (P_c > K_pc)
					{
						MRS = true;
						t_B2 = 0;
						//fprintf(lvlog, "MRS\r\n");
					}
				}
				else
				{
					goto taubypass;
				}
			}
			tau3 += T_2 * (dotM_2 / dotM_3);
			T_2 = 0;
		taubypass:
			if (Ct >= Ct_o)
			{
				goto relightentry2;
			}
			else
			{
				tau2 = tau2N + (V_ex2 * MFS - dt_c / 2.0 - tau2N)*pow(Ct / Ct_o, 4);
				tau2N -= dt_c;
				Ct += dt_c;
				//fprintf(lvlog, "Art. Tau Mode 2: tau2 = %f, tau2N = %f, Ct = %f, Diff = %f\r\n", tau2, tau2N, Ct, tau2 - V_ex2 * MFS);
				goto relightentry3;
			}
		}
		if (S4B_IGN == true) {
			//fprintf(lvlog, "S-IVB 1st BURN\n");
			if (Ct >= Ct_o) {
			relightentry1:
				tau3 = V_ex3 * MFS;
				//fprintf(lvlog, "Normal Tau: tau3 = %f\r\n", tau3);
			}
			else {
				tau3 = tau3N + (V_ex3 *MFS - dt_c / 2 - tau3N)*pow((Ct / Ct_o), 4);
				tau3N = tau3N - dt_c;
				Ct = Ct + dt_c;
				//fprintf(lvlog, "Art. Tau Mode 3: tau3 = %f, tau3N = %f, Ct = %f, Diff = %f\r\n", tau3, tau3N, Ct, tau3 - V_ex3 * MFS);
			}
			GATE = false; //end chi freeze
			T_c = 0;
			T_2 = 0;
			T_1 = 0;
			//fprintf(lvlog, "GATE = false, T_c = 0, T_1 = 0, T_2 = 0\r\n");
			goto chitilde;
		}
		if (S2_BURNOUT == true) {
			fprintf(lvlog, "SII CUTOFF\n");
			if (T_c < 0) {
				//this prevents T_c from getting negative in case of late SIVB ignition
				T_c = 0;
				T_2 = 0;
				T_1 = 0;
				//fprintf(lvlog, "T_c = 0, T_1 = 0, T_2 = 0\r\n");
				goto chitilde;
			}
			else {
				//chi freeze, kill the first two stage integrals
				GATE = true;
				T_2 = 0;
				T_1 = 0;
				//fprintf(lvlog, "GATE = true, T_1 = 0, T_2 = 0\r\n");
				goto chitilde;
			}
		}
		if (MRS == true) {
			//fprintf(lvlog, "Post-MRS\n");
			if (t_B1 <= t_B3) {
			relightentry2:
				tau2 = V_ex2 * MFS;
				//fprintf(lvlog, "Normal Tau: tau2 = %f, F/m = %f\r\n", tau2, 1.0 / MFS);
			}
			else {
				// This is the "ARTIFICIAL TAU" code.
				t_B3 += dt_c;
				tau2 = tau2 + (T_1*(dotM_1 / dotM_2));
				//fprintf(lvlog, "Art. Tau: tau2 = %f, T_1 = %f, dotM_1 = %f dotM_2 = %f \r\n", tau2, T_1, dotM_1, dotM_2);
				//fprintf(lvlog, "Diff: %f \r\n", (tau2 - V_ex2 * MFS));
			}
			// This T_2 test is also tested after T_1 < 0 etc etc
		relightentry3:
			if (T_2 > 0) {
				T_2 = T_2 + T_1 * (dotM_1 / dotM_2);
				T_1 = 0;
				//fprintf(lvlog, "T_1 = 0\r\nT_2 = %f, dotM_1 = %f, dotM_2 = %f \r\n", T_2, dotM_1, dotM_2);
				// Go to CHI-TILDE LOGIC
			}
			else {
				T_2 = 0;
				T_1 = 0;
				//fprintf(lvlog, "T_1 = 0, T_2 = 0\r\n");
				// Go to CHI-TILDE LOGIC
			}
			if (!S4B_REIGN && T_2 <= ART) { GATE = true; }//pre SIVB-staging chi-freeze
		}
		else {
			//fprintf(lvlog, "Pre-MRS\n");
			if (T_1 < 0) {
				// If we're out of first-stage IGM time
				// Artificial Tau
				tau2 = tau2 + (T_1*(dotM_1 / dotM_2));
				//fprintf(lvlog, "Art. Tau: tau2 = %f, T_1 = %f, dotM_1 = %f, dotM_2 = %f \r\n", tau2, T_1, dotM_1, dotM_2);
				if (T_2 > 0) {
					T_2 = T_2 + T_1 * (dotM_1 / dotM_2);
					T_1 = 0;
					//fprintf(lvlog, "T_2 = %f, T_1 = %f, dotM_1 = %f, dotM_2 = %f \r\n", T_2, T_1, dotM_1, dotM_2);
				}
				else {
					T_2 = 0;
					T_1 = 0;
					//fprintf(lvlog, "T_2 = 0\r\n");
				}
			}
			else {
				tau1 = V_ex1 * MFS;
				//fprintf(lvlog, "Normal Tau: tau1 = %f, F/m = %f\r\n", tau1, 1.0 / MFS);
			}
		}

		// CHI-TILDE LOGIC
		// STAGE INTEGRAL CALCULATIONS				
	chitilde:

		//Special Apollo 9 logic

		if (FixedAttitudeBurn && LVDC_Timebase == 6)
		{
			if (MRS == false)
			{
				T_2 = T_2 - dt_c;
			}
			else
			{
				Tt_3 = Tt_3 - dt_c;
			}

			//fprintf(lvlog, "FIXED ATT BURN: T_2 = %f, Tt_3 = %f \r\n", T_2, Tt_3);

			if (Tt_3 <= 0 && S4B_REIGN == true) {
				//Time for S4B cutoff? We need to check that here -IGM runs every 2 sec only, but cutoff has to be on the second		
				DVASW = DVASW | MSKSSS4C1;
				SwitchSelectorProcessor(0);
				fprintf(lvlog, "SIVB SCHEDULED CUTOFF! TMM = %f \r\n", TMM);
			}

			return;
		}

		//fprintf(lvlog, "--- STAGE INTEGRAL LOGIC ---\r\n");
		Pos4 = mul(MX_G, PosS);
		//fprintf(lvlog, "Pos4 = %f, %f, %f\r\n", Pos4.x, Pos4.y, Pos4.z);
		//fprintf(lvlog, "T_1 = %f,T_2 = %f\r\n", T_1, T_2);
		L_1 = V_ex1 * log(tau1 / (tau1 - T_1));
		J_1 = (L_1 * tau1) - (V_ex1 * T_1);
		S_1 = (L_1 * T_1) - J_1;
		Q_1 = (S_1 * tau1) - ((V_ex1 * pow(T_1, 2)) / 2);
		P_1 = (J_1 * tau1) - ((V_ex1 * pow(T_1, 2)) / 2);
		U_1 = (Q_1 * tau1) - ((V_ex1 * pow(T_1, 3)) / 6);
		//fprintf(lvlog, "L_1 = %f, J_1 = %f, S_1 = %f, Q_1 = %f, P_1 = %f, U_1 = %f\r\n", L_1, J_1, S_1, Q_1, P_1, U_1);

		L_2 = V_ex2 * log(tau2 / (tau2 - T_2));
		J_2 = (L_2 * tau2) - (V_ex2 * T_2);
		S_2 = (L_2 * T_2) - J_2;
		Q_2 = (S_2 * tau2) - ((V_ex2 * pow(T_2, 2)) / 2);
		P_2 = (J_2 * tau2) - ((V_ex2 * pow(T_2, 2)) / 2);
		U_2 = (Q_2 * tau2) - ((V_ex2 * pow(T_2, 3)) / 6);
		//fprintf(lvlog, "L_2 = %f, J_2 = %f, S_2 = %f, Q_2 = %f, P_2 = %f, U_2 = %f\r\n", L_2, J_2, S_2, Q_2, P_2, U_2);

		L_12 = L_1 + L_2;
		J_12 = J_1 + J_2 + (L_2 * T_1);
		S_12 = S_1 - J_2 + (L_12 * (T_2 + T_c));
		Q_12 = Q_1 + Q_2 + (S_2 * T_1) + (J_1 * T_2);
		P_12 = P_1 + P_2 + (T_1 * ((2 * J_2) + (L_2 * T_1)));
		U_12 = U_1 + U_2 + (T_1 * ((2 * Q_2) + (S_2 * T_1))) + (T_2 * P_1);
		//fprintf(lvlog, "L_12 = %f, J_12 = %f, S_12 = %f, Q_12 = %f, P_12 = %f, U_12 = %f\r\n", L_12, J_12, S_12, Q_12, P_12, U_12);

		Lt_3 = V_ex3 * log(tau3 / (tau3 - Tt_3));
		//fprintf(lvlog, "Lt_3 = %f, tau3 = %f, Tt_3 = %f\r\n", Lt_3, tau3, Tt_3);

		Jt_3 = (Lt_3 * tau3) - (V_ex3 * Tt_3);
		//fprintf(lvlog, "Jt_3 = %f", Jt_3);
		Lt_Y = (L_12 + Lt_3);
		//fprintf(lvlog, ", Lt_Y = %f\r\n", Lt_Y);

		// SELECT RANGE OPTION				
	gtupdate:	// Target of jump from further down
		//fprintf(lvlog, "--- GT UPDATE ---\r\n");

		if (Tt_T <= eps_1) {
			// RANGE ANGLE 2 (out-of orbit)
			//fprintf(lvlog, "RANGE ANGLE 2\r\n");
			//sprintf(oapiDebugString(),"LVDC: RANGE ANGLE 2: %f %f",Tt_T,eps_1);
			// LVDC_GP_PC = 30; // STOP
			sin_gam = ((PosS.x*DotS.x) + (PosS.y*DotS.y) + (PosS.z*DotS.z)) / (R*V);
			cos_gam = pow(1.0 - pow(sin_gam, 2), 0.5);
			dot_phi_1 = (V*cos_gam) / R;
			dot_phi_T = (V_T*cos(gamma_T)) / R_T;
			phi_T = atan2(Pos4.z, Pos4.x) + (((dot_phi_1 + dot_phi_T) / 2.0)*Tt_T);
			//fprintf(lvlog, "V = %f, dot_phi_1 = %f, dot_phi_T = %f, phi_T = %f\r\n", V, dot_phi_1, dot_phi_T, phi_T);
		}
		else {
			// RANGE ANGLE 1 (into orbit)
			//fprintf(lvlog, "RANGE ANGLE 1\r\n");
			d2 = (V * Tt_T) - Jt_3 + (Lt_Y * Tt_3) - (ROV / V_ex3) *
				((tau1 - T_1) * L_1 + (tau2 - T_2) * L_2 + (tau3 - Tt_3) * Lt_3) *
				(Lt_Y + V - V_T);
			phi_T = atan2(Pos4.z, Pos4.x) + (1.0 / R_T)*(S_12 + d2)*cos(gamma_T);
			//fprintf(lvlog, "V = %f, d2 = %f, phi_T = %f\r\n", V, d2, phi_T);
		}
		// FREEZE TERMINAL CONDITIONS TEST
		if (!(Tt_T <= eps_3)) {
			// UPDATE TERMINAL CONDITIONS
			//fprintf(lvlog, "UPDATE TERMINAL CONDITIONS\r\n");
			f = phi_T + alpha_D;
			R_T = p / (1 + ((e*(cos(f)))));
			//fprintf(lvlog, "f = %f, R_T = %f, phi_T = %f, alpha_D = %f\r\n", f, R_T, phi_T, alpha_D);
			V_T = K_5 * pow(1 + ((2 * e)*(cos(f))) + pow(e, 2), 0.5);
			gamma_T = atan2((e*(sin(f))), (1 + (e*(cos(f)))));
			G_T = -mu / pow(R_T, 2);
			//fprintf(lvlog, "V_T = %f, gamma_T = %f, G_T = %f\r\n", V_T, gamma_T, G_T);
		}
		// ROT TEST
		if (ROT) {
			// ROTATED TERMINAL CONDITIONS (out-of-orbit)
			//fprintf(lvlog, "ROTATED TERMINAL CONDITIONS\r\n");
			//sprintf(oapiDebugString(),"LVDC: ROTATED TERMINAL CNDS");
			xi_T = R_T * cos(gamma_T);
			dot_zeta_T = V_T;
			dot_xi_T = 0.0;
			ddot_zeta_GT = G_T * sin(gamma_T);
			ddot_xi_GT = G_T * cos(gamma_T);
			phi_T = phi_T - gamma_T;
			//fprintf(lvlog, "xi_T = %f, dot_zeta_T = %f, dot_xi_T = %f\r\n", xi_T, dot_zeta_T, dot_xi_T);
			//fprintf(lvlog, "ddot_zeta_GT = %f, ddot_xi_GT = %f\r\n", ddot_zeta_GT, ddot_xi_GT);

			// LVDC_GP_PC = 30; // STOP
		}
		else {
			// UNROTATED TERMINAL CONDITIONS (into-orbit)
			//fprintf(lvlog, "UNROTATED TERMINAL CONDITIONS\r\n");
			xi_T = R_T;
			dot_zeta_T = V_T * (cos(gamma_T));
			dot_xi_T = V_T * (sin(gamma_T));
			ddot_zeta_GT = 0;
			ddot_xi_GT = G_T;
			//fprintf(lvlog, "xi_T = %f, dot_zeta_T = %f, dot_xi_T = %f\r\n", xi_T, dot_zeta_T, dot_xi_T);
			//fprintf(lvlog, "ddot_zeta_GT = %f, ddot_xi_GT = %f\r\n", ddot_zeta_GT, ddot_xi_GT);
		}
		// ROTATION TO TERMINAL COORDINATES
		//fprintf(lvlog, "--- ROTATION TO TERMINAL COORDINATES ---\r\n");
		// This is the last time PosS is referred to.
		MX_phi_T.m11 = (cos(phi_T));    MX_phi_T.m12 = 0; MX_phi_T.m13 = ((sin(phi_T)));
		MX_phi_T.m21 = 0;               MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
		MX_phi_T.m31 = (-sin(phi_T)); MX_phi_T.m32 = 0; MX_phi_T.m33 = (cos(phi_T));
		//fprintf(lvlog, "MX_phi_T R1 = %f %f %f\r\n", MX_phi_T.m11, MX_phi_T.m12, MX_phi_T.m13);
		//fprintf(lvlog, "MX_phi_T R2 = %f %f %f\r\n", MX_phi_T.m21, MX_phi_T.m22, MX_phi_T.m23);
		//fprintf(lvlog, "MX_phi_T R3 = %f %f %f\r\n", MX_phi_T.m31, MX_phi_T.m32, MX_phi_T.m33);

		MX_K = mul(MX_phi_T, MX_G);
		//fprintf(lvlog, "MX_K R1 = %f %f %f\r\n", MX_K.m11, MX_K.m12, MX_K.m13);
		//fprintf(lvlog, "MX_K R2 = %f %f %f\r\n", MX_K.m21, MX_K.m22, MX_K.m23);
		//fprintf(lvlog, "MX_K R3 = %f %f %f\r\n", MX_K.m31, MX_K.m32, MX_K.m33);

		PosXEZ = mul(MX_K, PosS);
		DotXEZ = mul(MX_K, DotS);
		//fprintf(lvlog, "PosXEZ = %f %f %f\r\n", PosXEZ.x, PosXEZ.y, PosXEZ.z);
		//fprintf(lvlog, "DotXEZ = %f %f %f\r\n", DotXEZ.x, DotXEZ.y, DotXEZ.z);

		VECTOR3 RTT_T1, RTT_T2;
		RTT_T1.x = ddot_xi_GT; RTT_T1.y = 0;        RTT_T1.z = ddot_zeta_GT;
		RTT_T2 = ddotG_act;
		//fprintf(lvlog, "RTT_T1 = %f %f %f\r\n", RTT_T1.x, RTT_T1.y, RTT_T1.z);
		//fprintf(lvlog, "RTT_T2 = %f %f %f\r\n", RTT_T2.x, RTT_T2.y, RTT_T2.z);

		RTT_T2 = mul(MX_K, RTT_T2);
		//fprintf(lvlog, "RTT_T2 (mul) = %f %f %f\r\n", RTT_T2.x, RTT_T2.y, RTT_T2.z);

		RTT_T1 = RTT_T1 + RTT_T2;
		//fprintf(lvlog, "RTT_T1 (add) = %f %f %f\r\n", RTT_T1.x, RTT_T1.y, RTT_T1.z);

		DDotXEZ_G = _V(0.5*RTT_T1.x, 0.5*RTT_T1.y, 0.5*RTT_T1.z);
		//fprintf(lvlog, "ddot_XEZ_G = %f %f %f\r\n", DDotXEZ_G.x, DDotXEZ_G.y, DDotXEZ_G.z);

		// ESTIMATED TIME-TO-GO
		//fprintf(lvlog, "--- ESTIMATED TIME-TO-GO ---\r\n");

		dot_dxit = dot_xi_T - DotXEZ.x - (DDotXEZ_G.x*Tt_T);
		dot_detat = -DotXEZ.y - (DDotXEZ_G.y * Tt_T);
		dot_dzetat = dot_zeta_T - DotXEZ.z - (DDotXEZ_G.z * Tt_T);
		//fprintf(lvlog, "dot_XEZt = %f %f %f\r\n", dot_dxit, dot_detat, dot_dzetat);
		dV = pow((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)), 0.5);
		dL_3 = (((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)) / Lt_Y) - Lt_Y) / 2;
		// if(dL_3 < 0){ sprintf(oapiDebugString(),"Est TTG: dL_3 %f (X/E/Z %f %f %f) @ Cycle %d (TB%d+%f)",dL_3,dot_dxit,dot_detat,dot_dzetat,IGMCycle,LVDC_Timebase,LVDC_TB_ETime);
		//	LVDC_GP_PC = 30; break; } 

		dT_3 = (dL_3*(tau3 - Tt_3)) / V_ex3;
		T_3 = Tt_3 + dT_3;
		T_T = Tt_T + dT_3;
		//fprintf(lvlog, "dV = %f, dL_3 = %f, dT_3 = %f, T_3 = %f, T_T = %f\r\n", dV, dL_3, dT_3, T_3, T_T);

		// TARGET PARAMETER UPDATE
		if (!(UP > 0)) {
			//fprintf(lvlog, "--- TARGET PARAMETER UPDATE ---\r\n");
			UP = 1;
			Tt_3 = T_3;
			Tt_T = T_T;
			//fprintf(lvlog, "UP = 1, Tt_3 = %f, Tt_T = %f\r\n", Tt_3, Tt_T);
			Lt_3 = Lt_3 + dL_3;
			Lt_Y = Lt_Y + dL_3;
			Jt_3 = Jt_3 + (dL_3*T_3);
			//fprintf(lvlog, "Lt_3 = %f, Lt_Y = %f, Jt_3 = %f\r\n", Lt_3, Lt_Y, Jt_3);

			// NOTE: This is perfectly valid. Just because Dijkstra and Wirth think otherwise
			// does not mean it's gospel. I shouldn't have to defend my choice of instructions
			// because a bunch of people read the title of the paper with no context and take
			// it as a direct revelation from God with no further study into the issue.
			//fprintf(lvlog, "RECYCLE\r\n");
			goto gtupdate; // Recycle. 
		}

		// tchi_y AND tchi_p CALCULATIONS
		//fprintf(lvlog, "--- tchi_y/p CALCULATION ---\r\n");

		L_3 = Lt_3 + dL_3;
		J_3 = Jt_3 + (dL_3*T_3);
		S_3 = (L_3*T_3) - J_3;
		Q_3 = (S_3*tau3) - ((V_ex3*pow(T_3, 2)) / 2);
		P_3 = (J_3*(tau3 + (2 * T_1c))) - ((V_ex3*pow(T_3, 2)) / 2);
		U_3 = (Q_3*(tau3 + (2 * T_1c))) - ((V_ex3*pow(T_3, 3)) / 6);
		//fprintf(lvlog, "L_3 = %f, J_3 = %f, S_3 = %f, Q_3 = %f, P_3 = %f, U_3 = %f\r\n", L_3, J_3, S_3, Q_3, P_3, U_3);

		// This is where velocity-to-be-gained is generated.

		dot_dxi = dot_dxit - (DDotXEZ_G.x   * dT_3);
		dot_deta = dot_detat - (DDotXEZ_G.y  * dT_3);
		dot_dzeta = dot_dzetat - (DDotXEZ_G.z * dT_3);
		//fprintf(lvlog, "dot_dXEZ = %f %f %f\r\n", dot_dxi, dot_deta, dot_dzeta);

		//sprintf(oapiDebugString(),".dxi = %f | .deta %f | .dzeta %f | dT3 %f",
		//	dot_dxi,dot_deta,dot_dzeta,dT_3);

		L_Y = L_12 + L_3;
		tchi_y_last = tchi_y;
		tchi_p_last = tchi_p;
		tchi_y = atan2(dot_deta, pow(pow(dot_dxi, 2) + pow(dot_dzeta, 2), 0.5));
		tchi_p = atan2(dot_dxi, dot_dzeta);
		UP = -1;
		//fprintf(lvlog, "L_Y = %f, tchi_y = %f, tchi_p = %f, UP = -1\r\n", L_Y, tchi_y, tchi_p);

		if (CHIBARSTEER == false && Tt_T <= eps_2)
		{
			if (BOOST)
			{
				ModeCode25[MC25_TerminalGuidanceFirstBurn] = true;
				DT_N = DT_N4;
				MS25DT = 25.0*DT_N;
				MS04DT = 0.04 / DT_N;
			}
			else
			{
				ModeCode26[MC26_TerminalGuidanceSecondBurn] = true;
				DT_N = DT_N4;
				MS25DT = 25.0*DT_N;
				MS04DT = 0.04 / DT_N;
			}
			CHIBARSTEER = true;
		}

		// *** END OF CHI-TILDE LOGIC ***
		// Is it time for chi-tilde mode?
		if (CHIBARSTEER) {
			fprintf(lvlog, "CHI BAR STERRING ON, REMOVE ALTITUDE CONSTRAINS (K_1-4 = 0)\r\n");
			// Yes
			// Go to the test that we would be testing if HSL was true
			K_1 = 0; K_2 = 0; K_3 = 0; K_4 = 0;
			// See the note above if the presence of this goto bothers you.
			// sprintf(oapiDebugString(),"LVDC: HISPEED LOOP ENTRY: Tt_T %f eps_2 %f", Tt_T,eps_2); LVDC_GP_PC = 30; break; // STOP
			goto hsl;
		}
		else {
			// No.
			// YAW STEERING PARAMETERS
			//fprintf(lvlog, "--- YAW STEERING PARAMETERS ---\r\n");

			J_Y = J_12 + J_3 + (L_3*T_1c);
			S_Y = S_12 - J_3 + (L_Y*T_3);
			Q_Y = Q_12 + Q_3 + (S_3*T_1c) + ((T_c + T_3)*J_12);
			K_Y = L_Y / J_Y;
			D_Y = S_Y - (K_Y*Q_Y);
			//fprintf(lvlog, "J_Y = %f, S_Y = %f, Q_Y = %f, K_Y = %f, D_Y = %f\r\n", J_Y, S_Y, Q_Y, K_Y, D_Y);

			deta = PosXEZ.y + (DotXEZ.y*T_T) + ((DDotXEZ_G.y*pow(T_T, 2)) / 2) + (S_Y*(sin(tchi_y)));
			K_3 = deta / (D_Y*(cos(tchi_y)));
			K_4 = K_Y * K_3;
			//fprintf(lvlog, "deta = %f, K_3 = %f, K_4 = %f\r\n", deta, K_3, K_4);

			// PITCH STEERING PARAMETERS
			//fprintf(lvlog, "--- PITCH STEERING PARAMETERS ---\r\n");

			L_P = L_Y * cos(tchi_y);
			C_2 = cos(tchi_y) + (K_3*sin(tchi_y));
			C_4 = K_4 * sin(tchi_y);
			J_P = (J_Y*C_2) - (C_4*(P_12 + P_3 + (pow(T_1c, 2)*L_3)));
			//fprintf(lvlog, "L_P = %f, C_2 = %f, C_4 = %f, J_P = %f\r\n", L_P, C_2, C_4, J_P);

			S_P = (S_Y*C_2) - (C_4*Q_Y);
			Q_P = (Q_Y*C_2) - (C_4*(U_12 + U_3 + (pow(T_1c, 2)*S_3) + ((T_3 + T_c)*P_12)));
			K_P = L_P / J_P;
			D_P = S_P - (K_P*Q_P);
			//fprintf(lvlog, "S_P = %f, Q_P = %f, K_P = %f, D_P = %f\r\n", S_P, Q_P, K_P, D_P);

			dxi = PosXEZ.x - xi_T + (DotXEZ.x*T_T) + ((DDotXEZ_G.x*pow(T_T, 2)) / 2) + (S_P*(sin(tchi_p)));
			K_1 = dxi / (D_P*cos(tchi_p));
			K_2 = K_P * K_1;
			//fprintf(lvlog, "dxi = %f, K_1 = %f, K_2 = %f, cos(tchi_p) = %f\r\n", dxi, K_1, K_2, cos(tchi_p));
		}
	}
	else {
	hsl:		// HIGH-SPEED LOOP ENTRY				
				// CUTOFF VELOCITY EQUATIONS
		//fprintf(lvlog, "--- CUTOFF VELOCITY EQUATIONS ---\r\n");
		V_0 = V_1;
		V_1 = V_2;
		//V_2 = 0.5 * (V+(pow(V_1,2)/V));
		V_2 = V;
		dtt_1 = dtt_2;
		dtt_2 = dt_c;
		//fprintf(lvlog, "V = %f, Tt_t = %f\r\n", V, Tt_T);
		//fprintf(lvlog, "V = %f, V_0 = %f, V_1 = %f, V_2 = %f, dtt_1 = %f, dtt_2 = %f\r\n", V, V_0, V_1, V_2, dtt_1, dtt_2);
		if (Tt_T <= eps_4 && V + V_TC >= V_T) {
			//fprintf(lvlog, "--- HI SPEED LOOP ---\r\n");
			// TGO CALCULATION
			//fprintf(lvlog, "--- TGO CALCULATION ---\r\n");
			if (GATE5 == false && ModeCode26[MC26_Guidance_Reference_Failure] == false) {
				//fprintf(lvlog, "CHI FREEZE\r\n");
				// CHI FREEZE
				tchi_y = tchi_y_last;
				tchi_p = tchi_p_last;
				GATE = true;
				HSL = true;
				GATE5 = true;
				T_GO = T_3;
				DT_N = 0.7; //HSL takes about 0.7 seconds to run
				//fprintf(lvlog, "HSL = true, GATE5 = true, T_GO = %f\r\n", T_GO);
				//Set up engine pump purge control valve enable on sequence on first S-IVB burn
				if (BOOST)
				{
					DVASW = DVASW | MSKSSSPEC;
					SwitchSelectorProcessor(0);
				}
			}
			if (BOOST == true) {
				//fprintf(lvlog, "BOOST-TO-ORBIT ACTIVE\r\n");
				// dT_4 CALCULATION
				if (LVDC_Timebase == 40)
				{
					t_3i = TB4a + T_c;
				}
				else
				{
					t_3i = TB4 + T_c;
				}
				dT_4 = TAS - t_3i - T_4N;
				//dT_4 = t_3i - T_4N;
				//fprintf(lvlog, "t_3i = %f, dT_4 = %f\r\n", t_3i, dT_4);
				if (fabs(dT_4) <= dT_LIM) {
					dTt_4 = dT_4;
				}
				else {
					//fprintf(lvlog, "dTt_4 CLAMPED\r\n");
					dTt_4 = dT_LIM;
				}
				//fprintf(lvlog, "dTt_4 = %f\r\n", dTt_4);
			}
			else {
				// TRANSLUNAR INJECTION VELOCITY
				//fprintf(lvlog, "TRANSLUNAR INJECTION\r\n");
				double dotR = dotp(PosS, DotS) / R;
				R_T = R + dotR * (T_GO - dt_c);
				V_T = sqrt(C_3 + 2.0*mu / R_T);
				dV_B = dV_BR;
				//sprintf(oapiDebugString(),"LVDC: HISPEED LOOP, TLI VELOCITY: %f %f %f %f %f",Tt_T,eps_4,V,V_TC,V_T);
				//fprintf(lvlog, "TLI VELOCITY: R %f dotR %f T_GO %f dt_c %f R_T %f V_T: %f\r\n", R, dotR, T_GO, dt_c, R_T, V_T);
				// LVDC_GP_PC = 30; // STOP
			}
			// TGO DETERMINATION
			//fprintf(lvlog, "--- TGO DETERMINATION ---\r\n");

			a_2 = (((V_2 - V_1)*dtt_1) - ((V_1 - V_0)*dtt_2)) / (dtt_2*dtt_1*(dtt_2 + dtt_1));
			a_1 = ((V_2 - V_1) / dtt_2) + (a_2*dtt_2);
			T_GO = ((V_T - dV_B) - V_2) / (a_1 + a_2 * T_GO);
			T_CO = TAS + T_GO;
			//fprintf(lvlog, "a_2 = %f, a_1 = %f, T_GO = %f, T_CO = %f, V_T = %f\r\n", a_2, a_1, T_GO, T_CO, V_T);

			//sprintf(oapiDebugString(),"TB%d+%f | CP/Y %f %f | -HSL- TGO %f",LVDC_Timebase,LVDC_TB_ETime,CommandedAttitude.y,CommandedAttitude.z,T_GO);
			return;
		}
		// End of high-speed loop
	}
	// GUIDANCE TIME UPDATE
	//fprintf(lvlog, "--- GUIDANCE TIME UPDATE ---\r\n");
	if (BOOST) {
		if (S4B_IGN) {
			T_3 = T_3 - dt_c;
		}
		else {
			if (S2_BURNOUT) {
				T_c = T_c - dt_c;
			}
			else {
				if (MRS == false) {
					T_1 = T_1 - dt_c;
				}
				else {
					if (t_B1 <= t_B3) {
						T_2 = T_2 - dt_c;
					}
					else {
						// Here if t_B1 is bigger.
						//fprintf(lvlog, "t_B1 = %f, t_B3 = %f\r\n", t_B1, t_B3);
						T_1 = (((dotM_1*(t_B3 - t_B1)) - (dotM_2*t_B3))*DT_N) / (dotM_1*t_B1);
					}
				}
			}
		}
		//fprintf(lvlog, "T_1 = %f, T_2 = %f, T_3 = %f, T_c = %f dt_c = %f\r\n", T_1, T_2, T_3, T_c, dt_c);
	}
	else {
		// MRS TEST
		//fprintf(lvlog, "MRS TEST\r\n");
		//sprintf(oapiDebugString(),"LVDC: MRS TEST");
		if (MRS)
		{
			if (t_B2 <= t_B4)
			{
				T_3 = T_3 - dt_c;
			}
			else
			{
				T_2 = (dotM_2*(t_B4 - t_B2) - dotM_3 * t_B4)*dt_c / (dotM_2*t_B2);
			}
		}
		else
		{
			T_2 = T_2 - dt_c;
		}
		//fprintf(lvlog, "T_2 = %f, T_3 = %f, dt_c = %f\r\n", T_2, T_3, dt_c);
		// LVDC_GP_PC = 30; // STOP
	}
	Tt_3 = T_3;
	T_1c = T_1 + T_2 + T_c;
	Tt_T = T_1c + Tt_3;
	//fprintf(lvlog, "Tt_3 = %f, T_1c = %f, Tt_T = %f\r\n", Tt_3, T_1c, Tt_T);
	if (GATE) {
		// FREEZE CHI
		//fprintf(lvlog, "Thru GATE; CHI FREEZE\r\n");
		//sprintf(oapiDebugString(),"LVDC: CHI FREEZE");
		return;
	}
	else {
		// IGM STEERING ANGLES
		//fprintf(lvlog, "--- IGM STEERING ANGLES ---\r\n");

		//sprintf(oapiDebugString(),"IGM: K_1 %f K_2 %f K_3 %f K_4 %f",K_1,K_2,K_3,K_4);
		Xtt_y = ((tchi_y)-K_3 + (K_4 * DT_N));
		Xtt_p = ((tchi_p)-K_1 + (K_2 * DT_N)) - phi_T - PI05;
		//fprintf(lvlog, "Xtt_y = %f, Xtt_p = %f\r\n", Xtt_y, Xtt_p);
		sin_chi_Yit = sin(Xtt_p);
		cos_chi_Yit = cos(Xtt_p);
		sin_chi_Zit = sin(Xtt_y);
		cos_chi_Zit = cos(Xtt_y);

		CommandedAttitude.x = 0.0 * RAD;    // ROLL

		if (ModeCode26[MC26_SMCActive] == true)
		{
			SteeringMisalignmentCorrection();
		}

		ChiComputations(1);
	}
}

//Main reference for this was NTRS 19720013551
void LVDCSV::SwitchSelectorProcessor(int entry)
{
	//entry:
	//0 = Switch selector check for alternate sequence
	//1 = Normal switch selector check
	//2 = Switch selector initialization for time base set
	//3 = Switch selector forced reset
	//4 = Switch selector hung stage test
	//5 = Switch selector stage and address issuance
	//6 = Switch selector verify address
	//7 = Switch selector read time check
	//8 = Switch selector read issuance
	//9 = Switch selector reset
	//10 = Switch selector complement stage and address

	//fprintf(lvlog, "[TB%d+%f] Switch Selector Processor Entry %d\r\n", LVDC_Timebase, LVDC_TB_ETime, entry);

	switch (entry)
	{
	case 1:
		goto MSS05;
	case 2:
		goto MSS10;
	case 3:
		goto MSS20;
	case 4:
		goto MSS30;
	case 5:
		goto MSS40;
	case 6:
		goto MSS50;
	case 7:
		goto MSS55;
	case 8:
		goto MSS60;
	case 9:
		goto MSS70;
	}

	//MSS00: Switch selector check for alternate sequence
	FASE = false;
	//S-IVB cutoff requested?
	if (DVASW & MSKSSS4C0)
	{
		DVASW = DVASW & MSKSSWV;
		if (VASPI & MSKSSS4C0) goto SS0060;
		//Timer1Interrupt(true);
		VASPI = MSKSSS4C0;
		SST1PTR = KSSINDXSIVB;
		goto SS1050;
	}
	//Pump purge alternate sequence requested?
	if (DVASW & MSKSSSPEC)
	{
		DVASW = DVASW & MSKSSWV;
		//Timer1Interrupt(true);
		VASPI = MSKSSSPEC;
		SST1PTR = KSSINDXSIVA;
		goto SS1050;
	}
	//Alternate time base 6C requested?
	if (DVASW & MSKSSTB6C)
	{
		DVASW = DVASW & (~MSKSSTB6C);
		VASPI = VASPI | MSKSST6C;
		ModeCode26[MC26_StartTB6C] = true;
		SST1PTR = KSSINDXTB6C;
		SSTUPD(VATRR);
		goto SS1050;
	}
	//Class 1 alternate sequence requested?
	if (DVASW & MSKSSCLS1)
	{
		//Save current table in progress pointers in class 1
		VSC10 = SST1PTR;
		//Save current alt. seq. in progress indicator
		VSC11 = VASPI;
		//Save time in TB at start of alt. seq.
		VSC12 = VATRR;
		//Set alt. seq. in prog ind for class 1
		VASPI = MSKSSCL1;
		//Switch selector time update
		SSTUPD(VATRR);
		//Set table advance route for class 1 alt. seq.
		FTADV = true;
		//Alternate time base 6A requested?
		if (DVASW & MSKSSTB6A)
		{
			//Reset time base 6A request bit
			DVASW = DVASW & (~MSKSSTB6A);
			//Set bit in mode code 26 for TB6A
			ModeCode26[MC26_TB6a_Started] = true;
			//Set SS table pointers for TB6A alt. seq.
			SST1PTR = KSSINDXTB6A;
		}
		else if (DVASW & MSKSSS4C1)
		{
			//Reset special S-IVB cutoff bit
			DVASW = DVASW & (~MSKSSS4C1);
			//Set SS table pointers for S-IVB cutoff
			SST1PTR = KSSINDXS4C1;
		}
		else
		{
			//Reset time base 6B requested bit
			DVASW = DVASW & (~MSKSSTB6B);
			//Set bit in mode code 26 for TB6B
			ModeCode26[MC26_TB6b_Started] = true;
			//Set SS table pointers for TB6B alt. seq.
			SST1PTR = KSSINDXTB6B;
		}
		goto SS1050;
	}
	if (FSSAC) goto SS0060;
	goto SS0000;
MSS05:
	FSSAC = false;
SS0000:
	//In the actual code this is a check on equality with the maximum positive number
	if (SSTABLE[SST1PTR].time < 10000000.0)
	{
	SS0010:
		//Switch selector time update
		SSTUPD(VSTGO);
		//Save time-to-go to next SS
		VSTGO = VSSRT - VSTGO;
		//Within 500 ms of next SS?
		if (VSTGO < 0.5) goto MSS30;
		//Time update requested?
		if (DFTUP)
		{
			DVTGB = DVTGB + VGBIA;
			VGBIA = 0.0;
			DFTUP = false;
			goto SS0010;
		}
		//Alternative sequence requested?
		if (DVASW != 0) goto SS0170;
		//Compute time for hung stage test
		VSSTM = VSTGO + DVTRB - DVTGB - KCSSK;
		DVSST = VSSTM + TMR;
		SSM = 4 + 3;
		if (DFIL3 == false)
		{
			//Reschedule T1
			Timer1Interrupt(true);
		}
		goto SS0060;
	}
//SS0015:
	if (DVASW != 0) goto SS0170;
	//Schedule switch selector check
	TEMP = RealTimeClock;
	DVSST = TMM + 500.0 + (TEMP - DVRTC);
	SSM = 1 + 3;
	if (DFIL3 == false)
	{
		Timer1Interrupt(true);
	}
	goto SS0060;
SS0170:
	//Process alternate sequence
	//Is a class 3 alt. sequence requested?
	if (!(DVASW & MSKSSCLS3))
	{
		//SS0191: process a class 4 alt. sequence request
		//Acquisition gain requested?
		if (DVASW & MSKSSACQU)
		{
			DVASW = DVASW & (~MSKSSACQU);
			SST2PTR = KSSINDXGAIN;
			SSTUPD(VATR4);
		}
		//Time base 6D requested?
		else if (DVASW & MSKSSTB6D)
		{
			DVASW = DVASW & (~MSKSSTB6D);
			SST2PTR = KSSINDXTB6D;
			SSTUPD(VATR4);
		}
		//Lunar impact sequence
		else
		{
			DVASW = DVASW & (~MSKSSLI);
			SST2PTR = KSSINDXALU;
			VATR4 = 0.0;
		}
		FCLS4 = true;
		FTADV = false;
		//FHST = true;
		SSSelectionAndSetupRoutine();
		goto SS0000;
	}
	//Is an alternate sequence requested?
	if (VASPI != 0) goto SS0060;
	//SS0221: Process a class 3 alt. sequence request
	VSC30 = SST1PTR;
	VSC31 = VASPI;
	VSC32 = VATRR;
	VASPI = MSKSSCL3;
	SSTUPD(VATRR);
	FTADV = true;
	//FHST = true;
	//Generalized switch selector requested
	if (DVASW & MSKSSGNSS)
	{
		DVASW = DVASW & (~MSKSSGNSS);
		SST1PTR = KSSINDXGSS;
		goto SS0230;
	}
	//S-band antenna to low requested
	if (DVASW & MSKSSSBLO)
	{
		DVASW = DVASW & (~MSKSSSBLO);
		SST1PTR = KSSINDXSBLO;
		goto SS0230;
	}
	//S-band antenna to high requested
	if (DVASW & MSKSSSBHI)
	{
		DVASW = DVASW & (~MSKSSSBHI);
		SST1PTR = KSSINDXSBHI;
		goto SS0230;
	}
	//S-band antenna to omni requested
	if (DVASW & MSKSSSBOM)
	{
		DVASW = DVASW & (~MSKSSSBOM);
		SST1PTR = KSSINDXSBLO;
		goto SS0230;
	}
	//ECS water valve close requested
	if (DVASW & MSKSSECSV)
	{
		SST1PTR = KSSINDXECSV;
		goto SS0230;
	}
	//ECS water valve open requested
	if (DVASW & MSKSSECS1)
	{
		SST1PTR = KSSINDXECS1;
		goto SS0230;
	}
	//Is TB5A requested?
	if (DVASW & MSKSSTB5A)
	{
		DVASW = DVASW & (~MSKSSTB5A);
		SST1PTR = KSSINDXTB7A;
		goto SS0230;
	}
	//S-II MRS requested
	SST1PTR = KSSINDXTB3A;
	DVASW = DVASW & (~MSKSST3A);
SS0230:
	SSSelectionAndSetupRoutine();
	goto SS0000;
SS0060:
	if (FASE == false)
	{
		FASE = true;
		//TBD: Release previously enabled interrupts
	}
	return;
MSS10: // Switch selector initialization for time base set
	VASPI = 0;
	VATRR = 0.0;
	FCLS4 = false;
	DVASW = DVASW & MSKSSWV;
	FTADV = true;
	if (LVDC_Timebase == 40)
	{
		SST1PTR = SSTTBPTR[8];
	}
	else
	{
		SST1PTR = SSTTBPTR[LVDC_Timebase - 1];
	}
SS1050:
	SSSelectionAndSetupRoutine();
	if (FSSAC) goto MSS20;
	VSSW = KSSB1;
	//FHST = true;
	goto SS0000;
MSS20: //Switch selector forced reset
	//Issue forced reset
	//FHST = false;
	//Schedule switch selector check
	SSM = 1 + 3;
	SSTUPQ(KSSB8);
	VSSW = KSSB5;
	goto SS0060;
MSS30: //Switch selector hung stage test
	FSSAC = true;
	if (VSNA1 == 0 && VSNA2 == 0)
	{
		FSSAC = false;
		AdvanceToNextSSRoutine();
		goto SS0000;
	}
	goto SS4000;
MSS40: //Switch selector stage and address issuance
	//Real LVDC does a delay here
SS4000:
	//Issue switch selector stage and address
	//TBD: Write to LVDA here maybe
	//Schedule verify address
	SSM = 6 + 3;
	SSTUPQ(VSSW);
	goto SS0060;
MSS50: //Switch selector verify address
	goto MSS55;
MSS55: //Switch selector read time check
	//S-IVB cutoff SS requested?
	if (VASPI & MSKSSS4C0)
	{
		DVSST = 1e10;
		return;
	}
	if (VSSRT == 0.0) goto MSS60;
	//Switch selector time update
	SSTUPD(DVTRB);
	//Within bias time of read?
	if (VSSRT - DVTRB <= 0.0) goto MSS60;
	//Schedule read command
	VSSTM = VSSRT - DVTGB - 0.0; //KSSRB here and in the if above at some point
	DVSST = VSSTM + TMR;
	SSM = 8 + 3;
	if (DFIL3 == false)
	{
		Timer1Interrupt(true);
	}
	return;
MSS60: //Switch selector read issuance
	//Issue read command and stage selected
	lvda.SwitchSelector(VSNA1, VSNA2);
	fprintf(lvlog, "[TB%d+%f] Switch Selector command issued: Stage %d Channel %d\r\n", LVDC_Timebase, LVDC_TB_ETime, VSNA1, VSNA2);
	//Schedule reset read
	SSM = 9 + 3;
	SSTUPQ(KSSB2);
	//S-IVB cutoff SS in progress?
	if (VASPI & MSKSSS4C0)
	{
		VASPI = 0;
		//DFILE = DFILE or MSKFPSCORD
	}
	//TBD
	return;
MSS70:
	//Schedule normal SS check
	SSM = 1 + 3;
	SSTUPQ(KSSB3);
	AdvanceToNextSSRoutine();
	VSSW = KSSB1;
	//TBD: FHST = something
	//Open water valve issued?
	if (VSNA1 == SWITCH_SELECTOR_IU && VSNA2 == 107)
	{
		DVASW = DVASW & (~MSKSSECS1);
		//DFWV = false;
	}
	//Close water valve issued?
	else if (VSNA1 == SWITCH_SELECTOR_IU && VSNA2 == 108)
	{
		DVASW = DVASW & (~MSKSSECSV);
		//DFWV = true;
	}
	//Enable S/C control of Saturn SS issued?
	else if (VSNA1 == SWITCH_SELECTOR_IU && VSNA2 == 68)
	{
		DPM[DIN9_SCControlOfSaturn] = false;
	}
	return;
}

void LVDCSV::SSTUPD(double &time)
{
	TEMP = RealTimeClock;
	time = DVTRB = DVTGB + DVTRR + (TEMP - DVRTC);
}

void LVDCSV::SSTUPQ(double BIAS)
{
	TEMP = RealTimeClock;
	DVTRB = DVTGB + DVTRR + (TEMP - DVRTC);
	VSSTM = BIAS + DVTRR + (TEMP - DVRTC);
	DVSST = VSSTM + TMR;
	if (DFIL3 == false)
	{
		Timer1Interrupt(true);
	}
}

void LVDCSV::AdvanceToNextSSRoutine()
{
	if (FTADV)
	{
		SST1PTR++;
	}
	else
	{
		SST2PTR++;
	}
	SSSelectionAndSetupRoutine();
}

void LVDCSV::SSSelectionAndSetupRoutine()
{
	//SS210
	if (FTADV) goto SS2020;
//SS2160:
	if (SSTABLE[SST2PTR].time >= 0) goto SS2070;
	FCLS4 = false;
	//TBD: Some mode code bits
	goto SS2090;
SS2020:
	if (SSTABLE[SST1PTR].time >= 0) goto SS2030;
	//SS0111
	//Process return from alternate sequence
	//Pump purge alt seq in progress?
	if (VASPI & MSKSSSPEC)
	{
		VASPI = MSKSSS4C0;
		DVASW = DVASW & MSKSSWV;
		SST1PTR = KSSINDXSIVB;
		goto SS2020;
	}
	//Class 3 alt sequence in progress?
	if (VASPI & MSKSSCL3)
	{
		SST1PTR = VSC30;
		VASPI = VSC31;
		VATRR = VSC32;
		goto SS2020;
	}
	//Class 1 alt sequence in progress?
	if (VASPI & MSKSSCL1)
	{
		SST1PTR = VSC10;
		VASPI = VSC11;
		VATRR = VSC12;
		goto SS2020;
	}
	//Reset alternate sequence in progress indicator
	VASPI = 0;
	//Reset alternate time base reference
	VATRR = 0.0;
	//T6 return to T5
	if (first_op)
	{
		SST1PTR = KSSINDXTB5A;
	}
	else
	{
		SST1PTR = KSSINDXTB5B;
	}
	//Should this be here?
	StartTimebase5();
	goto SS2020;
SS2030:
	if (FCLS4 == false) goto SS2040;
SS2070:
	if (SSTABLE[SST1PTR].time + VATRR - 0.5 >= SSTABLE[SST2PTR].time + VATR4)
	{
		FTADV = false;
		VSSRT = SSTABLE[SST2PTR].time + VATR4;
		VSNA1 = SSTABLE[SST2PTR].stage;
		VSNA2 = SSTABLE[SST2PTR].channel;
	}
	else
	{
	SS2090:
		FTADV = true;
	SS2040:
		VSSRT = SSTABLE[SST1PTR].time + VATRR;
		VSNA1 = SSTABLE[SST1PTR].stage;
		VSNA2 = SSTABLE[SST1PTR].channel;
	}
//SS2050:
	VHSTW1 = VSNA1;
	VHSTW2 = VSNA2;
	return;
}

void LVDCSV::EventsProcessor(int entry)
{
	//entry: 0 = timer 2 entry, 1 = time base change entry, 2 = reschedule entry

	const static int EPTTBINDX[10] = { 0, 5, 13, 18, 38, 55, 71, 93, 107, 110 };

	if (entry == 1)
	{
		goto MEP05;
	}
	else if (entry == 2)
	{
		goto MEP10;
	}

	//Timer 2 Entry
EP00:
	if (DFTBCEP == true)
	{
	EP04A:
		DFTBCEP = false;
		goto EP02;
	}
	fprintf(lvlog, "Events Processor: TB%d+%.3lf IDX %d Task %d\r\n", LVDC_Timebase, LVDC_TB_ETime, EPTINDX, EPTABLE[EPTINDX]);
	switch (EPTABLE[EPTINDX])
	{
	case -1: //Do Nothing
		break;
	case 1: //Schedule Water Methanol (TB0+0.0)
		break;
	case 2: //Start looking for liftoff (TB0+16.0)
		DPM[DIN24_Liftoff] = false;
		MLM = 2;
		break;
	case 3: //Nominal liftoff (flight simulation) (TB0+17.0)
		StartTimeBase1(0);
		break;
	case 4: //Start looking for backup liftoff acceleration (TB0+17.5)
		NISTAT[16] = true;
		break;
	case 5: //Time Base 1 Setup (TB1+0.0)
		NISTAT[8] = true;
		break;
	case 6: //Command Init of Yaw Maneuver (TB1+1.0)
		fprintf(lvlog, "[%d+%f] Yaw maneuver\r\n", LVDC_Timebase, LVDC_TB_ETime);
		CommandedAttitude.z = 1.25*RAD;
		break;
	case 7: //Start checking for engine failure (TB1+6.0)
		DPM[DIN11_SICInboardEngineOutB] = false;
		DPM[DIN14_SICOutboardEngineOut] = false;
		break;
	case 8: //Command Term of Yaw Maneuver (TB1+9.0)
		CommandedAttitude.z = 0.0*RAD;
		break;
	case 9: //Set Accel Reason. Test (TB1+10.0)
		break;
	case 10: //Something with multiple engines cutoff?  (TB1+14.0)
		break;
	case 11: //Start Time Base 2 (TB1+X.X, mission dependant)
		StartTimeBase2();
		break;
	case 12: //Disable thrust constraint (TB2+0.0)
		break;
	case 13: //Enable INT5 and DIN 18 (TB2+0.0)
		DVIH[INT5_SICOutboardEnginesCutoffA] = false;
		DPM[DIN18_SICOutboardEngineCutoffB] = false;
		break;
	case 14: // (TB2+18.4)
		//TBD: Start looking for INT5 and DIN18 here instead of above? Only in actual flight
		break;
	case 15: //Flight simulation start of TB3 (TB2+27.5)
		StartTimeBase3();
		break;
	case 16: //Time Base 3 Setup (TB3+0)
		SMCY = SMCZ = 0.0;
		DPM[DIN11_SICInboardEngineOutB] = true;
		DPM[DIN14_SICOutboardEngineOut] = true;
		break;
	case 17: //Set accelerometer reasonability test constants (TB3+0)
		break;
	case 18: //Change gimbal reasonability test constants (TB3+0)
		break;
	case 19: //Something at TB3+0, probably for only flight sim or real flight
		break;
	case 20: //Dequeue time tilt (TB3+0)
		NISTAT[8] = false;
		ModeCode25[MC25_StopPitchManeuver] = true;
		break;
	case 21: //F/M uncertainty for thrust misalignment (TB3+0)
		break;
	case 22: //Enable DIN 22 and INT 2 (TB3+1.4)
		DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = false;
		DPM[DIN22_SCInitSIISIVBSepB_SIVBEngineCutoffB] = false;
		break;
	case 23: //Set acceleration backup profile (TB3+4.4)
		S2_IGNITION = true;
		t_21 = TMM;
		break;
	case 24: //Set accelerometer reasonability test constants (TB3+4.4)
		break;
	case 25: //Enable DIN 19 (TB3+6.7)
		DPM[DIN19_SIIEnginesOut] = false;
		break;
	case 26: //Something at TB3+6.7, probably for only flight sim or real flight
		DPM[DIN13_SIIInboardEngineOut] = false;
		DPM[DIN21_SIIOutboardEngineOut] = false;
		break;
	case 27: //Enqueue F/M Calc, Smoothing (TB3+6.7)
		NISTAT[3] = true;
		break;
	case 28: //Enqueue IGM (TB3+40.6)
		NISTAT[10] = true;
		ModeCode25[MC25_SII_IGMBeforeEMRC] = true;
		break;
	case 29: //Set Minor Loop Parameters (TB3+40.6)
		DT_N = DT_N3;
		MS25DT = 25.0*DT_N;
		MS04DT = 0.04 / DT_N;
		break;
	case 30: //Set SMC Flag (TB3+58.6)
		DFSMC = true;
		break;
	case 31: //Enqueue SMC (TB3+60.6)
		ModeCode26[MC26_SMCActive] = true;
		break;
	case 32: //S-II IECO (TB3+299.0), actual flight only
		if (SIICenterEngineCutoff)
		{
			ModeCode25[MC25_SIIInboardEngineOut] = true;
			DPM[DIN13_SIIInboardEngineOut] = true;
			//T_EO2 = 1;
		}
		break;
	case 33: //TB3+355.0 Enable INT6
		DVIH[INT6_SIIEnginesCutoff] = false;
		break;
	case 34: //TB3+388.5 (flight sim S-II cutoff?)
		break;
	case 35: //TB4+0
		SMCY = SMCZ = 0.0;
		ModeCode26[MC26_SMCActive] = false;
		MF[0] = MF[1] = MF[2] = MF[3] = MF[4] = MFSArr[0] = MFSArr[1] = MFSArr[2] = MFSArr[3] = MFS = MF1;
		break;
	case 36: //TB4+6.5
		DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = false;
		break;
	case 37: //TB4+8.6
		S4B_IGN = true;
		break;
	case 38: //Start checking for TB5 (TB4+10)
		DVIH[INT4_SIVBEngineOutB] = false;
		DPM[DIN5_SIVBEngineOutA] = false;
		NISTAT[17] = true;
		break;
	case 39: //Enqueue F/M Calc, Smoothing (TB4+12)
		NISTAT[3] = true;
		break;
	case 40: //TB4+15
		ModeCode26[MC26_SMCActive] = true;
		break;
	case 51: //Time Base 5 Start
		//Start chi freeze
		AttitudeManeuverState = 0;
		OGStatusWord[0] = true;
		//Inhibit detection of S-IVB engine out
		DVIH[INT4_SIVBEngineOutB] = true;
		DPM[DIN5_SIVBEngineOutA] = true;
		//Inhibit manual initiation of S-IVB engine cutoff
		DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = true;
		DPM[DIN22_SCInitSIISIVBSepB_SIVBEngineCutoffB] = true;
		//Start looking for S/C separation (for Apollo 8)
		DPM[DIN4_SpacecraftSeparation] = false;

		//HSL Exit settings
		S4B_IGN = false;
		GATE = false;
		GATE5 = false;
		Tt_T = 1000;
		HSL = false;
		BOOST = false;
		CHIBARSTEER = false;

		DT_N = DT_N5;
		MS25DT = 25.0*DT_N;
		MS04DT = 0.04 / DT_N;
		MSLIM1 = 0.024*RAD;
		MSLIM2 = 0.016*RAD;

		SMCY = SMCZ = 0.0;
		ModeCode26[MC26_SMCActive] = false;
		break;
	case 52: //TB5+10
		lvda.SetStage(STAGE_ORBIT_SIVB);
		break;
	case 53: //Maneuver to local horizontal (TB5+20)
		fprintf(lvlog, "Initiate orbrate\r\n");
		AttitudeManeuverState = -1;
		alpha_1 = 0.0;
		alpha_2 = 0.0;
		CommandedAttitude.x = 0.0;
		OGStatusWord[1] = true;
		break;
	case 54: //Transition to coasting flight (TB5+100)
		fprintf(lvlog, "Orbit Initialization\r\n");
		PhaseActivator(false);
		DT_N = DT_N6;
		MSLIM1 = 0.06*RAD;
		MSLIM2 = 0.04*RAD;
		MS25DT = 10.0*DT_N;
		MS04DT = 0.1 / DT_N;
		if (ModeCode26[MC26_Guidance_Reference_Failure] == false)
		{
			MSK5 = 0.24*RAD;
		}
		DPM[DIN20_SIVBIgnitionSequenceStart] = false;
		break;
	case 66: //Time Base 6 start (TB6+0.0)
		ModeCode27[MC27_SCInControl] = false;
		DPM[DIN20_SIVBIgnitionSequenceStart] = true;
		ModeCode26[MC26_SCInitOfSIVBCutoff] = false;
		//Reset TB6a, b and c bits
		ModeCode26[MC26_TB6a_Started] = false;
		ModeCode26[MC26_TB6b_Started] = false;
		ModeCode26[MC26_StartTB6C] = false;
		AttitudeManeuverState = -1;
		if (first_op) {
			alpha_1 = K_P1 + K_P2 * dTt_4; //restart angles
			alpha_2 = K_Y1 + K_Y2 * dTt_4;
			CommandedAttitude.x = 0.0 * RAD;
			fprintf(lvlog, "Orient for restart\r\n");
		}
		else {
			alpha_1 = 0 * RAD;
			alpha_2 = 0 * RAD;
			CommandedAttitude.x = 0.0 * RAD;
			fprintf(lvlog, "Maintain orbrate\r\n");
		}
		break;
	case 67: //Check on TLI Inhibit(TB6+41.0)
		break;
	case 68: //Enable DIN3 (TB6+48.0)
		DPM[DIN3_O2H2BurnerMalfunction] = false;
		break;
	case 69: //Switch point between TB6a and TB6b (TB6+341.3)
		break;
	case 70: //Disable DIN3 (TB6+496.7)
		DPM[DIN3_O2H2BurnerMalfunction] = true;
		break;
	case 71: //Start checking on TLI Inhibit all the time(TB6+497.3)
		break;
	case 72: //Stop checking on TLI inhibit(TB6+560.0)
		DPM[DIN6_TLI_Inhibit] = true;
		DPM[DIN4_SpacecraftSeparation] = true;
		break;
	case 73: //Restart Calculations (TB6+578.6)
		NISTAT[6] = true;
		break;
	case 74: //TB6+580.3
		NISTAT[3] = true;
		S4B_REIGN = true;
		MSLIM1 = 0.04*RAD;
		MSLIM2 = 0.04*RAD;
		DT_N = DT_N3;
		MS25DT = 10.0*DT_N;
		MS04DT = 0.1 / DT_N;
		break;
	case 75: //TB6+583
		//Start detecting S-IVB engine out
		DVIH[INT4_SIVBEngineOutB] = false;
		DPM[DIN5_SIVBEngineOutA] = false;
		DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = false;
		DPM[DIN22_SCInitSIISIVBSepB_SIVBEngineCutoffB] = false;
		break;
	case 76: //TB6+584
		//Enqueue IGM
		NISTAT[10] = true;
		NISTAT[12] = false;
		break;
	case 77: //TB6+590
		DPM[DIN23_SCInitiationOfSIVBEngineCutoff] = false;
		DVIH[INT7_GuidanceReferenceRelease] = false;
		ModeCode26[MC26_SMCActive] = true;
		break;
	case 87: //Start Time Base 7
		//Start chi freeze
		AttitudeManeuverState = 0;
		//Inhibit detection of S-IVB engine out
		DVIH[INT4_SIVBEngineOutB] = true;
		DPM[DIN5_SIVBEngineOutA] = true;
		//Inhibit manual initiation of S-IVB engine cutoff
		DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = true;
		DPM[DIN22_SCInitSIISIVBSepB_SIVBEngineCutoffB] = true;
		//Disable CMC cutoff
		DPM[DIN23_SCInitiationOfSIVBEngineCutoff] = true;
		DVIH[INT7_GuidanceReferenceRelease] = true;
		//Start looking for S/C separation (for Apollo 8)
		DPM[DIN4_SpacecraftSeparation] = false;

		ModeCode26[MC26_SMCActive] = false;
		SMCY = SMCZ = 0.0;
		break;
	case 88: //Time in Timebase 7 to begin maneuver to local horizontal attitude (TB7+20)
		AttitudeManeuverState = -1;
		alpha_1 = 0.0;
		alpha_2 = 0.0;
		CommandedAttitude.x = 0.0;
		break;
	case 89: //Time in Timebase 7 to enter orbit initialize and resume orbit navigation (TB7+20)
		fprintf(lvlog, "Orbit Initialization\r\n");
		PhaseActivator(false);
		DT_N = DT_N6;
		MSLIM1 = 0.06*RAD;
		MSLIM2 = 0.04*RAD;
		MS25DT = 10.0*DT_N;
		MS04DT = 0.1 / DT_N;
		if (ModeCode26[MC26_Guidance_Reference_Failure] == false)
		{
			MSK5 = 0.24*RAD;
		}

		//Orbit Init
		ddotS = ddotG_act;
		fprintf(lvlog, "EarthRel Acceleration: %f %f %f \r\n", ddotS.x, ddotS.y, ddotS.z);
		break;
	case 90: //Maneuver to separation attitude, set attitude rate limits (TB7+900.0)
		AttitudeManeuverState = -2;
		alpha_1 = XLunarAttitude.y;
		alpha_2 = XLunarAttitude.z;
		CommandedAttitude.x = XLunarAttitude.x;
		MSLIM1 = 0.1*RAD;
		MSLIM2 = 0.1*RAD;
		//Set up next task, reset of attitude rates, in 300 seconds. Might be useful for updated sep maneuver time
		EPTTIM[EPTINDX + 1] = DVTRR + 300.0;
		fprintf(lvlog, "T&D attitude\r\n");
		break;
	case 91: //Reset attitude rate limits (TB7+1200.0)
		MSLIM1 = 0.06*RAD;
		MSLIM2 = 0.04*RAD;
		break;
	case 92: //Maneuver to slingshot attitude (TB7)
		AttitudeManeuverState = -1;
		alpha_1 = XLunarSlingshotAttitude.y;
		alpha_2 = XLunarSlingshotAttitude.z;
		CommandedAttitude.x = XLunarSlingshotAttitude.x;
		fprintf(lvlog, "Slingshot attitude\r\n");
		break;
	case 100: //Maneuver to slingshot attitude (TB8)
		AttitudeManeuverState = -1;
		alpha_1 = XLunarSlingshotAttitude.y;
		alpha_2 = XLunarSlingshotAttitude.z;
		CommandedAttitude.x = XLunarSlingshotAttitude.x;
		fprintf(lvlog, "Slingshot attitude\r\n");
		break;
	case 101: //Maneuver to comm attitude (TB8)
		AttitudeManeuverState = -1;
		alpha_1 = XLunarCommAttitude.y;
		alpha_2 = XLunarCommAttitude.z;
		CommandedAttitude.x = XLunarCommAttitude.x;
		fprintf(lvlog, "Communications attitude\r\n");
		break;
	case 102: //TB4a+0
		SMCY = SMCZ = 0.0;
		ModeCode26[MC26_SMCActive] = false;
		MF[0] = MF[1] = MF[2] = MF[3] = MF[4] = MFSArr[0] = MFSArr[1] = MFSArr[2] = MFSArr[3] = MFS = MF1;
		break;
	case 103: //TB4a+11.5
		DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = false;
		//Enqueue F/M calculations
		NISTAT[3] = true;
		break;
	case 104: //TB4a+13.3
		S4B_IGN = true;
		//Enqueue IGM
		NISTAT[10] = true;
		ModeCode25[MC25_SII_IGMBeforeEMRC] = true;
		DT_N = DT_N3;
		MS25DT = 25.0*DT_N;
		MS04DT = 0.04 / DT_N;
		break;
	case 105: //Start checking for TB5 (TB4a+15)
		ModeCode26[MC26_SMCActive] = true;
		DVIH[INT4_SIVBEngineOutB] = false;
		DPM[DIN5_SIVBEngineOutA] = false;
		NISTAT[17] = true;
		break;
	}
	if (DFTBCEP == true)
	{
		goto EP04A;
	}
	//Next event
	EPTINDX++;
	if (EPTABLE[EPTINDX])
	{
		goto EP03;
	}
	T2STAT[2] = false;
	fprintf(lvlog, "Events Processor Disabled. TB%d+%.3lf IDX %d\r\n", LVDC_Timebase, LVDC_TB_ETime, EPTINDX);
	if (DFIL1 == false)
	{
		Timer2Interrupt(true);
	}
EP02:
	//TBD: Release previously enabled interrupts
	return;
EP03:
	if (EPTTIM[EPTINDX] == VTOLD)
	{
		//TBD: Release previously enabled interrupts
		goto EP00;
	}
	VTOLD = EPTTIM[EPTINDX];
	DLTTL[2] = TMR + VTOLD;
	goto EP02;
MEP05://Time Base Change Entry
	fprintf(lvlog, "Events Processor Time Base Change Entry\r\n");
	//Special timebase 4a code
	if (LVDC_Timebase == 40)
	{
		EPTINDX = EPTTBINDX[9] - 1;
	}
	else
	{
		EPTINDX = EPTTBINDX[LVDC_Timebase] - 1;
	}
MEP10: //Reschedule Entry
	EPTINDX++;
	if (EPTABLE[EPTINDX])
	{
		goto EP08;
	}
	T2STAT[2] = false;
	fprintf(lvlog, "Events Processor Disabled. TB%d+%.3lf IDX %d\r\n", LVDC_Timebase, LVDC_TB_ETime, EPTINDX);
EP20:
	if (DFIL1 == false)
	{
		Timer2Interrupt(true);
	}
	return;
EP08:
	VTOLD = EPTTIM[EPTINDX];
	DLTTL[2] = TI + VTOLD;
	T2STAT[2] = true;
	fprintf(lvlog, "Events Processor scheduled for GRR+%.3lf IDX %d\r\n", DLTTL[2], EPTINDX);
	goto EP20;
//EGP07
}

void LVDCSV::PeriodicProcessor()
{

}

void LVDCSV::MinorLoop(int entry)
{
	//fprintf(lvlog, "[%d+%f] *** Minor Loop ***\r\n", LVDC_Timebase, LVDC_TB_ETime);

	CutoffLogic();

	//Liftoff search
	if (entry == 2 && LVDC_Timebase == 0 && DPM[DIN24_Liftoff] == false && lvda.GetLiftoff())
	{
		StartTimeBase1(0);
	}

	if (ModeCode26[MC26_Guidance_Reference_Failure] == false)
	{
		if (LVDC_Timebase > 0 && lvda.GetLVIMUFailure())
		{
			ModeCode26[MC26_Guidance_Reference_Failure] = true;
			lvda.SetOutputRegisterBit(GuidanceReferenceFailureA, true);
			lvda.SetOutputRegisterBit(GuidanceReferenceFailureB, true);
		}

		if (!ModeCode26[MC26_Guidance_Reference_Failure])
		{
			CurrentAttitude = lvda.GetLVIMUAttitude();
		}
	}

	if (ModeCode27[MC27_SCInControl] || ModeCode26[MC26_SCControlAfterGRF])
	{
		PCommandedAttitude = CommandedAttitude = CurrentAttitude;
		AttitudeError = AttitudeErrorOld = _V(0.0, 0.0, 0.0);
	}
	else if (!ModeCode26[MC26_Guidance_Reference_Failure])
	{
		//Attitude error commands
		PCommandedAttitude = PCommandedAttitude + DChi_apo;
		for (int i = 0;i < 3;i++)
		{
			if (PCommandedAttitude.data[i] >= PI2)
			{
				PCommandedAttitude.data[i] -= PI2;
			}
			if (PCommandedAttitude.data[i] < 0)
			{
				PCommandedAttitude.data[i] += PI2;
			}
		}

		//sprintf(oapiDebugString(), "FM %.4lf FMS %.4lf TGO: %.4lf CUR: %.4lf %.4lf %.4lf ACMD: %.4lf %.4lf %.4lf CMD: %.4lf %.4lf %.4lf", Fm, 1.0/ MFS, Tt_T, CurrentAttitude.x*DEG, CurrentAttitude.y*DEG, CurrentAttitude.z*DEG,
		//	CommandedAttitude.x*DEG, CommandedAttitude.y*DEG, CommandedAttitude.z*DEG, PCommandedAttitude.x*DEG, PCommandedAttitude.y*DEG, PCommandedAttitude.z*DEG);

		//calculate delta attitude
		DeltaAtt.x = fmod((CurrentAttitude.x - PCommandedAttitude.x + TWO_PI), TWO_PI);
		if (DeltaAtt.x > PI) { DeltaAtt.x -= TWO_PI; }
		DeltaAtt.y = fmod((CurrentAttitude.y - PCommandedAttitude.y + TWO_PI), TWO_PI);
		if (DeltaAtt.y > PI) { DeltaAtt.y -= TWO_PI; }
		DeltaAtt.z = fmod((CurrentAttitude.z - PCommandedAttitude.z + TWO_PI), TWO_PI);
		if (DeltaAtt.z > PI) { DeltaAtt.z -= TWO_PI; }

		// ROLL ERROR
		AttitudeError.x = -(DeltaAtt.x + A3 * DeltaAtt.y);
		// PITCH ERROR
		AttitudeError.y = -(A1 * DeltaAtt.y + A2 * DeltaAtt.z);
		// YAW ERROR
		AttitudeError.z = -(-A4 * DeltaAtt.y + A5 * DeltaAtt.z);

		//Magnitude limit
		if (abs(AttitudeError.x) > MSK16)
		{
			fprintf(lvlog, "Minor Loop: Roll magnitude limited. Magnitude = %f Limit = %f\r\n", AttitudeError.x*DEG, MSK16*DEG);
			if (AttitudeError.x > 0)
			{
				AttitudeError.x = MSK16;
			}
			else
			{
				AttitudeError.x = -MSK16;
			}
		}
		if (abs(AttitudeError.y) > MSK6)
		{
			fprintf(lvlog, "Minor Loop: Pitch magnitude limited. Magnitude = %f Limit = %f\r\n", AttitudeError.y*DEG, MSK6*DEG);
			if (AttitudeError.y > 0)
			{
				AttitudeError.y = MSK6;
			}
			else
			{
				AttitudeError.y = -MSK6;
			}
		}
		if (abs(AttitudeError.z) > MSK6)
		{
			fprintf(lvlog, "Minor Loop: Yaw magnitude limited. Magnitude = %f Limit = %f\r\n", AttitudeError.z*DEG, MSK6*DEG);
			if (AttitudeError.z > 0)
			{
				AttitudeError.z = MSK6;
			}
			else
			{
				AttitudeError.z = -MSK6;
			}
		}
		//Rate limit
		for (int i = 0;i < 3;i++)
		{
			if (AttitudeError.data[i] - AttitudeErrorOld.data[i] > MSK5)
			{
				fprintf(lvlog, "Minor Loop: Rate limited. Axis = %d Magnitude = %f Limit = %f\r\n", i + 1, (AttitudeError.data[i] - AttitudeErrorOld.data[i])*DEG, MSK5*DEG);
				AttitudeError.data[i] = AttitudeErrorOld.data[i] + MSK5;
				DFSMC = false;
			}
			else if (AttitudeError.data[i] - AttitudeErrorOld.data[i] < -MSK5)
			{
				fprintf(lvlog, "Minor Loop: Rate limited. Axis = %d Magnitude = %f Limit = %f\r\n", i + 1, (AttitudeError.data[i] - AttitudeErrorOld.data[i])*DEG, MSK5*DEG);
				AttitudeError.data[i] = AttitudeErrorOld.data[i] - MSK5;
				DFSMC = false;
			}
		}

		AttitudeErrorOld = AttitudeError;
	}

	if (LVDC_Timebase > 0)
	{
		lvda.SetFCCAttitudeError(AttitudeError);
	}
	else
	{
		lvda.SetFCCAttitudeError(_V(0, 0, 0));
	}

	MLT = TMM + MLD;
}

void LVDCSV::CutoffLogic()
{
	if (ModeCode25[MC25_FirstSIVBCutoffCommand] == false && HSL == true && S4B_IGN == true)
	{
		//Accounts for an average half minor loop delay
		if (T_CO - TMM <= 0.02)
		{
			SwitchSelectorProcessor(8);
			fprintf(lvlog, "SIVB VELOCITY CUTOFF! TMM = %f \r\n", TMM);
			ModeCode25[MC25_FirstSIVBCutoffCommand] = true;
		}
	}
	if (ModeCode26[MC26_SecondSIVBCutoffCommand] == false && HSL == true && S4B_REIGN == true)
	{
		//Accounts for an average half minor loop delay and also a 20ms delay in issuing the cutoff command
		if (T_CO - TMM <= 0.04)
		{
			DVASW = DVASW | MSKSSS4C1;
			SwitchSelectorProcessor(0);
			fprintf(lvlog, "SIVB VELOCITY CUTOFF! TMM = %f \r\n", TMM);
			ModeCode26[MC26_SecondSIVBCutoffCommand] = true;
		}
	}
}

void LVDCSV::MinorLoopSupport()
{
	//fprintf(lvlog, "MINOR LOOP SUPPORT\r\n");

	//Attitude Increments
	DChi = CommandedAttitude - PCommandedAttitude;
	for (int i = 0;i < 3;i++)
	{
		if (DChi.data[i] > PI)
		{
			DChi.data[i] -= PI2;
		}
		else if (DChi.data[i] < -PI)
		{
			DChi.data[i] += PI2;
		}
	}
	//fprintf(lvlog, "DChi = %f %f %f\r\n", DChi.x*DEG, DChi.y*DEG, DChi.z*DEG);

	DChi_apo.x = (DChi.x) / (MLR*DT_N);
	DChi_apo.y = (DChi.y) / (MLR*DT_N);
	DChi_apo.z = (DChi.z) / (MLR*DT_N);
	//fprintf(lvlog, "DChi_apo = %f %f %f\r\n", DChi_apo.x*DEG, DChi_apo.y*DEG, DChi_apo.z*DEG);
	//Magnitude limit
	if (abs(DChi_apo.x) > MSLIM1)
	{
		fprintf(lvlog, "Roll magnitude limit. Magnitude = %f Limit = %f\r\n", DChi_apo.x*DEG, MSLIM1*DEG);
		if (DChi_apo.x > 0)
		{
			DChi_apo.x = MSLIM1;
		}
		else
		{
			DChi_apo.x = -MSLIM1;
		}
	}
	if (abs(DChi_apo.y) > MSLIM2)
	{
		fprintf(lvlog, "Pitch magnitude limit. Magnitude = %f Limit = %f\r\n", DChi_apo.y*DEG, MSLIM2*DEG);
		if (DChi_apo.y > 0)
		{
			DChi_apo.y = MSLIM2;
		}
		else
		{
			DChi_apo.y = -MSLIM2;
		}
	}
	if (abs(DChi_apo.z) > MSLIM2)
	{
		fprintf(lvlog, "Yaw magnitude limit. Magnitude = %f Limit = %f\r\n", DChi_apo.z*DEG, MSLIM2*DEG);
		if (DChi_apo.z > 0)
		{
			DChi_apo.z = MSLIM2;
		}
		else
		{
			DChi_apo.z = -MSLIM2;
		}
	}
	//Gimbal-to-Body Transformation
	Chi_xp_apo = PCommandedAttitude.x + DChi_apo.x*MLR*DT_N;
	Chi_zp_apo = PCommandedAttitude.z + DChi_apo.z*MLR*DT_N;
	//fprintf(lvlog, "Gimbal-to-Body Transformation. Chi_xp_apo = %f Chi_zp_apo = %f\r\n", Chi_xp_apo*DEG, Chi_zp_apo*DEG);
	theta_xa = 0.5*(Chi_xp_apo + CurrentAttitude.x);
	if (abs(Chi_xp_apo - CurrentAttitude.x) > PI05)
	{
		theta_xa += PI;
	}
	if (theta_xa >= PI2)
	{
		theta_xa -= PI2;
	}
	theta_za = 0.5*(Chi_zp_apo + CurrentAttitude.z);
	if (abs(Chi_zp_apo - CurrentAttitude.z) > PI05)
	{
		theta_za += PI;
	}
	if (theta_za >= PI2)
	{
		theta_za -= PI2;
	}
	//fprintf(lvlog, "Gimbal-to-Body Transformation. theta_xa = %f theta_za = %f\r\n", theta_xa*DEG, theta_za*DEG);
	A1 = cos(theta_xa) * cos(theta_za);
	A2 = sin(theta_xa);
	A3 = sin(theta_za);
	A4 = sin(theta_xa) * cos(theta_za);
	A5 = cos(theta_xa);
	//fprintf(lvlog, "Euler Correction. A1 = %f A2 = %f A3 = %f A4 = %f A5 = %f\r\n", A1, A2, A3, A4, A5);
}

void LVDCSV::StartTimeBase1(int entry)
{
	//entry:
	//0 = from minor loop (starts immediately), 1 = from major loop (backup)

	if (entry == 1)
	{
		//TBD: Backup acceleration start of TB1
		if (LVDC_TB_ETime > 150.0)
		{
			LVDC_Stop = true;
			return;
		}
		return;  //TBD
	}

	LVDC_Timebase = 1;
	TimeBaseChangeRoutine();
	TB1 = TI;
	DT_N = DT_N2;
	MS25DT = 25.0*DT_N;
	MS04DT = 0.04 / DT_N;
	ModeCode25[MC25_BeginTB1] = true;
	lvda.SwitchSelector(SWITCH_SELECTOR_SI, 0);
	lvda.SetOutputRegisterBit(FiringCommitEnable, false);
	if (MLM == 2)
	{
		MLM = 0;
	}
	DPM[DIN24_Liftoff] = true;
	NISTAT[16] = false;
}

void LVDCSV::StartTimeBase2()
{
	// Begin timebase 2
	if (ModeCode26[MC26_Guidance_Reference_Failure] == false && DotS.z < 500.0)
	{
		LVDC_Stop = true;
		return;
	}

	LVDC_Timebase = 2;
	TimeBaseChangeRoutine();
	TB2 = TI;
	ModeCode25[MC25_BeginTB2] = true;
	DPM[DIN11_SICInboardEngineOutB] = true;
}

void LVDCSV::StartTimeBase3()
{
	LVDC_Timebase = 3;
	TimeBaseChangeRoutine();
	TB3 = TI;
	ModeCode25[MC25_BeginTB3] = true;

	DVIH[INT5_SICOutboardEnginesCutoffA] = true;
	DPM[DIN18_SICOutboardEngineCutoffB] = true;
}

void LVDCSV::StartTimeBase4()
{
	// S2 OECO, start TB4
	//Is this needed?
	S2_BURNOUT = true;
	MRS = false;

	LVDC_Timebase = 4;
	TimeBaseChangeRoutine();
	TB4 = TI;

	//Inhibit all interrupts and DINs that can start TB4
	DVIH[INT6_SIIEnginesCutoff] = true;
	DPM[DIN19_SIIEnginesOut] = true;
	DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = true;

	//Also inhibit the engine failure inputs
	DPM[DIN13_SIIInboardEngineOut] = true;
	DPM[DIN21_SIIOutboardEngineOut] = true;

	//And set the corresponding mode code bits
	ModeCode25[MC25_SIIInboardEngineOut] = true;
	ModeCode25[MC25_SIIOutboardEngineOut] = true;

	//Stop F/M Calculations
	NISTAT[3] = false;
}

void LVDCSV::CheckTimeBase57()
{
	int cut = 0;
	//Interrupt 4
	if (DVIH[INT4_SIVBEngineOutB] == false && lvda.GetSIVBEngineOutB()) cut++;
	//DIN 5
	if (DPM[DIN5_SIVBEngineOutA] == false && lvda.GetSIVBEngineOutA()) cut++;
	//Velocity cutoff by LVDC
	if (S4B_IGN)
	{
		if (ModeCode25[MC25_FirstSIVBCutoffCommand])
		{
			cut++;
		}
	}
	else if (S4B_REIGN)
	{
		if (ModeCode26[MC26_SecondSIVBCutoffCommand])
		{
			cut++;
		}
	}
	//Acceleration less than 1 m/s in last boost major loop
	if (V < VOLD + 1.0)
	{
		cut++;
	}

	if (cut >= 2)
	{
		if (LVDC_Timebase == 4 || LVDC_Timebase == 40)
		{
			//Start TB5
			StartTimebase5();
		}
		else if (LVDC_Timebase == 6)
		{
			//Start TB7
			StartTimebase7();
		}
	}
}

void LVDCSV::SCInitiationOfSIISIVBSeparationInterrupt()
{
	directstagereset = true;

	if (LVDC_Timebase == 3)
	{
		StartTimebase4A();
	}
	else
	{
		if (ModeCode26[MC26_SCInitOfSIVBCutoff] == false)
		{
			DVASW = DVASW | MSKSSS4C1;
			SwitchSelectorProcessor(0);
			ModeCode26[MC26_SCInitOfSIVBCutoff] = true;
		}
	}
}

void LVDCSV::StartTimebase4A()
{
	lvda.SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
	S2_BURNOUT = true;
	MRS = false;
	LVDC_Timebase = 40;
	TimeBaseChangeRoutine();
	TB4a = TI;

	Tt_3 = Tt_3 + Cf * (V_S2T - V) + dT_cost;
	T_1 = 0;
	T_2 = 0;
	T_c = 0;
	T_1c = 0;
	Tt_T = Tt_3;
	ROV = ROVs;
	fprintf(lvlog, "[%d+%f] Direct stage interrupt received! Guidance update executed!\r\n", LVDC_Timebase, LVDC_TB_ETime);

	//Inhibit all interrupts and DINs that can start TB4(a)
	DVIH[INT6_SIIEnginesCutoff] = true;
	DPM[DIN19_SIIEnginesOut] = true;
	DVIH[INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA] = true;
	DPM[DIN22_SCInitSIISIVBSepB_SIVBEngineCutoffB] = true;

	//Also inhibit the engine failure inputs
	DPM[DIN13_SIIInboardEngineOut] = true;
	DPM[DIN21_SIIOutboardEngineOut] = true;

	//And set the corresponding mode code bits
	ModeCode25[MC25_SIIInboardEngineOut] = true;
	ModeCode25[MC25_SIIOutboardEngineOut] = true;
	ModeCode26[MC26_SCInitOfSIISIVBSeparation] = true;

	//Stop F/M Calculations
	NISTAT[3] = false;
}

void LVDCSV::StartTimebase5()
{
	LVDC_Timebase = 5;
	//Dequeue Time Base 5/7 module
	NISTAT[17] = false;
	if (ModeCode25[MC25_BeginTB5])
	{
		//Return to TB5 from TB6c
		DT_N = DT_N6;
		MS25DT = 10.0*DT_N;
		MS04DT = 0.1 / DT_N;
		MSLIM1 = 0.06*RAD;
		MSLIM2 = 0.04*RAD;
		TB6 = 100000;
		TI = TMR = TB5;
		SystemTimeUpdateRoutine();
		//Set phase to 1, so that phase activator goes to phase 2
		DVP = 1;
		PhaseActivator(false);
		fprintf(lvlog, "Returning to TB5 at TMM %.2lf TB %.2lf\r\n", TMM, LVDC_TB_ETime);
	}
	else
	{
		//Start TB5 from TB4 or TB4a
		TimeBaseChangeRoutine();
		TB5 = TI;
		ModeCode25[MC25_BeginTB5] = true;

		//Stop F/M Calculations
		NISTAT[3] = false;
		//Stop IGM and S-IVB Cutoff Prediction
		NISTAT[10] = false;
		NISTAT[11] = false;
		//Start Orbit Guidance
		NISTAT[12] = true;
		//Start TLI calculations
		NISTAT[14] = true;
	}
}

void LVDCSV::StartTimebase7()
{
	LVDC_Timebase = 7;
	//Dequeue Time Base 5/7 module
	NISTAT[17] = false;
	//Stop F/M Calculations
	NISTAT[3] = false;
	//Stop IGM and S-IVB Cutoff Prediction
	NISTAT[10] = false;
	NISTAT[11] = false;
	//Start Orbit Guidance
	NISTAT[12] = true;
	//Start TLI calculations
	NISTAT[14] = true;
	TimeBaseChangeRoutine();
	TB7 = TI;

	//HSL Exit settings
	GATE = false;
	GATE5 = false;
	Tt_T = 1000;
	HSL = false;
	S4B_REIGN = false;

	//Timing settings
	DT_N = DT_N5;
	MS25DT = 25.0*DT_N;
	MS04DT = 0.04 / DT_N;
	MSLIM1 = 0.024*RAD;
	MSLIM2 = 0.016*RAD;
	ModeCode26[MC26_TB7_Started] = true;

	lvda.TLIEnded();
}

void LVDCSV::PhaseIIandIVControl(int entry)
{
	switch (entry)
	{
	case 1: //Every 8 seconds in phases II and IV
		OrbitNavigation();
		AcquisitionGainLoss();
		DLTTL[6] = TAS + 8.0;
		break;
	case 2: //Once per second in phases II and IV
		DiscreteProcessor();
		NavigationExtrapolation();
		OrbitGuidance();
		MinorLoopSupport();
		DLTTL[7] = TMM + 1.0;
		break;
	case 3: //Once per second in phase II
		TimeToGoToRestartAndBetaTest();
		DLTTL[8] = TMM + 1.0;
		break;
	}
}

void LVDCSV::OrbitNavigation()
{
	fprintf(lvlog, "[%d+%f] ORBITAL NAVIGATION\r\n", LVDC_Timebase, LVDC_TB_ETime);
	RTEMP1 = PosS + DotS * 4.0 + ddotS * 8.0;
	VTEMP1 = DotS + ddotS * 4.0;
	ATEMP1 =  OrbitalNavigationAcceleration(RTEMP1, VTEMP1, CurrentAttitude);
	RPT = PosS + DotS * 8.0 + ATEMP1 * 32.0;
	VPT = VTEMP1 + ATEMP1 * 8.0;
	MidPointAttitude = CurrentAttitude + DChi_apo * MLR*4.0;
	APT = OrbitalNavigationAcceleration(RPT, VPT, MidPointAttitude);
	DRT = DotS * 8.0 + (ddotS + ATEMP1 * 2.0)*32.0 / 3.0;
	PosS = PosS + DRT;
	DVT = (ddotS + ATEMP1 * 4.0 + APT)*4.0 / 3.0;
	DotS = DotS + DVT;
	ddotS = OrbitalNavigationAcceleration(PosS, DotS, CurrentAttitude);
	TAS = TAS + 8.0;
	R = length(PosS);
	V = length(DotS);
	CG = length(ddotS);

	fprintf(lvlog, "Inertial Attitude: %f %f %f \r\n", CurrentAttitude.x*DEG, CurrentAttitude.y*DEG, CurrentAttitude.z*DEG);
	fprintf(lvlog, "EarthRel Position: %f %f %f \r\n", PosS.x, PosS.y, PosS.z);
	fprintf(lvlog, "EarthRel Velocity: %f %f %f \r\n", DotS.x, DotS.y, DotS.z);
	fprintf(lvlog, "EarthRel Acceleration: %f %f %f \r\n", ddotS.x, ddotS.y, ddotS.z);
	fprintf(lvlog, "State vector time: %f \r\n", TAS);
	fprintf(lvlog, "Gravity Acceleration: %f \r\n", CG);
	fprintf(lvlog, "Total Velocity: %f \r\n", V);
	fprintf(lvlog, "Dist. from Earth's Center: %f \r\n", R);
	//fprintf(lvlog, "S: %f \r\n", S);
	//fprintf(lvlog, "P: %f \r\n", P);
	VECTOR3 drtest = SVCompare();
	fprintf(lvlog, "SV Accuracy: %f %f %f\r\n", drtest.x, drtest.y, drtest.z);
}

void LVDCSV::AcquisitionGainLoss()
{
	theta_R = omega_E * TAS;
	MGA = _M(cos(theta_R), 0, sin(theta_R), 0, 1, 0, -sin(theta_R), 0, cos(theta_R));
	MSA = mul(MX_A, MGA);
	PosA = mul(MSA, PosS);

	bool acquisition = false;
	for (int i = 0;i < 11;i++)
	{
		d_A = dotp(PosA, C_A[i]) - R_STA;
		if (d_A >= 0)
		{
			acquisition = true;
		}
	}
	
	if (acquisition)
	{
		if (TBA <= TBL)
		{
			//AOS
			DVASW = DVASW | MSKSSACQU;
			SwitchSelectorProcessor(0);
			TBA = TMM;
			fprintf(lvlog, "[%d+%f] Acquisition of Signal\r\n", LVDC_Timebase, LVDC_TB_ETime);
		}
	}
	else
	{
		if (TBL <= TBA)
		{
			//LOS
			TBL = TMM;
			fprintf(lvlog, "[%d+%f] Loss of Signal\r\n", LVDC_Timebase, LVDC_TB_ETime);
		}
	}
}

void LVDCSV::NavigationExtrapolation()
{
	T_SON = TMM - TAS;
	R_OG = PosS + DotS * T_SON;
}

void LVDCSV::SteeringMisalignmentCorrection()
{
	//Always enable the flag again here
	if (DFSMC == false)
	{
		fprintf(lvlog, "Steering Misalignment Correction inhibited\r\n");
		DFSMC = true;
		return;
	}
	SMCY = SMCY + SMCG * ((ddotM_act.z + ddotM_act.x*tan(DVCA[0] - SMCY)) / (ddotM_act.x - ddotM_act.z*tan(DVCA[0] - SMCY)))*dt_c;
	SMCZ = SMCZ + SMCG * ((sin(DVCA[1] - SMCZ) - ddotM_act.y *MF[0]) / (cos(DVCA[1] - SMCZ)))*dt_c;
	DFSMC = true;
	//sprintf(oapiDebugString(), "SMC %lf %lf XM %lf ZM %lf TERM %lf", SMCY*DEG, SMCZ*DEG, ddotM_act.x, ddotM_act.z, tan(DVCA[0] - SMCY));
	fprintf(lvlog, "Steering Misalignment Correction: PITCH = %f, YAW = %f\r\n", SMCY*DEG, SMCZ*DEG);
}

void LVDCSV::ChiComputations(int entry)
{
	//entry:
	//1 = from IGM and orbital guidance
	//2 = from boost major loop
	VECTOR3 VT;
	VT = tmul(MX_G, _V(cos_chi_Yit*cos_chi_Zit, sin_chi_Zit, -sin_chi_Yit * cos_chi_Zit));
	//fprintf(lvlog, "VT (mul) = %f %f %f\r\n", VT.x, VT.y, VT.z);

	X_S1 = VT.x;
	X_S2 = VT.y;
	X_S3 = VT.z;
	//fprintf(lvlog, "X_S1-3 = %f %f %f\r\n", X_S1, X_S2, X_S3);

	// FINALLY - COMMANDS!
	X_Zi = asin(X_S2) + SMCZ;			// Yaw
	if (X_Zi < 0)
	{
		X_Zi += PI2;
	}
	X_Yi = atan2(-X_S3, X_S1) + SMCY;	// Pitch
	if (X_Yi < 0)
	{
		X_Yi += PI2;
	}

	//fprintf(lvlog, "*** COMMAND ISSUED ***\r\n");
	//fprintf(lvlog, "PITCH = %f, YAW = %f\r\n", X_Yi*DEG, X_Zi*DEG);
	// IGM is supposed to generate attitude directly.
	CommandedAttitude.y = X_Yi; // PITCH
	CommandedAttitude.z = X_Zi; // YAW;	
	//Limit yaw to +/-45�
	if (CommandedAttitude.z < -45 * RAD && CommandedAttitude.z >= -180 * RAD)
	{
		CommandedAttitude.z = -45 * RAD;
		DFSMC = false;
	}
	if (CommandedAttitude.z > 45 * RAD && CommandedAttitude.z <= 180 * RAD)
	{
		CommandedAttitude.z = 45 * RAD;
		DFSMC = false;
	}
}

void LVDCSV::OrbitGuidance()
{
	//fprintf(lvlog, "ORBITAL GUIDANCE\r\n");

	//R_OG only gets calculated in phases II and IV
	if (DVP == 1 || DVP == 3)
	{
		R_OG = PosS;
	}

	if (LVDC_Timebase == 5)
	{
		//Reschedule TD&E in Earth orbit
		if (TimeCheck(TI5F3) && ModeCode27[MC27_InhibitManeuver3] && OGStatusWord[2] == false)
		{
			double dttemp = 85.0*3600.0;
			TI5F3 += dttemp;
			TI5F4 += dttemp;
			TI5F5 += dttemp;
		}

		//Retro attitude
		if (TimeCheck(TI5F5) && ModeCode27[MC27_InhibitManeuver5] == false && OGStatusWord[4] == false)
		{
			AttitudeManeuverState = -1;
			alpha_1 = PI;
			alpha_2 = 0.0;
			CommandedAttitude.x = PI;
			OGStatusWord[4] = true;
		}
		//Maneuver back to local horizontal
		else if (TimeCheck(TI5F4) && ModeCode27[MC27_InhibitManeuver4] == false && OGStatusWord[3] == false)
		{
			AttitudeManeuverState = -1;
			alpha_1 = 0.0;
			alpha_2 = 0.0;
			CommandedAttitude.x = 0.0;
			OGStatusWord[3] = true;
		}
		//Maneuver to Earth orbit TD&E Attitude
		else if (TimeCheck(TI5F3) && ModeCode27[MC27_InhibitManeuver3] == false && OGStatusWord[2] == false)
		{
			AttitudeManeuverState = -2;
			CommandedAttitude.x = XLunarAttitude.x;
			alpha_1 = XLunarAttitude.y;
			alpha_2 = XLunarAttitude.z;
			OGStatusWord[2] = true;
		}
	}
	else if (LVDC_Timebase == 7)
	{
		//Maneuver to slingshot/comm attitude
		if (LVDC_TB_ETime > T7M11 && ModeCode27[MC27_InhibitManeuver11] == false && OGStatusWord[10] == false && EPTINDX == 99)
		{
			EventsProcessor(2);
			OGStatusWord[10] = true;
		}
		//Maneuver to separation attitude
		else if (LVDC_TB_ETime > T7M10 && ModeCode27[MC27_InhibitManeuver10] == false && OGStatusWord[9] == false && EPTINDX == 96)
		{
			EventsProcessor(2);
			OGStatusWord[9] = true;
		}
	}

	if (AttitudeManeuverState != 2)
	{
		//-2 = inertial hold of local reference, -1 = local reference, 0 = chi freeze, 1 = inertial reference
		switch (AttitudeManeuverState)
		{
		case -2:
			fprintf(lvlog, "Initiate inertial hold of local reference\r\n");
			ModeCode27[MC27_InertialHoldInProgress] = true;
			ModeCode27[MC27_TrackLocalHoriz] = false;
			ModeCode27[MC27_AttHoldWrtLocalContRetFromSC] = false;
			ModeCode27[MC27_AttHoldWrtInertialContRetFromSC] = false;

			//Calculate this once
			Pos4 = mul(MX_G, R_OG);
			R4 = sqrt(Pos4.x*Pos4.x + Pos4.z*Pos4.z);
			sin_chi_Yit = (Pos4.x * cos(alpha_1) + Pos4.z * sin(alpha_1)) / (-R4);
			cos_chi_Yit = (Pos4.z * cos(alpha_1) - Pos4.x * sin(alpha_1)) / (-R4);
			sin_chi_Zit = sin(alpha_2);
			cos_chi_Zit = cos(alpha_2);
			ChiComputations(1);
			break;
		case -1:
			fprintf(lvlog, "Initiate tracking of local reference\r\n");
			ModeCode27[MC27_InertialHoldInProgress] = false;
			ModeCode27[MC27_TrackLocalHoriz] = true;
			ModeCode27[MC27_AttHoldWrtLocalContRetFromSC] = false;
			ModeCode27[MC27_AttHoldWrtInertialContRetFromSC] = false;
			break;
		case 0:
			fprintf(lvlog, "Initiate chi freeze\r\n");
			CommandedAttitude = CurrentAttitude;
			ModeCode27[MC27_InertialHoldInProgress] = true;
			ModeCode27[MC27_TrackLocalHoriz] = false;
			ModeCode27[MC27_AttHoldWrtLocalContRetFromSC] = false;
			ModeCode27[MC27_AttHoldWrtInertialContRetFromSC] = false;
			break;
		case 1:
			fprintf(lvlog, "Initiate inertial hold\r\n");
			ModeCode27[MC27_InertialHoldInProgress] = true;
			ModeCode27[MC27_TrackLocalHoriz] = false;
			ModeCode27[MC27_AttHoldWrtLocalContRetFromSC] = false;
			ModeCode27[MC27_AttHoldWrtInertialContRetFromSC] = false;
			break;
		}

		AttitudeManeuverState = 2;
	}

	if (ModeCode27[MC27_SCInControl] == false)
	{
		if (ModeCode27[MC27_TrackLocalHoriz] || ModeCode27[MC27_AttHoldWrtLocalContRetFromSC])
		{
			//fprintf(lvlog, "Maintain orbrate\r\n");
			Pos4 = mul(MX_G, R_OG); //here we compute the steering angles...
			R4 = sqrt(Pos4.x*Pos4.x + Pos4.z*Pos4.z);
			sin_chi_Yit = (Pos4.x * cos(alpha_1) + Pos4.z * sin(alpha_1)) / (-R4);
			cos_chi_Yit = (Pos4.z * cos(alpha_1) - Pos4.x * sin(alpha_1)) / (-R4);
			sin_chi_Zit = sin(alpha_2);
			cos_chi_Zit = cos(alpha_2);
			ChiComputations(1);
			return;
		}
	}
	//fprintf(lvlog, "Inertial attitude hold or SC takeover\r\n");
}

void LVDCSV::TimeToGoToRestartAndBetaTest()
{
	//fprintf(lvlog, "TIME-TO-GO TO RESTART AND BETA TEST\r\n");
	// TLI restart & targeting logic;
restartprep:
	//Manual TB6 Initiation
	if (DPM[DIN20_SIVBIgnitionSequenceStart] == false && lvda.GetCMCSIVBIgnitionSequenceStart() && ModeCode26[MC26_Guidance_Reference_Failure])
	{
		fprintf(lvlog, "CMC has commanded S-IVB Ignition Sequence Start! \r\n");
		goto INHcheck;
	}

	//Determine if XLUNAR-INHIBIT
	if (GATE0)	//Restart prep enabled?
	{
		goto INHcheck;
	}
	if (!GATE1)	//OOB targeting enabled?
	{
		if (GATE2)	//First opportunity targeting passed?
		{
			//Second opportunity targeting from TABLE

			tgt_index = 0;
			while (t_D > TABLE15[1].target[tgt_index].t_D)
			{
				tgt_index++;
			}
			if (tgt_index < 1)
			{
				tgt_index = 1;
			}
			fprintf(lvlog, "Target index = %d \r\n", tgt_index);

			double tdint0, tdint1;

			tdint0 = TABLE15[1].target[tgt_index - 1].t_D;
			tdint1 = TABLE15[1].target[tgt_index].t_D;

			RAS = LinInter(tdint0, tdint1, TABLE15[1].target[tgt_index - 1].RAS*RAD, TABLE15[1].target[tgt_index].RAS*RAD, t_D);
			DEC = LinInter(tdint0, tdint1, TABLE15[1].target[tgt_index - 1].DEC*RAD, TABLE15[1].target[tgt_index].DEC*RAD, t_D);
			C_3 = LinInter(tdint0, tdint1, TABLE15[1].target[tgt_index - 1].C_3, TABLE15[1].target[tgt_index].C_3, t_D);
			cos_sigma = LinInter(tdint0, tdint1, TABLE15[1].target[tgt_index - 1].cos_sigma, TABLE15[1].target[tgt_index].cos_sigma, t_D);
			e_N = LinInter(tdint0, tdint1, TABLE15[1].target[tgt_index - 1].e_N, TABLE15[1].target[tgt_index].e_N, t_D);

			fprintf(lvlog, "Selected TLI Targeting Parameters (Second Opportunity): \r\n");
			fprintf(lvlog, "RAS: %f, DEC: %f, C_3 = %f, cos_sigma = %f, e_N = %f \r\n", RAS*DEG, DEC*DEG, C_3, cos_sigma, e_N);

			f = TABLE15[1].f*RAD;
			beta = TABLE15[1].beta*RAD;
			alpha_TS = TABLE15[1].alphaS_TS*RAD;
			T_ST = TABLE15[1].T_ST;
			R_N = TABLE15[1].R_N;
			tau3R = TABLE15[1].TAU3R;
			Tt_3R = TABLE15[1].T3PR;
			T_2R = TABLE15[1].T2IR;
			dV_BR = TABLE15[1].dV_BR;
			TargetVector = _V(cos(RAS)*cos(DEC), sin(RAS)*cos(DEC), sin(DEC));
			GATE1 = true;
		}
		else
		{
			//First opportunity targeting from TABLE

			tgt_index = 0;
			while (t_D > TABLE15[0].target[tgt_index].t_D)
			{
				tgt_index++;
			}
			if (tgt_index < 1)
			{
				tgt_index = 1;
			}
			fprintf(lvlog, "Target index = %d \r\n", tgt_index);

			double tdint0, tdint1;

			tdint0 = TABLE15[0].target[tgt_index - 1].t_D;
			tdint1 = TABLE15[0].target[tgt_index].t_D;

			RAS = LinInter(tdint0, tdint1, TABLE15[0].target[tgt_index - 1].RAS*RAD, TABLE15[0].target[tgt_index].RAS*RAD, t_D);
			DEC = LinInter(tdint0, tdint1, TABLE15[0].target[tgt_index - 1].DEC*RAD, TABLE15[0].target[tgt_index].DEC*RAD, t_D);
			C_3 = LinInter(tdint0, tdint1, TABLE15[0].target[tgt_index - 1].C_3, TABLE15[0].target[tgt_index].C_3, t_D);
			cos_sigma = LinInter(tdint0, tdint1, TABLE15[0].target[tgt_index - 1].cos_sigma, TABLE15[0].target[tgt_index].cos_sigma, t_D);
			e_N = LinInter(tdint0, tdint1, TABLE15[0].target[tgt_index - 1].e_N, TABLE15[0].target[tgt_index].e_N, t_D);

			fprintf(lvlog, "Selected TLI Targeting Parameters (First Opportunity): \r\n");
			fprintf(lvlog, "RAS: %f, DEC: %f, C_3 = %f, cos_sigma = %f, e_N = %f \r\n", RAS*DEG, DEC*DEG, C_3, cos_sigma, e_N);

			f = TABLE15[0].f*RAD;
			beta = TABLE15[0].beta*RAD;
			alpha_TS = TABLE15[0].alphaS_TS*RAD;
			T_ST = TABLE15[0].T_ST;
			R_N = TABLE15[0].R_N;
			tau3R = TABLE15[0].TAU3R;
			T_2R = TABLE15[0].T2IR;
			Tt_3R = TABLE15[0].T3PR;
			dV_BR = TABLE15[0].dV_BR;
			TargetVector = _V(cos(RAS)*cos(DEC), sin(RAS)*cos(DEC), sin(DEC));
			GATE1 = GATE2 = true;
		}
	}

	//Did ground (MCC/RTCC) provide a target update?
	if (TU)
	{
		//Check for 7- or 10-parameter update
		if (TU10)
		{
			/*
			Target uploading parameters go here; depends on RTCC socketing

			10-parameter update relies on the LVDC to determine the restart time, through the so-called S*T_p test. This starts TB6 when the dot product of the target vector and the burn node
			(pseudonodal) vector is less than a specified magnitude, effectively determining that the cross-plane error in vectors is sufficiently small.

			Needs 10 parameters:
			T_X,Y,Z		Target vector in ephemeral coordinates
			alpha_TS	Desired angle between unit target vector and unit nodal vector
			beta		Angle between pseudonodal vector and radius vector, inplane at restart time
			theta_N		Angle of descending node of transfer ellipse, referenced from launch point
			C_3			Vis-viva energy of transfer ellipse
			f			True anomaly of transfer ellipse
			cos_sigma	Cosine of angle between perigee vector and target vector
			T_ST		Time constant for S*T_P test (determines reignition time and validity of transfer solution)
			*/
		}
		else
		{
			/*
			Target uploading parameters go here; depends on RTCC socketing

			7-parameter update provides a pre-calculated ellipse and TB6 time, using the preloaded T_RG to select the ignition time. This bypasses the S*T_P test, and goes straight into IGM pre-calcs
			without further ado, aside from the INHIBIT checks. alpha_D_op is also set to 0 to bypass onboard calculation of alpha_D.

			Needs 7 parameters:
			T_RP		Time to initate restart preparations (TB6)
			C_3			Vis-viva energy of transfer ellipse
			Inclination	Desired inclination of transfer ellipse, equatorial ref.
			e_N			Eccentricity of the nominal transfer ellipse
			alpha_D		Angle between perigee and descending nodal vector of transfer ellipse
			f			True anomaly of transfer ellipse
			*/

			alpha_D_op = 0;
			first_op = false;

			if (TMM - TB5 - T_RP < 0) //Sufficient time after TB6?
			{
				return;
			}
			goto INHcheck;
		}
	}

	if (ModeCode27[MC27_TDE_Enable]) //TLI permanently disabled?
	{
		return;
	}

	if (LVDC_TB_ETime - T_ST < 0) //Sufficient time before S*T_P test?
	{
		fprintf(lvlog, "Time until first TB6 check = %f \r\n", LVDC_TB_ETime - T_ST);
		return;
	}

	//Determination of S-bar and S-bar-dot
	theta_E = theta_EO + TVRATE * t_D;
	MEG = _M(cos(theta_E), sin(theta_E), 0.0, 0.0, 0.0, -1.0, -sin(theta_E), cos(theta_E), 0.0);
	MX_EPH = mul(tmat(MX_A), MEG);

	T_P = mul(MX_EPH, unit(TargetVector));
	N = unit(crossp(R_OG, DotS));
	PosP = crossp(N, unit(R_OG));
	Sbar = unit(R_OG)*cos(beta) + PosP * sin(beta);
	DotP = crossp(N, DotS / length(R_OG));

	Sbardot = DotS / length(PosS)*cos(beta) + DotP * sin(beta);

	if (dotp(Sbardot, T_P) < 0 && dotp(Sbar, T_P) <= cos(alpha_TS))
	{
		goto INHcheck;
	}
	else
	{
		return;
	}

INHcheck:
	if (lvda.SIVBInjectionDelay() && LVDC_Timebase != 6)	//XLUNAR switch to INHIBIT in the CSM?
	{
		if (first_op == false)
		{
			//Permanent TLI inhibt
			GATE0 = false;
			ModeCode27[MC27_TDE_Enable] = true;
			return;
		}
		else
		{
			//Select second opportunity targeting
			GATE0 = GATE1 = false;
			first_op = false;
			return;
		}
	}
	else if (!GATE0)
	{
		GATE0 = true;	//Bypass targeting routines
		LVDC_Timebase = 6;
		TimeBaseChangeRoutine();
		TB6 = TI;		
		
		goto restartprep;
	}
}

void LVDCSV::RestartCalculations()
{
	fprintf(lvlog, "RESTART CALCULATIONS\r\n");
	//Calculates IGM parameters for out-of-orbit burn
	if (TU)
	{
		if (!TU10)
		{
			p = (mu / C_3)*(pow(e, 2) - 1);
			goto O3GMatrix;
		}
	}

	//Nominal ellipse calculations go here
	cos_psiT = dotp(Sbar, T_P);
	sin_psiT = sqrt(1.0 - pow(cos_psiT, 2));
	Sbar_1 = (Sbar*cos_psiT - T_P)*(1.0 / sin_psiT);
	Cbar_1 = crossp(Sbar_1, Sbar);
	Inclination = acos(dotp(_V(MX_A.m21, MX_A.m22, MX_A.m23), Cbar_1));
	X_1 = dotp(_V(MX_A.m31, MX_A.m32, MX_A.m33), crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23)));
	X_2 = dotp(_V(MX_A.m11, MX_A.m12, MX_A.m13), crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23)));
	theta_N = atan2(X_1, X_2);
	p_N = mu / C_3 * (pow(e_N, 2) - 1.0);
	T_M = p_N / (1.0 - e_N * cos_sigma);
	R = length(PosS);
	e = R / R_N * (e_N - 1) + 1.0;
	p = mu / C_3 * (pow(e, 2) - 1.0);

	if (alpha_D_op)
	{
		//alpha_D = acos(dotp(Sbar, T_P)) - acos((1.0 - p / T_M) / e) + atan2(X_1, X_2);
		alpha_D = acos(dotp(Sbar, T_P)) - acos((1.0 - p / T_M) / e) + atan2(dotp(Sbar_1, crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23))), dotp(Sbar, crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23))));
	}
	else
	{
		alpha_D = TABLE15[1].target[tgt_index].alpha_D;
	}

	fprintf(lvlog, "Elliptic parameters: Inc: %f�, e: %f, p: %f, theta_N: %f�, alpha_D: %f�, f: %f�\r\n", Inclination*DEG, e, p, theta_N*DEG, alpha_D*DEG, f*DEG);

O3GMatrix:
	MX_B = _M(cos(theta_N), 0, sin(theta_N), sin(theta_N)*sin(Inclination), cos(Inclination), -cos(theta_N)*sin(Inclination),
		-sin(theta_N)*cos(Inclination), sin(Inclination), cos(theta_N)*cos(Inclination));
	MX_G = mul(MX_B, MX_A);
	R_T = p / (1.0 + e * cos(f));
	K_5 = sqrt(mu / p);
	V_T = K_5 * sqrt(1.0 + 2.0 * e*cos(f) + pow(e, 2));
	gamma_T = atan((e*sin(f)) / (1.0 + e * cos(f)));
	G_T = -mu / pow(R_T, 2);

	fprintf(lvlog, "TLI Targets: R_T: %f, V_T: %f, gamma_T: %f, G_T: %f\r\n", R_T, V_T, gamma_T, G_T);

	//Update IGM parameters
	Ct = 0.0;
	dotM_2 = dotM_2R;
	dotM_3 = dotM_3R;
	P_c = 0.0;
	ROT = ROTR;
	ROV = ROVR;
	T_2 = T_2R;
	T_1c = T_2;
	Tt_3 = Tt_3R - K_T3 * dTt_4;
	Tt_T = T_2 + Tt_3;
	t_B4 = 0.0;
	V_ex2 = V_ex2R;
	V_ex3 = V_ex3R;
	V_TC = 150;
	eps_1 = eps_1R;
	eps_2 = eps_2R;
	eps_3 = eps_3R;
	eps_4 = eps_4R;
	tau3 = tau3R - dTt_4;

	fprintf(lvlog, "Tt_3 = %f, dTt_4 = %f\r\n", Tt_3, dTt_4);

	//Queues itself out
	NISTAT[6] = false;
}

VECTOR3 LVDCSV::SVCompare()
{
	VECTOR3 pos, vel, newpos, newvel;
	MATRIX3 mat;
	double MJD_L, MJD_cur, MJD_LD, xtemp, GMT_GRR, MJD_GRR, DT_GRR_act, DT_TAS;
	//Actual current MJD
	MJD_cur = oapiGetSimMJD();
	//GMT of GRR (doesn't have to be accurate)
	GMT_GRR = RealTimeClock - T_L;
	//MJD of GRR (doesn't have to be accurate)
	MJD_GRR = MJD_cur - GMT_GRR / 3600.0 / 24.0;
	//MJD_LD, MJD at midnight of liftoff day
	xtemp = modf(MJD_GRR, &MJD_LD);
	//Precise MJD of GRR
	MJD_L = MJD_LD + T_L / 3600.0 / 24.0;
	//Actual time since GRR
	DT_GRR_act = (MJD_cur - MJD_L)*24.0*3600.0;
	//Difference to state vector time
	DT_TAS = DT_GRR_act - TAS;
	mat = OrbMech::Orbiter2PACSS13(MJD_L, PHI, KSCLNG, Azimuth);
	lvda.GetRelativePos(pos);
	lvda.GetRelativeVel(vel);
	newpos = mul(mat, pos);
	newvel = mul(mat, vel);

	VECTOR3 i, j, k;
	j = unit(crossp(newvel, newpos));
	k = unit(-newpos);
	i = crossp(j, k);
	MATRIX3 Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z); //rotation matrix to LVLH

	return mul(Q_Xx, PosS + DotS * DT_TAS - newpos);
}

double LVDCSV::LinInter(double x0, double x1, double y0, double y1, double x)
{
	return y0 + (y1 - y0)*(x - x0) / (x1 - x0);
}

bool LVDCSV::TimebaseUpdate(double dt)
{
	if (LVDC_Timebase == 5 || LVDC_Timebase == 7 || LVDC_Timebase == 8)
	{
		VGBIA = dt;
		DFTUP = true;
		return true;
	}

	return false;
}

bool LVDCSV::GeneralizedSwitchSelector(int stage, int channel)
{
	if (LVDC_Timebase == 5 || (LVDC_Timebase == 6 && LVDC_TB_ETime < 570.0) || LVDC_Timebase == 7 || LVDC_Timebase == 8)
	{
		if (stage >= 0 && stage < 4)
		{
			if (channel > 0 && channel < 113)
			{
				SSTABLE[KSSINDXGSS].time = 0.0;
				SSTABLE[KSSINDXGSS].stage = stage;
				SSTABLE[KSSINDXGSS].channel = channel;
				DVASW = DVASW | MSKSSGNSS;
				SwitchSelectorProcessor(0);
				return true;
			}
		}
	}

	return false;
}

bool LVDCSV::RestartManeuverEnable()
{
	if (LVDC_Timebase == 5)
	{
		ModeCode27[MC27_TDE_Enable] = false;

		return true;
	}

	return false;
}

bool LVDCSV::TDEEnable()
{
	if (LVDC_Timebase == 5 && LVDC_TB_ETime > 100.0)
	{
		ModeCode27[MC27_TDE_Enable] = true;
		ModeCode27[MC27_InhibitManeuver3] = false;
		ModeCode27[MC27_InhibitManeuver4] = false;

		return true;
	}

	return false;
}

bool LVDCSV::RemoveInhibitManeuver4()
{
	if (LVDC_Timebase == 5)
	{
		ModeCode27[MC27_InhibitManeuver4] = false;

		return true;
	}

	return false;
}

bool LVDCSV::InhibitSeparationManeuver()
{
	if (LVDC_Timebase == 5 || LVDC_Timebase == 7)
	{
		ModeCode27[MC27_InhibitManeuver10] = true;

		return true;
	}

	return false;
}

bool LVDCSV::UpdateSeparationManeuver(double dt)
{
	if (LVDC_Timebase == 5 || LVDC_Timebase == 7)
	{
		ModeCode27[MC27_InhibitManeuver10] = false;
		T7M10 += dt * 60.0;

		return true;
	}

	return false;
}

bool LVDCSV::EvasiveManeuverEnable()
{
	if (LVDC_Timebase == 7 && LVDC_TB_ETime > 3600.0)
	{
		if (ModeCode27[MC27_InhibitManeuver13] == false)
		{
			CommandedAttitude.z = PI2 - CommandedAttitude.z;
			ModeCode27[MC27_InhibitManeuver13] = true;
			return true;
		}
	}
	return false;
}

bool LVDCSV::TimeBase8Enable()
{
	if (LVDC_Timebase == 7)
	{
		if (SSTTBPTR[7] != 0)
		{
			ModeCode27[MC27_TB8EnableDCSCmdEnable] = true;

			return true;
		}
	}
	return false;
}

bool LVDCSV::SIVBIULunarImpact(double tig, double dt, double pitch, double yaw)
{
	if (LVDC_Timebase == 8)
	{
		if (tig > LVDC_TB_ETime + 10.0)
		{
			if (!ImpactBurnInProgress)
			{
				AttitudeManeuverState = -1;
				alpha_1 = pitch;
				alpha_2 = yaw;
				T_ImpactBurn = tig;
				dT_ImpactBurn = dt;
				ImpactBurnEnabled = true;

				return true;
			}
		}
	}

	return false;
}

bool LVDCSV::ExecuteCommManeuver()
{
	if (LVDC_Timebase == 8)
	{
		if (!ImpactBurnInProgress)
		{
			AttitudeManeuverState = -1;
			alpha_1 = XLunarCommAttitude.y;
			alpha_2 = XLunarCommAttitude.z;
			CommandedAttitude.x = XLunarCommAttitude.x;
			ImpactBurnEnabled = false;

			return true;
		}
	}

	return false;
}

bool LVDCSV::DiscreteOutputTest(int bit, bool on)
{
	if (bit >= 0 && bit <= 12)
	{
		lvda.SetOutputRegisterBit(bit, on);
		return true;
	}
	return false;
}

bool LVDCSV::NavigationUpdate(VECTOR3 DCSRVEC, VECTOR3 DCSVVEC, double DCSNUPTIM)
{
	if ((LVDC_Timebase == 5 || LVDC_Timebase == 7) && poweredflight == false && DCSNUPTIM > TMM + 10.0)
	{
		NUPTIM = DCSNUPTIM;
		Pos_Nav = DCSRVEC;
		Vel_Nav = DCSVVEC;
		ModeCode27[MC27_DCSNavigationUpdateAccepted] = true;
		T2STAT[4] = true;
		DLTTL[4] = NUPTIM;
		Timer2Interrupt(true);
		fprintf(lvlog, "Navigation update received! R %f %f %f V %f %f %f T %f \r\n", DCSRVEC.x, DCSRVEC.y, DCSRVEC.z, DCSRVEC.x, DCSRVEC.y, DCSRVEC.z, DCSNUPTIM);
		return true;
	}
	return false;
}

bool LVDCSV::TLITargetingUpdate(double T_RP, double C_3, double Inclination, double theta_N, double e, double alpha_D, double f)
{
	if (LVDC_Timebase == 5 && T_RP > LVDC_TB_ETime + 10.0)
	{
		TU = true;
		TU10 = false;

		this->T_RP = T_RP;
		this->C_3 = C_3;
		this->Inclination = Inclination;
		this->theta_N = theta_N;
		this->e = e;
		this->alpha_D = alpha_D;
		this->f = f;

		fprintf(lvlog, "TLI targeting update received! T_RP: %f, C_3: %f, Inc: %f�, e: %f, alpha_D: %f�, f: %f�, theta_N: %f�\r\n", T_RP, C_3, Inclination*DEG, e, alpha_D*DEG, f*DEG, theta_N*DEG);

		return true;
	}
	return false;
}

VECTOR3 LVDCSV::GravitationSubroutine(VECTOR3 Rvec, bool J2only)
{
	VECTOR3 GTEMP;
	double P_34, S_34;
	Y_u = -(Rvec.x*MX_A.m21 + Rvec.y*MX_A.m22 + Rvec.z*MX_A.m23); //position component south of equator
	double rtemp = length(Rvec); //instantaneous distance from earth's center
	double AoverR = a / rtemp;
	double YoverR = Y_u / rtemp;
	if (J2only)
	{
		S_34 = 0.0;
		P_34 = 0.0;
	}
	else
	{
		S_34 = H * pow(AoverR, 3)*(YoverR)*(3.0 - 7.0*pow(YoverR, 2)) + D / 7.0*pow(AoverR, 4)*(3.0 - 42.0*pow(YoverR, 2) + 63.0*pow(YoverR, 4));
		P_34 = H / 5.0*(AoverR)*(15.0*pow(YoverR, 2) - 3.0) + D / 7.0*pow(AoverR, 2)*YoverR*(12.0 - 28.0*pow(YoverR, 2));
	}
	S = (-mu / pow(rtemp, 3))*(1.0 + J * pow(AoverR, 2)*(1.0 - 5.0 * pow(YoverR, 2)) + S_34);
	P = (-mu / pow(rtemp, 2))*pow(AoverR, 2) *(2.0*J*(YoverR)+P_34);
	GTEMP.x = Rvec.x*S - MX_A.m21*P; //gravity acceleration vector
	GTEMP.y = Rvec.y*S - MX_A.m22*P;
	GTEMP.z = Rvec.z*S - MX_A.m23*P;
	return GTEMP;
}

VECTOR3 LVDCSV::DragSubroutine(VECTOR3 Rvec, VECTOR3 Vvec, VECTOR3 Att)
{
	//fprintf(lvlog, "DRAG SUBROUTINE\r\n");
	//fprintf(lvlog, "Rvec = %f %f %f Vvec = %f %f %f\r\n", Rvec.x, Rvec.y, Rvec.z, Vvec.x, Vvec.y, Vvec.z);
	//Hack for old scenarios. Remove at some point
	if (length(W_ES) == 0.0)
	{
		W_ES = -_V(MX_A.m21, MX_A.m22, MX_A.m23)*omega_E;
	}
	h = length(Rvec) - a;
	if (h > h_2)
	{
		rho = 0.0;
	}
	else if (h < h_1)
	{
		rho = rho_c;
	}
	else
	{
		rho = Rho[0] + Rho[1] * h + Rho[2] * pow(h, 2) + Rho[3] * pow(h, 3) + Rho[4] * pow(h, 4) + Rho[5] * pow(h, 5);
	}
	//fprintf(lvlog, "h = %f rho = %e\r\n", h, rho);
	V_R = Vvec - crossp(W_ES, Rvec);
	v_R = length(V_R);
	//fprintf(lvlog, "V_R = %f %f %f v_R = %f\r\n", V_R.x, V_R.y, V_R.z, v_R);
	cos_alpha = 1.0 / v_R * (V_R.x*cos(Att.y)*cos(Att.z)+V_R.y*sin(Att.z)-V_R.z*sin(Att.y)*cos(Att.z));
	drag_area = Drag_Area[0] + Drag_Area[1] * cos_alpha + Drag_Area[2] * pow(cos_alpha, 2) + Drag_Area[3] * pow(cos_alpha, 3) + Drag_Area[4] * pow(cos_alpha, 4);
	//fprintf(lvlog, "cos_alpha = %f drag_area = %f\r\n", cos_alpha, drag_area);
	//TBD: Remove factor 0.05 when RTCC takes drag into account properly
	VECTOR3 A_DS = -V_R * rho * drag_area*K_D*v_R*0.05;
	//fprintf(lvlog, "A_DS = %e %e %e\r\n", A_DS.x, A_DS.y, A_DS.z);
	return A_DS;
}

VECTOR3 LVDCSV::VentSubroutine(VECTOR3 Att)
{
	if (LVDC_Timebase != 5)
	{
		return _V(0, 0, 0);
	}

	//fprintf(lvlog, "VENT SUBROUTINE\r\n");
	for (i = 0;i < 6;i++)
	{
		if (LVDC_TB_ETime < VTIM[i]) break;
	}
	VECTOR3 A_V = _V(cos(Att.y)*cos(Att.z), sin(Att.z), -sin(Att.y)*cos(Att.z))*VENTA[i];
	//fprintf(lvlog, "Segment = %d Vent Acceleration = %lf Vector = %f %f %f\r\n", i, VENTA[i], A_V.x, A_V.y, A_V.z);
	return A_V;
}

VECTOR3 LVDCSV::OrbitalNavigationAcceleration(VECTOR3 Rvec, VECTOR3 Vvec, VECTOR3 Att)
{
	return GravitationSubroutine(Rvec, false) + DragSubroutine(Rvec, Vvec, Att) + VentSubroutine(Att);
}

bool LVDCSV::TimeCheck(double t_event)
{
	//negative event time for time since GRR
	if (t_event < 0)
	{
		if (TMM >= abs(t_event))
		{
			return true;
		}
	}
	else
	{
		if (DVTRR >= t_event)
		{
			return true;
		}
	}

	return false;
}