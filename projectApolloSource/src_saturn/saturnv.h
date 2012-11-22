/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  The actual SaturnV class.

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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.5  2012/11/04 13:33:13  meik84
  *	LVDC++
  *	
  *	Revision 1.4  2012/03/24 19:14:26  tschachim
  *	Moved LVDC++ to PostStep
  *	
  *	Revision 1.3  2010/02/22 14:23:31  tschachim
  *	Apollo 7 S-IVB on orbit attitude control, venting and Saturn takeover mode for the VAGC.
  *	
  *	Revision 1.2  2009/07/04 21:36:09  dseagrav
  *	LVDC++ update
  *	Should not break anything else
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.39  2008/04/11 12:19:18  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.38  2008/01/16 04:14:24  movieman523
  *	Rewrote docking probe separation code and moved the CSM_LEM code into a single function in the Saturn class.
  *	
  *	Revision 1.37  2008/01/14 04:31:11  movieman523
  *	Initial tidyup: ASTP should now work too.
  *	
  *	Revision 1.36  2007/12/04 20:26:36  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.35  2007/09/04 13:51:56  dseagrav
  *	IGM first stage. It doesn't work yet.
  *	
  *	Revision 1.34  2007/08/11 16:31:25  jasonims
  *	Changed name of BuildFirstStage to ChangeSatVBuildState  to clarify function...
  *	A little more Capcom...
  *	
  *	Revision 1.33  2007/06/23 21:20:38  dseagrav
  *	LVDC++ Update: Now with Pre-IGM guidance
  *	
  *	Revision 1.32  2007/06/06 15:02:19  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.31  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.30  2006/07/27 23:24:11  tschachim
  *	The Saturn 1b now has the Saturn V IGM autopilot.
  *	
  *	Revision 1.29  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.28  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.27  2006/05/27 11:50:04  movieman523
  *	Improved INT20 support, and made LET jettison work any time during launch on Saturn V.
  *	
  *	Revision 1.26  2006/05/27 00:54:28  movieman523
  *	Simplified Saturn V mesh code a lot, and added beginnings ot INT-20.
  *	
  *	Revision 1.25  2006/05/26 22:01:50  movieman523
  *	Revised stage handling some. Removed two of the three second-stage functions and split out the mesh and engine code.
  *	
  *	Revision 1.24  2006/04/25 13:40:22  tschachim
  *	New KSC.
  *	
  *	Revision 1.23  2006/02/21 12:02:26  tschachim
  *	Moved TLI sequence to the IU.
  *	
  *	Revision 1.22  2006/01/31 01:26:18  lazyd
  *	Autopilot change
  *	
  *	Revision 1.21  2006/01/26 03:31:57  movieman523
  *	Less hacky low-res mesh support for Saturn V.
  *	
  *	Revision 1.20  2006/01/24 13:46:30  tschachim
  *	Smoother staging with more eye-candy.
  *	
  *	Revision 1.19  2006/01/15 02:38:59  movieman523
  *	Moved CoG and removed phantom thrusters. Also delete launch site when we get a reasonable distance away.
  *	
  *	Revision 1.18  2006/01/12 14:48:12  tschachim
  *	Added prelaunch tank venting.
  *	
  *	Revision 1.17  2005/12/13 10:13:56  tschachim
  *	Fixed SIVB separation offset (hopefully).
  *	
  *	Revision 1.16  2005/11/25 02:03:47  movieman523
  *	Fixed mixture-ratio change code and made it more realistic.
  *	
  *	Revision 1.15  2005/11/24 20:31:23  movieman523
  *	Added support for engine thrust decay during launch.
  *	
  *	Revision 1.14  2005/11/23 01:43:13  movieman523
  *	Added SII stage DLL.
  *	
  *	Revision 1.13  2005/11/20 01:06:27  movieman523
  *	Saturn V now uses SIVB DLL too.
  *	
  *	Revision 1.12  2005/10/31 10:42:25  tschachim
  *	New VAB.
  *	
  *	Revision 1.11  2005/08/30 18:37:39  movieman523
  *	Added support for new interstage meshes.
  *	
  *	Revision 1.10  2005/08/17 00:01:59  movieman523
  *	Added ECS indicator switch, revised state saving, revised Timestep code to pass in the delta-time so we don't need to keep calculating it.
  *	
  *	Revision 1.9  2005/08/15 18:48:50  movieman523
  *	Moved the stage destroy code into a generic function for Saturn V and 1b.
  *	
  *	Revision 1.8  2005/08/05 13:07:06  tschachim
  *	Added crawler callback function LaunchVesselRolloutEnd,
  *	added keyboard handling
  *	
  *	Revision 1.7  2005/08/01 19:07:47  movieman523
  *	Genericised code to deal with SM destruction on re-entry, and did some tidying up of Saturn 1b code.
  *	
  *	Revision 1.6  2005/07/31 01:43:13  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.5  2005/07/30 02:05:55  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.4  2005/04/22 13:55:31  tschachim
  *	Added SecoSound otherwise I can't build
  *	
  *	Revision 1.3  2005/03/09 00:25:28  chode99
  *	Added header stuff for SII retros.
  *	
  *	Revision 1.2  2005/02/18 00:38:44  movieman523
  *	Moved sounds into generic Saturn class from Saturn V.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

///
/// \brief Saturn V launch vehicle class.
/// \ingroup Saturns
///
class SaturnV: public Saturn {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	SaturnV (OBJHANDLE hObj, int fmodel);
	virtual ~SaturnV();

	//
	// General functions that handle calls from Orbiter.
	//

	void Timestep(double simt, double simdt, double mjd);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPostStep (double simt, double simdt, double mjd);

	///
	/// \brief Orbiter state loading function.
	/// \param scn Scenario file to load from.
	/// \param status Pointer to current vessel status.
	///
	void clbkLoadStateEx (FILEHANDLE scn, void *status);

	///
	/// \brief Orbiter class configuration function.
	/// \param cfg File to load configuration defaults from.
	///
	void clbkSetClassCaps (FILEHANDLE cfg);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);

	void initSaturnV();

	// called by crawler 
	void LaunchVehicleRolloutEnd();		// after arrival on launch pad
	void LaunchVehicleBuild();			// build/unbuild during assembly
	void LaunchVehicleUnbuild();

	///
	/// \brief Set up J2 engines as fuel venting thruster.
	/// \todo Not implemented yet, see Saturn1b
	virtual void SetVentingJ2Thruster() {};

	//
	// Functions that external code shouldn't need to access.
	//

private:

	void ChangeSatVBuildState (int bstate);
	void SetFirstStage ();
	void SetFirstStageEngines ();
	void SetSecondStage ();
	void SetSecondStageMesh (double offset);
	void SetSecondStageEngines (double offset);
	void SetThirdStage ();
	void SetThirdStageMesh (double offset);
	void SetThirdStageEngines (double offset);
	MESHHANDLE GetInterstageMesh();
	void AttitudeLaunch1();
	void AttitudeLaunch2();
	void AttitudeLaunch4();
	void AutoPilot(double autoT);
	void SetSIICMixtureRatio (double ratio);
	void SetSIVbCMixtureRatio(double ratio);
	void MoveEVA();

	void SeparateStage (int stage);

	void SetVehicleStats();

	///
	/// Calculate the mass of the various stages of the vehicle once we know
	/// what type it is and what state it's in.
	/// \brief Calculate stage masses.
	///
	void CalculateStageMass ();

	void SetupMeshes();

protected:

	//
	// Mission stage functions.
	//

	void StageOne(double simt, double simdt);
	void StageTwo(double simt);
	void StageFour(double simt, double simdt);
	void StageLaunchSIVB(double simt);

	void ConfigureStageMeshes(int stage_state);
	void ConfigureStageEngines(int stage_state);
	void CreateStageOne();

	//
	// Mission-support functions.
	//

	void SaveVehicleStats(FILEHANDLE scn);

	//
	// Odds and ends.
	//

	void DoFirstTimestep(double simt);
	void ActivatePrelaunchVenting();
	void DeactivatePrelaunchVenting();
	void ActivateStagingVent();
	void DeactivateStagingVent();

	//
	// Class variables.
	//

	bool GoHover;

	//
	// End state.
	//

	///
	/// \brief Number of retro rockets on stage one.
	///
	int SI_RetroNum;

	///
	/// \brief Number of retro rockets on stage two.
	///
	int SII_RetroNum;

	///
	/// \brief Number of ullage rockets on stage two.
	///
	int SII_UllageNum;

	//
	// Sounds.
	//

	Sound DockS;
	Sound SpeedS;
	Sound S5P100;
	Sound SRover;
	Sound SecoSound;
	
	// DS20070205 LVDC++
	// This code is here temporarily for the development of the LVDC++.
	// Right now it only affects the Saturn V, so it's here. Later as it improves it can be applied to the S1B
	// and moved to somewhere more appropriate.
	// If it's here, I don't have to wait for everything saturn-derived to rebuild if I modify it.

	FILE* lvlog;									// LV Log file

	int LVDC_Timebase;								// Time Base
	double LVDC_TB_ETime;                           // Time elapsed since timebase start

	int LVDC_GP_PC;									// Guidance Program: Program Counter
	double S1C_Sep_Time;							// S1C Separation Counter

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
	double Velocity[3];								// Velocity
	double Position[3];								// Position
	VECTOR3 WV;										// Gravity
	double sinceLastIGM;							// Time since last IGM run
	double IGMInterval;								// IGM Interval
	int IGMCycle;									// IGM Cycle Counter (for debugging)

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
//	bool Azimuth_Inclination_Mode;					// Ground Targeting uses Azimuth to determine Inclination
//	bool Azimuth_DscNodAngle_Mode;					// Ground Targeting uses Azimuth to determine Descending Nodal Angle
	bool init;										// GRR initialization done
	bool poweredflight;								// Powered flight flag
	bool liftoff;									// lift-off flag
	bool Direct_Ascent;                             // Direct Ascent Mode flag
	bool S1C_Engine_Out;							// S1C Engine Failure Flag
	bool directstageint;							// Direct Stage Interrupt
	bool HSL;										// High-Speed Loop flag
	int  T_EO1,T_EO2;								// Pre-IGM Engine-Out Constant
	bool ROT;										// Rotate terminal conditions
	int  UP;										// IGM target parameters updated
	bool BOOST;										// Boost To Orbit
	bool S2_IGNITION;								// SII Ignition flag
	bool S2_ENGINE_OUT;								// SII Engine out flag
	bool S4B_IGN;									// SIVB Ignition
	bool S2_BURNOUT;								// SII Burn Out
	bool MRS;										// MR Shift
	bool GATE;										// Logic gate for switching IGM steering
	bool GATE0;										// Permit entry to restart preparation
	bool GATE1;										// Permit entry to out-of-orbit targeting
	bool GATE2;										// Permit only one pass through 1st-opportunity targeting
	bool GATE3;										// Permit entry to IGM out-of-orbit targeting
	bool GATE4;										// Permit only one pass through direct-staging guidance update
	bool GATE5;										// Logic gate that ensures only one pass through cutoff initialization
	bool INH,INH1,INH2;								// Dunno yet
	bool TU;										// Gate for processing targeting update
	bool TU10;										// Gate for processing ten-paramemter targeting update
	bool first_op;									// switch for first TLI opportunity

	// LVDC software variables, PAD-LOADED BUT NOT NECESSARILY CONSTANT!
	VECTOR3 XLunarAttitude;							// Attitude the SIVB enters when TLI is done, i.e. at start of TB7
	double IncFromAzPoly[6];						// Inclination-From-Azimuth polynomial
	double IncFromTimePoly[6];                      // Inclination-From-Time polynomial
	double DNAFromAzPoly[6];						// Descending Nodal Angle from Azimuth polynomial
	double DNAFromTimePoly[6];						// Descending Nodal Angle from Time polynomial
	double B_11,B_21;								// Coefficients for determining freeze time after S1C engine failure
	double B_12,B_22;								// Coefficients for determining freeze time after S1C engine failure	
	double V_ex1,V_ex2,V_ex3;						// IGM Exhaust Velocities
	double V_TC;									// Velocity parameter used in high-speed cutoff
	double tau1;									// Time to consume all fuel before S2 MRS
	double tau2;									// Time to consume all fuel between MRS and S2 Cutoff
	double tau3;									// Time to consume all fuel of SIVB
	double tau3N;									// Artificial tau mode parameters
	double Fm;										// Sensed acceleration
	double Tt_T;									// Time-To-Go computed using Tt_3
	double Tt_3;									// Estimated third or fifth stage burn time
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
	double T_1c;									// Burn time of IGM first, second, and coast guidance stages
	double eps_1;									// IGM range angle calculation selection
	double eps_2;									// Guidance option selection time
	double eps_3;									// Terminal condition freeze time
	double eps_4;									// Time for cutoff logic entry
	double ROV,ROVs;								// Constant for biasing terminal-range-angle
	double mu;										// Product of G and Earth's mass
	double phi_L;								// Geodetic latitude of launch site: cos
	double dotM_1;									// Mass flowrate of S2 from approximately LET jettison to second MRS
	double dotM_2;									// Mass flowrate of S2 after second MRS
	double dotM_3;									// Mass flowrate of S4B during first burn
	double t_B1;									// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	double t_B3;									// Time from second S2 MRS signal
	double t;										// Time from accelerometer reading to next steering command
	double TA1,TA2;									// Time parameters used in orbital guidance
	double Ct;										// Time from start of 3rd or 4th stage IGM
	double Ct_o;									// Time from use of artifical tau mode
	double Cf;										// Constant used for S2/S4B direct staging
	double SMCG;									// Steering misalignment correction gain
	double TS4BS;									// Time from direct-stage interrupt to start IGM.
	double TSMC1,TSMC2;								// Time test for steering misalignment test relative to TB3,TB4
	double V_S2T;									// Nominal S2 cutoff velocity
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
	double dT_3;									// Correction to third or fifth stage burn time
	double dT_4;									// Difference between nominal and actual 1st S4B burn time
	double dTt_4;									// Limited value of above
	double T_T;										// Time-To-Go computed using T_3
	double tchi_y_last,tchi_p_last;							// Angles to null velocity deficiencies without regard to terminal data
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

	void lvdc_init();								// Initialization
	void lvdc_timestep(double simt, double simdt);	// LVDC timestep call
};

extern void LoadSat5Meshes();


const double STG0O = 20.4;
const double STG1O = -5.25;
const VECTOR3 OFS_STAGE1 =  { 0, 0, -54.0 + STG0O};
const VECTOR3 OFS_STAGE12 =  { 0, 0, -30.5 -STG1O};
const VECTOR3 OFS_STAGE2 =  { 0, 0, -17.2 - STG1O};
const VECTOR3 OFS_STAGE3 =  { 0, 0, 2. - STG2O};
const VECTOR3 OFS_STAGE31 =  { -1.48,-1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE32 =  { 1.48,-1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE33 =  { 1.48,1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE34 =  { -1.48,1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_SM = {0, 0, -2.05}; 
const VECTOR3 OFS_ABORT =  { 0,0,STG0O};
const VECTOR3 OFS_ABORT2 =  { 0,0,-STG1O};
const VECTOR3 OFS_CM_CONE =  { 0, 0, 36.05-12.25-21.5};
