/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Launch Vehicle Digital Computer (C++ Implementation Header)

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
#include "LVIMU.h"
class Saturn1b;

class LVDC1B {
public:
	LVDC1B();										// Constructor
	void init(Saturn1b* own);
	void timestep(double simt, double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
private:
	bool Initialized;								// Clobberness flag
	FILE* lvlog;									// LV Log file
	Saturn1b* owner;
	LVIMU lvimu;									// ST-124-M3 IMU (LV version)
	LVRG lvrg;										// LV rate gyro package

	bool LVDC_Stop;									// Program Stop Flag
	int LVDC_Timebase;								// Time Base
	double LVDC_TB_ETime;                           // Time elapsed since timebase start
	double S1B_Sep_Time;							// S1B Separation Counter
	int IGMCycle;									// IGM Cycle Counter (for debugging)

	// These are boolean flags that are NOT real flags in the LVDC SOFTWARE. (I.E. Hardware flags)
	bool LVDC_EI_On;								// Engine Indicator lights on
	bool LVDC_GRR;                                  // Guidance Reference Released
	bool CountPIPA;									// PIPA Counter Enable
	
	// These are variables that are not really part of the LVDC software.
	VECTOR3 AttRate;                                // Attitude Change Rate
	VECTOR3 AttitudeError;                          // Attitude Error
	VECTOR3 DeltaAtt;

	// Event Times
	double t_fail;									// S1C Engine Failure time
	double T_ar;									// S1C Tilt Arrest Time
	double t_1;										// Backup timer for Pre-IGM pitch maneuver
	double t_2;										// Time to initiate pitch freeze for S1C engine failure
	double t_3;										// Constant pitch freeze for S1C engine failure prior to t_2
	double t_3i;									// Clock time at S4B ignition
	double t_4;										// Upper bound of validity for first segment of pitch freeze
	double t_5;										// Upper bound of validity for second segment of pitch freeze
	double t_6;										// Time to terminate pitch freeze after S1C engine failure
	double dT_F;									// Period of frozen pitch in S1C
	double T_S1,T_S2,T_S3;							// Times for Pre-IGM pitch polynomial
	double T_LET;									// LET Jettison Time
	double dt_LET;									// Nominal interval between S2 ignition and LET jettison
	// IGM event times
	double sinceLastIGM;							// Time since last IGM run
	double IGMInterval;								// IGM Interval
	double T_1;										// Time left in first-stage IGM
	double T_2;										// Time left in second and fourth stage IGM

	// These are boolean flags that are real flags in the LVDC SOFTWARE.
	bool GRR_init;									// GRR initialization done
	bool poweredflight;								// Powered flight flag
	bool liftoff;									// lift-off flag
	bool S1B_Engine_Out;							// S1C Engine Failure Flag
	bool HSL;										// High-Speed Loop flag
	int  T_EO1,T_EO2;								// Pre-IGM Engine-Out Constant
	int  UP;										// IGM target parameters updated
	bool BOOST;										// Boost To Orbit
	bool S4B_IGN;									// SIVB Ignition
	bool MRS;										// MR Shift
	bool GATE;										// Logic gate for switching IGM steering
	bool GATE5;										// Logic gate that ensures only one pass through cutoff initialization
	bool INH,INH1,INH2;								// Dunno yet
	
	// LVDC software variables, PAD-LOADED BUT NOT NECESSARILY CONSTANT!
	double B_11,B_21;								// Coefficients for determining freeze time after S1C engine failure
	double B_12,B_22;								// Coefficients for determining freeze time after S1C engine failure	
	double V_ex1,V_ex2;								// IGM Exhaust Velocities
	double V_TC;									// Velocity parameter used in high-speed cutoff
	double tau1;									// Time to consume all fuel before S2 MRS
	double tau2;									// Time to consume all fuel between MRS and S2 Cutoff
	double Fm;										// Sensed acceleration
	double Tt_T;									// Time-To-Go computed using Tt_3
	double Tt_2;									// Estimated second stage burn time
	double eps_2;									// Guidance option selection time
	double eps_3;									// Terminal condition freeze time
	double eps_4;									// Time for cutoff logic entry
	double ROV,ROVs;								// Constant for biasing terminal-range-angle
	double mu;										// Product of G and Earth's mass
	double sin_phi_L;								// Geodetic latitude of launch site: sin
	double cos_phi_L;								// Geodetic latitude of launch site: cos
	double phi_lng;
	double phi_lat;
	double dotM_1;									// Mass flowrate of S2 from approximately LET jettison to second MRS
	double dotM_2;									// Mass flowrate of S2 after second MRS
	double t_B1;									// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	double t_B3;									// Time from second S2 MRS signal
	double t;										// Time from accelerometer reading to next steering command
	double TA1,TA2,TA3;								// Time parameters for on-orbit maneuvers
	double SMCG;									// Steering misalignment correction gain
	double TSMC1,TSMC2;								// Time test for steering misalignment test relative to TB3,TB4
	double alpha_1;									//orbital guidance pitch
	double alpha_2;									//orbital guidance yaw
	double K_P1;									// restart attitude coefficients
	double K_P2;
	double K_Y1;
	double K_Y2;
	
	// PAD-LOADED TABLES
	double Fx[5][5];								// Pre-IGM pitch polynomial
	double fx[7];									// Inclination from azimuth polynomial
	double gx[7];									// Descending Node Angle from azimuth polynomial

	// LVDC software variables, NOT PAD-LOADED
	double Azimuth;									// Azimuth
	double Inclination;								// Inclination
	double DescNodeAngle;							// Descending Node Angle -- THETA_N
	double Azo,Azs;									// Variables for scaling the -from-azimuth polynomials
	VECTOR3 CommandedAttitude;						// Commanded Attitude (RADIANS)
	VECTOR3 PCommandedAttitude;						// Previous Commanded Attitude (RADIANS)
	VECTOR3 ACommandedAttitude;						// Actual Commanded Attitude (RADIANS)
	VECTOR3 CommandRateLimits;						// Command Rate Limits
	VECTOR3 CurrentAttitude;						// Current Attitude   (RADIANS)
	double F;										// Force in Newtons, I assume.	
	double A1,A2,A3,A4,A5;
	double K_p,K_y,K_r;
	double K_Y,K_P,D_P,D_Y;							// Intermediate variables in IGM
	double P_1,P_2;									// Intermediate variables in IGM
	double L_1,L_2,dL_2,Lt_2,L_P,L_Y,Lt_Y;			// Intermediate variables in IGM
	double J_1,J_2,Jt_2,J_Y,J_P;					// Intermediate variables in IGM
	double S_1,S_2,S_P,S_Y;							// Intermediate variables in IGM
	double U_1,U_2;									// Intermediate variables in IGM
	double Q_1,Q_2,Q_Y,Q_P;							// Intermediate variables in IGM
	double d2;										// Intermediate variables in IGM
	double f;										// True anomaly of the predicted cutoff radius vector
	double e;										// Eccentricity of the transfer ellipse
	double C_2,C_4;									// IGM coupling terms for pitch steering
	double C_3;										// Vis-Viva energy of desired transfer ellipse
	double p;										// semilatus rectum of terminal ellipse
	double K_1,K_2,K_3,K_4;							// Correction to chi-tilde steering angles, K_i
	double K_5;										// IGM terminal velocity constant
	double R;										// Instantaneous Radius Magnitude
	double R_T;										// Desired terminal radius
	double V;										// Instantaneous vehicle velocity
	double V_T;										// Desired terminal velocity
	double V_i,V_0,V_1,V_2;							// Parameters for cutoff velocity computation
	double ups_T;									// Desired terminal flight-path angle
	MATRIX3 MX_A;									// Transform matrix from earth-centered plumbline to equatorial
	MATRIX3 MX_B;									// Transform matrix from equatorial to orbital coordinates
	MATRIX3 MX_G;									// Transform matrix from earth-centered plumbline to orbital
	MATRIX3 MX_K;									// Transform matrix from earth-centered plumbline to terminal
	MATRIX3 MX_phi_T;								// Matrix made from phi_T
	double phi_T;									// Angle used to estimate location of terminal radius in orbital plane
	VECTOR3 Pos4;									// Position in the orbital reference system
	VECTOR3 PosS;									// Position in the earth-centered plumbline system.
	VECTOR3 DotS;									// VELOCITY in the earth-centered plumbline system
	VECTOR3 DotM_act;								// actual sensed velocity from platform
	VECTOR3 ddotG_act;								// actual computed acceleration from gravity
	VECTOR3 ddotM_act;								// actual sensed acceleration from platform
	VECTOR3 DotG_act;								// actual computed velocity from gravity
	VECTOR3 DotM_last;								// last sensed velocity from platform
	VECTOR3 ddotG_last;								// last computed acceleration from gravity
	VECTOR3 ddotM_last;								// last sensed acceleration from platform
	VECTOR3 DotG_last;								// last computed velocity from gravity
	VECTOR3 Dot0;									// initial velocity
	double Y_u;										// position component south of equator
	double S,P;										// intermediate variables for gravity calculation
	double a;										// earth equatorial radius
	double J;										// coefficient for second zonal gravity harmonic
	double CG;
	double alpha_D;									// Angle from perigee to DN vector
	bool alpha_D_op;								// Option to determine alpha_D or load it
	bool i_op;										// Option to determine inclination or load it
	bool theta_N_op;								// Option to determine descemdind node angle or load it
	double G_T;										// Magnitude of desired terminal gravitational acceleration
	double xi_T,eta_T,zeta_T;						// Desired position components in the terminal reference system
	VECTOR3 PosXEZ;									// Position components in the terminal reference system
	VECTOR3 DotXEZ;									// Instantaneous something
	double deta,dxi;								// Position components to be gained in this axis
	double dT_2;									// Correction to third or fifth stage burn time
	double T_T;										// Time-To-Go computed using T_2
	double tchi_y_last,tchi_p_last;					// Angles to null velocity deficiencies without regard to terminal data
	double tchi_y,tchi_p;							// Angles to null velocity deficiencies without regard to terminal data
	double dot_zeta_T,dot_xi_T,dot_eta_T;			// Time derivitaves of xi/eta/zeta_T?
	double ddot_zeta_GT,ddot_xi_GT;
	VECTOR3 DDotXEZ_G;								// ???
	double ddot_xi_G,ddot_eta_G,ddot_zeta_G;								
	double dot_dxit,dot_detat,dot_dzetat;			// Intermediate velocity deficiency used in time-to-go computation
	double dot_dxi,dot_deta,dot_dzeta;				// More Deltas
	double Xtt_y,Xtt_p;								// IGM computed steering angles in terminal system
	double X_S1,X_S2,X_S3;							// Direction cosines of the thrust vector
	double sin_ups,cos_ups;							// Sine and cosine of upsilon (flight-path angle)
	double dot_phi_1,dot_phi_T;						// ???
	double dt;										// Nominal powered-flight or coast-guidance computation-cycle interval
	double dt_c;									// Actual computation cycle time
	double dtt_1,dtt_2;								// Used in TGO determination
	double a_1,a_2;									// Acceleration terms used to determine TGO
	double T_GO;									// Time before S4B shutdown
	double T_CO;									// Predicted time of S4B shutdown, from GRR
	double dV;
	double dV_B;									// Velocity cutoff bias for orbital insertion
	double TAS;										// Time from GRR
	double t_clock;									// Time from liftoff
	double X_Zi,X_Yi;								// Generated Pitch and Yaw Command
	double sin_chi_Yit;
	double cos_chi_Yit;
	double sin_chi_Zit;
	double cos_chi_Zit;
	// TABLE15 and TABLE25
	// According to the Apollo 11 LV Trajectory manual, these were 15 and 25 point data tables
	// dependent on time into the launch window.

	double TABLE15_f;								// True anomaly of the predicted cutoff radius vector
	double TABLE15_e;								// Eccentricity of the transfer ellipse
	double TABLE15_C_3;								// Vis-Viva energy of desired transfer ellipse
	//flight control computer
	double a_0p;									// pitch error gain
	double a_0y;									// yaw error gain
	double a_0r;									// roll error gain
	double a_1p;									// pitch rate gain
	double a_1y;									// yaw rate gain
	double a_1r;									// roll rate gain
	double beta_pc;									// commanded pitch thrust direction
	double beta_yc;									// commanded yaw thrust direction
	double beta_rc;									// commanded roll thrust direction
	double beta_p1c;								// commanded actuator angles in pitch/yaw for resp. engine
	double beta_p2c;
	double beta_p3c;
	double beta_p4c;
	double beta_y1c;
	double beta_y2c;
	double beta_y3c;
	double beta_y4c;
	double eps_p;									//error command for APS engines: pitch
	double eps_ypr;									//error command for APS engines: yaw mixed +roll
	double eps_ymr;									//error command for APS engines: yaw mixed -roll
	// TABLE25 is apparently only used on direct-ascent
};