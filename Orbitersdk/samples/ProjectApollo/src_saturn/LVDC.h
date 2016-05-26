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

/* *******************
 * LVDC++ SV VERSION *
 ******************* */

/// DS20150720 LVDC++ ON WHEELS
///
/// \brief LVDC++ computer class.
/// \ingroup Saturns
///

class LVDC {
public:
	LVDC();											// Constructor
	void Init(Saturn* vs);
	void TimeStep(double simt, double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	double SVCompare();
	double LinInter(double x0, double x1, double y0, double y1, double x);
private:
	Saturn* owner;									// Saturn LV
	LVIMU lvimu;									// ST-124-M3 IMU (LV version)
	LVRG lvrg;										// LV rate gyro package
	FILE* lvlog;									// LV Log file
	bool Initialized;								// Clobberness flag

	int LVDC_Timebase;								// Time Base
	double LVDC_TB_ETime;                           // Time elapsed since timebase start

	int LVDC_Stop;									// Guidance Program: Program Stop Flag
	double S1_Sep_Time;								// S1C Separation Counter


	// These are boolean flags that are NOT real flags in the LVDC SOFTWARE. (I.E. Hardware flags)
	bool LVDC_EI_On;								// Engine Indicator lights on
	bool LVDC_GRR;                                  // Guidance Reference Released
	bool CountPIPA;									// PIPA Counter Enable
	bool S2_Startup;								// S2 Engine Start
	
	// These are variables that are not really part of the LVDC software.
	double GPitch[4],GYaw[4];						// Amount of gimbal to command per thruster
	double OPitch[4],OYaw[4];						// Previous value of above, for rate limitation
	double RateGain,ErrorGain;						// Rate Gain and Error Gain values for gimbal control law
	VECTOR3 AttRate;                                // Attitude Change Rate
	VECTOR3 AttitudeError;                          // Attitude Error
	VECTOR3 WV;										// Gravity
	double sinceLastCycle;							// Time since last IGM run
	double IGMInterval;								// IGM Interval
	int IGMCycle;									// IGM Cycle Counter (for debugging)
	int OrbNavCycle;								// Orbital cycle counter (for debugging)

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
	double t_21;									// Time of S2 ignition from lift off
	double dT_F;									// Period of frozen pitch in S1C
	double T_S1,T_S2,T_S3;							// Times for Pre-IGM pitch polynomial
	double T_LET;									// LET Jettison Time
	double dt_LET;									// Nominal interval between S2 ignition and LET jettison
	double dT_cost;									// Parameter for direct stageing guidance update; value and sense unkown
	// IGM event times
	double T_0;										// Time bias for adjusting IGM when one SII engine fails between SII IGN and IGM start
	double T_1;										// Time left in first-stage IGM
	double T_2;										// Time left in second and fourth stage IGM
	double T_3;										// Time left in third and fifth stage IGM
	double T_4N;									// Nominal time of S4B first burn
	double dT_LIM;									// Limit to dT_4;

	// These are boolean flags that are real flags in the LVDC SOFTWARE.
	bool init;										// GRR initialization done
	bool poweredflight;								// Powered flight flag
	bool liftoff;									// lift-off flag
	bool Direct_Ascent;                             // Direct Ascent Mode flag
	bool S1_Engine_Out;								// S1B/C Engine Failure Flag
	bool directstageint;							// Direct Stage Interrupt
	bool HSL;										// High-Speed Loop flag
	int  T_EO1,T_EO2;								// Pre-IGM Engine-Out Constant
	bool ROT;										// Rotate terminal conditions
	bool ROTR;										// Rotate terminal conditions during out-of-orbit burn
	int  UP;										// IGM target parameters updated
	bool BOOST;										// Boost To Orbit
	bool S2_IGNITION;								// SII Ignition flag
	bool S2_ENGINE_OUT;								// SII Engine out flag
	bool S4B_IGN;									// SIVB Ignition
	bool S4B_REIGN;									// SIVB Reignition
	bool S2_BURNOUT;								// SII Burn Out
	bool MRS;										// MR Shift
	bool GATE;										// Logic gate for switching IGM steering
	bool GATE0;										// Permit entry to restart preparation
	bool GATE1;										// Permit entry to out-of-orbit targeting
	bool GATE2;										// Permit only one pass through 1st-opportunity targeting
	bool GATE3;										// Permit entry to IGM out-of-orbit targeting
	bool GATE4;										// Permit only one pass through direct-staging guidance update
	bool GATE5;										// Logic gate that ensures only one pass through cutoff initialization
	bool GATE6;										// Logic gate that ensures only one pass through separation attitude calculation
	bool INH,INH1,INH2;								// Dunno yet (INH appears to be the manual XLUNAR INHIBIT signal, at least)
	bool TU;										// Gate for processing targeting update
	bool TU10;										// Gate for processing ten-paramemter targeting update
	bool first_op;									// switch for first TLI opportunity

	// LVDC software variables, PAD-LOADED BUT NOT NECESSARILY CONSTANT!
	VECTOR3 XLunarAttitude;							// Attitude the SIVB enters when TLI is done, i.e. at start of TB7
	double B_11,B_21;								// Coefficients for determining freeze time after S1C engine failure
	double B_12,B_22;								// Coefficients for determining freeze time after S1C engine failure	
	double V_ex1,V_ex2,V_ex3;						// IGM Exhaust Velocities
	double V_ex2R, V_ex3R;							// IGM exhaust velocities during out-of-orbit burn
	double V_TC;									// Velocity parameter used in high-speed cutoff
	double tau1;									// Time to consume all fuel before S2 MRS
	double tau2;									// Time to consume all fuel between MRS and S2 Cutoff
	double tau3;									// Time to consume all fuel of SIVB
	double tau3R;									// Time to consume SIVB fuel from assumed MRS to cutoff
	double tau2N;									// Artificial tau mode parameters
	double tau3N;
	double Fm;										// Sensed acceleration
	VECTOR3 TargetVector;							// Target vector for out-of-orbit targeting
	double X_1, X_2;								// Intermediate variables for out-of-orbit targeting
	double Tt_T;									// Time-To-Go computed using Tt_3
	double Tt_3;									// Estimated third or fifth stage burn time
	double Tt_3R;									// Initial prediction of fifth-stage IGM
	double T_c;										// Coast time between S2 burnout and S4B ignition
	double TB1;										// Time of TB1
	double TB2;										// Time of TB2
	double TB3;										// Time of TB3
	double TB4;										// Time of TB4
	double TB4A;									// Time of TB4a
	double TB5;										// Time of TB5
	double TB6;										// Time of TB6
	double TB7;										// Time of TB7
	double T_IGM;									// Time from start of TB6 to IGM start during second SIVB burn
	double T_RG;									// Time from TB6 start to reignition for second SIVB burn
	double T_RP;									// Time for restart preparation (TB6 start)
	double T_ST;									// Time for the S*T_P test in 10-parameter targeting
	double T_1c;									// Burn time of IGM first, second, and coast guidance stages
	double T_2R;									// Time for 4th-stage IGM
	double eps_1,eps_1R;							// IGM range angle calculation selection (sub-R denotes out-of-orbit)
	double eps_2,eps_2R;							// Guidance option selection time (sub-R denotes out-of-orbit)
	double eps_3,eps_3R;							// Terminal condition freeze time (sub-R denotes out-of-orbit)
	double eps_4,eps_4R;							// Time for cutoff logic entry (sub-R denotes out-of-orbit)
	double ROV,ROVs;								// Constant for biasing terminal-range-angle
	double ROVR;									// Constant for baising terminal-range-angle during out-of-orbit burn
	double mu;										// Product of G and Earth's mass
	double phi_L;									// Geodetic latitude of launch site: cos
	double dotM_1;									// Mass flowrate of S2 from approximately LET jettison to second MRS
	double dotM_2;									// Mass flowrate of S2 after second MRS
	double dotM_2R;									// Mass flow rate of S4B before presumed MRS during out-of-orbit burn
	double dotM_3;									// Mass flowrate of S4B during first burn
	double dotM_3R;									// Mass flow rate of S4B after presumed MRS during out-of-orbit burn
	double t_B1;									// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	double t_B2;									// Transition time for SIVB MRS from 4.5 to 5
	double t_B3;									// Time from second S2 MRS signal
	double t;										// Time from accelerometer reading to next steering command
	double t_D;										// Time into launch window
	double t_S;										// Time used to scale inclination and descending nodal polynomials
	double t_D0, t_D1, t_D2, t_D3;					// Times of the opening and closing of launch windows
	double t_DS0, t_DS1, t_DS2, t_DS3;				// Times to segment the azimuth calculation polynomial
	double t_SD1, t_SD2, t_SD3;						// Times used to scale the azimuth polynomial
	double t_B4;									// Time from S4B MRS (Saturn V)
	double T_L;										// Launch time from midnight
	double T_LO;									// Reference launch time from midnight
	double TA1,TA2;									// Time parameters used in orbital guidance
	double P_c;										// Time parameter for forced MRS mode when T_2 becomes less than 0 in out-of-orbit burn
	double Ct;										// Time from start of 3rd or 4th stage IGM
	double Ct_o;									// Time from use of artifical tau mode
	double Cf;										// Constant used for S2/S4B direct staging
	double SMCG;									// Steering misalignment correction gain
	double TS4BS;									// Time from direct-stage interrupt to start IGM.
	double TSMC1, TSMC2, TSMC3;						// Time test for steering misalignment test relative to TB3,TB4,TB6
	double V_S2T;									// Nominal S2 cutoff velocity
	double alpha_1;									// orbital guidance pitch
	double alpha_2;									// orbital guidance yaw
	double theta_EO;								// Angle between vernal equinox and launch meridian at reference lauch time (T_LO)
	double K_P1, K_P2, K_Y1, K_Y2;					// restart attitude coefficients
	double K_T3;									// Slope of dT_3 vs. dT_4 curve
	double omega_E;									// Rotational rate of the Earth
	double K_pc;									// Constant time used to force MRS in out-of-orbit mode
	double R_N;										// Nominal radius at SIVB reignition
	
	// PAD-LOADED TABLES
	double Fx[5][5];								// Pre-IGM pitch polynomial
	double fx[7];									// Inclination from azimuth polynomial
	double fxt[7];									// Inclination from time polynomial
	double gx[7];									// Descending Node Angle from azimuth polynomial
	double gxt[7];									// Descending Node Angle from time polynomial
	double hx[3][5];								// Azimuth from time polynomial
	double Rho[6];									// Coasting flight air density polynomial (ref. orbital radius)
	double Drag_Area[5];							// Coasting flight drag area polynomial (ref. flight path angle)


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
	VECTOR3 N;										// Unit vector normal to parking-orbit plane
	VECTOR3 T_P;									// Unit target vector in ephemeral coordinates
	double F;										// Force in Newtons, I assume.	
	double K_Y,K_P,D_P,D_Y;							// Intermediate variables in IGM
	double P_1,P_2,P_3,P_12;						// Intermediate variables in IGM
	double L_1,L_2,L_3,dL_3,Lt_3,L_12,L_P,L_Y,Lt_Y;	// Intermediate variables in IGM
	double J_1,J_2,J_3,J_12,Jt_3,J_Y,J_P;			// Intermediate variables in IGM
	double S_1,S_2,S_3,S_12,S_P,S_Y;				// Intermediate variables in IGM
	double U_1,U_2,U_3,U_12;						// Intermediate variables in IGM
	double Q_1,Q_2,Q_3,Q_12,Q_Y,Q_P;				// Intermediate variables in IGM
	double d2;										// Intermediate variables in IGM
	double f;										// True anomaly of the predicted cutoff radius vector
	double e;										// Eccentricity of the transfer ellipse
	double e_N;										// Nominal eccentricity of the transfer ellipse
	double C_2,C_4;									// IGM coupling terms for pitch steering
	double C_3;										// Vis-Viva energy of desired transfer ellipse
	double p;										// semilatus rectum of terminal ellipse
	double p_N;										// Desired semilatus rectum of transfer ellispe
	double T_M;										// Magnitude of the negative target vector
	double K_1,K_2,K_3,K_4;							// Correction to chi-tilde steering angles, K_i
	double K_5;										// IGM terminal velocity constant
	double R;										// Instantaneous Radius Magnitude
	double R_T;										// Desired terminal radius
	double V;										// Instantaneous vehicle velocity
	double V_T;										// Desired terminal velocity
	double V_R;										// Velocity referenced to atmosphere (true airspeed)
	double V_i,V_0,V_1,V_2;							// Parameters for cutoff velocity computation
	double RAS, DEC;								// Right ascension and declination of the target vector
	double cos_sigma;								// Cosine of the angle between perigee vector and target vector
	double cos_psiT;								// Cosine of angle between nodal and target vector at SIVB reignition
	double sin_psiT;								// Sine of angle between nodal and target vector at SIVB reignition
	double beta;									// Angle between pseudonodal vector and radius vector at TB6 time
	double alpha_TS;								// Angle used to determine desired off-plane boundary for the S*T_p test
	double alphaS_TS;								// Desired angle for S*T_p test
	MATRIX3 MX_A;									// Transform matrix from earth-centered plumbline to equatorial
	MATRIX3 MX_B;									// Transform matrix from equatorial to orbital coordinates
	MATRIX3 MX_G;									// Transform matrix from earth-centered plumbline to orbital
	MATRIX3 MX_K;									// Transform matrix from earth-centered plumbline to terminal
	MATRIX3 MX_phi_T;								// Matrix made from phi_T
	MATRIX3 MX_EPH;									// Transform matrix from ephemeral to earth-centered plumbline
	double phi_T;									// Angle used to estimate location of terminal radius in orbital plane
	VECTOR3 Pos4;									// Position in the orbital reference system
	VECTOR3 PosS;									// Position in the earth-centered plumbline system
	VECTOR3 PosS_4sec;								// Estimated position in earth-centered plumbline after 4 seconds
	VECTOR3 PosS_8secP;								// Estimated position in earth-centered plumbline after 8 seconds, initial
	VECTOR3 PosS_8sec;								// Estimated position in earth-centered plumbline after 8 seconds, final
	VECTOR3 DotS;									// Velocity in the earth-centered plumbline system
	VECTOR3 DotS_4sec;								// Estimated velocity in earth-centered plumbline after 4 seconds
	VECTOR3 DotS_8sec;								// Estimated velocity in earth-centered plumbline after 8 seconds, final
	VECTOR3 DotS_8secP;								// Estimated position in earth-centered plumbline after 8 seconds, initial
	VECTOR3 DDotS_4sec;								// Estimated acceleration in earth-centered plumbline after 4 seconds
	VECTOR3 DDotS_8secP;							// Estimated acceleration in earth-centered plumbline after 8 seconds, initial
	VECTOR3 DotS_R;									// Velocity relative to atmospheric frame of reference
	VECTOR3 PosP;									// Position in parking orbit plane
	VECTOR3 DotP;									// Velocity in parking orbit plane
	VECTOR3 Sbar;									// Position of pseudonodal vector
	VECTOR3 Sbardot;								// Velocity of pseudonodal vector
	VECTOR3 Cbar_1;									// Unit vector normal to transfer ellipse plane
	VECTOR3 Sbar_1;									// Unit vector normal to nodal vector
	VECTOR3 DDotS_D;								// Atmospheric drag
	VECTOR3 DotM_act;								// actual sensed velocity from platform
	VECTOR3 ddotG_act;								// actual computed acceleration from gravity
	VECTOR3 ddotM_act;								// actual sensed acceleration from platform
	VECTOR3 DotG_act;								// actual computed velocity from gravity
	VECTOR3 DotM_last;								// last sensed velocity from platform
	VECTOR3 ddotG_last;								// last computed acceleration from gravity
	VECTOR3 ddotM_last;								// last sensed acceleration from platform
	VECTOR3 DotG_last;								// last computed velocity from gravity
	VECTOR3 DDotV;									// Precomputed venting acceleration
	double Y_u;										// position component south of equator
	double S,P, S_34, P_34;							// intermediate variables for gravity calculation
	double a;										// earth equatorial radius
	double J;										// coefficient for second zonal gravity harmonic
	double H;										// coefficient for third zonal gravity harmonic
	double D;										// coefficient for fourth zonal gravity harmonic
	double CG;
	double gamma;									// Computed flight path angle
	double gamma_T;									// Desired terminal flight path angle
	double alpha_D;									// Angle from perigee to DN vector
	bool alpha_D_op;								// Option to determine alpha_D or load it
	bool i_op;										// Option to determine inclination or load it
	bool theta_N_op;								// Option to determine descemdind node angle or load it
	double G_T;										// Magnitude of desired terminal gravitational acceleration
	double rho;										// Atmospheric density
	double drag_area;								// Drag area of the vehicle
	double xi_T,eta_T,zeta_T;						// Desired position components in the terminal reference system
	VECTOR3 PosXEZ;									// Position components in the terminal reference system
	VECTOR3 DotXEZ;									// Instantaneous velocity components in the terminal reference system
	double deta,dxi;								// Position components to be gained in this axis
	double dT_3;									// Correction to third or fifth stage burn time
	double dT_4;									// Difference between nominal and actual 1st S4B burn time
	double dTt_4;									// Limited value of above
	double T_T;										// Time-To-Go computed using T_3
	double tchi_y_last,tchi_p_last;					// Angles to null velocity deficiencies without regard to terminal data
	double tchi_y,tchi_p;							// Angles to null velocity deficiencies without regard to terminal data
	double dot_zeta_T,dot_xi_T,dot_eta_T;			// Time derivitaves of xi/eta/zeta_T?
	double ddot_zeta_GT,ddot_xi_GT;
	double theta_E;									// Angle between vernal equinox and launch meridian at actual launch time (T_L)
	double theta_N;									// Angle between descending node and launch meridian of out-of-orbit target
	VECTOR3 DDotXEZ_G;								// Gravitational components in terminal reference system
	VECTOR3 DDotXEZ_GT;								// Gravitational components at desired terminal radius
	double dot_dxit,dot_detat,dot_dzetat;			// Intermediate velocity deficiency used in time-to-go computation
	double dot_dxi,dot_deta,dot_dzeta;				// More Deltas
	double Xtt_y,Xtt_p;								// IGM computed steering angles in terminal system
	double X_S1,X_S2,X_S3;							// Direction cosines of the thrust vector
	double sin_gam,cos_gam;							// Sine and cosine of gamma (flight-path angle)
	double dot_phi_1,dot_phi_T;						// ???
	double dt;										// Nominal powered-flight or coast-guidance computation-cycle interval
	double dt_c;									// Actual computation cycle time
	double dtt_1,dtt_2;								// Used in TGO determination
	double a_1,a_2;									// Acceleration terms used to determine TGO
	double T_GO;									// Time before S4B shutdown
	double T_CO;									// Predicted time of S4B shutdown, from GRR
	double dV;
	double dV_B;									// Velocity cutoff bias for orbital insertion
	double dV_BR;									// Velocity cutoff bias for out-of-orbit burn
	double TAS;										// Time from GRR
	double t_clock;									// Time from liftoff
	double X_Zi,X_Yi;								// Generated Pitch and Yaw Command
	double sin_chi_Yit;
	double cos_chi_Yit;
	double sin_chi_Zit;
	double cos_chi_Zit;
	// TABLE15
	/*
		These tables store the precomputed out-of-orbit targeting data for the Saturn V launches.
	TABLE15 is for the first injection opportunity, and TABLE25 is likewise for the second one.
	Both tables use the same data for reference launch time (opening of launch window), 
	injection timing tests, and so on.
	*/
	struct SVTABLE
	{
		// These variables store the targeting data that is fixed for each launch time 
		static double T_LO;			// Reference time of launch from midnight (doubles as opening of launch window)
		double alphaS_TS;			// Nominal angle between nodal vector and target ellipse vector
		double beta;				// Constant angle defining the pseudonodal vector relative to radius vector at TB6 time
		double  T_ST;				// Time after launch for the out-of-orbit targeting to perform the S*T_P test (determine injection validity and restart time)
		double f;					// True anomaly at cutoff of transfer ellipse

		//This data structure stores the actual launch tables. Array indexing should make it easier to iterate through the launch times and select the desired launch information.
		struct target_table {
			double t_D;			// Time of launch after reference launch time (time of launch after window opens)
								// NOTE: This is a discrete time chosen to accomodate a pad recycle after a hold or abort. 
								// Launches MUST occur on the designated times to use these TABLEs. Otherwise, in-flight target updates are required.
			double cos_sigma;	// Cosine of the angle between perigee vector and target vector
			double C_3;			// Vis-viva energy of target ellipse
			double e_N;			// Eccentricity of target ellispe
			double RAS;			// Right ascension of apogee of target ellipse
			double DEC;			// Declination of apogee of target ellispe
			double alpha_D;		// Angle between perigee and descending node
		}target[15];
	}TABLE15[2];
	int tgt_index;				// Non-LVDC variable to enable selecting the correct set of injection parameters

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

	friend class MCC;
	friend class ApolloRTCCMFD;
};

/* ********************
 * LVDC++ S1B VERSION *
 ******************** */

class LVDC1B {
public:
	LVDC1B();										// Constructor
	void init(Saturn* own);
	void timestep(double simt, double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
private:
	bool Initialized;								// Clobberness flag
	FILE* lvlog;									// LV Log file
	Saturn* owner;
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
	double tau1;									// Time to consume all fuel before S4 MRS
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
	double gamma_T;									// Desired terminal flight-path angle
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