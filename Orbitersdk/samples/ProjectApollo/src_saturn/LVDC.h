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

#include <vector>
#include <bitset>

#define LVDC_TIMESTEP 0.01

class LVDA;

struct SwitchSelectorSet
{
	double time;
	int stage;
	int channel;
};

/* *******************
 * LVDC++ SV VERSION *
 ******************* */

/// DS20150720 LVDC++ ON WHEELS
///
/// \brief LVDC++ computer class.
/// \ingroup Saturns
///

class LVDC
{
public:
	LVDC(LVDA &lvd);
	virtual ~LVDC() {}
	virtual void TimeStep(double simdt) = 0;
	virtual void Init() = 0;
	virtual void SaveState(FILEHANDLE scn) = 0;
	virtual void LoadState(FILEHANDLE scn) = 0;
	virtual bool GetGuidanceReferenceFailure() = 0;
	virtual bool TimebaseUpdate(double dt) = 0;
	virtual bool GeneralizedSwitchSelector(int stage, int channel) = 0;
	virtual bool LMAbort() { return false; }
	virtual bool RestartManeuverEnable() { return false; }
	virtual bool TDEEnable() { return false; }
	virtual bool RemoveInhibitManeuver4() { return false; }
	virtual bool TimeBase8Enable() { return false; }
	virtual bool EvasiveManeuverEnable() { return false; }
	virtual bool SIVBIULunarImpact(double tig, double dt, double pitch, double yaw) { return false; }
	virtual bool ExecuteCommManeuver() { return false; }
	virtual bool LaunchTargetingUpdate(double V_T, double R_T, double theta_T, double inc, double dsc, double dsc_dot, double t_grr0) { return false; }
	virtual bool DiscreteOutputTest(int bit, bool on) = 0;
	virtual bool NavigationUpdate(VECTOR3 DCSRVEC, VECTOR3 DCSVVEC, double DCSNUPTIM) = 0;
	void PrepareToLaunch();

	//Mathematical functions
	double log(double a);

	FILE* lvlog;									// LV Log file
protected:
	double RealTimeClock;
	bool ReadyToLaunch;
	//Start time of current time base measured from GRR
	double TI;
	LVDA &lvda;
};

class LVDCSV: public LVDC {
public:
	LVDCSV(LVDA &lvd);											// Constructor
	void Init();
	void TimeStep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void ReadFlightSequenceProgram(char *fspfile);

	bool GetGuidanceReferenceFailure() { return ModeCode26[MC26_Guidance_Reference_Failure]; }

	VECTOR3 SVCompare();
	double LinInter(double x0, double x1, double y0, double y1, double x);

	//DCS Commands
	bool TimebaseUpdate(double dt);
	bool GeneralizedSwitchSelector(int stage, int channel);
	bool RestartManeuverEnable();
	bool TDEEnable();
	bool RemoveInhibitManeuver4();
	bool InhibitSeparationManeuver();
	bool UpdateSeparationManeuver(double dt);
	bool EvasiveManeuverEnable();
	bool TimeBase8Enable();
	bool SIVBIULunarImpact(double tig, double dt, double pitch, double yaw);
	bool ExecuteCommManeuver();
	bool DiscreteOutputTest(int bit, bool on);
	bool NavigationUpdate(VECTOR3 DCSRVEC, VECTOR3 DCSVVEC, double DCSNUPTIM);

	//Public Variables
	double T_L;									// Time of GRR in seconds since midnight
private:								// Saturn LV

	//Internal Functions
	bool GetInterrupt(int rupt);
	void ProcessInterrupt(int rupt);
	void SystemTimeUpdateRoutine();
	void TimeBaseChangeRoutine();
	void Timer1Interrupt(bool timer1schedule);
	void Timer2Interrupt(bool timer2schedule);
	void NonInterruptSequencer(bool phase13);

	VECTOR3 DragSubroutine(VECTOR3 Rvec, VECTOR3 Vvec);
	bool TimeCheck(double t_event);
	void StartTimebase4A();
	void StartTimebase5();
	void StartTimebase7();
	void SCInitiationOfSIISIVBSeparationInterrupt();

	//MODULES
	//Phase Activator (PA)
	void PhaseActivator(bool init);
	void Phase13ApplicationProgramInit();
	void Phase24ApplicationProgramInit();
	//Acelerometer Read (AR)
	void AccelerometerRead();
	//Simulated Accelerometers (SA)
	void SimulatedAccelerometers();
	//AccelerometerProcessing (AP)
	void AccelerometerProcessing();
	//Periodic Processor (PP)
	void PeriodicProcessor();
	//Events Processor (EP)
	void EventsProcessor(int entry);
	//Minor Loop (ML)
	void MinorLoop(int entry);
	//Minor Loop Support (MS)
	void MinorLoopSupport();
	//Switch Selector Processor (SS)
	void SwitchSelectorProcessor(int entry);
	void SSTUPD(double &time);
	void SSTUPQ(double BIAS);
	void AdvanceToNextSSRoutine();
	void SSSelectionAndSetupRoutine();
	//Time Base 1 (TB1)
	void StartTimeBase1(int entry);
	//Time Base 2 (TB2)
	void StartTimeBase2();
	//Time Base 3 (TB3)
	void StartTimeBase3();
	//Time Base 4 (TB4)
	void StartTimeBase4();
	//Time Base 5/7 (TB57)
	void CheckTimeBase57();
	//Phase II and IV Control (CM)
	void PhaseIIandIVControl(int entry);
	//Orbit Navigation (NZ)
	void OrbitNavigation();
	//NavigationExtrapolation (NI)
	void NavigationExtrapolation();
	//Cutoff Logic (CO)
	void CutoffLogic();
	//Chi Computations (CC)
	void ChiComputations(int entry);
	//Time-to-go to Restart and Beta Test (RS)
	void TimeToGoToRestartAndBetaTest();
	//Orbit Guidance (OG)
	void OrbitGuidance();
	//F/M Calculations (DV)
	void FMCalculations();
	//Discrete Processor (DP)
	void DiscreteProcessor();
	//Discrete Inputs 11, 14 and 17 (S-IC engine out)
	void DiscreteProcessor1();
	//Discrete Inputs 13 and 21 (S-II engine out)
	void DiscreteProcessor2();
	//Discrete Input 22 (S-II/S-IVB direct staging and S-IVB cutoff)
	void DiscreteProcessor3();
	//Bost Navigation (NE)
	void BoostNavigation();
	//Time Tilt Guidance (TT)
	void TimeTiltGuidance();
	//Iterative Guidance Mode (IG)
	void IterativeGuidanceMode();
	//Variable Launch Window (LA)
	void VariableLaunchWindow();
	//Gravitation Acceleration (GR)
	VECTOR3 GravitationSubroutine(VECTOR3 Rvec, bool J2only);
	//Restart Calculations (TC)
	void RestartCalculations();
	//Acquisition Gain/Loss (GL)
	void AcquisitionGainLoss();

	char FSPFileName[256];
	bool Initialized;								// Clobberness flag

	//Hardware Counters
	int Timer1Counter, Timer2Counter;

	//Task tables and indizes

	//Flight phase indicator
	int DVP;
	//Timer 2 task status table
	bool T2STAT[11];
	//Timer 2 task execution time table
	double DLTTL[11];
	//Events processor table index
	int EPTINDX;
	//Time of execution for an event
	double EPTTIM[131];
	int EPTABLE[131];
	//Non-interrupt sequence task table (phase 1/3)
	//0 = Accelerometer Read (AR)
	//1 = Simulated Accelerometers (SA)
	//2 = Accelerometer Processing (AP)
	//3 = F/M Calculations (DV)
	//4 = Discrete Processor (DP)
	//5 = Boost Navigation (NE)
	//6 = Restart Calculations (TC)
	//7 = Phase Activator (PA)
	//8 = Time Tilt Guidance (TT)
	//9 = Chi Computations (CC1)
	//10 = IGM (IG)
	//11 = S-IVB Cutoff Prediction (HS)
	//12 = Orbit Guidance (OG)
	//13 = Target Update (TG)
	//14 = Time-to-Go to Restart and Beta Test (RS)
	//15 = Time Base 6 Check (CS)
	//16 = Time Base 1 (TB1)
	//17 = Time Base 5/7 (TB57)
	//18 = Minor Loop Support (MS)
	//19 = Simulated Platform Gimbal Angles (PG)
	//20 = Etc Btc (EB)
	bool NISTAT[21];

	//Timer stuff

	double DVACT; //Real time clock (RTC) reading associated with TAS
	double DVRTC; //Real time clock associated with last time update
	double DVTRR; //Elapsed total time in current reference
	double DVERT; //Time error associated with time update
	double DVTRB; //Elapsed time in current time base including ground bias updates
	//Real time clock reading at last interrupt
	double TEX;
	//Elapsed time in mission from GRR at last time update
	double TMM;
	//Mission time at start of reference
	double TMR;
	//Timer 1 function to be scheduled
	int GST1M;
	//Timer 2 function to be scheduled
	int DGST2;
	//Time for next timer 2 function
	double DV2TG;
	//Events processor previous event time
	double VTOLD;
	//Timer 2 interrupt level in progress indicator
	bool DFIL1;
	//External interrupt level in progress indicator
	bool DFIL2;
	//Timer 1 interrupt level in progress indicator
	bool DFIL3;
	//Minor loop initial rate
	double MIR;
	//Minor loop orbit rate
	double MOR;
	//Minor loop rate
	double MLD;
	//Execution time for next minor loop
	double MLT;
	//Periodic processor current mission time
	double POT;
	//Switch selector execution time
	double DVSST;
	//Switch selector function to be scheduled
	int SSM;
	//Minor loop function to be scheduled (0 = Normal Minor Loop, 1 = Flight Simulation Minor Loop, 2 = Minor Loop with Liftoff Search)
	int MLM;
	//Accumulated ground bias time update
	double DVTGB;
	//Time base change indicator for events processor
	bool DFTBCEP;

	//Switch selector

	unsigned DVASW;		//Switch selector request status
	unsigned VASPI;		//Alternate sequence in progress status word
	double VSSTM;		//Temporary SS time storage
	double VSSRT;		//Switch selector time of issuance
	double VATRR;		//Alternate SS sequence time start
	double VATR4;		//Class 4 sequence time start
	bool FTADV;			//Normal or class 4 table advance flag
	bool FCLS4;			//Class 4 SS sequence in progress flag
	unsigned SST1PTR;	//Normal switch selector table pointer
	unsigned SST2PTR;	//Class 4 switch selector table pointer
	int VSNA1;			//Storage for next SS stage
	int VSNA2;			//Storage for next SS channel
	int VHSTW1;			//Storage for last SS stage
	int VHSTW2;			//Storage for last SS channel
	bool FSSAC;			//Switch selector processing in progress flag
	bool FASE;			//Alternate sequence in progress flag
	double VSTGO;		//Time-to-go to next SS function
	unsigned SSTTBPTR[9];	//Table of pointers to switch selector table for each time base
	double VSSW;			//Bias time
	//Indizes for switch selector table
	unsigned KSSINDXTB7A, KSSINDXTB6A, KSSINDXTB6B, KSSINDXTB6C, KSSINDXTB3A, KSSINDXTB5A, KSSINDXTB5B, KSSINDXSIVA, KSSINDXSIVB, KSSINDXS4C1, KSSINDXGSS;
	unsigned KSSINDXSBLO, KSSINDXSBHI, KSSINDXSBOM, KSSINDXECSV, KSSINDXECS1, KSSINDXGAIN, KSSINDXTB6D, KSSINDXALU;
	double KSSB1, KSSB2, KSSB3, KSSB5, KSSB8, KCSSK;
	bool DFTUP;				//Time update waiting indicator
	double VGBIA;			//Time base bias to be implemented
	//Class 1/3 temporary storage for SST1PTR, VASPI, VATRR
	unsigned VSC30, VSC31, VSC10, VSC11;
	double VSC32, VSC12;

	int LVDC_Timebase;								// Time Base
	double LVDC_TB_ETime;                           // Time elapsed since timebase start

	int LVDC_Stop;									// Guidance Program: Program Stop Flag

	// These are boolean flags that are NOT real flags in the LVDC SOFTWARE. (I.E. Hardware flags)
	bool CountPIPA;									// PIPA Counter Enable
	bool directstagereset;							// Direct Stage Reset
	
	// These are variables that are not really part of the LVDC software.
	VECTOR3 AttitudeError;                          // Attitude Error
	VECTOR3 DeltaAtt;
	VECTOR3 AttitudeErrorOld;
	VECTOR3 WV;										// Gravity
	double sinceLastCycle;							// Time since last IGM run
	int IGMCycle;									// IGM Cycle Counter (for debugging)
	double t_S1C_CECO;								// Time since launch for S-1C center engine cutoff
	bool SIICenterEngineCutoff;
	bool FixedAttitudeBurn;
	double t_TB8Start;

	// Event Times
	double T_ar;									// S1C Tilt Arrest Time
	double t_1;										// Backup timer for Pre-IGM pitch maneuver
	double t_2;										// Time to initiate pitch freeze for S1C engine failure
	double t_3;										// Constant pitch freeze for S1C engine failure prior to t_2
	double t_3i;									// Clock time at S4B ignition
	double t_4;										// Upper bound of validity for first segment of pitch freeze
	double t_5;										// Upper bound of validity for second segment of pitch freeze
	double TMEFRZ;									// Time to terminate pitch freeze after S1C engine failure
	double t_21;									// Time of S2 ignition from lift off
	double dT_F;									// Period of frozen pitch in S1C
	double T_S1,T_S2,T_S3;							// Times for Pre-IGM pitch polynomial
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
	bool Direct_Ascent;                             // Direct Ascent Mode flag
	bool HSL;										// High-Speed Loop flag
	double T_EO1;
	int T_EO2;										// Pre-IGM Engine-Out Constant
	bool ROT;										// Rotate terminal conditions
	bool ROTR;										// Rotate terminal conditions during out-of-orbit burn
	int  UP;										// IGM target parameters updated
	bool BOOST;										// Boost To Orbit
	bool CHIBARSTEER;								// Terminal guidance flag
	bool S2_IGNITION;								// SII Ignition flag
	bool S4B_IGN;									// SIVB Ignition
	bool S4B_REIGN;									// SIVB Reignition
	bool S2_BURNOUT;								// SII Burn Out
	bool MRS;										// MR Shift
	bool GATE;										// Logic gate for switching IGM steering
	bool GATE0;										// Permit entry to restart preparation
	bool GATE1;										// Permit entry to out-of-orbit targeting
	bool GATE2;										// Permit only one pass through 1st-opportunity targeting
	bool GATE5;										// Logic gate that ensures only one pass through cutoff initialization
	bool TU;										// Gate for processing targeting update
	bool TU10;										// Gate for processing ten-paramemter targeting update
	bool first_op;									// switch for first TLI opportunity
	bool TerminalConditions;						// Use preset terminal conditions (R_T, V_T, gamma_T and G_T) for into-orbit targeting
	bool ImpactBurnEnabled;							// Lunar impact burn has been enabled
	bool ImpactBurnInProgress;						// Lunar impact burn is in progress
	std::bitset<26> ModeCode24, ModeCode25, ModeCode26, ModeCode27;
	std::bitset<26> DPM;							//Mask word that specifies which DIN's are to be processed when they change from OFF to ON
	std::bitset<12> DVIH;							//Interrupt inhibit
	std::bitset<12> InterruptState;					//To prevent continual interrupts

	// LVDC software variables, PAD-LOADED BUT NOT NECESSARILY CONSTANT!
	VECTOR3 XLunarAttitude;							// Attitude the SIVB enters when TLI is done, i.e. at start of TB7
	VECTOR3 XLunarSlingshotAttitude;				// Attitude the SIVB enters for slingshot maneuver.
	VECTOR3 XLunarCommAttitude;						// Attitude the SIVB enters for communication.
	double ART;										// Time-to-go in S-II stage at which guidance is arrested
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
	double MFS;										// Current smoothed (filtered) value of (M/F)
	double MFK[9];									// (M/F)S filter coefficients
	double MFSArr[4];								// First through fourth consecutive past values of (M/F)S
	double MF[5];									// Present through third past consecuitve values of (M/F)
	double LVIMUMJD;
	double DTTEMP;
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
	double TB4a;									// Time of TB4a
	double TB5;										// Time of TB5
	double TB6;										// Time of TB6
	double TB7;										// Time of TB7
	double TB8;										// Time of TB8
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
	double PHI;										// Geodetic latitude of launch site
	double PHIP;									// Geocentric latitude of launch site
	double KSCLNG;									// Longitude of the launch site
	double R_L;										// Radius from geocentric center of the Earth to the center of the IU on launch pad
	double dotM_1;									// Mass flowrate of S2 from approximately LET jettison to second MRS
	double dotM_2;									// Mass flowrate of S2 after second MRS
	double dotM_2R;									// Mass flow rate of S4B before presumed MRS during out-of-orbit burn
	double dotM_3;									// Mass flowrate of S4B during first burn
	double dotM_3R;									// Mass flow rate of S4B after presumed MRS during out-of-orbit burn
	double t_B1;									// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	double t_B2;									// Transition time for SIVB MRS from 4.5 to 5
	double t_B3;									// Time from second S2 MRS signal
	double t_D;										// Time into launch window
	double t_S;										// Time used to scale inclination and descending nodal polynomials
	double t_D0, t_D1, t_D2, t_D3;					// Times of the opening and closing of launch windows
	double t_DS0, t_DS1, t_DS2, t_DS3;				// Times to segment the azimuth calculation polynomial
	double t_SD1, t_SD2, t_SD3;						// Times used to scale the azimuth polynomial
	double t_B4;									// Time from S4B MRS (Saturn V)
	double T_LO;									// Reference launch time from midnight
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
	double TVRATE;									// Earth rotation rate (0 is used for fixed azimuth missions)
	double K_pc;									// Constant time used to force MRS in out-of-orbit mode
	double R_N;										// Nominal radius at SIVB reignition
	double K_D;										// Orbital drag model constant
	double rho_c;									// Constant rho for use when altitude is less than h_1
	double h_1;										// Lower limit of h for atmospheric density polynomial
	double h_2;										// Upper limit of h for atmospheric density polynomial
	double T_ImpactBurn;							// Time of ignition of lunar impact burn
	double dT_ImpactBurn;							// Burn duration of lunar impact burn
	double DT_N1;									// Nominal DT from T0+0.0 until T1+0.0
	double DT_N2;									// Nominal DT from T1+0.0 until T3+T3_IGM
	double DT_N3;									// Nominal DT from T3+T3_IGM until T5-eps2 and T6+T_IGM until T7-eps2
	double DT_N4;									// Nominal DT from T5-eps2 until T5+0 and T7-eps2 until T7+0
	double DT_N5;									// Nominal DT from T5+0.0 until T5+BN5 and T6+0 until T6+T_IGM
	double DT_N6;									// Nominal DT from T5+BN5 until T6+0 and T7+BN4 until EOM
	double T_SON;									// Time since last orbital navigation pass
	// Normally set to 2, set to the desired type in the events processor. -2 = inertial hold of local reference, -1 = local reference, 0 = chi freeze, 1 = inertial reference
	int AttitudeManeuverState;
	std::bitset<15> OGStatusWord;					// Flagword that indicates the maneuvers that have already been implemented
	double TI5F3;									// Time of Earth orbit separation maneuver
	double TI5F4;									// Time of Earth orbit maneuver back to horizontal tracking
	double TI5F5;									// Time of Earth orbit retro maneuver
	double T7M10;									// Time in TB7 to compute inertial attitude corresponding to locally referenced separation maneuver
	double T7M11;									// Time in TB7 for the slingshot/LOX dump maneuver (Apollo 8)
	
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
	double Azo,Azs;									// Variables for scaling the -from-azimuth polynomials
	VECTOR3 CommandedAttitude;						// Commanded Attitude (RADIANS)
	VECTOR3 PCommandedAttitude;						// Previous Commanded Attitude (RADIANS)
	VECTOR3 CurrentAttitude;						// Current Attitude   (RADIANS)
	VECTOR3 N;										// Unit vector normal to parking-orbit plane
	VECTOR3 T_P;									// Unit target vector in ephemeral coordinates
	double F;										// Force in Newtons, I assume.	
	double A1, A2, A3, A4, A5;
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
	double VOLD;									// Vehicle velocity during previous major loop
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
	MATRIX3 MGA;									// Transformation matrix from G-system to A-system
	MATRIX3 MSA;									// Transformation matrix from S-system to A-system
	double phi_T;									// Angle used to estimate location of terminal radius in orbital plane
	VECTOR3 Pos4;									// Position in the orbital reference system
	VECTOR3 PosA;									// Position in earth-fixed telemetry station system
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
	VECTOR3 DotG_act;								// actual computed velocity from gravity
	VECTOR3 DotM_last;								// last sensed velocity from platform
	VECTOR3 ddotG_last;								// last computed acceleration from gravity
	VECTOR3 DotG_last;								// last computed velocity from gravity
	VECTOR3 DDotV;									// Precomputed venting acceleration
	VECTOR3 C_A[11];								// Array of telemetry station unit vectors
	double Y_u;										// position component south of equator
	double S,P, S_34, P_34;							// intermediate variables for gravity calculation
	double a;										// earth equatorial radius
	double J;										// coefficient for second zonal gravity harmonic
	double H;										// coefficient for third zonal gravity harmonic
	double D;										// coefficient for fourth zonal gravity harmonic
	double CG;
	double cos_alpha;								// cosine of the angle of attack
	double gamma_T;									// Desired terminal flight path angle
	double alpha_D;									// Angle from perigee to DN vector
	bool alpha_D_op;								// Option to determine alpha_D or load it
	bool i_op;										// Option to determine inclination or load it
	bool theta_N_op;								// Option to determine descemdind node angle or load it
	double G_T;										// Magnitude of desired terminal gravitational acceleration
	double rho;										// Atmospheric density
	double drag_area;								// Drag area of the vehicle
	double xi_T;									// Desired position component in the terminal reference system
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
	double dt_c;									// Actual computation cycle time
	double dt_g;									// Actual guidance cylce time
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
	double h;										// Altitude of the vehicle above the oblate spheroid of the earth
	double DT_N;									// Nominal value of DT
	double MLR;										// Minor loop rate
	double MS25DT;									// Number of minor loops per DT_N through minor loop support
	double MS04DT;									// The reciprocal of MS25DT
	double MSK5;									// Ladder rate limit (all channels)
	double MSK6;									// Ladder magnitude limit (pitch and yaw channels)
	double MSK16;									// Ladder magnitude limit (roll channel)
	double MSLIM1;									// Attitude command (chi) rate limit (roll channel)
	double MSLIM2;									// Attitude command (chi) rate limit (pitch and yaw channel)
	VECTOR3 DChi;									// Computed change Chi_apo (CommandedAttitude) for present major loop
	VECTOR3 DChi_apo;								// Computed Chi guidance command increment for minor loop
	double Chi_xp_apo;								// Predicted value of Chi_x_apo at end of present major loop
	double Chi_zp_apo;								// Predicted value of Chi_z_apo at end of present major loop
	double theta_xa;								// Average value of theta_x during next major loop pass
	double theta_za;								// Average value of theta_z during next major loop pass
	VECTOR3 R_OG;									// Radius vector for use in orbital guidance
	VECTOR3 RTEMP1, VTEMP1, ATEMP1, RPT, VPT, APT, DRT, DVT, ddotS;
	double R4;
	double NUPTIM;									// Time from GRR at which a DCS Navigation Update is to be implemented
	VECTOR3 Pos_Nav, Vel_Nav;
	double TEMP;
	int i;
	double theta_R;									// Total angular rotation of earth since GRR
	double d_A;										// Altitude of the vehicle above the horizon of a telemetry station
	double R_STA;									// Mean radius of telemetry stations
	double TBA;										// Time of station acquisition
	double TBL;										// Time of station loss

	//Switch Selector Table
	std::vector<SwitchSelectorSet> SSTABLE;

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
		double alphaS_TS;			// Nominal angle between nodal vector and target ellipse vector
		double beta;				// Constant angle defining the pseudonodal vector relative to radius vector at TB6 time
		double T_ST;				// Time after launch for the out-of-orbit targeting to perform the S*T_P test (determine injection validity and restart time)
		double f;					// True anomaly at cutoff of transfer ellipse
		double R_N;					// Restart radius
		double T2IR;				// Nominal Duration of fourth stage of IGM
		double T3PR;				// IGM phase 5 time-to-go
		double TAU3R;				// Time to deplete S-IVB mass from S-IVB EMR
		double dV_BR;				// Thrust decay velocity bias

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

	// TABLE25 is apparently only used on direct-ascent

	//Interrupts
	enum SV_Interrupt_Bits
	{
		INT1_CommandLVDARCA110Interrupt,
		INT2_SCInitSIISIVBSepA_SIVBEngineCutoffA,
		INT3_RCA110AInterrupt,
		INT4_SIVBEngineOutB,
		INT5_SICOutboardEnginesCutoffA,
		INT6_SIIEnginesCutoff,
		INT7_GuidanceReferenceRelease,
		INT8_CommandDecoderInterrupt,
		INT9_TLCMemoryError,
		INT10_DataReadyFromCIU,
		INT11_Timer2Interrupt,
		INT12_Timer1Interrupt
	};

	//Discrete Inputs
	enum SV_DiscreteInput_Bits
	{
		DIN1_RCA110Sync = 0,
		DIN2_CommandDecoder,
		DIN3_O2H2BurnerMalfunction,
		DIN4_SpacecraftSeparation,
		DIN5_SIVBEngineOutA,
		DIN6_TLI_Inhibit,
		DIN7_Spare,
		DIN8_Spare,
		DIN9_SCControlOfSaturn,
		DIN10_SIISIVBSeparation,
		DIN11_SICInboardEngineOutB,
		DIN12_SICSIISeparation,
		DIN13_SIIInboardEngineOut,
		DIN14_SICOutboardEngineOut,
		DIN15_SIIAftInterstageSeparation,
		DIN16_GRRAlert,
		DIN17_SICInboardEngineOutA,
		DIN18_SICOutboardEngineCutoffB,
		DIN19_SIIEnginesOut,
		DIN20_SIVBIgnitionSequenceStart,
		DIN21_SIIOutboardEngineOut,
		DIN22_SCInitSIISIVBSepB_SIVBEngineCutoffB,
		DIN23_SCInitiationOfSIVBEngineCutoff,
		DIN24_Liftoff
	};

	//Mode Code Bits
	enum ModeCode25_Bits
	{
		MC25_BeginTB6 = 0,
		MC25_BeginTB5,
		MC25_FirstSIVBCutoffCommand,
		MC25_TerminalGuidanceFirstBurn,
		MC25_SII_SIVB_Separation = 8,
		MC25_SIIInboardEngineOut,
		MC25_SIIOutboardEngineOut,
		MC25_SII_IGMBeforeEMRC = 13,
		MC25_SIISkirtSeparation,
		MC25_SIICutoff,
		MC25_SIC_OutboardEngineCutoff = 16,
		MC25_SIC_InboardEngineCutoff,
		MC25_BeginTB3,
		MC25_BeginTB2,
		MC25_StopPitchManeuver,
		MC25_StopRollManeuver = 22,
		MC25_StartPitchAndRollManeuver,
		MC25_BeginTB1,
		MC25_BeginTB0
	};

	enum ModeCode26_Bits
	{
		MC26_Preflight_Abort = 0,
		MC26_TB6b_Started = 2,
		MC26_StartTB6_DIN20,
		MC26_StartTB6C,
		MC26_First_TLI_Inhibited,
		MC26_Second_TLI_Inhibited,
		MC26_Guidance_Reference_Failure,
		MC26_Memory_Failure,
		MC26_TB6a_Started,
		MC26_SMCActive,
		MC26_SCInitOfSIISIVBSeparation = 16,
		MC26_SCControlAfterGRF,
		MC26_SCInitOfSIVBCutoff,
		MC26_TB7_Started,
		MC26_SecondSIVBCutoffCommand = 21,
		MC26_TerminalGuidanceSecondBurn
	};

	enum ModeCode27_Bits
	{
		MC27_InhibitManeuver13 = 0,
		MC27_InhibitManeuver11,
		MC27_InhibitManeuver10,
		MC27_InhibitManeuver9,
		MC27_InhibitManeuver5,
		MC27_InhibitManeuver4,
		MC27_InhibitManeuver3,
		MC27_H2OControlValveLogicActive,
		MC27_AttHoldWrtInertialContRetFromSC,
		MC27_AttHoldWrtLocalContRetFromSC,
		MC27_TB8_Started,
		MC27_TargetUpdateReceived,
		MC27_TDE_Enable,
		MC27_SCInControl,
		MC27_InertialHoldInProgress,
		MC27_TrackLocalHoriz,
		MC27_TimeBaseUpdateAccepted,
		MC27_DCSNavigationUpdateAccepted,
		MC27_AntennaOmni,
		MC27_AntennaHighGain,
		MC27_AntennaLowGain,
		MC27_CommManInTB8,
		MC27_TB8EnableDCSCmdEnable = 24,
		MC27_PoweredFlightDCSInhibitRemoved
	};

	//Masks
	static const unsigned MSKSSCLS1 = 0000003770U; //Class 1 mask
	static const unsigned MSKSSCLS3 = 0077574000U; //Class 3 mask (NTRS document had 077774000, but that would make TB6D class 3, which is not the case)
	static const unsigned MSKSSWV   = 0003000000U; //Water valve mask
	//DVASW

	//Class 1
	static const unsigned MSKSSS4C0 = 0400000000U; //S-IVB cutoff
	static const unsigned MSKSSSPEC = 0200000000U; //S-IVB pump purge
	static const unsigned MSKSSTB6C = 0100000000U; //TB6C alternate sequence
	//Class 3
	static const unsigned MSKSSGNSS = 0040000000U; //Generalized switch selector
	static const unsigned MSKSSSBLO = 0020000000U; //S-Band antenna to low
	static const unsigned MSKSSSBHI = 0010000000U; //S-Band antenna to high
	static const unsigned MSKSSSBOM = 0004000000U; //S-Band antenna to omni
	static const unsigned MSKSSECSV = 0002000000U; //ECS water valve close
	static const unsigned MSKSSECS1 = 0001000000U; //ECS water valve open
	static const unsigned MSKSST3A  = 0000400000U; //S-II MRS sequence
	static const unsigned MSKSSTB6D = 0000200000U; //TB6D alternate sequence
	static const unsigned MSKSSTB5A = 0000100000U; //TB5A alternate sequence (Apollo 8 only)
	//5 spare

	//Class 2
	static const unsigned MSKSSTB6A = 0000002000U; //TB6A alternate sequence
	static const unsigned MSKSSTB6B = 0000001000U; //TB6B alternate sequence
	static const unsigned MSKSSS4C1 = 0000000400U; //S-IVB special cutoff
	//5 spare

	//Class 4
	static const unsigned MSKSSACQU = 0000000004U;
	static const unsigned MSKSSLI   = 0000000002U;
	//1 spare

	//VASPI
	static const unsigned MSKSSCL3  = 0100000000U;
	static const unsigned MSKSSCL1  = 0040000000U;
	static const unsigned MSKSST6C  = 0004000000U;

	friend class MCC;
	friend class ApolloRTCCMFD;
	friend class RTCC;
	friend class ARCore;
};

/* ********************
 * LVDC++ S1B VERSION *
 ******************** */

class LVDC1B: public LVDC {
public:
	LVDC1B(LVDA &lvd);										// Constructor
	void Init();
	void TimeStep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	void ReadFlightSequenceProgram(char *fspfile);

	void SwitchSelectorProcessing(const std::vector<SwitchSelectorSet> &table);
	bool SwitchSelectorSequenceComplete(std::vector<SwitchSelectorSet> &table);

	bool GetGuidanceReferenceFailure() { return ModeCode27[MC27_GRFDiscretesSet]; }

	VECTOR3 SVCompare();

	//DCS Commands
	bool TimebaseUpdate(double dt);
	bool GeneralizedSwitchSelector(int stage, int channel);
	bool LMAbort();
	bool InhibitAttitudeManeuver();
	bool LaunchTargetingUpdate(double v_t, double r_t, double theta_t, double inc, double dsc, double dsc_dot, double t_grr0);
	bool DiscreteOutputTest(int bit, bool on);
	bool NavigationUpdate(VECTOR3 DCSRVEC, VECTOR3 DCSVVEC, double DCSNUPTIM);

	//Public Variables
	double Azimuth;									// Azimuth
	double PHI;										// Geodetic latitude of launch site
	double T_GRR;									// Time of GRR in seconds since midnight
private:

	VECTOR3 GravitationSubroutine(VECTOR3 Rvec, bool J2only);
	VECTOR3 DragSubroutine(VECTOR3 Rvec, VECTOR3 Vvec);

	bool Initialized;								// Clobberness flag
	char FSPFileName[256];

	bool LVDC_Stop;									// Program Stop Flag
	int LVDC_Timebase;								// Time Base
	double LVDC_TB_ETime;                           // Time elapsed since timebase start
	int IGMCycle;									// IGM Cycle Counter (for debugging)

	// These are boolean flags that are NOT real flags in the LVDC SOFTWARE. (I.E. Hardware flags)
	bool LVDC_GRR;                                  // Guidance Reference Released
	bool CountPIPA;									// PIPA Counter Enable
	
	// These are variables that are not really part of the LVDC software.
	VECTOR3 AttitudeError;                          // Attitude Error
	VECTOR3 DeltaAtt;
	VECTOR3 AttitudeErrorOld;
	int CommandSequence;

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
	int OrbitalGuidanceCycle;						// Number of minor loops since since last guidance run during orbital flight
	double T_1;										// Time left in first-stage IGM
	double T_2;										// Time left in second and fourth stage IGM
	double t_D;										// Time of launch after reference launch time (time of launch after window opens)

	// These are boolean flags that are real flags in the LVDC SOFTWARE.
	bool GRR_init;									// GRR initialization done
	bool poweredflight;								// Powered flight flag
	bool S1B_Engine_Out;							// S1C Engine Failure Flag
	bool S1B_CECO_Commanded;
	bool HSL;										// High-Speed Loop flag
	int  T_EO1,T_EO2;								// Pre-IGM Engine-Out Constant
	int  UP;										// IGM target parameters updated
	bool BOOST;										// Boost To Orbit
	bool S4B_IGN;									// SIVB Ignition
	bool GATE;										// Logic gate for switching IGM steering
	bool GATE5;										// Logic gate that ensures only one pass through cutoff initialization
	bool TerminalConditions;						// Use preset terminal conditions (R_T, V_T, gamma_T and G_T) for into-orbit targeting
	std::bitset<26> ModeCode24, ModeCode25, ModeCode27;
	int MinorLoopCounter;
	int MinorLoopCycles;

	// LVDC software variables, PAD-LOADED BUT NOT NECESSARILY CONSTANT!
	double A_zL;									// Position I Azimuth
	double B_11,B_21;								// Coefficients for determining freeze time after S1C engine failure
	double B_12,B_22;								// Coefficients for determining freeze time after S1C engine failure	
	double V_ex1,V_ex2;								// IGM Exhaust Velocities
	double V_TC;									// Velocity parameter used in high-speed cutoff
	double tau1;									// Time to consume all fuel before S4 MRS
	double tau2;									// Time to consume all fuel between MRS and S2 Cutoff
	double Fm;										// Sensed acceleration
	double LVIMUMJD;
	double DTTEMP;
	double Tt_T;									// Time-To-Go computed using Tt_3
	double Tt_2;									// Estimated second stage burn time
	double eps_2;									// Guidance option selection time
	double eps_3;									// Terminal condition freeze time
	double eps_4;									// Time for cutoff logic entry
	double ROV;										// Constant for biasing terminal-range-angle
	double mu;										// Product of G and Earth's mass
	double PHIP;									// Geocentric latitude of launch site
	double KSCLNG;									// Longitude of the launch site
	double R_L;										// Radius from geocentric center of the Earth to the center of the IU on launch pad
	double omega_E;									// Rotational rate of the Earth
	double dotM_1;									// Mass flowrate of S2 from approximately LET jettison to second MRS
	double dotM_2;									// Mass flowrate of S2 after second MRS
	double t_B1;									// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	double t_B3;									// Time from second S2 MRS signal
	double t;										// Time from accelerometer reading to next steering command
	double SMCG;									// Steering misalignment correction gain
	double TSMC1,TSMC2;								// Time test for steering misalignment test relative to TB3,TB4
	double T_L_apo;									// Predicted time of liftoff (in GMT) in seconds
	double Lambda_0;								// Value of DescNodeAngle valid for a liftoff at launch window opening
	double lambda_dot;								// Time rate of change of Lambda_0
	double T_GRR0;									// Nominal value of T_GRR
	double DT_N1, DT_N2, DT_N3, DT_N4, DT_N5, DT_N6;// Nominal DT for various mission phases
	double T_SON;									// Time since last orbital navigation pass
	
	// PAD-LOADED TABLES
	double Fx[5][5];								// Pre-IGM pitch polynomial
	double Ax[4];									// Variable azimuth polynomial

	// LVDC software variables, NOT PAD-LOADED
	double Inclination;								// Inclination
	double DescNodeAngle;							// Descending Node Angle -- THETA_N
	VECTOR3 CommandedAttitude;						// Commanded Attitude (RADIANS)
	VECTOR3 PCommandedAttitude;						// Previous Commanded Attitude (RADIANS)
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
	MATRIX3 MX_A;									// Transform matrix from earth-centered plumbline to equatorial (EDD name MSG)
	MATRIX3 MX_B;									// Transform matrix from equatorial to orbital coordinates (EDD name MG4)
	MATRIX3 MX_G;									// Transform matrix from earth-centered plumbline to orbital (EDD name MS4)
	MATRIX3 MX_K;									// Transform matrix from earth-centered plumbline to terminal (EDD name MSV)
	MATRIX3 MX_phi_T;								// Matrix made from phi_T (EDD name M4V)
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
	double H;										// coefficient for third zonal gravity harmonic
	double D;										// coefficient for fourth zonal gravity harmonic
	double CG;
	double alpha_D;									// Angle from perigee to DN vector
	bool alpha_D_op;								// Option to determine alpha_D or load it
	double G_T;										// Magnitude of desired terminal gravitational acceleration
	double xi_T;									// Desired position component in the terminal reference system
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
	double DT_B;									// Time bias to compensate for S-IVB engine thrust decay after cutoff
	double TAS;										// Time from GRR
	double t_clock;									// Time from liftoff
	double X_Zi,X_Yi;								// Generated Pitch and Yaw Command
	double sin_chi_Yit;
	double cos_chi_Yit;
	double sin_chi_Zit;
	double cos_chi_Zit;
	double DT_N;									// Nominal value of DT
	double MLR;										// Minor loop rate
	double MS25DT;									// Number of minor loops per DT_N through minor loop support
	double MS04DT;									// The reciprocal of MS25DT
	double MSK5;									// Ladder rate limit (all channels)
	double MSK6;									// Ladder magnitude limit (pitch and yaw channels)
	double MSK16;									// Ladder magnitude limit (roll channel)
	double MSLIM1;									// Attitude command (chi) rate limit (roll channel)
	double MSLIM2;									// Attitude command (chi) rate limit (pitch and yaw channel)
	VECTOR3 DChi;									// Computed change Chi_apo (CommandedAttitude) for present major loop
	VECTOR3 DChi_apo;								// Computed Chi guidance command increment for minor loop
	double Chi_xp_apo;								// Predicted value of Chi_x_apo at end of present major loop
	double Chi_zp_apo;								// Predicted value of Chi_z_apo at end of present major loop
	double theta_xa;								// Average value of theta_x during next major loop pass
	double theta_za;								// Average value of theta_z during next major loop pass
	VECTOR3 R_OG;									// Radius vector for use in orbital guidance
	VECTOR3 RTEMP1, VTEMP1, ATEMP1, RPT, VPT, APT, DRT, DVT, ddotS;
	double R4;
	double NUPTIM;									// Time from GRR at which a DCS Navigation Update is to be implemented
	VECTOR3 Pos_Nav, Vel_Nav;

	//ORBITAL GUIDANCE PROGRAM
	double SP_M[10];								// In-plane attitude parameters of orbital guidance maneuvers
	double CP_M[10];
	double SY_M[10];
	double CY_M[10];
	double RA_M[10];
	int TYP_M[10];									// +1 = inertial reference, 0 = chi freeze, -1 = local reference, -2 = inertial hold of local reference
	double t_Maneuver[10];							// Time of orbital attitude maneuver. Positive for TB4 referenced, negative for time since GRR
	int OrbitManeuverCounter;
	double SPITCH, CPITCH, SYAW, CYAW, ROLLA;
	double Y_ref, Z_ref, X_ref;
	//Parameter used by execute generalized maneuver, execute special maneuver, and return to nominal timeline
	//DCS commands, indicating orbital maneuver type.
	std::bitset<5> GOMTYP;
	double T_SOM;

	//Switch Selector Tables
	std::vector<SwitchSelectorSet> SSTTB[5];	// [1...4] 0 never used!
	std::vector<SwitchSelectorSet> SSTALT1;

	// TABLE25 is apparently only used on direct-ascent

	//Mode Code Bits
	enum ModeCode25_Bits
	{
		MC25_TLC_Memory_Error = 0,
		MC25_TB4Begin,
		MC25_SIVBCutoff1Issued,
		MC25_TerminalGuidance,
		MC25_SIVBEngineStartOnIssued = 5,
		MC25_ManualSIVBCutoffInitiation = 7,
		MC25_SIVBIGMAfterEMRC = 12,
		MC25_SIVBIGMBeforeEMRC,
		MC25_SMCActive = 15,
		MC25_SIBOutboardEngineOut,
		MC25_SIBInboardEngineOut,
		MC25_BeginTB3,
		MC25_BeginTB2,
		MC25_SIBTimeTiltPitchFreeze,
		MC25_RollManeuverComplete = 22,
		MC25_InitiateSIBPitchRollGuidance,
		MC25_BeginTB1,
		MC25_BeginTB0
	};

	enum ModeCode27_Bits
	{
		MC27_DCSSpecialManeuverAImplemented = 4,
		MC27_DCSSpecialManeuverBImplemented,
		MC27_DCSExecuteGeneralizedManeuverAccepted,
		MC27_DCSWaterControlValeLogicInhibitAccepted,
		MC27_InertialAttHoldManeuverAfterSCControl,
		MC27_TrackLocalReferenceAfterSCControl,
		MC27_SCControlAfterGRF,
		MC27_GRFDiscretesSet,
		MC27_SCInControl = 13,
		MC27_InertialAttHoldInProgress,
		MC27_LocalReferenceManeuverInProgress,
		MC27_DCSTimeBaseUpdateAccepted,
		MC27_DCSNavigationUpdateAccepted,
		MC27_DCSPoweredFlightInhibitRemoved = 25
	};
};

#define LVDC_START_STRING "LVDC_BEGIN"
#define LVDC_END_STRING "LVDC_END"
