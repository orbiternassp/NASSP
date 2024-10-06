/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC)

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

#if !defined(_PA_RTCC_H)
#define _PA_RTCC_H

#include <vector>
#include <deque>
#include <bitset>
#include "../src_sys/yaAGC/agc_engine.h"
#include "../src_lm/yaAGS/aea_engine.h"
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/LDPP.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "../src_rtccmfd/RTCCTables.h"
#include "../src_rtccmfd/TLMCC.h"
#include "../src_rtccmfd/TLIProcessor.h"
#include "../src_rtccmfd/LOITargeting.h"
#include "../src_rtccmfd/LMGuidanceSim.h"
#include "../src_rtccmfd/CoastNumericalIntegrator.h"
#include "../src_rtccmfd/EnckeIntegrator.h"
#include "../src_rtccmfd/RTCC_EMSMISS.h"
#include "../src_rtccmfd/RTCCSystemParameters.h"
#include "../src_rtccmfd/GeneralPurposeManeuver.h"
#include "../src_rtccmfd/LWP.h"
#include "../src_rtccmfd/AnalyticEphemerisGenerator.h"
#include "../src_rtccmfd/RTCCDisplayFormatting.h"
#include "MCCPADForms.h"

class Saturn;

#define RTCC_START_STRING	"RTCC_BEGIN"
#define RTCC_END_STRING	    "RTCC_END"

#define RTCC_LAMBERT_MULTIAXIS 0
#define RTCC_LAMBERT_XAXIS 1

#define RTCC_LAMBERT_SPHERICAL 0
#define RTCC_LAMBERT_PERTURBED 1

#define RTCC_MPT_CSM 1
#define RTCC_MPT_SIVB 2
#define RTCC_MPT_LM 3

//CSM present
#define RTCC_CONFIG_C 0
//S-IVB present
#define RTCC_CONFIG_S 1
//LM ascent stage present
#define RTCC_CONFIG_A 2
//LM descent stage present
#define RTCC_CONFIG_D 3

#define RTCC_CONFIGCHANGE_NONE 0
#define RTCC_CONFIGCHANGE_UNDOCKING 1
#define RTCC_CONFIGCHANGE_DOCKING 2

#define RTCC_ENGINETYPE_CSMRCSPLUS2 1
#define RTCC_ENGINETYPE_CSMRCSPLUS4 2
#define RTCC_ENGINETYPE_CSMRCSMINUS2 3
#define RTCC_ENGINETYPE_CSMRCSMINUS4 4
#define RTCC_ENGINETYPE_LOX_DUMP 16
#define RTCC_ENGINETYPE_LMRCSPLUS2 17
#define RTCC_ENGINETYPE_LMRCSPLUS4 18
#define RTCC_ENGINETYPE_LMRCSMINUS2 19
#define RTCC_ENGINETYPE_LMRCSMINUS4 20
#define RTCC_ENGINETYPE_CSMSPS 33
#define RTCC_ENGINETYPE_LMAPS 34
#define RTCC_ENGINETYPE_LMDPS 35
#define RTCC_ENGINETYPE_SIVB_MAIN 36

#define RTCC_MANVEHICLE_CSM 1
#define RTCC_MANVEHICLE_SIVB 2
#define RTCC_MANVEHICLE_LM 3

#define RTCC_ATTITUDE_INERTIAL 1
#define RTCC_ATTITUDE_MANUAL 2
#define RTCC_ATTITUDE_LAMBERT 3
#define RTCC_ATTITUDE_PGNS_EXDV 4
#define RTCC_ATTITUDE_AGS_EXDV 5
#define RTCC_ATTITUDE_SIVB_IGM 6
#define RTCC_ATTITUDE_PGNS_DESCENT 7
#define RTCC_ATTITUDE_PGNS_ASCENT 8
#define RTCC_ATTITUDE_AGS_ASCENT 9

#define RTCC_REFSMMAT_TYPE_CUR 1
#define RTCC_REFSMMAT_TYPE_PCR 2
#define RTCC_REFSMMAT_TYPE_TLM 3
#define RTCC_REFSMMAT_TYPE_OST 4
#define RTCC_REFSMMAT_TYPE_MED 5
#define RTCC_REFSMMAT_TYPE_DMT 6
#define RTCC_REFSMMAT_TYPE_DOD 7
#define RTCC_REFSMMAT_TYPE_LCV 8
#define RTCC_REFSMMAT_TYPE_AGS 9
#define RTCC_REFSMMAT_TYPE_DOK 10
#define RTCC_REFSMMAT_TYPE_LLA 11
#define RTCC_REFSMMAT_TYPE_LLD 12

#define RTCC_LMPOS_BEST 0
#define RTCC_LMPOS_PGNCS 1
#define RTCC_LMPOS_AGS 2
#define RTCC_LMPOS_MED 3

#define RTCC_COORDINATES_ECI 0
#define RTCC_COORDINATES_ECT 1
#define RTCC_COORDINATES_MCI 2
#define RTCC_COORDINATES_MCT 3
#define RTCC_COORDINATES_EMP 4

//MANUAL ENTRY DEVICES

//Computation for Lunar Descent Planning
struct MED_K16
{
	double VectorTime = 0.0;
	int Mode = 1;
	int Sequence = 1;
	double GETTH1 = 0.0;
	double GETTH2 = 0.0;
	double GETTH3 = 0.0;
	double GETTH4 = 0.0;
	double DesiredHeight = 60.0*1852.0;
	int Vehicle = RTCC_MPT_CSM; //1 = CSM, 3 = LEM (Instead of Vector ID)
};

//Fuel Remaining
struct MED_M49
{
	int Table = 1;
	double SPSFuelRemaining = -1;
	double CSMRCSFuelRemaining = -1;
	double SIVBFuelRemaining = -1;
	double LMAPSFuelRemaining = -1;
	double LMRCSFuelRemaining = -1;
	double LMDPSFuelRemaining = -1;
};

//Change Vehicle Weight
struct MED_M50
{
	int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
	double CSMWT = -1.0;
	double SIVBWT = -1.0;
	double LMWT = -1.0;
	double LMASCWT = -1.0;
	double WeightGET = 0.0;
};

//Change Vehicle Area or K-Factor
struct MED_M51
{
	int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
	double CSMArea = 129.4*0.3048*0.3048;
	double SIVBArea = 365.0*0.3048*0.3048;
	double LMAscentArea = 200.57*0.3048*0.3048;
	double LMDescentArea = 200.57*0.3048*0.3048;
	double KFactor = 1.0;
};

//Input initial configuration for Mission Plan Table
struct MED_M55
{
	int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
	std::string ConfigCode;
	double VentingGET = -1.0;
	double DeltaDockingAngle = -720.0;
};

//TLI Direct Input
struct MED_M68
{
	int Table = 2; //1 = LEM, 2 = CSM
	int Opportunity = 1; //1-2
};

//Transfer a DKI, SPQ, or a Descent Plan to the MPT
struct MED_M70
{
	int Plan = 0; //-1 = Descent Plan, 0 = SPQ, 1 = DKI
	double DeleteGET = 0.0;
	int Thruster = RTCC_ENGINETYPE_CSMRCSPLUS2; //Thruster for the maneuver
	int Attitude = 4;		//Attitude option (1 = Inertial, 2 = Manual, 3 = Lambert, 4 = PGNS External DV, 5 = AGS External DV)
	double UllageDT = -1;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool Iteration = false; //false = do not iterate, true = iterate
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
	bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
};

//Transfer a Two Impulse Maneuver to the MPT
struct MED_M72
{
	int Table = 1; //1 = multiple solution, 2 = corrective solution
	int Plan = 1; // Plan number to be transferred
	double DeleteGET = 0.0; //Deletes all maneuvers in both tables occurring after the input GET (no delete if 0)
	int Thruster = RTCC_ENGINETYPE_CSMRCSPLUS4; //Thruster for the maneuver
	int Attitude = RTCC_ATTITUDE_PGNS_EXDV;		//Attitude option
	double UllageDT = 0.0;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool Iteration = false; //false = do not iterate, true = iterate
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
	bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
};

struct TwoImpulseOpt
{
	int mode;		//1 = Corrective Combination (NCC), 2 = Multiple Solution/Two-Impulse Computation (TPI), 3 = Single Solution, 4 = Transfer Plan, 5 = DKI/SPQ
	double T1;	//GMT of the maneuver
	double T2;	// GMT of the arrival
	EphemerisData sv_A;		//Chaser state vector
	EphemerisData sv_P;		//Target state vector
	int ChaserVehicle = 1;	//1 = CSM, 3 = LEM
	//0 = Time of both maneuvers fixed, 1 = time of first maneuver fixed, 2 = time of second maneuver fixed
	int IVFLAG = 0;
	double TimeStep = 10.0;
	double TimeRange = 0.0;

	//Corrective combination options
	//Minimum height difference
	double DH_min;
	//Maximum height difference
	double DH_max;
	double T2_min;
	double T2_max;
	//0 = use item 11 as time increment of second maneuver
	//1 = use item 11 as terminal phase slip time increment
	int CCReqInd;
	//Item 11: Time increment of second maneuver
	double dt_inc;
	double TPILimit;
	//Height increment
	double dh_inc;

	//Single solution options
	//1 = Multiple, 2 = Corrective Combination
	int SingSolTable = 1;
	//1 to 13
	int SingSolNum = 1;
	//false = 2 quads, true = 4 quads
	bool UllageQuads = true;
	//false = Target, true = Horizon
	bool LOSMode = false;
	double DeltaPitch = 0.0;
	double RelMoTimeStep = 0.0;

	//External request
	double DH = 0.0;
	double PhaseAngle = 0.0;
	double WT = 0.0;
	double Elev = 0.0;
};

struct LambertMan //Data for Lambert targeting
{
	int mode;		//0 = General, 1 = Corrective Combination (NCC), 2 = Two-Impulse Computation (TPI)
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	int N;		//number of revolutions
	int axis;	//Multi-axis or horizontal burn
	int Perturbation; //Spherical or non-spherical gravity
	VECTOR3 Offset = _V(0, 0, 0); //Offset vector
	SV sv_A;		//Chaser state vector
	SV sv_P;		//Target state vector
	int ChaserVehicle = 1;	//1 = CSM, 3 = LEM
	bool storesolns = false;

	//For mode 1 and 2
	double PhaseAngle = 0.0;
	double DH = 0.0;
	double ElevationAngle = 26.6*RAD;
	double TravelAngle = 130.0*RAD;
};

struct AP7ManPADOpt
{
	AP7ManPADOpt();

	double TIG;					// Time of Ignition
	VECTOR3 dV_LVLH;			// Delta V in LVLH coordinates
	int enginetype;				// Engine type used for the maneuver
	bool HeadsUp;				// Orientation during the maneuver
	MATRIX3 REFSMMAT;			// REFSMMAT during the maneuver
	double sxtstardtime;		// Time delay for the sextant star check (in case no star is available during the maneuver)
	double navcheckGET;			// Time for the navcheck. 0 = no nav check
	EphemerisData sv0;			// State vector as input
	PLAWDTOutput WeightsTable;	// Table with spacecraft weights
	bool UllageThrusterOpt;		// false = 2 thrusters, true = 4 thrusters
	double UllageDT;			// Ullage duration, set to 0.0 if no ullage
};

struct AP11ManPADOpt
{
	AP11ManPADOpt();

	double TIG;					// Time of Ignition (GET)
	VECTOR3 dV_LVLH;			// Delta V in LVLH coordinates
	int enginetype;				// Engine type used for the maneuver
	bool HeadsUp;				// Orientation during the maneuver
	MATRIX3 REFSMMAT;			// REFSMMAT during the maneuver
	double sxtstardtime;		// Time delay for the sextant star check (in case no star is available during the maneuver)
	EphemerisData RV_MCC;		// State vector as input
	PLAWDTOutput WeightsTable;	// Table with spacecraft weights
	bool UllageThrusterOpt;		// Ullage: false = 2 thrusters, true = 4 thrusters
	double UllageDT;			// Ullage duration, set to 0.0 if no ullage
};

struct AP11LMManPADOpt
{
	AP11LMManPADOpt();

	double TIG;					// Time of Ignition
	VECTOR3 dV_LVLH;			// Delta V in LVLH coordinates
	int enginetype;				// Engine type used for the maneuver
	bool HeadsUp;				// Orientation during the maneuver
	MATRIX3 REFSMMAT;			// REFSMMAT during the maneuver
	double sxtstardtime;		// time delay for the sextant star check (in case no star is available during the maneuver)
	EphemerisData RV_MCC;		// State vector as input
	PLAWDTOutput WeightsTable;	// Table with spacecraft weights
};

struct AP10CSIPADOpt
{
	AP10CSIPADOpt();

	EphemerisData sv0;			// State vector of active vehicle
	double t_CSI;				// Time of CSI (GET)
	double t_TPI;				// Time of TPI (GET)
	VECTOR3 dV_LVLH;			// DV of CSI maneuver (LVLH)
	MATRIX3 REFSMMAT;			// REFSMMAT during the maneuver
	int enginetype;				//Engine type used for the maneuver
	PLAWDTOutput WeightsTable;	// Table with spacecraft weights
};

struct AP7TPIPADOpt
{
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	EphemerisData sv_A;
	EphemerisData sv_P;
	double mass;
};

struct AP9LMTPIPADOpt
{
	EphemerisData sv_A; //Chaser state vector
	EphemerisData sv_P; //Target state vector
	double GMT_TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;	//REFSMMAT
};

struct AP9LMCDHPADOpt
{
	AP9LMCDHPADOpt();

	EphemerisData sv_A; //Chaser state vector
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;	//REFSMMAT
};

struct EarthEntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double TIGguess; //Initial estimate for the TIG
	double ReA = 0; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	bool nominal; //Calculates minimum DV deorbit or nominal 31.7° line deorbit
	int enginetype = RTCC_ENGINETYPE_CSMSPS;		//Engine type used for the maneuver
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool prediction = false;	//0 = Entry maneuver, 1 = Entry prediction
};

struct EntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double TIGguess; //Initial estimate for the TIG or baseline TIG for abort and MCC maneuvers
	int type; //Type of reentry maneuver (1 = ATP, 2 = TCUA, 3 = FCUA)
	double lng; //Longitude of the desired splashdown coordinates
	int enginetype;		//Engine type used for the maneuver
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	// relative range override
	double r_rbias = 1285.0;
	//Maximum DV
	double dv_max = 2804.0;
	double t_Z = 0.0;	//Estimate time of landing
	int ATPLine = 0;
};

struct EntryResults
{
	VECTOR3 dV_LVLH;
	double P30TIG;
	double latitude, longitude;
	double GET05G, GET400K, RTGO, VIO, ReA, Incl, FlybyAlt;
	int precision;
	SV sv_preburn;
	SV sv_postburn;
	bool solutionfound = false;
};

struct TwoImpulseResuls
{
	EphemerisData sv_tig;
	VECTOR3 dV;
	VECTOR3 dV2;
	VECTOR3 dV_LVLH;
	VECTOR3 dV_LVLH2;
	double t_TPI;
	double T1;
	double T2;
	bool SolutionFound;
};

struct SPQResults
{
	double t_CSI;
	double t_CDH;
	double t_TPI;
	double DH;
	VECTOR3 dV_CSI;
	VECTOR3 dV_CDH;
	SV sv_C[5];
	SV sv_C_apo[5];
	SV sv_T[5];
	int err = 0;
};

struct RTEMoonOpt
{
	VESSEL* vessel;			//Reentry vessel
	double TIGguess = 0.0;	//Initial estimate for the TIG
	double EntryLng;		//Entry longitude
	int returnspeed = -1;	//0 = slow return, 1 = normal return, 2 = fast return
	SV RV_MCC;				//State vector as input
	int RevsTillTEI = 0;	//Revolutions until TEI
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool entrylongmanual = true; //Targeting a landing zone or a manual landing longitude
	int enginetype = RTCC_ENGINETYPE_CSMSPS; //Engine type used for the maneuver
	double Inclination = 0.0;	//Specified return inclination (sign is azimuth option)
	// 12: PTP discrete option (not implemented yet)
	// 14: ATP discrete option
	// 16: UA discrete option
	// 22: PTP tradeoff display (not implemented yet)
	// 24: ATP tradeoff display (not implemented yet)
	// 32: PTP search option (not implemented yet)
	// 34: ATP search option
	// 36: UA search option
	int SMODE = 34;
	//Approximate landing time
	double t_zmin = 0;
	int ATPLine = 0;
};

struct TradeoffDataDisplay
{
	TradeoffData data[10][44];
	unsigned curves;
	unsigned NumInCurve[10];
};

struct TradeoffDataDisplayBuffer
{
	TradeoffDataDisplayBuffer();

	double xval[10][44];
	double yval[10][44];
	unsigned curves;
	unsigned NumInCurve[10];
	int XLabels[3];
	int YLabels[3];
	int TZDisplay[10];
	double TZxval[10];
	double TZyval[10];
	std::string XAxisName;
	std::string YAxisName;
	std::string Site;
};

struct REFSMMATOpt
{
	VESSEL* vessel; //vessel
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int REFSMMATopt; //REFSMMAT options: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = Attitude, 8 = LS during TLC
	double REFSMMATTime; //Time for the REFSMMAT calculation
	double LSLng; //longitude for the landing site REFSMMAT
	double LSLat; //latitude for the landing site REFSMMAT
	double LSAzi; //approach azimuth for the landing site REFSMMAT
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool HeadsUp = true; //Orientation during the maneuver
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	MATRIX3 PresentREFSMMAT;	//Present REFSMMAT (for option 9)
	VECTOR3 IMUAngles;			//Desired Attitude (for option 9)
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;				//State vector as input
};

struct AP7BLKOpt
{
	int n; //number of PAD entries
	std::vector<double> lng; //Splashdown longitudes
	std::vector<double> GETI; //Ignition times
	std::vector<std::string> area; //Splashdown areas
};

struct AP11BLKOpt
{
	int n; //number of PAD entries
	std::vector<double> lng; //Splashdown longitudes
	std::vector<double> GETI; //Ignition times
	std::vector<double> T_Z; //Estimated landing time
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct EarthEntryPADOpt
{
	double P30TIG; //Time of Ignition (deorbit maneuver)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (deorbit maneuver)
	MATRIX3 REFSMMAT;
	bool preburn; //
	double lat; //splashdown latitude
	double lng; //splashdown longitude
	SV sv0;
	int Thruster = RTCC_ENGINETYPE_CSMSPS;
	double InitialBank = 0.0;
	double GLevel = 0.2;
};

struct LunarEntryPADOpt
{
	double P30TIG; //Time of Ignition (MCC)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (MCC)
	MATRIX3 REFSMMAT;
	bool direct = true; //0 = with MCC, 1 = without
	double lat; //splashdown latitude
	double lng; //splashdown longitude
	SV sv0;
	bool SxtStarCheckAttitudeOpt = true; //true = sextant star attitude check at entry attitude, false = sextant star check at horizon check attitude
};

struct TLIPADOpt
{
	EphemerisData sv0; //vessel state vector
	MATRIX3 REFSMMAT;
	VECTOR3 SeparationAttitude; //LVLH IMU angles
	double ConfigMass;
	int InjOpp; //Injection opportunity (1 or 2)
	bool StudyAid = false; //false = Planned TLI, true = from TLI processor
};

struct P27Opt
{
	EphemerisData sv0; //vessel state vector
	double SVGET; //GET of the state vector
	double navcheckGET; //GET of the Nav Check
};

struct AGSSVOpt
{
	EphemerisData sv;
	MATRIX3 REFSMMAT;
	bool csm;
	bool landed = false;
};

struct LunarLiftoffTimeOpt
{
	LunarLiftoffTimeOpt();

	//Flag that controls at which time CSI is done
	//0: CSI is done 90° from insertion
	//1: CSI is done at an input elapsed time from insertion
	//2: CSI is done at LM apocynthion
	int I_BURN;
	//Flag that controls at what position TPI is done
	//1: TPI is done over an input longitude
	//2: TPI is at an input time
	int I_TPI;
	//Flag that controls CDH position
	//0: CDH is done at the upcoming apsis after CSI
	//1, 3, 5, etc., CDH is done I_CDH*period/2 after CSI
	int I_CDH;
	//Time base for CSI if CSM is active vehicle and CSI was done on an apsis
	double t_BASE;
	//Flag that controls the search option for the absolute opening and closing of the launch window
	//0: compute the launch window for only the input DH table
	//1: compute the complete window
	int I_SRCH;
	//Flag that controls position for final maneuver
	//0: Compute final maneuver for a rendezvous solution
	//1: compute final maneuver for an offset in height and phase
	int I_OS;
	//Number of maneuvering vehicle
	//1: CSM is maneuvering vehicle
	//2: LM is maneuvering vehicle
	int M;
	//Number of non-maneuvering vehicle
	//1: CSM is non-maneuvering vehicle
	//2: LM is non-maneuvering vehicle
	int P;
	//Number of points used in the curve fit
	int N_CURV;
	//Number of entries in the DH table
	int L_DH;
	//Maximum in-orbit lifetime of the LM ascent stage pwoer systems
	double t_max;
	//Minimum safe height of an orbiting vehicle
	double H_S;
	//Rendezvous DV budget for both vehicles
	double DV_MAX[2];
	//Input table of coelliptic altitude differences
	double  DH[3];
	double theta_1;		//Angle travelled between liftoff and insertion
	double v_LH;
	double dt_1;		//Ascent burn time (liftoff to insertion)
	double v_LV;
	//Height at insertion
	double h_BO;
	//Angle from TPI to the earth-moon line
	double phi_TPI;
	//Yaw steer capability of LM
	double Y_S;
	//Time to travel from insertion to CSI
	double DT_B;
	//Height difference to begin curve fit with
	double DH_SRCH;
	//Height difference to increment curve fit with
	double DH_STEP;
	//Transfer angle for terminal phase
	double theta_F;
	//Elevation angle to initiate terminal phase on
	double E;
	//Offset height difference
	double DH_OFF;
	//Phase angle offset
	double dTheta_OFF;
	//Threshold time to determine which launch window is obtained
	double t_hole;
	//Time of TPI
	double t_TPI;
	double lat;
	double lng;
	double R_LLS;
	//Longitude at which TPI is to be scheduled
	double lng_TPI;
	SV sv_CSM;			//CSM State vector
};

struct LLTPOpt
{
	EphemerisData sv_CSM;
	double Y_S;
	double V_Z_NOM;
	double T_TH;
	double R_LS, lat_LS, lng_LS;
	double alpha_PF, dt_PF;
	double dt_INS_TPI;
	double h_INS;
	double DH_TPI, dTheta_TPI;
	double WT;
};

struct LunarLaunchTargetingTable
{
	double GETLOR = 0.0;
	double VH = 0.0;
	double GET_TPI = 0.0, GET_TPF = 0.0;
	double DV_TPI = 0.0, DV_TPF = 0.0;
	VECTOR3 DV_TPI_LVLH = _V(0, 0, 0), DV_TPF_LVLH = _V(0, 0, 0);
	double HA_TPI = 0.0, HP_TPI = 0.0;
	double HA_TPF = 0.0, HP_TPF = 0.0;
	double HA_T = 0.0, HP_T = 0.0;
};

struct PDIPADOpt
{
	double P30TIG; //Time of Ignition (MCC)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (MCC)
	MATRIX3 REFSMMAT;
	bool direct; //0 = with MCC, 1 = without
	VECTOR3 R_LS;	//Landing Site Vector
	double t_land;
	bool HeadsUp; //Orientation during the maneuver
	SV sv0;
};

struct ASCPADOpt
{
	VECTOR3 R_LS;
	double TIG;
	double v_LH;
	double v_LV;
	EphemerisData sv_CSM;
	MATRIX3 Rot_VL;	//Rotation Matrix, vessel to local, right-handed
};

struct LMARKTRKPADOpt
{
	EphemerisData sv0; //Input state vector
	double Elevation = 35.0*RAD; //Elevation angle at acquisition
	double LmkTime[4]; //initial guess for time over landmark (GET)
	double lat[4];		//landmark latitude
	double lng[4];		//landmark longitude
	double alt[4] = { 0,0,0,0 };	//landmark altitude
	int entries;
};

struct DKIOpt
{
	EphemerisData sv_CSM;
	EphemerisData sv_LM;
	//Maneuvering vehicle. 1 = CSM, 2 = LM
	int MV;
	//Flag which determines how the maneuver line is defined or computed
	//1 = input time, 2 = apoapsis, 3 = inactive vehicle apsis
	int IPUTNA;
	//Time of initial maneuver line
	double PUTTNA;
	//Value of initial maneuver line
	double PUTNA;
	//Maneuver line of NSR
	double NSR;
	//Delta height at TPI
	double DHSR;
	//Delta height at NCC
	double DHNCC;
	//M-line of maneuver line number at which rendezvous is to take place
	double MI;
	//Elevation angle at TPI
	double Elev;
	//Tolerance on phase angle
	double DOS = 0.0115*RAD;
	//Counter line or period option. false = counter line, true = period option
	bool IHALF = false;
	//Maneuver line point for the phasing maneuver
	double NC1;
	//Maneuver line point for the height maneuver
	double NH;
	//control flag for phase angle at TPI. true = input phase angle, false = no input
	bool KCOSR = false;
	//Phase angle desired at TPI (if input)
	double COSR;
	//Height tolerance
	double DHT = 100.0*0.3048;
	//Time of TPI
	double TTPI;
	//Time of TPF
	double TTPF;
	//Control flag for TPI time computation. 1 = Input TPI time, 2 = input TPF time, 3 = TPI at "TLIT" minutes into night, 
	//4 = TPI at "TLIT" minutes into day, 5: TPF at "TLIT" minutes into night, 6 = TPF at "TLIT" minutes into day
	int K46;
	//Delta time of lighting condition for TPI
	double TIMLIT;
	//Minimum delta time required between NSR and TPI
	double DTSR = 10.0*60.0;
	//Terminal phase angle (TPI to TPF)
	double WT;
	//Maneuver line point for the plane change maneuver
	double NPC = -1.0;
	//Control flag for initial phase angle wrapping. 0 = -180 to 180. 1 = 0 to 360. -1 = -360 to 0 and so on...
	int KRAP = 0;
	//Minimum height allowed in DKI plan
	double PMIN = 0.0;
	//Flag to relocated NH if minimum height is violated. false = no, true = yes
	bool LNH = false;
	//Number of additional M-lines desired
	int IDM = 0;
	//Flag to determine where to place in multiple plans. false = same point, 1 = relative to NSR
	bool MNH = false;

	//Skylab only
	//Skylab four-maneuver route
	bool I4 = false;
	//Maneuver line point for the NCC maneuver
	double NCC;
	//DT between NCC and NSR
	double dt_NCC_NSR;
};

struct DKICommon
{
	//Maneuvering vehicle
	int MV;
	//DT between NCC and NSR
	double dt_NCC_NSR;
	//Initial maneuver line
	double ANAI;
	//Time of initial maneuver line
	double TNAI;
	//Maneuver line point for the phasing maneuver
	double NC1;
	//Maneuver line point for the height maneuver
	double NH;
	//Maneuver line at NCC
	double NCC;
	//Maneuver line at NSR
	double NSR;
	//Maneuver line at NPC
	double NPC;
	//M-line of maneuver line number at which rendezvous is to take place
	double MI;
	//Delta time of lighting condition for TPI, in minutes!
	double TLIT;
	//Control flag for TPI time computation. 1 = Input TPI time, 2 = input TPF time, 3 = TPI at "TLIT" minutes into night, 
	//4 = TPI at "TLIT" minutes into day, 5: TPF at "TLIT" minutes into night, 6 = TPF at "TLIT" minutes into day
	int K46;
	//Minimum delta time required between NSR and TPI
	double DTSR;
	//Terminal phase angle (TPI to TPF)
	double WT;
	//Tolerance on phase angle
	double DOS;
	//Height tolerance
	double DHT;
	//Time of TPI
	double TTPI;
	//Phase angle at TPI
	double COSR;
	//Initial phase angle
	double theta_init;
	//Number of maneuvers scheduled
	int NOM;
	AEGDataBlock sv_before[5];
	AEGDataBlock sv_after[5];
	std::string ID[5];

	//Delta height at TPI
	double DHSR;
	//Delta height at NCC
	double DHNCC;
	//Counter line or period option. false = counter line, true = period option
	bool IHALF = false;
};

struct SPQOpt //Coelliptic Sequence Processor
{
	SV sv_A;
	SV sv_P;
	//Time of CSI maneuver (<= 0 if not scheduled)
	double t_CSI;
	//Time of CDH maneuver
	double t_CDH;
	double t_TPI;				// Only for calculation type = 0
	double DH = 15.0*1852.0;	// Only for calculation type = 1
	double E = 26.6*RAD;
	double WT = 130.0*RAD;
	//Height iteration. 0 = fixed TIG at TPI, 1 = fixed DH at CDH
	int K_CDH;
	int K_TPI = 0;		//-1 = Midpoint of darkness, 0 = on time, 1 = on longitude
	int ChaserID = 1;
	//0 = Plane change not requested, 1 = plane change requested
	bool N_PC = false;
	//Plane change threshold
	double T_BNPC = 0.0;
	//Initial phase angle (0 = -180° to 180°, 1 = 0 to 180°, 2 = -180° to 0)
	int I_Theta = 0;
	//0 = CDH not scheduled, 1 = CDH scheduled
	bool CDH = true;
	//1 = CDH at next apsis (AEG), 2 = CDH on time, 3 = angle from CSI, 4 = CDH at next apsis (Keplerian)
	int I_CDH = 3;
	//Number of apsis since CSI (for CDH at next apsis options)
	int N_CDH = 1;
	bool OptimumCSI = false;
	//0 = CSI and CDH in-plane, 1 = CSI and CDH parallel to target
	bool ParallelDVInd = false;
	//Angle from CSI to CDH (for I_CDH = 3)
	double DU_D = PI;
	//Optimum CSI range
	double dt_CSI_Range = 15.0*60.0;
};

struct PDAPOpt //Powered Descent Abort Program
{
	SV sv_A;
	SV sv_P;
	double TLAND;
	MATRIX3 REFSMMAT;
	VECTOR3 R_LS;
	double dt_stage;
	//LM vehicle weight immediately after staging
	double W_TAPS;
	//LM weight representative of DPS fuel depletion
	double W_TDRY;
	//DT between successive abort points
	double dt_step;
	//Time from Insertion to CSI
	double dt_CSI = 50.0*60.0;
	//Time of TPI
	double t_TPI;
	//dt added to dt_CSI for generation of the second set of targeting coefficients
	double dt_2CSI = 110.0*60.0;
	//dt added to t_TPI for generation of the second set of targeting coefficients
	double dt_2TPI = 7117.0;
	//One or two segments (Apollo 11 style vs. all later missions)
	bool IsTwoSegment = false;
	//time between orbit insertion and the canned maneuver
	double dt_CAN = 50.0*60.0;
	//DV of the canned maneuver
	double dv_CAN = 10.0*0.3048;
	//Minimum apogee altitude limit for the insertion orbit; reference from the landing site radius
	double h_amin = 30.0*1852.0; //First segment
	double h_2amin = 30.0*1852.0; //Second segment
	//Flag to use the long profile in the first set of targeting coefficients
	bool LongProfileFirst = false;
};

struct PDAPResults
{
	//Apollo 11 abort coefficients
	double ABTCOF1;
	double ABTCOF2;
	double ABTCOF3;
	double ABTCOF4;
	//Term in LM desired semi-major axis
	double DEDA224;
	//Lower limit on semi-major axis
	double DEDA225;
	//Upper limit on semi-major axis
	double DEDA226;
	//Factor in LM desired semi-major axis
	double DEDA227;
	//Limiting phase angle
	double Theta_LIM;
	//Minimum apolune radius permitted in the insertion orbit
	double R_amin;
	double K1, K2, J1, J2;
};

struct DockAlignOpt	//Docking Alignment Processor
{
	//Option 1: LM REFSMMAT from CSM REFSMMAT, CSM attitude, docking angle and LM gimbal angles
	//Option 2: LM gimbal angles from CSM REFSMMAT, LM REFSMMAT, CSM gimbal angles and docking angle
	//Option 3: CSM gimbal angles from CSM REFSMMAT, LM REFSMMAT, LM gimbal angles and docking angle
	int type;
	MATRIX3 CSM_REFSMMAT;
	MATRIX3 LM_REFSMMAT;
	VECTOR3 CSMAngles;
	VECTOR3 LMAngles;
	double DockingAngle = 0.0;
};

struct FIDOOrbitDigitals
{
	FIDOOrbitDigitals();
	double GET;		//Ground elapsed time associated with present position data
	char VEHID[64];	//Vehicle name
	int REV;		//Current revolution number associated with subject vehicle and central body
	char REF[64];	//Reference planet
	char STAID[16]; //Last vector used for updating the ephemeris
	double GMTID;	//GMT of the state vector
	double GETID;	//GET of the state vector
	int NV1;		//Number of vectors used for interpolation for present position values
	double H;		//Current height
	double V;		//Current inertial velocity
	double GAM;		//Current inertial flight path angle
	double A;		//Semimajor axis of orbital ellipse
	double E;		//Eccentricity of orbital ellipse
	double I;		//Orbital inclination to Earth or linar equator
	double HA;		//Height of next apogee at GETA
	double PA;		//Latitude of next apogee at GETA
	double LA;		//Longitude of next apogee at GETA
	double GETA;	//Time of arrival at next apogee
	double HP;		//Height of next apogee at GETP
	double PP;		//Latitude of next apogee at GETP
	double LP;		//Longitude of next apogee at GETP
	double GETP;	//Time of arrival at next apogee
	double LPP;		//Present position, longitude
	double PPP;		//Present position, latitude
	double GETCC;	//GET of arrival at next rev crossing
	double TAPP;	//Present position, true anomaly
	double LNPP;	//Longitude of ascending node (Earth-fixed or moon-fixed)
	double GETL;	//Time spacecraft will pass over L
	int REVL;		//Revolution associated with GETL
	double L;		//The longitude associated with GETL
	double TO;		//Orbital period
	double K;		//K-Factor
	double ORBWT;	//Total current weight
	int REVR;		//Revolution of requested apogee/perigee
	char REFR[64];	//Reference planet of requested vector
	double GETBV;	//Time tag of vector from which apogee/perigee values were computed
	int NV2;		//Number of vectors used in interpolating for base vector for predicted apogee/perigee data
	double HAR;		//Height of next apogee at GETA, as requested
	double PAR;		//Latitude of next apogee at GETA, as requested
	double LAR;		//Longitude of next apogee at GETA, as requested
	double GETAR;	//Time of arrival at next apogee, as requested
	double HPR;		//Height of next apogee at GETP, as requested
	double PPR;		//Latitude of next apogee at GETP, as requested
	double LPR;		//Longitude of next apogee at GETP, as requested
	double GETPR;	//Time of arrival at next apogee, as requested
	int Error;
};

struct FIDOOrbitDigitalsOpt
{
	SV sv_A;
	double MJD;		//MJD to update the state vector to (only used in continuous update)
};

struct SpaceDigitals
{
	SpaceDigitals();
	int TUP;			//Update number associated with subject vehicle
	char VecID[16];		//Identification of the last vector used to update the ephemeris
	double WEIGHT;		//Total vehicle weight
	double GMTV;		//Greenwich time-tag of the vector
	double GETV;		//Ground elapsed time-tag of the vector
	double GETAxis;		//Ground elapsed time used to define the earth-moon line
	double GETR;		//Ground elapsed time reference (elapsed time of an event)
	double GET;			//Current ground elapsed time for which orb params were computed
	char REF[64];		//Inertial reference body used to compute orb params
	double V;			//Current velocity
	double PHI;			//Current latitude
	double H;			//Current altitude above spherical Earth or above moon assuming landing site radius
	double ADA;			//Current true anomaly
	double GAM;			//Current inertial flightpath angle
	double LAM;			//Current longitude
	double PSI;			//Current heading angle
	char VEHID[4];		//Vehicle for which the space digitals are computed
	double GETVector1;	//Ground elapsed time of the vector used to compute quantities below
	char REF1[64];		//Inertial reference body used to compute quantities for GET Vector 1
	double WT;			//Total vehicle weight at GET vector 1
	double GETA;		//Ground elapsed time of next apogee (referenced from GET Vector 1)
	double HA;			//Height of apogee (referenced from GET Vector 1)
	double HP;			//Height of perigee (referenced from GET Vector 1)
	double H1;			//Altitude above reference radius (referenced from GET Vector 1)
	double V1;			//Inertial velocity (referenced from GET Vector 1)
	double GAM1;		//Flightpath angle (referenced from GET Vector 1)
	double PSI1;		//Heading angle (referenced from GET Vector 1)
	double PHI1;		//Geodetic latitude (referenced from GET Vector 1)
	double LAM1;		//Longitude (referenced from GET Vector 1)
	double HS;			//Altitude above a spherical earth or moon (referenced from GET Vector 1)
	double HO;			//Altitude above a oblate earth or spherical moon (referenced from GET Vector 1)
	double PHIO;		//Geocentric latitude (referenced from GET Vector 1)
	double IEMP;		//Inclination of the trajectory with respect to the earth-moon plane
	double W1;			//Argument of periapsis (referenced from GET Vector 1)
	double OMG;			//Right ascension of the ascending node (inertial)
	double PRA;			//Inertial right ascension of perigee
	double A1;			//Semi-major axis
	double L1;			//Argument of latitude
	double E1;			//Eccentricity
	double I1;			//Inclination
	double GETVector2;
	double GETSI;		//GET of vehicle passing through the lunar SOI with negative lunar altitude rate
	double GETCA;		//GET of closest approach to the moon
	double VCA;			//Inertial velocity at the point of closest approach
	double HCA;			//Altitude at the point of closest approach above landing site radius
	double PCA;			//Latitude of closest approach
	double LCA;			//Longitude of closest approach
	double PSICA;		//Heading angle of closest approach
	double GETMN;		//GET of arrival at the node
	double HMN;			//Height of arrival at the node
	double PMN;			//Latitude of arrival at the node
	double LMN;			//Longitude of arrival at the node
	double DMN;			//Wedge angle between planes of approach hyperbola and lunar parking orbit
	double GETVector3;
	double GETSE;		//GET of vehicle passing through the lunar SOI with positive lunar altitude rate
	double GETEI;		//GET of entry interface
	double VEI;			//Earth centered inertial velocity at entry interface
	double GEI;			//Inertial flightpath angle at entry interface
	double PEI;			//Latitude at entry interface
	double LEI;			//Longitude at entry interface
	double PSIEI;		//Heading angle at entry interface
	double GETVP;		//GET of arrival at vacuum perigee
	double VVP;			//Earth centered velocity at vacuum perigee
	double HVP;			//Altitude at vacuum perigee
	double PVP;			//Latitude at vacuum perigee
	double LVP;			//Longitude at vacuum perigee
	double PSIVP;		//Heading angle at vacuum perigee
	double IE;			//Inclination angle at vacuum perigee
	double LN;			//Geographic longitude of the earth return ascending node
	int TUN1, TUN2, TUN3;
	std::string errormessage;
};

struct SpaceDigitalsOpt
{
	SV sv_A;
	double LSLat;
	double LSLng;
	double LSAzi;
	double t_land;
};

struct CheckoutMonitor
{
	CheckoutMonitor();

	char VEH[4];	//Vehicle associated with parameters
	double GET;		//Ground elapsed time tag of vector
	double GMT;		//Greenwich mean time tag of the vector
	char VID[64];	//Identification of the last vector used to update the ephemeris
	VECTOR3 Pos;	//Position vector
	VECTOR3 Vel;	//Velocity vector
	double V_i;		//Inertial velocity
	double gamma_i;	//Inertial flight-path angle
	double psi;		//Inertial azimuth
	double phi_c;	//Geocentric/selenographic latitude
	double lambda;	//Geographic/selenographic longitude
	double h_s;		//Height of vehicle above a spherical Earth or moon
	double h_a;		//Height of apogee above a spherical Earth or moon
	double h_p;		//Height of perigee above a spherical Earth or moon
	double h_o_NM;	//Height above an oblate Earth (NM)
	double h_o_ft;	//Height above an oblate Earth (feet)
	double phi_D;	//Geodetic/selenographic latitude
	double lambda_D;//Mean geographic/true inertial longitude
	double a;		//Semi-major axis of orbital ellipse
	double e;		//Eccentricity of orbital ellipse
	double i;		//Orbital inclination
	double omega_p;	//Argument of perigee
	double Omega;	//Right ascension of the ascending node (inertial)
	double nu;		//True anomaly
	double m;		//Mean anomaly
	double R;		//Vector magnitude radius
	int R_Day[3];	//Calendar day of liftoff (day:month:year)
	double A;		//Vehicle area at GET
	double K_Fac;	//Current atmospheric density multiplier
	char CFG[4];	//Vehicle configuration at GET
	char RF[4];		//Reference frame (ECT, ECI, MCT, MCI)
	double WT;		//Total weight for above configuration
	double WC;		//Total CSM weight
	double WL;		//Total LM weight
	double SPS;		//Total SPS propellant remaining
	double RCS_C;	//Total CSM RCS propellant remaining
	double APS;		//Total APS propellant remaining
	double DPS;		//Total DPS propellant remaining
	double RCS_L;	//Total LM RCS propellant remaining
	double J2;		//Total J2 propellant remaining
	double LOC;		//Lift-off time of the CSM
	double GRRC;	//Guidance reference release time of the spacecraft
	double ZSC;		//Time that S/C register was zeroed
	double GRRS;	//Guidance reference release time of the S-IVB
	double LAL;		//Landing time of the LM (Is this really on the checkout monitor?)
	double LOL;		//Lift-off time of the LM
	double ZSL;		//Time that the LGC register was zeroed
	double ZSA;		//Time that the AGS register was zeroed
	double EPHB;	//The begin time of the ephemeris
	double EPHE;	//The end time of the ephemeris
	int UpdateNo;	//To indicate the number of times that the ephemeris was updated
	unsigned NV;	//Number of vectors used to interpolate for vector displayed
	double THT;		//MED threshold time
	double RTT;		//Reentry threshold time
	double LSB;		//Lunar stay begin time
	double LLS;		//Lunar stay termination
	double deltaL;	//Lunar declination
	double VSF;		//Vent scaling factor
	double EB2;		//Ephemeris begin time (other vehicle)
	double EE2;		//Ephemeris end time (other vehicle)
	VECTOR3 U_T;	//Unit thrust vector associated with the requested maneuver
	char Option[4];	//MED option to which displayed data corresponds (GET, GMT, MVI, MVE, RAD, ALT, FPA)

	int unit; //0 = ER, 1 = FT
	bool TABlank; //Is true anomaly blanked
	bool LSTBlank; //Lunar stay time blank
	bool MABlank; //Is mean anomaly blanked
	bool HOBlank; //Is oblate height blank

	std::string ErrorMessage;
};

struct DetailedManeuverTable
{
	DetailedManeuverTable();
	char C_STA_ID[10];
	double C_GMTV;
	double C_GETV;
	char CODE[10];
	char L_STA_ID[10];
	double L_GMTV;
	double L_GETV;
	char REF[10];
	char X_STA_ID[10];
	double X_GMTV;
	double X_GETV;
	double GETR;
	double WT;
	double WC;
	double WL;
	double WF;
	double GETI;
	double PETI;
	double DVM;
	double DVREM;
	double DVC;
	double DT_B;
	double DT_U;
	double DT_TO;
	double DV_TO;
	char REFSMMAT_Code[7];
	double DEL_P;
	double DEL_Y;
	VECTOR3 VG;
	VECTOR3 IMUAtt;
	VECTOR3 FDAIAtt;
	VECTOR3 LVLHAtt;
	double VF;
	double VS;
	double VD;
	double H_BI;
	double P_BI;
	double L_BI;
	double F_BI;
	double HA;
	double HP;
	double L_AN;
	double E;
	double I;
	double WP;
	double VP;
	double THETA_P;
	double DELTA_P;
	double P_LLS;
	double L_LLS;
	double R_LLS;
	double DH;
	double PHASE;
	double PHASE_DOT;
	double WEDGE_ANG;
	double YD;
	bool UntilDay; //false = until night, true = until day
	double TimeUntil;
	char PGNS_Veh[8];
	char AGS_Veh[8];
	double PGNS_GETI;
	VECTOR3 PGNS_DV;
	double AGS_GETI;
	VECTOR3 AGS_DV;
	double Lam_GETI;
	double Lam_TF;
	VECTOR3 Lam_R;
	int CFP_ID;
	double CFP_GETI;
	int CFP_APSIS;
	double CFP_ELEV;
	double CFP_TPI;
	double CFP_DT;
	char CFP_OPTION[8];
	bool isCSMTV;
	std::string error;
	int Attitude;
};

struct MPTVehicleDataBlock
{
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line, int &inttemp);

	//Word 12 (Bytes 1, 2)
	std::bitset<4> ConfigCode;
	//Word 12 (Bytes 3, 4)
	int ConfigChangeInd = 0;
	//Word 12 (Bytes 5, 6)
	int TUP = 0;
	//Word 13
	double CSMArea = 0.0;
	//Word 14
	double SIVBArea = 0.0;
	//Word 15
	double LMAscentArea = 0.0;
	//Word 16
	double LMDescentArea = 0.0;
	//Word 17
	double CSMMass = 0.0;
	//Word 18
	double SIVBMass = 0.0;
	//Word 19
	double LMAscentMass = 0.0;
	//Word 20
	double LMDescentMass = 0.0;
	//Word 21
	double CSMRCSFuelRemaining = 0.0;
	//Word 22
	double SPSFuelRemaining = 0.0;
	//Word 23
	double SIVBFuelRemaining = 0.0;
	//Word 24
	double LMRCSFuelRemaining = 0.0;
	//Word 25
	double LMAPSFuelRemaining = 0.0;
	//Word 26
	double LMDPSFuelRemaining = 0.0;
};

struct MPTManeuver
{
	MPTManeuver();
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	//Word 1
	std::string code;
	//Words 12-26
	MPTVehicleDataBlock CommonBlock;
	//Word 27
	std::string StationIDFrozen;
	//Word 28, GMT of frozen anchor vector
	double GMTFrozen;
	//Word 31 (Bytes 1, 2)
	int AttitudeCode;
	//Word 31 (Bytes 3, 4)
	int Thruster;
	//Word 31 (Bytes 5, 6), false = 2 thrusters, true = 4 thrusters
	bool UllageThrusterOpt;
	//Word 31 (Bytes 7,8)
	bool AttitudesInput;
	//Word 32 (Bytes 1, 2)
	std::bitset<4> ConfigCodeBefore;
	//Word 32 (Bytes 3, 4), Maneuvering vehicle: 1 = CSM, 2 = S-IVB, 3 = LM
	int TVC;
	//Word 32 (Bytes 5,6)
	int TrimAngleInd;
	//Word 32 (Bytes 7, 8)
	bool FrozenManeuverInd;
	//Word 33 (Bytes 1,2), 0 = Earth, 1 = Moon
	int RefBodyInd;
	//Word 33 (Bytes 3,4), 0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT
	int CoordSysInd;
	//Word 33 (Bytes 5,6), false = down, true = up
	bool HeadsUpDownInd;
	//Word 34
	double DockingAngle;
	//Word 35, GMT time to begin maneuver (first phase, aka ullage)
	double GMTMAN;
	//Word 36, total time of ullage including overlap
	double dt_ullage;
	//Word 37, time at 10% thrust for DPS (from engine on signal to throttle up)
	double DT_10PCT;
	//Word 38, duration of burn, for inertial and manual
	double dt;
	//Word 39, Delta V of maneuver, for inertial and manual
	double dv;
	//Word 40-42
	VECTOR3 A_T; //Unit thrust vector
	//Word 43-51
	VECTOR3 X_B, Y_B, Z_B; //Body direction matrix
	//Words 52-58
	EphemerisData FrozenManeuverVector;
	//Word 59, DPS scale factor (0 to 1)
	double DPSScaleFactor;
	//Word 61-63
	union
	{
		VECTOR3 dV_inertial; //In RTCC coordinates
		struct { double R_D, Y_D; }; //Ascent
		struct { double Inclination, theta_N, Eccentricity; }; //TLI
	};
	//Word 64-66
	union
	{
		VECTOR3 dV_LVLH; //In P30 coordinates
		struct { double R_D_dot, Y_D_dot, Z_D_dot; }; //Ascent
		struct { double C3, alpha_D, f; }; //TLI
	};
	//Word 67
	union
	{
		//ExtDVCoordInd and 
		int Word67i[2];
		//R_N or P for TLI
		double Word67d;
	};
	//K5 for TLI
	double Word68;
	//T_M or R_T for TLI
	double Word69;
	double Word70;
	double Word71;
	double Word72;
	//Words 73-84
	//For ascent maneuver this is: R, V, MJD of CSM
	//For TLI this is: P_RP, Y_RP, , T3, tau3, TLI ind,T2,Vex2,Mdot2,DV_BR,tau2N,dtP
	double Word73;
	double Word74;
	double Word75;
	double Word76;
	double Word77;
	union
	{
		double Word78d;
		//For TLI this is current and original TLI computation indicator (negative for specified TIG)
		//Original TLI indicator: 0 = if target parameters were input, otherwise (-2, -1, 1 and 2) only opportunity (and time of restart prep) was input
		//Iterable TLI means original TLI indicator is not 0
		int Word78i[2];
	};
	double Word79;
	double Word80;
	double Word81;
	double Word82;
	double Word83;
	double Word84;
	//Word 106
	double GMTI;
	//Word 110 (Byte 1)
	//X'FF'???
	//Word 148-150
	int TrajDet[3];

	//State vector at main engine on
	VECTOR3 R_BI;
	VECTOR3 V_BI;
	double GMT_BI;
	//State vector at burnout
	VECTOR3 R_BO;
	VECTOR3 V_BO;
	double GMT_BO;
	//State vector at ullage on/first phase
	VECTOR3 R_1;
	VECTOR3 V_1;
	double GMT_1;
	
	double TotalMassAfter;
	double TotalAreaAfter;
	double MainEngineFuelUsed;
	double RCSFuelUsed;
	double DVREM;
	double DVC;
	double DVXBT;
	double DV_M;
	double V_F, V_S, V_D;
	double P_H, Y_H, R_H;
	//Burn duration, GMTCO minus GMTI
	double dt_BD;
	//DT of tailoff
	double dt_TO;
	//DV of tailoff
	double dv_TO;
	//Pitch gimbal at ignition
	double P_G;
	//Yaw or roll gimbal at ignition
	double Y_G;
	//Latitude at burn initiation
	double lat_BI;
	//Longitude at burn initiation
	double lng_BI;
	//Height at burn initiation
	double h_BI;
	//True anomaly at burn initiation
	double eta_BI;
	//Eccentricity of orbit after this maneuver
	double e_BO;
	//Inclination of orbit after this maneuver
	double i_BO;
	//Argument of periapsis after this maneuver
	double g_BO;
	//Altitude at first apogee after the maneuver
	double h_a;
	double lat_a;
	double lng_a;
	double GMT_a;
	double h_p;
	double lat_p;
	double lng_p;
	double GMT_p;
	double GMT_AN;
	double lng_AN;
	//Impulsive maneuver time
	double IMPT;
};

struct MPTManDisplay
{
	MPTManDisplay();
	double GETBI;
	double DT;
	double DELTAV;
	double DVREM;
	double HA;
	double HP;
	std::string code;
};

struct MissionPlanTable
{
	MissionPlanTable();
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	//Word 1 (Byte 3,4)
	//Number of maneuvers in table
	unsigned ManeuverNum = 0;
	//Word 1 (Byte 5,6)
	//Not needed
	//Word 1 (Byte 7,8)
	unsigned MaxManeuverNum = 15;
	//Word 2
	std::string StationID;
	//Word 3
	//Anchor vector time
	double GMTAV = 0.0;
	//Word 4
	double KFactor = 0.0;
	//Word 8
	double LMStagingGMT = 0.0;
	//Word 9
	double UpcomingManeuverGMT = 1e70;
	//Word 11
	double SIVBVentingBeginGET = 0.0;
	//Words 12-26
	MPTVehicleDataBlock CommonBlock;
	double TotalInitMass = 0.0;
	double ConfigurationArea = 0.0;
	//Word 34
	double DeltaDockingAngle = 0.0;

	unsigned LastFrozenManeuver = 0;
	unsigned LastExecutedManeuver = 0;

	double TimeToBeginManeuver[15];
	double TimeToEndManeuver[15];
	double AreaAfterManeuver[15];
	double WeightAfterManeuver[15];

	std::deque<MPTManeuver> mantable;
};

struct BurnParameterTable
{
	//P1: DV, DV IND, DT
	double P1_DV = 0.0; //Vel increment or burn duration of the maneuver
	//0 = MAG, Magnitude of maneuver
	//1 = DVC, DV along X-body axis includes ullage, excludes tailoff
	//2 = XBT, Includes both ullage and tailoff
	int P1_DVIND = 0;
	double P1_DT = 0.0; //Time duration of maneuver, excludes ullage and tailoff

	//P2: VX, VY, VZ in command and display system
	VECTOR3 P2_DV = _V(0, 0, 0);

	//P3: VGX, VGY, VGZ DV Vector in IMU coordinates
	VECTOR3 P3_DV = _V(0, 0, 0);

	//P4: VF, VS, VD LVLH DV vector
	VECTOR3 P4_DV = _V(0, 0, 0);

	//P5: X, Y, Z, T Flight, C
	VECTOR3 P5_RT;
	double P5_TFLT; //Time from ignition to arrival at target vector
	double P5_C;	//Cross product steering constant

	//P6: RD, YD, RD DOT, YD DOT, ZD DOT, DT ascent man. targets
	double P6_RD;	//Desired injection radius
	double P6_YD;	//Desired injection cross range distance from the CSM orbital plane
	double P6_RD_DOT;	//Desired injection radial velocity
	double P6_YD_DOT;	//Desired injection crossrange velocity
	double P6_ZD_DOT;	//Desired injection downrange velocity
	double P6_DT;		//Time from descent ignition to descent abort

	//P7: DVX, DVY, DVZ maneuver residuals in RCS control axes
	VECTOR3 P7_DDV;
};

struct PMMXFRDirectInput
{
	int TableCode;
	unsigned ReplaceCode;
	double GMTI;
	int ThrusterCode;
	int AttitudeCode;
	int BurnParameterNumber;
	int CoordinateIndicator;
	double Pitch;
	double Yaw;
	double Roll;
	double dt_ullage;
	bool UllageThrusterOption;
	bool HeadsUpDownIndicator;
	double DT10P;
	int REFSMMATIndicator;
	int ConfigurationChangeIndicator;
	std::bitset<4> EndConfiguration;
	double DockingAngle;
	double DPSScaleFactor;
	int TrimAngleIndicator;
};

struct PMMXFR_Impulsive_Input
{
	int Table;
	int Plan;
	double DeleteGMT;
	int Thruster[4];
	int Attitude[4];
	double dt_ullage[4];
	bool UllageThrusterOption[4];
	bool IterationFlag[4];
	double DT10P[4];
	double DPSScaleFactor[4];
	bool TimeFlag[4];
	unsigned ReplaceCode = 0;
	int Type;
};

struct PMMMCDInput
{
	double GMTI;
	int Thruster;
	int Attitude;
	double dt_ullage;
	int ConfigChangeInd;
	int ConfigCodeAfter;
	int TVC;
	double DockingAngle;
	std::bitset<4> CCMI;
	int BPIND;
	int ID;
	bool UllageThrusterOpt;
	double DT_10PCT;
	double DPSScaleFactor;
	bool HeadsUpDownInd;
	int CoordinateInd;
	double StartTimeLimit;
	double EndTimeLimit;
	MissionPlanTable *mpt;
	MPTManeuver *prevman;
	unsigned CurrentManeuver;
	double Pitch, Yaw, Roll;
	EphemerisData sv_anchor;
	EphemerisData sv_other;
	bool FrozenManeuver;
	double BurnParm72;
	int BurnParm73;
	double BurnParm74;
	double BurnParm75;
	double BurnParm76;
	double BurnParm77;
	double BurnParm78;
	double BurnParm79;
	double WTMI;
	int RefMatCode = 1;
};

struct PMMMPTInput
{
	//State vector before maneuver
	EphemerisData sv_before;
	//Velocity after maneuver
	VECTOR3 V_aft;
	//Selected thruster for maneuver
	int Thruster;
	//Attitude mode
	int Attitude;
	//Docking angle
	double DockingAngle;
	//Delta T ullage
	double DETU;
	//Ullage thruster code (false = 2 thrusters, true = 4 thrusters)
	bool UT;
	//Vehicle configuration code
	int CONFIG;
	//Maneuvering vehicle code
	int VC;
	//DT at 10% thrust for DPS
	double DT_10PCT;
	//Scale factor for DPS engine
	double DPSScaleFactor;
	double VehicleArea;
	double VehicleWeight;
	double CSMWeight;
	double LMWeight;
	//Time flag. true = start at impulsive time, false = use optimum time
	bool IgnitionTimeOption;
	//Iterate flag. false = don't iterate, true = iterate
	bool IterationFlag;
	double LowerTimeLimit;
	double UpperTimeLimit;
	MissionPlanTable *mpt;
	unsigned CurrentManeuver;
	EphemerisData sv_other;
	bool HeadsUpIndicator = true;
	int TrimAngleInd = -1;
};

struct PMMSPTInput
{
	//Word 1
	int QUEID;
	//Word 2
	double GMT;
	//Word 3-5
	VECTOR3 R;
	//Word 6-8
	VECTOR3 V;
	//Word 10
	int Table;
	//Word 11
	unsigned ReplaceCode;
	int InjOpp;
	//Word 12 (Time of ignition for TLI confirmation, negative if not input)
	double T_RP = -1.;
	//Word 13
	int ThrusterCode;
	int AttitudeMode;
	//Word 14
	std::string StationID;
	double i; //Inclination of target orbit
	//Word 15, Longitude of descending node of target orbit
	double theta_N;
	//Word 16, Eccentricity of target conic (negative of targeting parameters not input)
	double e = -1.;
	//Word 17, equals (-mu/a), where a is semi-major axis of target conic
	double C3;
	//Word 18, true anomaly of the descending node of the target conic
	double alpha_D;
	//Word 19, estimated true anomaly at cutoff
	double f;
	//Word 20 (DT of burn for maneuver confirmation, negative if not input)
	double dt;
	//Word 29, configuration change indicator
	int CCI;
	//Word 30, configuration code at end of maneuver
	int CC;
	int TVC;
	//Word 32, configuration code at start of maneuver
	int CCMI;
	MPTManeuver *CurMan;
	MPTManeuver *PrevMan = NULL;
	MissionPlanTable *mpt;
	//Word 46
	double PresentGMT;
	//Word 47
	double StartTimeLimit;
	//Word 48
	double EndTimeLimit;
};

struct PMMLDPInput
{
	double TLAND;
	SV sv;
	MissionPlanTable *mpt;
	unsigned CurrentManeuver;
	int TrimAngleInd;
	bool HeadsUpDownInd;
};

struct PMMLDIInput
{
	double TLAND = 0.0;
	SV sv;
	double W_LMA;
	double W_LMD;
};

struct PMMLAIInput
{
	double m0;
	EphemerisData sv_CSM;
	double t_liftoff;
	double v_LH;
	double v_LV;
};

struct PCMATCArray
{
	//Words 31-44: State and time of preabort
	VECTOR3 R0;
	VECTOR3 V0;
	double T0;
	//Word 45: Solution type indicator (0 = AST, 1 = RTED)
	int IASD;
	//Word 46: Initial reference
	int REF;
	//Word 48: Stopping type (1 = gamma, -1 = time)
	int ISTP;
	//Words 49-50: Stopping gamma (for stopping type = 1)
	double gamma_stop;
	//Words 51-64: State and time of reentry
	VECTOR3 R_r;
	VECTOR3 V_r;
	double T_r;
	//Words 65-70: Unit thrust at MEI (RTED), output abort position vector (AST)
	VECTOR3 Vec1;
	//Words 71-76: Position at MEI (RTED), output abort velocity vector (AST)
	VECTOR3 Vec2;
	//Words 77-78: Open (RTED). output abort GMT? (AST)
	double Words77_78;
	//Words 79-80: Total DV? (RTED), DVX (AST)
	//Words 81-82: DV of tailoff (RTED), DVY (AST)
	//Words 83-84: CSM weight at abort (RTED), DVZ (AST)
	VECTOR3 Vec3;
	//Words 85-86: LM weight at abort
	double LMWeight;
	//Words 87-88: Docking angle
	double DockingAngle;
	//Words 89-90: DT of ullage
	double dt_ullage;
	//Words 91-92: DT of 10% thrust (both input and output)
	double DT_10PCT;
	//Words 93-94: DT of tailoff
	double DT_TO;
	//Words 95-112: XB, YB, ZB vehicle body axies
	VECTOR3 X_B, Y_B, Z_B;
	//Words 113-116: Ry, Rz trim angles at MEI
	double R_Y, R_Z;
	//Words 117-118: Velocity counter
	double DVC;
	//Word 130: Attitude control (3 Lambert, 4 Ext DV)
	int AttitudeCode;
	//Word 131: Thruster code
	int ThrusterCode;
	//Word 132: Ullage code
	bool UllageCode;
	//Word 133: Configuration code (as defined for MPT)
	int ConfigCode;
	//Word 134: Maneuvering vehicle (1 CSM, 3 LM)
	int TVC;
	//Word 135: Initial attitude (0-heads down, 1-up)
	bool HeadsUpDownInd;
	//Word 136: Trim angles (-1 compute, 1 system parameters)
	int TrimAngleInd;
	//Words 137-138: Time of midnight prior to launch
	double GMTBASE;
	//Word 159: Error indicator (0 = no error, 1 = coast error, 2 = powered error)
	int ErrInd;
	bool h_pc_on;
	//Words 259-272: State and time at main engine off
	EphemerisData sv_CO;
	//Words 273-286: Free flight state vector main engine on
	EphemerisData sv_BI;
	//Words 291-296: Velocity at main engine on
	VECTOR3 V_BI;
	//Words 297-298: Height at perigee
	double h_p;
	//Words 389-390: DVa total
	double dv_total;
	//Words 391-392: DT of main engine burn
	double dt_ME;
	//Words 393-394: Weight at end of maneuver
	double W_end;
	//Words-395-396: Weight at main engine on
	double W_on;
	//Words 397-398: T of main engine on
	double GMT_BI;
	//Words 399-400: Height at apogee
	double h_a;
};

struct StationContact
{
	StationContact();
	double GMTAOS;
	double GMTLOS;
	double GMTEMAX;
	double MAXELEV;
	std::string StationID;
	bool BestAvailableAOS;
	bool BestAvailableLOS;
	bool BestAvailableEMAX;
	int REV;

	//For sorting
	bool operator<(const StationContact& rhs) const;
};

struct OrbitStationContactsTable
{
	int Num = 0;
	StationContact Stations[45];
};

struct NextStationContactsTable
{
	NextStationContactsTable();
	//Station ID
	std::string STA[2][6];
	//GET of horizon ascent
	double GETHCA[2][6];
	//Duration from GETHCA to entrance into keyhole (GETKLOS - GETHCA)
	double DTKLOS[2][6];
	//Maximum elevation angle above station
	double EMAX[2][6];
	//Duration of contact
	double DTPASS[2][6];
	//Duration of time in keyhole
	double DTKH[2][6];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[2][6];
	//If true, maximum elevation occured before current GET
	bool BestAvailableEMAX[2][6];
	//Current GET
	double GET = 0.0;
};

struct PredictedSiteAcquisitionTable
{
	PredictedSiteAcquisitionTable();
	
	//Current page (1 to 2)
	int curpage;
	//Total number of pages (1 or 2)
	int pages;
	//Total number of contacts
	unsigned numcontacts[2];
	//Station ID of anchor vector
	std::string STAID;
	//Vehicle (CSM or LEM)
	std::string VEHICLE;
	//Revolution number
	int REV[2][21];
	//Sites
	std::string STA[2][21];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[2][21];
	//AOS GET
	double GETHCA[2][21];
	//If true, AOS occured before current GET
	bool BestAvailableLOS[2][21];
	//LOS GET
	double GETHCD[2][21];
	//If true, maximum elevation occured before current GET
	bool BestAvailableEMAX[2][21];
	//Max elevation
	double ELMAX[2][21];
};

struct ExperimentalSiteAcquisitionTable
{
	ExperimentalSiteAcquisitionTable();

	//Current page (1 to 2)
	int curpage;
	//Total number of pages (1 or 2)
	int pages;
	//Total number of contacts
	unsigned numcontacts[2];
	//Station ID of anchor vector
	std::string STAID;
	//Revolution number
	int REV[2][20];
	//Sites
	std::string STA[2][20];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[2][20];
	//AOS GET
	double GETAOS[2][20];
	//Ground range
	double GNDRNG[2][20];
	//Altitude
	double ALT[2][20];
	//If true, maximum elevation occured before current GET
	bool BestAvailableEMAX[2][20];
	//Max elevation
	double ELMAX[2][20];
	//Closest approach GET
	double GETCA[2][20];
	//LOS GET
	double GETLOS[2][20];
	//If true, AOS occured before current GET
	bool BestAvailableLOS[2][20];
};

struct LandmarkAcquisitionTable
{
	LandmarkAcquisitionTable();
	//Current page (1 to 3)
	int curpage;
	//Total number of pages (1 or 3)
	int pages;
	//Total number of contacts
	unsigned numcontacts[3];
	//Trajectory update number of subject vehicle ephemeris
	int TUN;
	//Identification of last vector used to update the ephemeris
	std::string VectorID;
	//Identification of landmark
	std::string STAID[3][20];
	//GET of acquisition of landmark site by vehicle
	double GETAOS[3][20];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[3][20];
	//GET of loss of sight of the landmark site by vehicle
	double GETLOS[3][20];
	//If true, LOS occurs after end of ephemeris
	bool BestAvailableLOS[3][20];
	//If true, closest approach occurs after end of ephemeris
	bool BestAvailableCA[3][20];
	//GET of closest approach of vehicle to the landmark
	double GETCA[3][20];
	//GET of local sunrise
	double GETSR[3][20];
	//GET of local sunset
	double GETSS[3][20];
	//Viewing angle from the vehicle to the landmark site at GETCA
	double Lambda[3][20];
	//Spacecraft altitude at GETCA
	double h[3][20];
	int err;
};

struct LunarStayTimesTable
{
	double LunarStayBeginGMT = -1;
	double LunarStayEndGMT = -1;
};

struct CapeCrossingTable
{
	CapeCrossingTable();
	int TUP;
	int ref_body;
	int NumRev;
	int NumRevFirst;
	int NumRevLast;
	double GMTEphemFirst;
	double GMTEphemLast;
	//Time of last known cape crossing before the time of the update vector (zero if unknown)
	double GMTCrossPrev;
	double GMTCross[30];
};

struct LunarDescentPlanningTable
{
	LunarDescentPlanningTable();
	double LMWT;
	double GMTV;
	double GETV;
	int MODE;
	double LAT_LLS;
	double LONG_LLS;
	double GETTH[4];
	double GETIG[4];
	double LIG[4];
	double DV[4];
	double AC[4];	//Altitude at cutoff?
	double HPC[4];	//Perilune height at cutoff?
	double DEL[4];	//???
	double THPC[4];	//???
	VECTOR3 DVVector[4];
	std::string MVR[4];
	double PD_ThetaIgn;
	double PD_PropRem;
	double PD_GETTH;
	double PD_GETIG;
	double PD_GETTD;
	char DescAzMode[4];
	double DescAsc;
	double SN_LK_A; //Sun look angle (elevation angle at landing site)
	int error;
};

struct SunriseSunsetData
{
	//Revolution number
	int REV = 0;
	//GET of terminator rise from liftoff
	double GETTR = 0.0;
	bool BestAvailableGETTR = false;
	//GET of sunrise from liftoff
	double GETSR = 0.0;
	bool BestAvailableGETSR = false;
	//Pitch at sunrise
	double theta_SR = 0.0;
	//Yaw at sunrise
	double psi_SR = 0.0;
	//GET of terminator set from liftoff
	double GETTS = 0.0;
	bool BestAvailableGETTS = false;
	//GET of sunset from liftoff
	double GETSS = 0.0;
	bool BestAvailableGETSS = false;
	//Pitch at sunset
	double theta_SS = 0.0;
	//Yaw at sunset
	double psi_SS = 0.0;
};

struct SunriseSunsetTable
{
	int num = 0;
	SunriseSunsetData data[8];
	std::string errormessage = "MED OUTDATED";
};

// Parameter block for Calculation(). Expand as needed.
struct calculationParameters {
	Saturn *src;		// Our ship
	VESSEL *tgt;		// Target ship
	double TLI;			// Time of TLI
	double LOI;			// Time of LOI/Pericynthion
	double SEP;			// Time of separation
	double DOI;			// Time of DOI
	double PDI;			// Time of PDI
	double LunarLiftoff;// Time of lunar liftoff
	double LSAzi;		// Approach azimuth to the lunar landing site
	double Insertion;	// Time of Insertion
	double Phasing;		// Time of Phasing
	double CSI;			// Time of CSI
	double CDH;			// Time of CDH
	double TPI;			// Time of TPI
	double TEI;			// Time of TEI
	double EI;			// Time of Entry Interface
	MATRIX3 StoredREFSMMAT;
	double TIGSTORE1;		//Temporary TIG storage
	VECTOR3 DVSTORE1;		//Temporary DV storage
	SV SVSTORE1;			//Temporary state vector storage
};

struct ASTInput
{
	double dgamma;
	double dpsi;
	double dv;
	double T_a;
	double T_r;
	double h_r_des;
	double lat_r_des;
	double lng_r_des;
	double azi_r_des;
	double dt_ar;
	double gamma_des;
	double gamma_stop;
	double R_E_equ;
	double R_E_pole;
	double RRBIAS;
	double w_E;
	double Area;
	double Weight;
	double lat_PLA;
	double lng_PLA;
	double GLevel;
	double BankAngle;
	double GLevelGuidInit;
	int RollDir;
	double GMTBASE;
};

struct ASTSettings
{
	//
	bool FuelCritical;
	//Mode indicator to RMMYNI (1 = zero lift, 2 = max lift, 3 = G&N, 6 = Constant G, then roll, 10 = Constant G) 
	int ReentryMode;
	int Ref;
	//Type of reentry stop (1 = gamma, -1 = T)
	int ReentryStop;
	//Target line indicator (1 = Primary, 2 = Secondary)
	int TargetLine;
	//Entry profile (1 = G&N, <= 0 Constant G)
	int EntryProfile;
	//Miss distance indicator (0 = do not compute, 1 = compute)
	int MissDistanceInd;
};

struct ASTData
{
	int ASTCode = 0;
	std::string StationID;
	//Time of abort (hr.)
	double AbortGMT;
	//Inclination of reentry (rad.)
	double incl_EI;
	//DV abort (Er/hr.)
	double dv;
	//Height of pericynthion (Er)
	double h_PC;
	//Reentry velocity magnitude (Er/hr.)
	double v_EI;
	//Gamma at reentry (rad.)
	double gamma_EI;
	//Time of reentry (GMT)
	double ReentryGMT;
	//Geodetic latitude of impact (rad.)
	double lat_SPL;
	//Longitude of impact (rad.)
	double lng_SPL;
	//Time of impact (hr.)
	double SplashdownGMT;
	//Time of vector in MPT (hr.)
	double VectorGMT;
	//Abort mode
	std::string AbortMode;
	//Primary reentry mode
	int ReentryMode;
	//Miss distance (Er)
	double MissDistance;
	//Site ID
	std::string SiteID;
	//G-level sensed (Er/hr.^2)
	double GLevel;
	//Reentry range bias (Er)
	double RRBIAS;
	//State and time at reentry (Er, Er/hr.)
	EphemerisData2 sv_EI;
	//Year of input
	int Year;
	//Geodetic latitude of target
	double lat_TGT;
	//Longitude of target
	double lng_TGT;
	//Preabort state and time (Er, Er/hr)
	EphemerisData sv_IG;
	//DV vector. Not actually in array???
	VECTOR3 DV;
};

struct RTEDMEDData
{
	int Thruster;
	int AttitudeMode;
	bool UllageThrusters;
	int ConfigCode;
	int ManVeh;
	bool HeadsUp;
	int PrimaryReentryMode;
	int BackupReentryMode;
	int IRM; //REFSMMAT number: -1 = input, 0 = Reentry, 1 = deorbit, 2 = orbital preferred, 3 = TEI, 4 = Lunar Entry (LVLH 0,0,0), 5 = Lunar Entry (LVLH 0,180,0)
	int StoppingMode; //-1 = Time, 1 = Gamma
	bool ManualEntry;
	int TrimInd;
	int Ref;
	int Column;
};

struct RTEDASTData
{
	EphemerisData sv_TIG;
	VECTOR3 DV;
	double h_r;
	double lat_r;
	double lng_r;
	double azi_r;
	double dt_ar;
	double gamma_r;
	double gamma_r_stop;
	double lat_tgt;
	double lng_tgt;

	//Not on actual array
	EphemerisData2 sv_r;
};

struct RTEDSPMData
{
	double UllageThrust;
	double MainEngineThrust;
	double UllageWLR;
	double MainEngineWLR;
	double dt_ullage;
	double dt_10PCT;
	double CSMWeight;
	double LMWeight;
	double DockingAngle;
	double KFactor;
	double CMWeight;
	double CMArea;
	double BankAngle;
	double GLevelReentryPrim;
	double GLevelReentryBackup;
	double GLevelConstant;
	double lng_T;
	double RollDirectionPrim;
	double RollDirectionBackup;
	double TDPS; //Maximum DT for DPS (SPS) without iterating
	double GNBankAngle;
	double DPS10PCTThrust;
	double DPS10PCTWLR;
	double ComputerThrust; //In AGC
};

struct RTEDigitalSolutionTable
{
	std::string RTEDCode;
	std::string ASTSolutionCode;
	std::string LandingSiteID;
	std::string ManeuverCode;
	int ThrusterCode = 0;
	std::string SpecifiedREFSMMAT;
	double VehicleWeight = 0.0;
	double TrueAnomaly = 0.0;
	std::string PrimaryReentryMode;
	VECTOR3 LVLHAtt = _V(0, 0, 0);
	VECTOR3 FDAIAtt = _V(0, 0, 0);
	double DVC = 0.0;
	double dt = 0.0;
	double dv = 0.0;
	int NumQuads = 0;
	double dt_ullage = 0.0;
	double PETI = 0.0;
	double GETI = 0.0;
	double GMTI = 0.0;
	std::string BackupReentryMode;
	double RollPET = 0.0;
	double LiftVectorOrientation = 0.0;
	double ReentryPET = 0.0;
	double v_EI = 0.0;
	double gamma_EI = 0.0;
	double lat_EI = 0.0; //Latitude and longitude at entry interface
	double lng_EI = 0.0;
	double lat_imp_2nd_max = 0.0;
	double lng_imp_2nd_max = 0.0;
	double lat_imp_tgt = 0.0; //Latitude and longitude of splashdown target
	double lng_imp_tgt = 0.0;
	double lat_imp_2nd_min = 0.0;
	double lng_imp_2nd_min = 0.0;
	double md_lat = 0.0;
	double md_lng = 0.0;
	double lat_imp_bu = 0.0; //Latitude and longitude of impact (backup mode)
	double lng_imp_bu = 0.0;
	double ImpactGET_bu = 0.0;
	double VectorGET = 0.0;
	double KFactor = 0.0;
	double CMWeight = 0.0;
	double PETReference = 0.0;
	std::string StationID;
	double MaxGLevelPrimary = 0.0;
	double GLevelRoll = 0.0;
	double R_Y = 0.0;
	double R_Z = 0.0;
	VECTOR3 IMUAtt = _V(0, 0, 0);
	double dt_10PCT = 0.0;
	VECTOR3 A_T = _V(0, 0, 0);
	VECTOR3 X_B = _V(0, 0, 0);
	VECTOR3 Y_B = _V(0, 0, 0);
	VECTOR3 Z_B = _V(0, 0, 0);
	double dv_TO = 0.0;
	double dt_TO = 0.0;
	double PostAbortWeight = 0.0;
	double MaxGLevelGMT = 0.0;
	double md_lat_bu = 0.0;
	double md_lng_bu = 0.0;
	VECTOR3 R_BI = _V(0, 0, 0);
	VECTOR3 V_BI = _V(0, 0, 0);
	double Inclination = 0.0;
	double h_p = 0.0;
	double h_a = 0.0;
	VECTOR3 DV_XDV = _V(0, 0, 0);
	VECTOR3 A_T_LVLH = _V(0, 0, 0);
	VECTOR3 R_BO = _V(0, 0, 0);
	VECTOR3 V_BO = _V(0, 0, 0);
	double GMT_BO = 0.0;
	double ImpactGET_max_lift = 0.0;
	double ImpactGET_zero_lift = 0.0;
	double RollGETBackup = 0.0;
	double MaxGLevelBackup = 0.0;
	double MaxGLevelGETBackup = 0.0;
	double lat_imp_prim = 0.0;  //Latitude and longitude of impact (primary mode)
	double lng_imp_prim = 0.0;
	double ImpactGET_prim = 0.0;
	int Error = 0;
	MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);

	//For transfer to MPT
	bool HeadsUpDownIndicator = false;
	int ConfigurationChangeIndicator = 0;
	int EndConfiguration = 0;
};

struct ELVCTRInputTable
{
	//Time to interpolate
	double GMT;
	//Order of interpolation
	unsigned ORER = 8;
	//Vehicle code (option 2 only)
	int L;
	//Ephemeris type (0 = Orbit, 8 = Reentry, option 2 only)
	int EphemerisType = 0;
};

struct ELVCTROutputTable2
{
	EphemerisData2 SV;
	//2 = Order of interpolation performed less than order of interpolation requested, 16 = Time for requested interpolation exceeds ephemeris end time
	//32 = Time for requested interpolation precedes first time in ephemeris, 64 = Invalid order of interpolation requested, 128 = Fewer than two vectors available
	int ErrorCode;
	//Order of interpolation performed
	unsigned ORER;
	//Vector property indicator (0 = Free flight, -1 = Lunar Stay, +1 = Maneuver)
	int VPI;
	//Update number
	int TUP;
};

struct TLITargetingParametersTable
{
	int Day = 0;

	//Launch day specific
	double T_LO;
	double theta_EO;
	double omega_E;
	double K_a1 = 0.0;
	double K_a2 = 0.0;
	double K_T3;

	double hx[3][5]; //Azimuth from time polynomial (radians)
	double t_D0, t_D1, t_D2, t_D3; //Times of the opening and closing of launch windows
	double t_DS0, t_DS1, t_DS2, t_DS3; //Times to segment the azimuth calculation polynomial
	double t_SD1, t_SD2, t_SD3;	//Times used to scale the azimuth polynomial

	//TLI opportunity specific
	double T_ST[2];
	double beta[2];
	double alpha_TS[2];
	double f[2];
	double R_N[2];
	double T3_apo[2];
	double tau3R[2];
	double T2[2];
	double Vex2[2];
	double Mdot2[2];
	double DV_BR[2];
	double tau2N[2];
	double KP0[2];
	double KY0[2];

	//TLI opportunity and launch time specific
	double cos_sigma[2][15];
	double C_3[2][15];
	double e_N[2][15];
	double RA[2][15];
	double DEC[2][15];
	double t_D[2][15];
};

class RTCC {

	friend class MCC;

public:
	RTCC();
	~RTCC();
	void Init(MCC *ptr);
	void Timestep(double simt, double simdt, double mjd);
	bool Calculation(int mission, int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);

	void SetManeuverData(double TIG, VECTOR3 DV);
	void GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD);
	void LoadLaunchDaySpecificParameters(int year, int month, int day);
	bool LoadMissionFiles();
	bool LoadMissionConstantsFile(std::string file);

	//Offline Programs

	//Build TLI targeting parameters table
	int QMMBLD(std::string file);
	//Search tape and build skeleton flight plan table
	int QMSEARCH(std::string file);
private:
	void LoadMissionInitParameters(int year, int month, int day);
	void InitializeCoordinateSystem();

	//Updates RTCC clocks
	void TimeUpdate();
public:
	void AP7TPIPAD(const AP7TPIPADOpt &opt, AP7TPI &pad);
	void AP9LMTPIPAD(const AP9LMTPIPADOpt &opt, AP9LMTPI &pad);
	void AP9LMCDHPAD(const AP9LMCDHPADOpt &opt, AP9LMCDH &pad);
	void TLI_PAD(const TLIPADOpt &opt, TLIPAD &pad);
	bool PDI_PAD(const PDIPADOpt &opt, AP11PDIPAD &pad);
	void LunarAscentPAD(const ASCPADOpt &opt, AP11LMASCPAD &pad);
	void EarthOrbitEntry(const EarthEntryPADOpt &opt, AP7ENT &pad);
	void LunarEntryPAD(const LunarEntryPADOpt &opt, AP11ENT &pad);
	//Conic Fit
	int PCZYCF(double R1, double R2, double PHIT, double DELT, double VXI2, double VYI2, double VXF1, double VYF1, double SQRMU, int NREVS, int body, double &a, double &e, double &f_T, double &t_PT);
	int PMMTIS(EphemerisData sv_A1, EphemerisData sv_P1, double dt, double DH, double theta, EphemerisData &sv_A1_apo, EphemerisData &sv_A2, EphemerisData &sv_A2_apo);
	int PMSTICN_ELEV(EphemerisData sv_A1, EphemerisData sv_P1, double phi_req, double mu, double &T_ELEV);
	void PMSTICN(const TwoImpulseOpt &opt, TwoImpulseResuls &res);
	//Two-Impulse Single Solution
	void PMMTISS();
	void LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res);
	double TPISearch(SV sv_A, SV sv_P, double elev);
	double FindDH(SV sv_A, SV sv_P, double TIGguess, double DH);
	MATRIX3 REFSMMATCalc(REFSMMATOpt *opt);
	void EntryTargeting(EntryOpt *opt, EntryResults *res);//VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude, double &GET05G, double &RTGO, double &VIO, double &ReA, int &precision);
	void BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res);
	//RTCC module name PMMEPP
	void TranslunarInjectionProcessor(bool mpt, EphemerisData *sv = NULL, PLAWDTOutput *WeightsTable = NULL);
	//RTCC module name PMMLCP
	void TranslunarMidcourseCorrectionProcessor(EphemerisData sv0, double CSMmass, double LMmass);
	int LunarDescentPlanningProcessor(EphemerisData sv, double W_LM);
	bool GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG);
	OBJHANDLE AGCGravityRef(VESSEL* vessel); // A sun referenced state vector wouldn't be much of a help for the AGC...
	int DetermineSVBody(EphemerisData2 sv);
	void RotateSVToSOI(EphemerisData &sv);
	EphemerisData RotateSVToSOI(EphemerisData2 sv);
	void NavCheckPAD(SV sv, AP7NAV &pad, double GET = 0.0);
	void AGSStateVectorPAD(const AGSSVOpt &opt, AP11AGSSVPAD &pad);
	void AP11LMManeuverPAD(const AP11LMManPADOpt &opt, AP11LMMNV &pad);
	void AP11ManeuverPAD(const AP11ManPADOpt &opt, AP11MNV &pad);
	void AP10CSIPAD(const AP10CSIPADOpt &opt, AP10CSI &pad);
	void CSMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool docked);
	void LMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool docked, bool asc = false);
	void RTEMoonTargeting(RTEMoonOpt *opt, EntryResults *res);
	void LunarOrbitMapUpdate(EphemerisData sv0, AP10MAPUPDATE &pad, double pm = -150.0*RAD);
	void LandmarkTrackingPAD(const LMARKTRKPADOpt &opt, AP11LMARKTRKPAD &pad);
	//S-IVB TLI IGM Pre-Thrust Targeting Module
	int PMMSPT(PMMSPTInput &in);
	int PCMSP2(TLITargetingParametersTable *tlitab, int J, double t_D, double &cos_sigma, double &C3, double &e_N, double &RA, double &DEC);
	void LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step);
	void FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, bool agc = true);
	void FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_tig, SV &sv_cut, bool agc = true);
	void EngineParametersTable(int enginetype, double &Thrust, double &WLR, double &OnboardThrust);
	int PoweredFlightProcessor(PMMMPTInput in, double &GMT_TIG, VECTOR3 &dV_LVLH);
	void PoweredFlightProcessor(SV sv0, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, SV &sv_pre, SV &sv_post, bool agc = true);
	void PoweredFlightProcessor(SV sv0, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc = true);
	void PoweredFlightProcessor(EphemerisData sv0, double mass, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc = true);
	double GetDockedVesselMass(VESSEL *vessel) const;
	SV StateVectorCalc(VESSEL *vessel, double SVMJD = 0.0);
	EphemerisData StateVectorCalcEphem(VESSEL *vessel);
	void ExecuteManeuver(EphemerisData sv, PLAWDTOutput WeightsTable_before, double P30TIG, VECTOR3 dV_LVLH, int Thruster, EphemerisData &sv_after, PLAWDTOutput &WeightsTable_after);
	SV ExecuteManeuver(SV sv, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster);
	SV ExecuteManeuver(SV sv, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster, MATRIX3 &Q_Xx, VECTOR3 &V_G);
	void PMMENS(VECTOR3 R, VECTOR3 V, double dt_pf, double y_s, double theta_PF, double h_bo, double V_H, double V_R, double GMT_LO, VECTOR3 R_LS, VECTOR3 &R_BO, VECTOR3 &V_BO, double &GMT_BO);
	void PMMLTR(AEGBlock sv_CSM, double T_LO, double V_H, double V_R, double h_BO, double t_PF, double P_FA, double Y_S, double r_LS, double lat_LS, double lng_LS, double &deltaw0, double &DR, double &deltaw, double &Yd, double &AZP);
	void LLWP_PERHAP(AEGHeader Header, AEGDataBlock sv, double &RAP, double &RPE);
	void LLWP_HMALIT(AEGHeader Header, AEGDataBlock *sv, AEGDataBlock *sv_temp, int M, int P, int I_CDH, double DH, double &dv_CSI, double &dv_CDH, double &t_CDH);
	void LunarLaunchWindowProcessor(const LunarLiftoffTimeOpt &opt);
	bool LunarLiftoffTimePredictionDT(const LLTPOpt &opt, LunarLaunchTargetingTable &res);

	struct LunarAscentProcessorInputs
	{
		//Landing site vector in MCT coordinates
		VECTOR3 R_LS;
		//LM mass at liftoff
		double m0;
		//CSM state vector near liftoff
		EphemerisData sv_CSM;
		//GMT of liftof
		double t_liftoff;
		//Insertion velocity
		double v_LH, v_LV;
	};

	struct LunarAscentProcessorOutputs
	{
		//Ascent powered flight arc
		double theta;
		//Ascent powered flight time
		double dt_asc;
		//Ascent total DV
		double dv;
		//LM state vector at ignition
		EphemerisData sv_IG;
		//Cross range at ignition
		double CR;
		//Phase angle at insertion
		double phase;
		//LM state vector at insertion
		EphemerisData sv_Ins;
		//LM mass at insertion
		double m1;
	};

	void LunarAscentProcessor(const LunarAscentProcessorInputs &in, LunarAscentProcessorOutputs &out);
	bool PoweredDescentProcessor(VECTOR3 R_LS, double TLAND, SV sv, RTCCNIAuxOutputTable &aux, EphemerisDataTable2 *E, SV &sv_PDI, SV &sv_land, double &dv);
	void EntryUpdateCalc(SV sv0, double entryrange, bool highspeed, EntryResults *res);
	void PMMDKI(SPQOpt &opt, SPQResults &res);
	//Velocity maneuver performer
	void PCMVMR(AEGDataBlock &CHASER, AEGDataBlock &TARGET, double DELVX, double DELVY, double DELVZ, double mu, double &Pitch, double &Yaw, int I);
	void PCMVMR(VECTOR3 R_C, VECTOR3 V_C, VECTOR3 R_T, VECTOR3 V_T, double DELVX, double DELVY, double DELVZ, int I, VECTOR3 &V_C_apo, double &Pitch, double &Yaw);
	//Elevation angle search subroutine
	int PCTETR(SV sv_C, SV sv_T, double WT, double ESP, double &TESP, double &TR);
	//Apogee, perigee, and offset determination
	void PCPICK(AEGHeader header, AEGDataBlock sv_C, AEGDataBlock sv_T, double &DH, double &Phase, double &HA, double &HP);
	void PCPICK(SV sv_C, SV sv_T, double &DH, double &Phase, double &HA, double &HP);
	//Apogee and perigee radius magnitude
	void PCHAPE(double R1, double R2, double R3, double U1, double U2, double U3, double &RAP, double &RPE);
	//Plane change time and velocity increments computations
	void PMMPNE(AEGHeader Header, AEGDataBlock sv_C, AEGDataBlock sv_T, double TREF, double FNPC, int KPC, int IPC, AEGDataBlock &SAVE, double &DI1, double &DH1);
	//DKI phase lag routine
	void PMMPHL(DKICommon &DKI, AEGHeader aegh, AEGDataBlock sv_I, double TXX, double &TTPI, double &TTPF);
	//DKI maneuver convergence
	void PMMITL(DKICommon &DKI, AEGHeader aegh, AEGDataBlock *sv, int J);
	void PCMCEM(AEGHeader &h, AEGDataBlock &sv_M, AEGDataBlock &sv_I, double mu);
	bool DockingInitiationProcessor(DKIOpt opt);
	int ConcentricRendezvousProcessor(const SPQOpt &opt, SPQResults &res);
	double CalculateTPITimes(SV sv0, int tpimode, double t_TPI_guess, double dt_TPI_sunrise);
	VECTOR3 LOICrewChartUpdateProcessor(EphemerisData sv0, MATRIX3 REFSMMAT, double p_EMP, double LOI_TIG, VECTOR3 dV_LVLH_LOI, double p_T, double y_T);
	SV coast(SV sv0, double dt);
	EphemerisData coast(EphemerisData sv1, double dt);
	EphemerisData coast(EphemerisData sv1, double dt, int veh);
	EphemerisData coast(EphemerisData sv1, double dt, double Weight, double Area, double KFactor, bool Venting);
	VECTOR3 HatchOpenThermalControl(double GMT, MATRIX3 REFSMMAT);
	VECTOR3 PointAOTWithCSM(MATRIX3 REFSMMAT, EphemerisData sv, int AOTdetent, int star, double dockingangle);
	void DockingAlignmentProcessor(DockAlignOpt &opt);
	VECTOR3 SkylabDockingAttitude(EphemerisData sv, MATRIX3 REFSMMAT, double DDA = 0.0);
	AEGBlock SVToAEG(EphemerisData sv, double Area, double Weight, double KFactor);
	//Apsides Determination Subroutine
	int PMMAPD(AEGHeader Header, AEGDataBlock Z, int KAOP, int KE, double *INFO, AEGDataBlock *sv_A, AEGDataBlock *sv_P);
	bool GETEval2(double get);
	bool PDIIgnitionAlgorithm(SV sv, VECTOR3 R_LS, double TLAND, SV &sv_IG, double &t_go, double &CR, VECTOR3 &U_IG, MATRIX3 &REFSMMAT);
	bool PoweredDescentAbortProgram(PDAPOpt opt, PDAPResults &res);
	MATRIX3 GetREFSMMATfromAGC(agc_t *agc, bool cmc);
	bool CalculateAGSKFactor(agc_t *agc, ags_t *aea, double &KFactor);

	//Actual RTCC Subroutines

	//AEG Service Routine
	void PMMAEGS(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out);
	//Time of Longitude Crossing Determination
	void PMMTLC(AEGHeader HEADER, AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double DESLAM, int &K, int INDVEC);
	//AEG Day/Night Determination
	void PMMDAN(AEGHeader Header, AEGDataBlock aeg, int IND, int &ERR, double &T1, double &T2);
	//Checkout Monitor Display
	void EMDCHECK(int veh, int opt, double param, double THTime, int ref, bool feet);
	//Detailed Maneuver Table Display
	void PMDDMT(int MPT_ID, unsigned ManNo, int REFSMMAT_ID, bool HeadsUp, DetailedManeuverTable &res);
	//Lunar Descent Planning Table Display
	void PMDLDPP(const LDPPOptions &opt, const LDPPResults &res, LunarDescentPlanningTable &table);
	//LEM gimbal angle + FDAI angle computation routine
	void EMGLMRAT(VECTOR3 X_P, VECTOR3 Y_P, VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll, double &PB, double &YB, double &RB);
	//GOST LM IMU/FDAI Angle Conversion
	VECTOR3 EMMGFDAI(VECTOR3 Att, bool IsIMU) const;
	//Vector rotation routine
	VECTOR3 GLMRTV(VECTOR3 A, double THET1, int K1, double THET2 = 0.0, int K2 = 0.0, double THET3 = 0.0, int K3 = 0);
	//Matrix rotation routine
	MATRIX3 GLMRTM(MATRIX3 A, double THET1, int K1, double THET2 = 0.0, int K2 = 0.0, double THET3 = 0.0, int K3 = 0);
	//Zero to Two Pi Arctangent Subroutine
	double GLQATN(double S, double C) const;
	//Cape Crossing Table Generation
	int RMMEACC(int L, int ref_frame, int ephem_type, int rev0);
	//Ascending Node Computation
	int RMMASCND(EphemerisDataTable2 &EPHEM, ManeuverTimesTable &MANTIMES, double GMT_min, double &lng_asc);
	//Environment Change Calculations
	struct EMMENVInputTable
	{
		double GMT;			//Threshold GMT for the environment change search
		int option;			//0 = Sun, 1 = Moon, 2 = star
		bool terminator;	//Search for terminator of above condition
		bool present;		//Search for condition being present (e.g. sunshine for sunrise)
		VECTOR3 u_vec; //Star unit vector for option 2
	};

	struct EMMENVOutputTable
	{
		double T_Change = 0.0; //Time of environment change
		int err = 0; //Error condition: 0 = no error, 1 = too many iterations, 2 = requested GMT not in ephemeris limits, 3 = ran out of ephemeris, 4 = interpolation error, 5 = fatal ephemeris error (not available or wrong coordinate system)
		bool IsActualChange = false; //Condition already existed at input GMT
	};

	bool EMMENVCondition(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, LunarStayTimesTable *LUNRSTAY, double GMT, int option, bool terminator, VECTOR3 u_vec, int &err);
	void EMMENV(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, LunarStayTimesTable *LUNRSTAY, const EMMENVInputTable &in, EMMENVOutputTable &out);
	
	//Sunrise/Sunset, Moonrise/Moonset Display
	void EMDSSMMD(bool sun, int ind, double param);
	int NewMPTTrajectory(int L, SV &sv0);
	//RTE Tradeoff Display Sort and Order Routine
	int PMQREAP(const std::vector<TradeoffData> &TOdata);
	//Return to Earth Abort Planning Supervisor
	void PMMREAP(int med);
	//Return to Earth Abort Planning Supervisor (Abort Scan Table)
	void PMMREAST(int med, EphemerisData *sv = NULL);
	//Return to Earth Abort Planning Supervisor (RTED)
	void PMMREDIG(bool mpt);
	bool DetermineRTESite(std::string Site);
	//RTE Trajectory Computer
	bool PCMATC(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	//Return to Earth Digital Supervisor
	void PMMPAB(const RTEDMEDData &MED, const RTEDASTData &AST, const RTEDSPMData &SPM, MATRIX3 &RFS, RTEDigitalSolutionTable &RID, int &IRED);
	//RTE Digital Reentry Subroutine
	void PCRENT(PCMATCArray &FD, const RTEDMEDData &IMD, const RTEDSPMData &SPS, double PHIMP, double LIMP, RTEDigitalSolutionTable &RED, int &ICC);
	//Lunar Orbit Insertion Computational Unit
	bool PMMLRBTI(EphemerisData sv);
	//Lunar Orbit Insertion Display
	void PMDLRBTI(const rtcc::LOIOptions &opt, const rtcc::LOIOutputData &out);
	//Central Manual Entry Device Decoder
	bool GMGMED(char *str);
	//MED Decoder for G, A and B MEDs
	void EMGABMED(int type, std::string med, std::vector<std::string> data, int &err, unsigned &param);
	//MED Decoder for C MEDs
	void CMRMEDIN(std::string med, std::vector<std::string> data, int &err, unsigned &param);
	//'F' MED Module
	int PMQAFMED(std::string med);
	int PMQAFMED(std::string med, std::vector<std::string> data);
	//K-MED Decoder
	void PMKMED(std::string med, std::vector<std::string> data, int &err, unsigned &param);
	//'M' MED Module
	int PMMMED(std::string med, std::vector<std::string> data);
	//'P' Code MED Processor
	int GMSMED(std::string med, std::vector<std::string> data);
	//'U' Code MED Processor
	int EMGTVMED(std::string med, std::vector<std::string> data);
	//Restart MED Decoder
	int GMSREMED(std::string med, std::vector<std::string> data);
	//High-Speed Restart MED Processor
	void GMLRESRT(int type);
	//Detailed Maneuver Table Math Module
	void PMMDMT(int L, unsigned man, RTCCNIAuxOutputTable *aux);
	//Skeleton flight plan interpolation program
	void PMMSFPIN();
	//Time Queue Control Load Module
	void EMSTIME(int L, int ID);
	void FDOLaunchAnalog1(EphemerisData sv);
	void FDOLaunchAnalog2(EphemerisData sv);
	double GetGMTLO() { return SystemParameters.MCGMTL; }
	void SetGMTLO(double gmt) { SystemParameters.MCGMTL = gmt; }
	double CalcGETBase();
	double GetGMTBase() { return SystemParameters.GMTBASE; }
	void SetGMTBase(double gmt) { SystemParameters.GMTBASE = gmt; }
	double GETfromGMT(double GMT) const;
	double GMTfromGET(double GET) const;
	double GetCMCClockZero() { return SystemParameters.MCGZSA * 3600.0; }
	double GetLGCClockZero() { return SystemParameters.MCGZSL * 3600.0; }
	double GetIUClockZero() { return SystemParameters.MCGRIC * 3600.0; }
	double GetAGSClockZero() { return SystemParameters.MCGZSS * 3600.0; }
	double GetIULaunchAzimuth() { return SystemParameters.MCLABN; }

	//Mission Planning

	//Maneuver Transfer Module
	int PMMXFR(int id, void* data);
	//Maneuver direct input and confirmation math module
	int PMMMCD(PMMMCDInput in, MPTManeuver &man);
	//Impulsive Maneuver Transfer Math Module
	int PMMMPT(PMMMPTInput &in, MPTManeuver &man);
	//Lunar Ascent Integrator
	int PMMLAI(PMMLAIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable2 *E = NULL);
	//LM Lunar Descent Numerical Integration Module
	int PMMLDI(PMMLDIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable2 *E = NULL);
	//LM Lunar Descent Pre-Thrust Targeting Module
	int PMMLDP(PMMLDPInput in, MPTManeuver &man);
	//Coast Numerical Integrator
	void PMMCEN(EphemerisData sv, double tmin, double tmax, int opt, double endcond, double dir, EphemerisData &sv_out, int &ITS);
	//Freeze, Unfreeze, Delete Processor
	void PMMFUD(int veh, unsigned man, int action, std::string StationID);
	//Vehicle Orientation Change Processor
	void PMMUDT(int L, unsigned man, int headsup, int trim);
	//Vector Routing Load Module
	void PMSVCT(int QUEID, int L);
	void PMSVCT(int QUEID, int L, StateVectorTableEntry sv0);
	//Vector Fetch Load Module
	int PMSVEC(int L, double GMT, CELEMENTS &elem, double &KFactor, double &Area, double &Weight, std::string &StaID, int &RBI);
	//Maneuver Execution Program
	void PMSEXE(int L, double gmt);
	//Earth Orbit Insertion Processor
	void PMMIEV(int L, double T_L);
	//SLV Targeting Load Module
	void PMMPAR(VECTOR3 RT, VECTOR3 VT, double TT);
	//Perigee Adjust
	void PMMPAD(AEGBlock sv, double mass, double THT, double dt, double H_P, int Thruster, double DPSScaleFactor);
	//Perigee Adjust Display
	void PMDPAD();
	//Mission Planning Print Load Module
	void PMXSPT(std::string source, int n);
	void PMXSPT(std::string source, std::vector<std::string> message);
	void OnlinePrintTimeDDHHMMSS(double TIME_SEC, std::string &time);
	void OnlinePrintTimeHHHMMSS(double TIME_SEC, std::string &time);
	void OnlinePrint(const std::string &source, const std::vector<std::string> &message);
	//Mission Control Print Program
	void GMSPRINT(std::string source, int n);
	void GMSPRINT(std::string source, std::vector<std::string> message);
	//Trajectory Update Control Module
	void EMSTRAJ(StateVectorTableEntry sv, int L);
	//Ephemeris Storage and Control Module
	void EMSEPH(int QUEID, StateVectorTableEntry &sv0, int &L, double PresentGMT);
	//Miscellaneous Numerical Integration Control Module
	void EMSMISS(EMSMISSInputTable *in);
	//Lunar Surface Ephemeris Generator
	void EMSLSF(EMSLSFInputTable &in);
	//Encke Integrator
	void EMMENI(EMMENIInputTable &in);
	//Spherical to inertial conversion
	int EMMXTR(double GMT, double rmag, double vmag, double rtasc, double decl, double fpav, double az, int RBI, VECTOR3 &R, VECTOR3 &V);
	//Orbital Elements Computations
	void EMMDYNEL(EphemerisData sv, TimeConstraintsTable &tab);
	//Anchor Vector Maintenance Module
	void EMGVECSTInput(int L, StateVectorTableEntry sv);
	int EMGVECSTOutput(int L, EphemerisData &sv);
	//Fetch nearest free flight vector
	int EMSFFV(double gmt, int L, EphemerisData &sv);
	int ThrusterNameToCode(std::string thruster);
	int AttitudeNameToCode(std::string attitude);

	bool MPTHasManeuvers(int L);
	//Weight Change Module
	int PMMWTC(int med);
	double GetOnboardComputerThrust(int thruster);
	void GetSystemGimbalAngles(int thruster, double &P_G, double &Y_G) const;
	double RTCCPresentTimeGMT();
	double RTCCMissionTime(int veh);
	OBJHANDLE GetGravref(int body);
	bool RTEManeuverCodeLogic(char *code, double lmascmass, double lmdscmass, int UllageNum, int &thruster, int &AttMode, int &ConfigCode, int &ManVeh, double &lmmass);

	// **MISSION PROGRAMS**

	// TRAJECTORY DETERMINATION (B)

	//Vector Comparison Display
	void BMDVEC();
	//Vector Panel Summary Display
	void BMDVPS();
	//Online print of trajectory determination
	void BMGPRIME(std::string source, int n);
	void BMGPRIME(std::string source, std::vector<std::string> message);
	//D.C. MED Decoder
	int BMQDCMED(std::string med, std::vector<std::string> data);
	//Vector Comparison Control
	void BMSVEC();
	//Vector Panel Summary Control
	void BMSVPS(int queid, int PBIID);
	int BMSVPSVectorFetch(const std::string &vecid, EphemerisData &sv_out);
	int GetStateVectorTableEntry(std::string VectorType, int mpt);

	// DIGITAL COMMAND SYSTEM (C)

	//CMC/LGC Navigation Update
	void CMMCMNAV(int veh, int mpt, double GETSV, int ref);
	void CMMCMNAV(int veh, int mpt, EphemerisData sv);
	//CMC External Delta-V Update Display
	void CMDAXTDV();
	//CMC External Delta-V Update Generator
	void CMMAXTDV(double GETIG, VECTOR3 DV_EXDV, int mpt = 0, unsigned man = 0);
	//LGC External Delta-V Update Generator
	void CMMLXTDV(double GETIG, VECTOR3 DV_EXDV, int mpt = 0, unsigned man = 0);
	//CMC and LGC REFSMMAT Update Generator
	void CMMRFMAT(int L, int id, int addr);
	//SLV Navigation Update
	void CMMSLVNAV(VECTOR3 R_ecl, VECTOR3 V_ecl, double GMT);
	//CMC/LGC Landing Site Update Load Generator
	void CMMCMCLS(int veh);
	//LGC Descent Target Update Load Generator
	void CMMDTGTU(double t_land);
	//Retrofire External Delta V Update Generator
	void CMMRXTDV(int source, int column);
	//Entry Update Generator
	void CMMENTRY(double lat, double lng);
	//CSM/LM Liftoff Update
	void CMMLIFTF(int L, double hrs);
	//CSM/LM Time Increment Update Generator
	void CMMTMEIN(int L, double hrs);
	//CMC/LGC Erasable Memory Update
	void CMMERMEM(int blocknum, int med, int line, const std::vector<int> &data);

	// MISSION CONTROL (G)

	//LEM AGS Navigation Updates Display
	void EMDAGSN(double GMT, int refs, int body);
	//Guidance Optics Display Supervisor
	void EMSGSUPP(int QUEID, int refs, int refs2 = -1, unsigned man = -1, bool headsup = true);
	//LEM Optics Supervisor
	void EMSLSUPP(int QUEID, int refs, int refs2 = -1, unsigned man = -1, bool headsup = true);
	//GOST REFSMMAT Maintenance
	void EMGSTGEN(int QUEID, int L1, int ID1, int L2, int ID2, double gmt, MATRIX3 *refs = NULL);
	//GOST Matrix Storage and Print
	void EMGSTSTM(int L, MATRIX3 REFS, int id, double gmt);
	void EMGSTGENName(int ID, char *Buffer);
	int EMGSTGENCode(const char *Buffer);
	//GOST REFSMMAT Maintenance
	void FormatREFSMMATCode(int ID, int num, char *buff);
	//Sun-Moon-Earth Occultation
	bool EMMGSTCK(VECTOR3 u_star, VECTOR3 R, int body, VECTOR3 R_EM, VECTOR3 R_ES);
	//Guidance Optics Support Table
	void EMMGSTMP();
	//Guidance Optics Display
	void EMDGSUPP(int err);
	//LEM Guidance Optics MSK Math
	void EMMGLMST(int mode);
	//LEM Optics Support Table Display
	void EMDGLMST();
	//FDO Orbit Digitals
	void EMMDYNMC(int L, int queid, int ind = 0, double param = 0.0);
	//FDO Space Digitals
	int EMDSPACE(int queid, int option = 0, double val = 0.0, double incl = 0.0, double ascnode = 0.0);
	int EMDSPACENoMPT(SV sv0, int queid, double gmt, double incl = 0.0, double ascnode = 0.0);
	//Orbit Station Contact Generation Control
	void EMSTAGEN(int L);
	//Generalized Contact Generator
	void EMGENGEN(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, const StationTable &stationlist, int body, OrbitStationContactsTable &res, LunarStayTimesTable *LUNSTAY = NULL);
	//Horizon Crossing Subprogram
	int EMXING(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, const StationData & station, int body, std::vector<StationContact> &acquisitions, LunarStayTimesTable *LUNSTAY);
	bool EMXINGLunarOccultation(EphemerisDataTable2 &ephemeris, ManeuverTimesTable &MANTIMES, double gmt, VECTOR3 R_S_equ, double &g_func, LunarStayTimesTable *LUNSTAY);
	//Next Station Contact Display
	void EMDSTAC();
	//Predicted Site Acquisition Display
	void EMDPESAD(int num, int veh, int ind, double vala, double valb, int body);
	//Ground Range and Altitude Subprogram
	void ECMEXP(EphemerisData sv, StationData *stat, int statbody, double &range, double &alt);
	//Landmark Acquisition Display
	void EMDLANDM(int L, double gmt, double dt, int ref);
	//Ground Point Characteristics Block Routine
	void EMGGPCHR(double lat, double lng, double alt, int body, double GHA, StationData *stat);
	//Display Updates
	void EMSNAP(int L, int ID);

	// LAUNCH/HIGH SPEED ABORT (L)

	//Platform Initialization Routine
	void LMMGRP(int veh, double gmt);
	//Launch On-Line Print
	void LMXPRNTR(std::string source, int n);
	void LMXPRNTR(std::string source, std::vector<std::string> message);
	//Orbit Determination Subroutine
	int LLBRTD(EphemerisData sv, int I, double SQMU, double PARM, EphemerisData &sv_out);
	int LLBRTD(EphemerisData sv, int I, double SQMU, double PARM, double H, double SQRAT, EphemerisData &sv_out, double &R_E);

	// **LIBRARY PROGRAMS**
	// TRAJECTORY DETERMINATION (B)
	//TBD
	// ORBIT TRAJECTORY COMPUTATIONS (E)
	//Ephemeris Fetch Routine
	int ELFECH(double GMT, int L, EphemerisData &SV);
	int ELFECH(double GMT, unsigned vec_tot, unsigned vec_bef, int L, EphemerisDataTable2 &EPHEM, ManeuverTimesTable &MANTIMES, LunarStayTimesTable &LUNSTAY);
	void ELGLCV(double lat, double lng, VECTOR3 &out, double rad = 0.0);
	void ELGLCV(double lat, double lng, MATRIX3 &out, double rad = 0.0);
	//Vector Count Routine
	int ELNMVC(double &TL, double TR, int L, unsigned &NumVec, int &TUP);
	//Variable Order Interpolation
	int ELVARY(EphemerisDataTable2 &EPH, unsigned ORER, double GMT, bool EXTRAP, EphemerisData2 &sv_out, unsigned &ORER_out);
	//Generalized Coordinate System Conversion Subroutine
	int ELVCNV(EphemerisData sv, int out, EphemerisData &sv_out);
	int ELVCNV(EphemerisData2 sv, int in, int out, EphemerisData2 &sv_out);
	int ELVCNV(double GMT, int in, int out, MATRIX3 &Rot);
	int ELVCNV(VECTOR3 vec, double GMT, int type, int in, int out, VECTOR3 &vec_out);
	int ELVCNV(std::vector<EphemerisData2> &svtab, int in, int out, std::vector<EphemerisData2> &svtab_out);
	int ELVCNV(std::vector<VECTOR3> VECTORS, double GMT, int type, int in, int out, std::vector<VECTOR3> &OUTPUT);

	//Extended Interpolation Routine
	void ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable2 &out);
	void ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable2 &out, EphemerisDataTable2 &EPH, ManeuverTimesTable &mantimes, LunarStayTimesTable *LUNRSTAY = NULL);
	// MISSION CONTROL (G)
	//Fixed Point Centiseconds to Floating Point Hours
	double GLCSTH(double FIXCSC);
	//Floating Point Hourse To Fixed Point Centiseconds
	double GLHTCS(double FLTHRS);
	//Subsatellite Position
	int GLSSAT(VECTOR3 R, double GMT, int RBI, double &lat, double &lng, double &alt);
	//Density
	void GLFDEN(double ALT, double &DENS, double &SPOS);

	struct GLUNIVInput
	{
		//Time of input vector
		double T0;
		//Input position vector
		VECTOR3 R0;
		//Input velocity vector
		VECTOR3 V0;
		//Option. 0 = time, 1 = height, 2 = radius, 3 = true anomaly change, 4 = flight path angle
		int Ind;
		//Square root of gravitational constant
		double SQRMU;
		//Time of desired vectors (option 0), estimated Earth radius at desired height (option 1), desired position vector magnitude (option 2),
		//desired change in true anomaly (-2PI < theta < 2PI, option 3), desired flight path angle (option 4)
		double PARM;
		//Desired height (option 1), +1 if output is desired from perifocus to apofocus, -1 if output is desired from apofocus to perifocus (option 2)
		double PARM2;
		//square of ratio of equatorial to polar Earth radii (option 1), +1 if we are to propagate forward in direction of orbital motion, -1 if we are to propagate backward (option 2)
		//+1 if we are to propagate forward in direction of orbital motion, -1 if we are to propagate backward (option 4)
		double PARM3;
	};
	struct GLUNIVOutput
	{
		//Time of output vectors
		double T1;
		//Output position vector
		VECTOR3 R1;
		//Output velocity vector
		VECTOR3 V1;
		//Error Code
		int ERR;
		//Computed value of the Earth radius at the desired height (option 1)
		double R_NEW;
	};
	void GLUNIV(const GLUNIVInput &in, GLUNIVOutput &out);
	// MISSION PLANNING (P)
	//Weight Determination at a Time
	void PLAWDT(const PLAWDTInput &in, PLAWDTOutput &out);
	bool PLEFEM(int IND, double HOUR, int YEAR, VECTOR3 *R_EM, VECTOR3 *V_EM, VECTOR3 *R_ES, MATRIX3 *PNL);

	// REENTRY COMPUTATIONS (R)
	//Coefficients of lift and drag interpolation subroutine
	void RLMCLD(double FMACH, int VEH, double &CD, double &CL);
	void RLMCLD(double FMACH, int VEH, double &CD, double &CL, double &ALFA);
	//Computes and outputs pitch, yaw, roll
	void RLMPYR(VECTOR3 X_P, VECTOR3 Y_P, VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll);
	//Time of Longitude Crossing Subroutine
	double RLMTLC(EphemerisDataTable2 &EPHEM, ManeuverTimesTable &MANTIMES, double long_des, double GMT_min, double &GMT_cross, EphemerisData2 &sv, LunarStayTimesTable *LUNRSTAY = NULL);
	//Reentry numerical integrator
	void RMMYNI(const RMMYNIInputTable &in, RMMYNIOutputTable &out);
	//Reentry Constant G Iterator
	void RMMGIT(EphemerisData2 sv_EI, double lng_T);
	//Retrofire Planning Control Module
	void RMSDBMP(EphemerisData sv, double CSMmass);
	//Recovery Target Selection Display
	void RMDRTSD(EphemerisDataTable2 &tab, int opt, double val, double lng);
	//Reentry MED Decoder
	int RMRMED(std::string med, std::vector<std::string> data);
	//Spacecraft Setting Control
	void RMSSCS(int entry);
	//External DV Parameters
	void RMDRXDV(bool rte);

	// **INTERMEDIATE LIBRARY PROGRAMS**
	// MISSION CONTROL (G)
	//Gimbal, Thrust and Weight Loss Rate Subroutine
	void GIMGBL(double CSMWT, double LMWT, double &RY, double &RZ, double &T, double &WDOT, int ITC, unsigned &IC, int IA, int IJ, double D);
	VECTOR3 GIMGB2(const double *WArr, const VECTOR3 *VecArr, int N, double W);
	// MISSION PLANNING (P)
	//LM AGS External DV Coordinate Transformation Subroutine
	VECTOR3 PIAEDV(VECTOR3 DV, VECTOR3 R_CSM, VECTOR3 V_CSM, VECTOR3 R_LM, bool i);
	//Right ascension of greenwich at Time T
	double PIAIES(double hour);
	//TBD: PIAIPP
	//TBD: PIANGL
	//TBD: PIAQRE
	//Arrival time at selenographic argument of latitude
	int PIATSU(AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double &isg, double &gsg, double &hsg);
	//Series Function Subroutine
	void PIBETA(double BETA, double ONOVA, double *F);
	//TBD: PIBGAM
	//Hour angle with Besselian input time
	double PIBSHA(double hour);
	//Impulse Analytical Burn
	void PIBURN(VECTOR3 R, VECTOR3 V, double T, double *B, VECTOR3 &ROUT, VECTOR3 &VOUT, double &TOUT);
	//TBD: PICNEL
	//TBD: PICREF
	//Coordinate transformation
	void PICSSC(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &lat, double &lng, double &gamma, double &azi);
	//Orbit Desired REFSMMAT Computation Subroutine
	MATRIX3 PIDREF(VECTOR3 AT, VECTOR3 R, VECTOR3 V, double PG, double YG, bool K);
	//TBD: PIDTFN
	//TBD: PIDVRS
	//TBD: PIEBETA
	//TBD: PIEMNT
	//External DV Coordinate Transformation Subroutine
	VECTOR3 PIEXDV(VECTOR3 R_ig, VECTOR3 V_ig, double WT, double T, VECTOR3 DV, bool i);
	//Apogee/perigee magnitude determination
	void PIFAAP(double a, double e, double i, double f, double u, double r, double &r_apo, double &r_peri);
	//TBD: PIFTCH
	//Calc. Greenwich hour angle at midnight preceeding launch
	double PIGBHA();
	//GMT hour angle
	double PIGMHA(double hour);
	//Universal Cartesian to Kepler Coordinates
	void PIMCKC(VECTOR3 R, VECTOR3 V, int body, double &a, double &e, double &i, double &l, double &g, double &h);
	//Time from perifocal pass to radius
	void PITFPC(double MUE, int K, double AINV, double p, double rad, double &TIME, double &PER, double &e) const;
	//Determine time of arrival at specific height in orbit
	int PITCIR(AEGHeader header, AEGDataBlock in, double R_CIR, AEGDataBlock &out);
	//Generate orbit normal and ascending node vectors from elements, and vice versa
	void PIVECT(VECTOR3 P, VECTOR3 W, double &i, double &g, double &h);
	void PIVECT(double i, double g, double h, VECTOR3 &P, VECTOR3 &W);

	// ** MISCELLANEOUS UTILITY PROGRAMS**
	//Sun/Moon ephemeris table from tape
	void QMEPHEM(int EPOCH, int YEAR, int MONTH, int DAY, double HOURS);
	//Sun-Moon ephemeris offline
	bool QMGEPH(int epoch, double gmtbase, double HOURS);
	//Reading of P&N ephemeris table
	void QMPNREAD(double gmtbase);

	// **AUXILIARY SUBROUTINES**
	//Delta True Anomaly Function
	double PCDETA(double beta1, double beta2, double r1, double r2);
	//Cotangens Routine
	double DCOTAN(double ang);

	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state
	void clbkPostCreation();

	MCC *mcc;
	struct calculationParameters calcParams;

	//RTCC file names, equivalent to tapes containing the data
	std::string SystemParametersFile;
	std::string	TLIFile;
	std::string SFPFile;

	//MEDs

	//Generate the near Earth tradeoff display
	struct MED_F70
	{
		std::string Site = "No Site!";
		//Time of vector (hrs.)
		double T_V = 0.0;
		//Minimum abort time (hrs.)
		double T_omin = 0.0;
		//Maximum abort time (hrs.)
		double T_omax = 0.0;
		//1 = Constant G reentry (HB1), 2 = G&N reentry (HGN)
		int EntryProfile = 1;
	} med_f70;

	//Generate the remote Earth tradeoff display
	struct MED_F71
	{
		int Page = 1;
		std::string Site = "No Site!";
		double T_V = 0.0;
		double T_omin = 0.0;
		double T_omax = 0.0;
		//1 = Constant G reentry (HB1), 2 = G&N reentry (HGN)
		int EntryProfile = 1;
	} med_f71;

	//Common variables of MED F75-77
	struct MED_F75_F77
	{
		std::string EntryProfile = "HB1";
		double Inclination = 0.0;
		double T_V = 0.0;		//Vector time
		double T_0_min = 0.0;	//Time of abort (or minimum time for F77)
		double T_Z = 0.0;		//Estimated time of landing (F76 and F77)
	} med_f75_f77;

	//Generation of Abort Scan Table for unspecified area
	struct MED_F75
	{
		std::string Type = "FCUA";
		double DVMAX = 10000.0;
		
	} med_f75;

	//Abort Scan Table generation for a specific site
	struct MED_F76
	{
		std::string Site = "MPL";
		double MissDistance = 0.0;
	} med_f76;

	//AST lunar search generation for specific site or FCUA
	struct MED_F77
	{
		std::string Site = "MPL";
		double T_max = 0.0;
		double MissDistance = 0.0;
		
	} med_f77;

	//RTE Digitals maneuver description
	struct MED_F80
	{
		int Column = 1; //1 = Primary, 2 = Manual
		int ASTCode = 101;
		std::string REFSMMAT = "CUR";
		std::string ManeuverCode = "CSUX"; //e.g. CSUX for CSM, SPS, undocked, External DV
		int NumQuads = 4; //2 or 4
		double UllageDT = 15.0;
		int TrimAngleInd = -1; //-1 = compute, 1 = system parameters
		double DockingAngle = 60.0*RAD;
		bool HeadsUp = true;
		bool Iterate = true; //false = "single"
	} med_f80;

	//RTE digitals manual maneuver input
	struct MED_F81
	{
		double VectorTime = 0.0;
		double IgnitionTime = 0.0;
		double lat_tgt = 0.0;
		double lng_tgt = 0.0;
		int RefBody = BODY_EARTH;
		VECTOR3 XDV = _V(0, 0, 0);
	} med_f81;

	//RTE digitals entry profile
	struct MED_F82
	{
		std::string PrimaryEP = "HGN";
		double PrimaryInitialBank = 0.0;
		double PrimaryGLIT = 0.05;
		std::string PrimaryRollDirection = "N";
		double PrimaryLongT = 9999.9;
		std::string BackupEP = "HB1";
		double BackupInitialBank = 0.0;
		double BackupGLIT = 2.0;
		std::string BackupRollDirection = "N";
		double BackupLongT = 9999.9;
	} med_f82;

	//Update return to Earth constraints
	struct MED_F86
	{
		std::string Constraint;
		double Value;
	} med_f86;

	//Update return to Earth constraints
	struct MED_F87
	{
		std::string Constraint;
		std::string Value;
	} med_f87;

	//Generate DKI
	struct MED_K00
	{
		//false = normal DKI, true = Skylab four-maneuver route.
		bool I4 = false;
		//Maneuver line point for the phasing maneuver
		double NC1 = 1.0;
		//Maneuver line point for the height maneuver
		double NH = 1.5;
		//Maneuver line point for the NCC maneuver
		double NCC = 4.0;
		//Maneuver line point for the coelliptic maneuver
		double NSR = 2.0;
		//Maneuver line point for the plane change maneuver
		double NPC = -1.0;
		//M-line or maneuver line number at which rendezvous is to take place
		double MI = 3.0;
		//DT between NCC and NSR maneuver (Skylab)
		double dt_NCC_NSR = 37.0*60.0;

		int ChaserVehicle = 1; //1 = CSM, 3 = LEM
	} med_k00;

	//Generate SPQ
	struct MED_K01
	{
		int ChaserVehicle = 1; //1 = CSM, 3 = LEM
		double ChaserThresholdGET = -1.0;
		double TargetThresholdGET = -1.0;
		//1 = CDH at upcoming apsis (AEG), 2 = CDH on time, 3 = angle from CSI, 4 = CDH at upcoming apsis (Keplerian)
		int I_CDH = 3;
		//For option 1
		int CDH_Apsis = 1;
		//For option 2
		double CDH_Time = 0.0;
		//For option 3
		double CDH_Angle = PI;
	} med_k01;

	//Maneuver Line Definition Initialization
	struct MED_K10
	{
		int MLDOption = 1; //1 = input time, 2 = apoapsis, 3 = inactive vehicle apsis
		double MLDTime = 0.0;
		double MLDValue = 1.0;
		std::string ChaserVectorID;
		std::string TargetVectorID;
	} med_k10;

	//Generate Launch Window
	struct MED_K15
	{
		//1 = CSM, 3 = LEM
		int Chaser = 3;
		double CSMVectorTime = 0.0;
		double ThresholdTime = 0.0;
		//CSI Flag: 0 = CSI done 90° from insertion, negative: CSI at LM apocynthion, positive: CSI done at a delta time from insertion
		double CSI_Flag = -1.0;
		//CDH Flag: 0 = CDH done at upcoming apsis after CSI, positive: CDH is done at N/2 after CSI, must be odd number
		int CDH_Flag = 1;
		//1 = TPI at longitude, 2 = TPI at time
		int TPIDefinition = 2;
		double TPIValue = 0.0;
		//Positive: request for complete launch window and the number of delta hts, negative: request for the input delta hts only
		int DeltaHTFlag = -1;
		double DH1 = 10.0*1852.0;
		double DH2 = 15.0*1852.0;
		double DH3 = 20.0*1852.0;
	} med_k15;

	//LOI Computation
	struct MED_K18
	{
		double HALOI1 = 170.0;
		double HPLOI1 = 60.0;
		double DVMAXp = 10000.0;
		double DVMAXm = 10000.0;
		double psi_DS = 270.0;
		double psi_MX = 271.0;
		double psi_MN = 269.0;
		double VectorTime = 0.0;
	} med_k18;

	//GPM Maneuver Computation
	struct MED_K20
	{
		int Vehicle = 1; //1 = CSM, 3 = LM
		double VectorTime = 0.0;
		double ThresholdTime = 0.0;
	} med_k20;

	//Perifocus Adjust Computation
	struct MED_K28
	{
		int VEH = RTCC_MPT_CSM;
		double VectorTime = 0.0;
		double ThresholdTime = 0.0;
		double TimeIncrement = 0.0;
		double H_P = 0.0;
		int Thruster = RTCC_ENGINETYPE_CSMSPS;
		double DPSScaleFactor = 0.0;
	} med_k28;

	//Two Impulse Computation
	struct MED_K30
	{
		int Vehicle = 1; //1 = CSM, 3 = LEM
		int IVFlag = 0; //0 = Time of both maneuvers fixed, 1 = Time of first maneuver fixed, 2 = time of second maneuver fixed
		double ChaserVectorTime = 0.0;
		double TargetVectorTime = 0.0;
		double StartTime = 0.0;
		double EndTime = 0.0;
		double TimeStep = 60.0;
		double TimeRange = 600.0;
	} med_k30;

	//Lunar Launch Targeting Processor (Apollo 14 and later, MED code is not from any documentation!)
	struct MED_K50
	{
		double GETTH = 0.0;
		double GETV = 0.0;
	} med_k50;

	//Transfer a GPM to the MPT
	struct MED_M65
	{
		int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
		int ReplaceCode = 0; //1-15
		int Thruster = RTCC_ENGINETYPE_CSMSPS; //Thruster for the maneuver
		int Attitude = RTCC_ATTITUDE_PGNS_EXDV;		//Attitude option
		double UllageDT = -1;	//Delta T of Ullage
		bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
		bool Iteration = false; //false = do not iterate, true = iterate
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
		bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
	} med_m65;

	//Direct input of a maneuver to the MPT (CSM and LEM)
	struct MED_M66
	{
		int Table = 1; //1 = CSM, 3 = LEM
		unsigned ReplaceCode = 0; //1-15
		double GETBI = 0; //Time of ignition
		int Thruster = RTCC_ENGINETYPE_CSMSPS; //Thruster for maneuver
		int AttitudeOpt = RTCC_ATTITUDE_PGNS_EXDV; //Attitude option
		int BurnParamNo = 2; //1 = P1, 2 = P2 etc.
		int CoordInd = 0; //0 = LVLH, 1 = IMU, 2 = FDAI
		VECTOR3 Att = _V(0, 0, 0);
		double UllageDT = 0.0;	//Delta T of Ullage
		bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
		bool HeadsUp = true; //false = heads down, true = heads up
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		int REFSMMATInd = RTCC_REFSMMAT_TYPE_CUR;	//Used for IMU and inertial coordinate options
		int ConfigChangeInd = 0; //0 = No change, 1 = Undocking, 2 = Docking
		std::string FinalConfig = "C";
		double DeltaDA = 0.0; //Delta docking angle
		double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
		int TrimAngleIndicator = 0; //0 = computed, 2 = system
	} med_m66;

	//LOI and MCC Transfer
	struct MED_M78
	{
		int Table = 1; //1 = CSM, 3 = LEM
		unsigned ReplaceCode = 0; //1-15
		bool Type = false; //false = MCC, true = LOI
		unsigned ManeuverNumber = 1; //Maneuver number in LOI or MCC table
		int Thruster = RTCC_ENGINETYPE_CSMSPS; //Thruster for maneuver
		int Attitude = RTCC_ATTITUDE_PGNS_EXDV; //Attitude option
		double UllageDT = 0.0;	//Delta T of Ullage
		bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
		bool Iteration = false; //false = do not iterate, true = iterate
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
		bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
	} med_m78;

	//Transfer ascent maneuver to MPT from Lunar Targeting
	struct MED_M85
	{
		int VEH = 3;
		int ReplaceCode = 0;
	} med_m85;

	//Direct Input of Lunar Descent Maneuver
	struct MED_M86
	{
		int Veh = 3; //3 = LEM
		int ReplaceCode = 0;
		bool TimeFlag = false; //false = Nominal landing time to be input, true = Ignition time (only if replacing descent maneuver)
		double Time = 0.0;
		double dt_ullage = 8.0;
		bool UllageQuads = true; //false = 2 thrusters, true = 4 thrusters
		bool HeadsUp = true; //false = heads down, true = heads up
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		int TrimAngleIndicator = 0; //0 = computed, 2 = system
	} med_m86;

	MED_K16 med_k16;

	MED_M49 med_m49;
	MED_M50 med_m50;
	MED_M51 med_m51;
	MED_M55 med_m55;
	MED_M68 med_m68;
	MED_M70 med_m70;
	MED_M72 med_m72;

	struct MED_S80
	{
		//1 = CSM, 3 = LEM
		int VEH = 1;
		//GMT if positive, GET if negative, time of base vector if 0
		double time = 0.0;
		int REF = BODY_EARTH;
		std::string VID[4];
		bool useMPT = false;
	} med_s80;

	//Data Tables
	CapeCrossingTable EZCCSM;
	CapeCrossingTable EZCLEM;
	SunriseSunsetTable EZSSTAB;
	SunriseSunsetTable EZMMTAB;
	TradeoffDataDisplay RTETradeoffTable;
	TradeoffDataDisplayBuffer RTETradeoffTableBuffer[5];
	MissionPlanTable PZMPTCSM, PZMPTLEM;
	DetailedManeuverTable DMTBuffer[2];
	BurnParameterTable PZBURN;
	LWPInputTable PZSLVCON;
	SLVTargetingParametersTable PZSLVTAR;

	struct PerigeeAdjustTableEntry
	{
		double Pitch = 0.0;
		double TIG = 0.0;
		double DV = 0.0;
		double DT = 0.0;
		double TA = 0.0;
		double H = 0.0;
		double H_A = 0.0;
	};

	struct PerigeeAdjustTable
	{
		int Sol = 0;
		double T_elem = 0.0;
		double T_thresh = 0.0;
		double TimeStep = 0.0;
		double H_A = 0.0;
		double H_P = 0.0;
		PerigeeAdjustTableEntry Man[6];
	} PZPADDIS;

	std::vector<std::string> MSK0050Buffer; //Perigee Adjust Display

	VECTOR3 EZJGSTAR[400];

	struct LunarSurfaceAlignmentTable
	{
		MATRIX3 LLD_REFSMMAT;
		bool LLDRefsPresent = false;
	} EZJGLSAD;

	struct GuidanceOpticsSupportTable
	{
		std::string CODE = "ZZZZZZZZ";			//Identification of maneuver to be performed
		VECTOR3 Att_H = _V(0, 0, 0);			//Pitch, yaw and roll attitudes for the maneuver references to the IMU aligned to a local vertical orientation. Associated with DMT REFSMMAT.
		double GETAC = 0.0;						//Ground elapsed time of alignment check
		double IGA = 0.0;
		std::string IRA = "ZZZZZZ";
		unsigned SXT_STAR[2] = { 0,0 };
		double SXT_SFT_INP[2] = { 0.0, 0.0 };
		double SXT_TRN_INP[2] = { 0.0, 0.0 };
		double SXT_SFT_RTCC[2] = { 0.0, 0.0 };
		double SXT_TRN_RTCC[2] = { 0.0, 0.0 };
		unsigned SCT_S[2] = { 0,0 };
		double SCT_SF = 0.0;
		double SCT_TR = 0.0;
		double SCT_M[2] = { 0.0, 0.0 };
		double SCT_R[2] = { 0.0, 0.0 };
		double SCT_RTASC[2] = { 0.0, 0.0 };
		double SCT_DEC[2] = { 0.0, 0.0 };
		unsigned BS_S[2] = { 0,0 };
		double BS_SPA[2] = { 0.0, 0.0 };
		double BS_SXP[2] = { 0.0, 0.0 };
		double BS_RTASC[2] = { 0.0, 0.0 };
		double BS_DEC[2] = { 0.0, 0.0 };
		VECTOR3 Landmark_LOS = _V(0, 0, 0);
		double Landmark_GET = 0.0;
		std::string Landmark_SC = "ZZZ";
		double Landmark_RA = 0.0;
		double Landmark_DEC = 0.0;
		VECTOR3 Att[2] = { _V(0, 0, 0), _V(0, 0, 0) };
		std::string MAT = "ZZZZZZ";
		MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	} EZJGSTTB;

	struct GOSTDisplay
	{
		GuidanceOpticsSupportTable data;
		std::string err;
	} GOSTDisplayBuffer;

	struct LunarOpticsSupportTable
	{
		int MODE = 0; //1 = ???, 2 = DOK, 3 = FLT, 4 = AGS, 5 = LUN, 6 = MAT, 7 = CHK
		VECTOR3 LM_ATT = _V(0, 0, 0);
		VECTOR3 CSM_ATT = _V(0, 0, 0);
		double DKAN = 0.0;
		double GETHORIZ = 0.0;
		double OGA = 0.0;

		//Star 1
		int star1 = 0;
		double RA1 = 0.0;
		double DEC1 = 0.0;
		VECTOR3 Att1 = _V(0, 0, 0);
		int L1 = 0; //Reticle line (0 = +X, 1 = -X, 2 = +Y, 3 = -Y)
		double A1_1 = 0.0; //AOT counter for cursor
		double A2_1 = 0.0; //AOT counter for spirale
		int D1 = 2;

		//Star 2
		int star2 = 0;
		double RA2 = 0.0;
		double DEC2 = 0.0;
		VECTOR3 Att2 = _V(0, 0, 0);
		int L2 = 0; //Reticle line (0 = +X, 1 = -X, 2 = +Y, 3 = -Y)
		double A1_2 = 0.0; //AOT counter for cursor
		double A2_2 = 0.0; //AOT counter for spirale
		int D2 = 2;

		//COAS
		unsigned COAS_star1 = 0;
		double COAS_EL1 = 0.0;
		double COAS_SXP1 = 0.0;
		unsigned COAS_star2 = 0;
		double COAS_EL2 = 0.0;
		double COAS_SXP2 = 0.0;
		int COAS_AXIS = 1; //1 = PX, 2 = PZ

		VECTOR3 StoredAttTLM = _V(0, 0, 0);
		VECTOR3 StoredAttMED = _V(0, 0, 0);

		bool showRealign = false;
		VECTOR3 GIMB_ANG = _V(0, 0, 0);
		VECTOR3 FDAI_ANG = _V(0, 0, 0);

		int REF1 = 0;
		int REF2 = 0;
		int REFSUSED = 0;

		bool REFSMMATValid = false;
		MATRIX3 StoredREFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	} EZJGSTBL;

	std::vector<std::string> LOSTDisplayBuffer;

	REFSMMATLocker EZJGMTX1, EZJGMTX3;
	FIDOOrbitDigitals EZSAVCSM, EZSAVLEM;
	SpaceDigitals EZSPACE;
	OrbitStationContactsTable EZSTACT1, EZSTACT3;
	NextStationContactsTable NextStationContactsBuffer;
	PredictedSiteAcquisitionTable EZACQ1, EZACQ3, EZDPSAD1, EZDPSAD3;
	ExperimentalSiteAcquisitionTable EZDPSAD2;
	LandmarkAcquisitionTable EZLANDU1;
	LunarLaunchTargetingTable PZLLTT;

	struct EvaluationVectorTable
	{
		//0 = CSM, CMC
		//1 = CSM, LGC
		//2 = CSM, AGS
		//3 = CSM, IU
		//4 = LM, CMC
		//5 = LM, LGC
		//6 = LM, AGS
		//7 = LM, IU
		StateVectorTableEntry data[8];
	} BZEVLVEC;

	struct UsableVectorTable
	{
		//0 = CSM, CMC
		//1 = CSM, LGC
		//2 = CSM, AGS
		//3 = CSM, IU
		//4 = CSM, HSR
		//5 = CSM, DC
		//6 = LM, CMC
		//7 = LM, LGC
		//8 = LM, AGS
		//9 = LM, IU
		//10 = LM, HSR
		//11 = LM, DC
		StateVectorTableEntry data[12];
	} BZUSEVEC;

	struct VectorCompareTableData
	{
		double GMT = 0.0;
		double HA = 0.0;
		double HP = 0.0;
		double v = 0.0;
		double gamma = 0.0;
		double psi = 0.0;
		double phi = 0.0;
		double lambda = 0.0;
		double h = 0.0;
		double a = 0.0;
		double e = 0.0;
		double i = 0.0;
		double theta_p = 0.0;
		double Omega = 0.0;
		double nu = 0.0;
		double U = 0.0;
		double V = 0.0;
		double W = 0.0;
		double U_dot = 0.0;
		double V_dot = 0.0;
		double W_dot = 0.0;
	};

	struct VectorCompareTable
	{
		bool error = false;
		int NumVec = 0;
		VectorCompareTableData data[4];
	} BZCCANOE;

	struct VectorCompareDisplay
	{
		bool showWPAndTA[4] = { false,false,false,false };
		bool showHA[4] = { false,false,false,false };
		int NumVec = 0;
		VectorCompareTableData data[4];
		double PET = 0.0;
		double GMTR = 0.0;
		std::string error = "TABLE NOT INITIALIZED";
	} VectorCompareDisplayBuffer;

	struct VectorPanelSummaryDisplay
	{
		double gmt = 0.0;
		//0 = CSM, 1 = LM
		std::string AnchorVectorID[2];
		std::string AnchorVectorGMT[2];
		std::string CurrentGMT;
		//0 = CMC, 1 = LGC, 2 = AGS, 3 = IU
		std::string CompUsableID[2][4];
		std::string CompUsableGMT[2][4];
		std::string CompEvalID[2][4];
		std::string CompEvalGMT[2][4];
		std::string CompTelemetryHighGMT[2][4];
		std::string CompTelemetryLowGMT[2][4];
		std::string HSRID[2];
		std::string HSRGMT[2];
		std::string DCID[2];
		std::string DCGMT[2];
		std::string LastManGMTUL[2];
		std::string LastManGMTBO[2];
	} VectorPanelSummaryBuffer;

	struct TelemetryTrajectoryInterfaceTable
	{
		//Block 1
		EphemerisData HighSpeedCMCCSMVector;
		//Block 2
		EphemerisData HighSpeedCMCLEMVector;
		//Block 3 (High-speed AGC optics data)
		//TBD
		//Block 4 (High-Speed CMC REFSMMAT)
		MATRIX3 CMC_REFSMMAT;
		bool CMCRefsPresent = false;
		//Block 5-8 Low-speed CMC data
		//TBD
		//Block 9
		EphemerisData HighSpeedLGCCSMVector;
		//Block 10
		EphemerisData HighSpeedLGCLEMVector;
		//Block 11
		EphemerisData HighSpeedAGSCSMVector;
		//Block 12
		EphemerisData HighSpeedAGSLEMVector;
		//Block 13 (LGC optics data)
		//TBD
		//Block 14 (high-speed LGC REFSMMAT)
		MATRIX3 LGC_REFSMMAT;
		bool LGCRefsPresent = false;
		//Block 15-20 (low speed data)
		//TBD
		//Block 21
		EphemerisData HighSpeedIUVector;
		//Block 22 (Low-speed IU vector)
		//TBD
	} BZSTLM;

	struct RelativeMotionDigitalsTableEntry
	{
		double GET = 0.0;
		double PETorShaft = 0.0;
		double R = 0.0;
		double YdotorTrun = 0.0;
		double RDOT = 0.0;
		double AZH = 0.0;
		double EL = 0.0;
		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;
		char XInd = ' ';
		char YInd = ' ';
		char ZInd = ' ';
		double Pitch = 0.0;
		double Yaw = 0.0;
		double Roll = 0.0;
		double PB = 0.0;
		double YB = 0.0;
		double RB = 0.0;
	};

	struct RelativeMotionDigitalsTable
	{
		std::string CSMSTAID;
		std::string LMSTAID;
		double GETR = 0.0;
		double CSMGMTV = 0.0;
		double CSMGETV = 0.0;
		double LMGMTV = 0.0;
		double LMGETV = 0.0;
		std::string REFSMMAT;
		std::string VEH;
		char AXIS = ' ';
		char Mode = ' ';
		RelativeMotionDigitalsTableEntry data[11];
		std::string error = "MED OUTDATED";

		int solns = 0;
		char Pitch = ' ';
		char Yaw = ' ';
		char Roll = ' ';
		std::string PETorSH;
		std::string YDotorT;
	} EZRMDT;

	struct ExperimentalSiteGroundPointTable
	{
		//-1 when empty, 0 = Earth, 1 = Moon
		int REF = -1;
		StationData Data[12];
	} EZEXSITE;

	struct LandmarkSitesTable
	{
		//-1 when empty, 0 = Earth, 1 = Moon
		int REF = -1;
		StationData Data[12];
	} EZLASITE;

	struct OrbitEphemerisTable
	{
		EphemerisDataTable2 EPHEM;
		ManeuverTimesTable MANTIMES;
		LunarStayTimesTable LUNRSTAY;
	} EZEPH1, EZEPH2;

	struct NutationPrecessionMatrices
	{
		MATRIX3 Mat[141]; //ECI to ECT
		//MJD of first matrix
		double mjd0 = 0.0;
	} EZNPMATX;

	struct TLITargetingParametersMasterTable
	{
		TLITargetingParametersTable data[10];
	} PZSTARGP;

	struct SIVBTLIMatrixTable
	{
		//Plumbline coordinate axes in ECI coordinates
		MATRIX3 EPH = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
		//Plumbline to parking orbit nodal system transformation matrix
		MATRIX3 GG = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
		//Plumbline to target orbit nodal system transformation matrix
		MATRIX3 G = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	} PZMATCSM, PZMATLEM;

	struct TLIPlanningTable
	{
		//INPUT
		//
		int mpt = 1;
		//Vector type. CMC, LGC, AGS, IU, HSR, DC or ANC
		std::string VectorType = "ANC";
		//2 = free-return, 3 = hybrid ellipse, 4 = E-type mission ellipse, 5 = non-free return
		int Mode = 4;
		//TLI opportunity (1-2)
		int Opportunity = 1;
		//TLI ignition for mode 4, estimated TIG for mode 5
		double GET_TLI = 0.0;
		//Apogee height for mode 4, nautical miles
		double h_ap = 5000.0;
		//Launch window
		bool IsPacficWindow = true;
		//Available DV, for mode 3, feet per second
		double dv_available = 5000.0;

		//CONSTANTS - THESE SHOULD BE SYSTEM PARAMETERS
		double DELTA = 0.0;
		double SIGMA = 7.5*RAD;
		
	} PZTLIPLN;

	struct TLIPlanningDisplayDataTable
	{
		double GET_TIG = 0.0;
		double GET_TB6 = 0.0;
		double T_b = 0.0;
		double dv_TLI = 0.0;
		double H_a = 0.0;
		double lat_ign = 0.0;
		double lng_ign = 0.0;
	} PZTPDDIS;

	struct TLIPlanningMPTInterfaceDataTable
	{
		bool DataIndicator = false; //false = nothing here, true = good data

		double T_RP;
		double i;
		double theta_N;
		double e;
		double C3;
		double alpha_D;
		double f;
		int Opportunity; //1-2
	} PZTPDXFR;

	struct TLIPlanningOutputTable
	{
		int DataIndicator = 0; //0 = None, 1 = 7 parameters, 2 = 10 parameters

		//Target vector
		VECTOR3 T;
		//Unit nodal vector which defines node of desired cutoff plane and parking orbit plane at time of restart preparation
		VECTOR3 S;
		//Position vector at time of restart preparation
		VECTOR3 R;
		//Velocity vector at time of restart preparation
		VECTOR3 V;
		double TB6;
		double TIG;

		double i;
		double theta_N;
		double sigma;
		double C3;

		SevenParameterUpdate elem;

	} PZTTLIPL;

	struct LOIElementsTable
	{
		EphemerisData sv_man_bef[8];
		VECTOR3 V_man_after[8];
	} PZLRBELM;

	struct LOIDisplayTableElement
	{
		double GETLOI = 0.0;	//Impulsive GET of LOI ignition
		double DVLOI1 = 0.0;	//Total DV of LOI-1 in feet per second
		double DVLOI2 = 0.0;	//Total DV of DOI/LOI-2 in feet per second
		double H_ND = 0.0;		//Height of the node (impulsive LOI ignition point)
		double f_ND_H = 0.0;	//True anomaly at LOI on the approach hyperbola (pre LOI)
		double H_PC = 0.0;		//Height of perilune on the first lunar orbit
		double Theta = 0.0;		//Angle between the desired lunar orbit plane and the actual achieved plane 
		double f_ND_E = 0.0;	//True anomaly at LOI on the first ellipse (post LOI)
	};

	struct LOIDisplayTable
	{
		std::string StaID = "STAT000";
		double VectorGET = 0.0;
		double lat_lls = 0.0;
		double lng_lls = 0.0;
		double R_lls = 0.0;
		double AZMN_f_ND = 0.0;
		double AZMX_f_ND = 0.0;
		double REVS1 = 0.0;
		int REVS2 = 0;
		double DHBIAS = 0.0;
		double AZ_LLS = 0.0;
		double f_LLS = 0.0;
		double HALOI1 = 0.0;
		double HPLOI1 = 0.0;
		double HALOI2 = 0.0;
		double HPLOI2 = 0.0;
		double DVMAXp = 0.0;
		double DVMAXm = 0.0;
		double RARPGT = 0.0;
		bool planesoln = true;
		double h_pc = 0.0; //Not actually for display
		LOIDisplayTableElement sol[8];
	} PZLRBTI;

	struct MCCTransferTable
	{
		EphemerisData sv_man_bef[6];
		VECTOR3 V_man_after[6];
	} PZMCCXFR;

	struct LDPPElementsTable
	{
		EphemerisData sv_man_bef[4];
		VECTOR3 V_man_after[4];
		int num_man;
		int plan[4];
		std::string code[4];
	} PZLDPELM;

	LunarDescentPlanningTable PZLDPDIS;

	struct LMPositionVectorTable
	{
		double lat[4];
		double lng[4];
		double rad[4];
	} BZLAND;

	struct HistoryAnchorVectorTable
	{
		StateVectorTableEntry AnchorVectors[10];
		int num = 0;
	} EZANCHR1, EZANCHR3;

	TimeConstraintsTable EZTSCNS1, EZTSCNS3;

	struct StationCharacteristicsBlock
	{
		StationData data;

		int RadarMount; //1 = AZ/EL, 2 = X/Y 30, 3 = POLAR, 4 = X/Y 85
		//Word 2 - Indicators
		//Site Types (8 = USB, 13 = Voice)
		char SiteType;
		//Word 3 - Teletype routing indicator and name
		//Word 18 - Local vertical deflection (longitude)
		double def_lng;
		//Word 19 - Local vertical deflection (latitude)
		double def_lat;
		//Word 20 - Azimuth deviation from true north
		double dev_azi;
		//Word 21 - Refraction modulus
		double mod_refr;
		//Word 22 - Comm processor LAPFL (?)
		//Word 23 - Range granularity
		double gran_range;
		//Word 24 - Angular granularity
		double gran_ang;
		//Word 25 - K-factor for refraction
		double KFactor;
		//Word 26-29 - Sigma coefficient
		double SigmaCoef[4];
		//Word 32 - USB transmitter frequency #1
		double Freq1;
		//Word 36 - USB Transmitter frequency #2
		double Freq2;
		//Word 37 - Maximum slant range
		double MaxSlantRange;
		//Word 38 - Range bias
		double RangeBias;
	};

	StationCharacteristicsBlock GZSTCH[28];

	struct GeneralConstraintsTable
	{
		//Block 1
		int Year = 0;
		//Block 2
		int RefDayOfYear = 0;
		//Block 3
		int DaysInYear = 0;
		//Block 4
		int MonthofLiftoff = 0;
		int DayofLiftoff = 0;
		int DaysinMonthofLiftoff = 0;
		//Block 5
		double DKIElevationAngle = 26.6*RAD;
		//Block 8
		double DKITerminalPhaseAngle = 130.0*RAD;
		//Block 9
		double TIDeltaH = 0.0;
		//Block 10
		double TIPhaseAngle = 0.0;
		//Block 11
		double TIElevationAngle = 26.6*RAD;
		//Block 12
		double TITravelAngle = 130.0*RAD;
		//Block 13
		double TINSRNominalTime = 0.0;
		//Block 14
		double TINSRNominalDeltaH = 0.0;
		//Block 15
		double TINSRNominalPhaseAngle = 0.0;
		//Block 18 Bytes 5-8
		int DKI_TP_Definition = 4; // 0 = input phase angle, 1-6: input or lighting
		//Block 25
		double DKIDeltaH_NSR = 0.0;
		double DKIDeltaH_NCC = 0.0;
		//Block 26
		double DKI_TPDefinitionValue = -23.0; //Can be input phase angle, TPI, TPF time or day/night
		//Block 27
		double DKIMinPerigee = 0.0;
		//Block 29
		double TPIDefinitionValue = 0.0;
		//Block 30
		double OrbitsFromCSItoTPI;
		//Block 31 Bytes 1-4
		int TPIDefinition = 3;
		//Block 31 Bytes 5-8
		int TPICounterNum;
		//Block 32 Bytes 5-8. Control flag for initial phase angle wrapping. 0 = -180 to 180. 1 = 0 to 360. -1 = -360 to 0 and so on...
		int DKIPhaseAngleSetting = 0;
		//Block 34
		int DeltaDays = 0;
		//Block 36
		double ActualDH;
		//Block 37
		double ActualPhaseAngle;
		//Block 38
		double ActualWedgeAngle;
		//Block 40
		double LDPPAzimuth = 0.0; //Greater or equal to zero, lower than 360°. If 0, LDPP will compute azimuth
		//Block 41
		double LDPPHeightofPDI = 50000.0*0.3048;
		//Block 42 1st word
		int LDPPDwellOrbits = 0; //Number of dwell orbits desired between DOI and PDI
		//Block 42 2nd word
		bool LDPPPoweredDescentSimFlag = false; //true = simulate powered descent
		//Block 43
		double LDPPTimeofPDI = 0.0;	//Time for powered descent ignition
		//Block 44
		double LDPPDescentFlightTime = 11.0*60.0; //Minutes
		//Block 45
		double LDPPDescentFlightArc = 15.0*RAD;
		double LDPPLandingSiteOffset = 15.0*RAD;
		//Block 46
		double SPQDeltaH = 15.0*1852.0;
		//Block 47
		double SPQElevationAngle = 26.6*RAD;
		//Block 48
		double SPQTerminalPhaseAngle = 130.0*RAD;
		//Block 49
		double SPQMinimumPerifocus = 0.0;

	} GZGENCSN;

	struct TwoImpulseMultipleSolutionTableEntry
	{
		double Time1 = 0.0;
		double DELV1 = 0.0;
		double YAW1 = 0.0;
		double PITCH1 = 0.0;
		double Time2 = 0.0;
		double DELV2 = 0.0;
		double YAW2 = 0.0;
		double PITCH2 = 0.0;
		double T_TPI = 0.0;
		char L = ' ';
		int C = 0;
	};

	struct TwoImpulseMultipleSolutionTable
	{
		bool Updating = false;
		int Solutions = 0;
		bool showTPI = false;
		int IVFLAG = 0;
		int MAN_VEH = 0;
		std::string CSMSTAID, LMSTAID;
		TwoImpulseMultipleSolutionTableEntry data[13];
	} PZTIPREG;

	struct TwoImpulseMultipleSolutionDisplay
	{
		std::string ErrorMessage;
		std::string CSMSTAID;
		std::string LMSTAID;
		double GETTH_CSM = 0.0;
		double GETTH_LM = 0.0;
		std::string MAN_VEH;
		std::string GETFRZ;
		std::string GMTFRZ;
		std::string GETVAR;
		std::string OPTION;
		double WT = 0.0;
		double PHASE = 0.0;
		double DH = 0.0;
		double GET1 = 0.0;
		double GMT1 = 0.0;
		std::string MinutesUntil;
		int Solutions = 0;
		bool showTPI = false;
		TwoImpulseMultipleSolutionTableEntry data[13];
	} TwoImpMultDispBuffer;

	struct TwoImpulseSingleSolutionTable
	{
		std::string CSMSTAID, LMSTAID;
	} PZTIPSS;

	struct CorrectiveCombinationSolutionTable
	{
		bool Updating = false;
		int Solutions = 0;
		int MAN_VEH = 0;
	} PZTIPCCD;

	struct LaunchInterfaceTable
	{
		//Block 1-9
		MATRIX3 IU1_REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
		//Block 11-19
		MATRIX3 IU2_REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
		//Block 56-62
		double GMT_T;
		VECTOR3 R_T;
		VECTOR3 V_T;
		//Block 73
		double Azimuth;
	} GZLTRA;

	struct RTEConstraintsTable
	{
		RTEConstraintsTable();

		//Block 11

		//Maximum allowable DV for tradeoff display (in ft/s)
		double DVMAX;
		//Minimum time of landing for tradeoff display (in hours GET)
		double TZMIN;
		//Maximum time of landing for tradeoff display (in hours GET)
		double TZMAX;
		//Constant g-level for use in return-to-earth digitals and EFCUA mode of the abort scan table (AST)
		double GMAX;
		//Minimum height of pericynthion (NM)
		double HMINMC;
		//Maximum return inclination (DEG)
		double IRMAX;
		//Relative range override (NM)
		double RRBIAS;
		//Maximum return velocity
		double VRMAX;
		//Vector source (0 = CSM, 1 = LEM)
		int VECID;
		//Vector type (0 = CMC, 1 = LGC, 2 = AGS, 3 = IU, 4 = HSR, 5 = DC, 6 = ANC, 7 = EPH)
		int VECTYPE;
		//Target line (0 = shallow, 1 = steep)
		int TGTLN;
		//Circumlunar motion (0 = either, 1 = direct, 2 = circumlunar)
		int MOTION;

		//Names of PTP sites
		std::string PTPSite[5];
		//Latitude of PTP sites
		double PTPLatitude[5];
		//Longitude of PTP sites
		double PTPLongitude[5];
		//Names of ATP sites
		std::string ATPSite[5];
		//Latitude and longitude of ATP data points
		double ATPCoordinates[5][10];

		//Block 12
		std::string RTESite = "No Site!";
		double RTEVectorTime; //Vector time in GMT (hrs)
		double RTET0Min; //Time of abort or minimum time in GMT (hrs)
		double RTET0Max; //Maximum time in GMT (hrs)
		double RTETimeOfLanding; //Landing time in GMT (hrs)
		double RTEUADVMax;
		double RTEPTPMissDistance;
		double RTEInclination;
		int EntryProfile;
		int RTETradeoffRemotePage;
		int RTESiteNum;
		bool RTEIsPTPSite;
		int RTEDigitalsColumn;
		int RTEDTrimAngleIndicator;
		int RTEDDockingAngle;
		int RTEDManeuverOrientation;
		bool RTEDIterationFlag;
		char RTEManeuverCode[4];

		//Block 13
		int RTETradeoffLabelling[5];

		ASTData AbortScanTableData[7];
		int LastASTCode = 0;

		RTEDigitalSolutionTable RTEDTable[2];
		int LastRTEDCode = 0;
	} PZREAP;

	struct RetrofireExternalDVDisplayData
	{
		bool Indicator = false;
		double GETI;
		VECTOR3 DV;
		double P_G, Y_G;
		double DT_TO, DV_TO;
		double H_apo, H_peri;
		double lat_IP, lng_IP;
	};

	struct RetrofireExternalDVDisplay
	{
		std::vector<RetrofireExternalDVDisplayData> data;
	} RetrofireEXDV;
	
	RetrofireTransferTable RZRFTT;
	ReentryConstraintsTable RZC1RCNS;
	RetrofireDisplayParametersTable RZRFDP;
	SpacecraftSettingTable RZDBSC1;

	struct ReentryOutputTable
	{

	} RZC1READ;

	struct RetrofireMEDSaveTable
	{
		//R20
		int R20_VEH = RTCC_MPT_CSM;
		double R20_GET = 0.0;
		double R20_lng = 0.0;

		//R30 (Separation or Shaping Maneuver)
		int R30_ColumnIndicator = 7; //Bitfield. 7 = all
		double R30_GETI_SH = 0.0; //GETI of shaping for manual input
		double R30_DeltaT_Sep = 20.0*60.0; //DT of sep maneuver
		int R30_Thruster = RTCC_ENGINETYPE_CSMRCSPLUS4;
		double R30_DeltaV = 5.0*0.3048;
		double R30_DeltaT = 0.0;
		VECTOR3 R30_Att = _V(0.0, -45.4*RAD, 180.0*RAD);
		double R30_Ullage_DT = 15.0;
		bool R30_Use4UllageThrusters = true;	//0 = two thrusters, 1 = four thrusters
		int R30_GimbalIndicator = -1; //-1 = compute, 1 = use system parameters

		//R31 (Retrofire Maneuver)
		int R31_Thruster = RTCC_ENGINETYPE_CSMSPS;		//1 = RCS+2, 2 = RCS+4, 3 = RCS-2, 4 = RCS-4, 33 = SPS
		int R31_GuidanceMode = 4;	//1 = Inertial, 4 = Guided (G&N)
		int R31_BurnMode = 3;		//1 = DV, 2 = DT, 3 = V, Gamma Target (only SPS)
		double R31_dt = 0.0;
		double R31_dv = 0.0;
		int R31_AttitudeMode = 2;	//1 = LVLH, 2 = 31.7° window line on horizon
		VECTOR3 R31_LVLHAttitude = _V(0.0, -48.5*RAD, PI);
		double R31_UllageTime = 15.0;
		bool R31_Use4UllageThrusters = true;	//0 = two thrusters, 1 = four thrusters
		int R31_REFSMMAT = 1;		//1 = CUR...
		int R31_GimbalIndicator = -1; //-1 = compute, 1 = use system parameters
		double R31_InitialBankAngle = 0.0;
		double R31_GLevel = 0.2;
		double R31_FinalBankAngle = 55.0*RAD;

		//R32
		int R32_Code = 1; //1 = Type 1, 2 = Type 2
		double R32_GETI = 0.0;
		double R32_lat_T = 0.0;
		double R32_lng_T = 0.0;
		double R32_MD = 1.0;

		//Actually determined by leaving the latitude blank on the MED
		int Type = 2;			//1 = Primary (lat and long), 2 = Contingency (long only)
	} RZJCTTC;

	struct RecoveryTargetDisplayEntry
	{
		bool DataIndicator = true; //false = data, true = no data
		bool AlternateLongitudeIndicator = false; //false = converged longitude, true = not converged
		int Rev = 0;
		double Azimuth = 0.0;
		double Latitude = 0.0;
		double Longitude = 0.0;
		double GET = 0.0;
		double GMT = 0.0;
	};

	struct RecoveryTargetDisplay
	{
		std::string VehicleName;
		std::string ErrorMessage;
		int Rev = 0;
		int Mission = 0;
		double InputLongitude = 0.0;
		std::string StationID;
		int TUP = 0;
		int CurrentPage = 1;
		int TotalNumPages = 1;
		int TotalNumEntries = 0;
		RecoveryTargetDisplayEntry table[40];
	} RZDRTSD;

	struct LMLaunchTargetTable
	{
		//K13
		double PoweredFlightArc = 10.0*RAD;
		double PoweredFlightTime = 7.0*60.0 + 15.0;
		double InsertionHeight = 60000.0*0.3048;
		double InsertionHorizontalVelocity = 5535.6*0.3048;
		double InsertionRadialVelocity = 32.0*0.3048;
		double YawSteerCap = 0.5*RAD;
		double MaxAscLifetime = 9.0*3600.0;
		double MinSafeHeight = 5.0*1852.0;
		double LMMaxDeltaV = 430.0*0.3048;
		double CSMMaxDeltaV = 430.0*0.3048;
		//K14
		//If CSI scheduled at apsis and CSM is chaser, then CSI will be done at LM apsis plus input time bias
		double dt_bias = 0.0;
		double ElevationAngle = 26.6*RAD;
		double TerminalPhaseTravelAngle = 130.0*RAD;
		double TPF_Height_Offset = 0.0;
		double TPF_Phase_Offset = 0.0;
		double Height_Diff_Begin = 20.0*1852.0;
		double Height_Diff_Incr = 5.0*1852.0;

		//Short profile
		double DT_DH = 15.0*1852.0;
		double DT_Theta_i = 1.69*RAD;
		double DT_Ins_TPI = 40.0*60.0;			//Fixed time from insertion to TPI
		double DT_Ins_TPI_NOM = 40.0*60.0;		//Nominal time from insertion to TPI
	} PZLTRT;

	struct LAIInputOutput
	{
		double t_launch = 0.0;
		double R_D, Y_D;
		double R_D_dot, Y_D_dot, Z_D_dot;
		EphemerisData sv_Insertion;
	} JZLAI;

	struct AEGBlockSaveTable
	{
		//Block 1: Multiple Solution
		EphemerisData SV_mult[2];
		//Block 2: Corrective Combination
		EphemerisData SV_CC[2];
		//Block 3: Transfer Data
		EphemerisData SV_before[2];
		VECTOR3 V_after[2];
		int plan[2];
		std::string code[2];
	} PZMYSAVE;

	struct GPMElementsTable
	{
		EphemerisData SV_before;
		VECTOR3 V_after;
		std::string code;
	} PZGPMELM;

	struct GPMResults
	{
		std::string STAID; //Call letters of station used for last trajectory update of the maneuvering vehicle
		double GETAV = 0.0;
		double Weight = 0.0;
		double GET_A = 0.0;
		double HA = 0.0;
		double long_A = 0.0;	//longitude of apoapsis
		double lat_A = 0.0;	//latitude of apoapsis
		double GET_P = 0.0;
		double HP = 0.0;
		double long_P = 0.0;	//longitude of periapsis
		double lat_P = 0.0;	//latitude of periapsis
		double A = 0.0;
		double E = 0.0;
		double I = 0.0;
		double Node_Ang = 0.0;
		double Del_G = 0.0;
		double Pitch_Man = 0.0;
		double Yaw_Man = 0.0;
		double H_Man = 0.0;
		double long_Man = 0.0;
		double lat_Man = 0.0;
		double GETTH = 0.0;
		double dv = 0.0;
		VECTOR3 DV = _V(0, 0, 0);
		double GET_TIG = 0.0;
		int Rev = 0;
		bool ShowImpact = false; //false = HP location, true = impact location
		int Err = 0;
	} PZGPMDIS;

	struct DKIElementsBlock
	{
		EphemerisData SV_before[5];
		VECTOR3 V_after[5];
	};

	struct DKIElementsTable
	{
		DKIElementsBlock Block[7];
	} PZDKIELM;

	struct DKIDisplayBlock
	{
		double ManGMT = 0.0;
		double dt = 0.0;
		double dv = 0.0;
		//Vehicle performing the maneuver
		int VEH;
		//Name of the maneuver
		std::string Man_ID;
		double PhaseAngle = 0.0;
		double DH = 0.0;
		double HA = 0.0;
		double HP = 0.0;
		double ManLine = 0.0;
		VECTOR3 DV_LVLH = _V(0, 0, 0);
		double Pitch = 0.0;
		double Yaw = 0.0;
	};

	struct DKIDataBlock
	{
		int Plan_M = 0;
		DKIDisplayBlock Display[5];
		int NumMan = 0;
		//0 = No plan, 1 = DKI, 2 = SPQ
		int PlanStatus = 0;
		//Maneuver lines
		double NC1 = 0.0;
		double NH = 0.0;
		double NCC = 0.0;
		double NSR = 0.0;
		double NPC = 0.0;
		double TTPI = 0.0;
	};

	struct DKIDataTable
	{
		//Block 1
		bool UpdatingIndicator = false;
		//Number of plans that were generated (SPQ always 1)
		int NumSolutions = 0;
		//Block 2 (Information Block)
		DKIDataBlock Block[7];
	} PZDKIT;

	struct RendezvousEvaluationDisplay
	{
		RendezvousEvaluationDisplay();
		int ID;
		int M;
		int NumMans;
		double GET[5];
		double DT[4];
		double DV[5];
		std::string VEH[5];
		std::string PURP[5];
		double CODE[5];
		double PHASE[5];
		double HEIGHT[5];
		double HA[5];
		double HP[5];
		double Pitch[5], Yaw[5];
		VECTOR3 DVVector[5];
		bool isDKI;
		std::string ErrorMessage;
	} PZREDT;

	struct RendezvousPlanningDisplayData
	{
		RendezvousPlanningDisplayData();
		int ID;
		int M;
		double NC1;
		double NH;
		double NSR;
		double NCC;
		double GETTPI;
		double NPC;
	};

	struct RendezvousPlanningDisplay
	{
		int plans = 0;
		RendezvousPlanningDisplayData data[7];
		std::string ErrorMessage;
	} PZRPDT;

	struct LunarRendezvousPlanningDisplayData
	{
		int ID = 0;
		int N = 0;
		double GETLO = 0.0;
		double DH = 0.0;
		double T_INS = 0.0;
		double T_CSI = 0.0;
		double T_CDH = 0.0;
		double T_TPI = 0.0;
		double T_TPF = 0.0;
		double DVCSI = 0.0;
		double DVCDH = 0.0;
		double DVTPI = 0.0;
		double DVTPF = 0.0;
		double DVT = 0.0;
	};

	struct LunarRendezvousPlanningDisplay
	{
		std::string CSMSTAID;
		double CSM_GMTV = 0.0;
		double CSM_GETV = 0.0;
		std::string ManVeh;
		double LSLat = 0.0;
		double LSLng = 0.0;
		double THT = 0.0;
		std::string LMSTAID;
		double LM_GMTV = 0.0;
		double LM_GETV = 0.0;
		double DT_CSI = 0.0;
		double DV_MAX = 0.0;
		double MINH = 0.0;
		double WT = 0.0;
		int plans = 0;
		std::string ErrorMessage;
		LunarRendezvousPlanningDisplayData data[7];
	} PZLRPT;

	struct MasterFlightPlanTableAzimuth
	{
		int iAzimuth = -1; //ID
		double dAzimuth = 0.0; //Actual value
		TLMCCDataTable data;
	};

	struct MasterFlightPlanTableOpportunity
	{
		int Opportunity = -1;
		std::vector<MasterFlightPlanTableAzimuth> data;
	};

	struct MasterFlightPlanTableDay
	{
		int day = -1;
		std::vector<MasterFlightPlanTableOpportunity> data;
	};

	struct MasterFlightPlanTable
	{
		std::vector<MasterFlightPlanTableDay> data;
	} PZMFPTAB;

	struct SkeletonFlightPlanTable
	{
		//Blocks
		//1: Preflight
		//2: Nominal nodal targets from options 2-5
		//3-5: TBD
		TLMCCDataTable blocks[2];
		//Block 6
		int DisplayBlockNum = 1;
		int Day;
		int Opportunity;
		double Azimuth;
	} PZSFPTAB;

	struct UpdatedSkeletonFlightPlansTable
	{
		//One for each MCC tradeoff display columns
		TLMCCDataTable blocks[6];
	} PZMCCSFP;

	struct TLMCCPlanningDisplay
	{
		TLMCCDisplayData data[6];
	} PZMCCDIS;

	struct MidcourseCorrectionPlanTable
	{
		//Block 1
		double VectorGET = 0.0;
		int Column = 1;
		int Mode = 1;
		double MidcourseGET = 0.0;
		bool Config = true; //false = undocked, true = docked
		int SFPBlockNum = 1;
		double h_PC = -1.0;
		double h_PC_mode5 = -1.0;
		double incl_fr = 0.0;
		double AZ_min = -110.0*RAD;
		double AZ_max = -70.0*RAD;
		//Minimum and maximum time at pericynthion (GET, hours)
		double TLMIN = 0.0;
		double TLMAX = 0.0;
		double gamma_reentry = -6.52*RAD;
		double Reentry_range = 1350.0;
		double H_PCYN_MIN = 40.0*1852.0;
		double H_PCYN_MAX = 5000.0*1852.0;
		double LATBIAS = 2.0*RAD;
		double H_A_LPO1 = 170.0*1852.0;
		double H_P_LPO1 = 60.0*1852.0;
		double H_A_LPO2 = 60.0*1852.0;
		double H_P_LPO2 = 8.23*1852.0;
		double REVS1 = 2.0;
		int REVS2 = 11;
		double SITEROT = -15.0*RAD;
		double ETA1 = 0.0;
		//Constants (some of these should be RTCC program constants, but we don't know their name)
		int LOPC_M = 3;
		int LOPC_N = 8;
		double TT1_DPS_MIN = 1.0*3600.0;
		double TT1_DPS_MAX = 120.0*3600.0;
		//Max inclination of powered return
		double INCL_PR_MAX = 40.0*RAD;
	} PZMCCPLN;

	struct LOITargetingInitTable
	{
		double HA_LLS = 60.0;
		double HP_LLS = 8.23;
		double DW = -15.0;
		double REVS1 = 2.0;
		int REVS2 = 11;
		double eta_1 = 0.0;
		double dh_bias = 0.0;
		bool PlaneSolnForInterSoln = true;
	} PZLOIPLN; //Figure out real name!

	struct ARMMEDSaveTable
	{
		ARMMEDSaveTable();
		void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
		void LoadState(FILEHANDLE scn, char *end_str);

		//Coelliptic

		//Elevation angle
		double E;
		//CSI: If zero CSI performed at first apolune after insertion. non-zero parameter used as delta time from insertion to CSI. (minutes)
		double CSIFlag;
		//CDH Indicator (+N: N apsis crossings from CSI to CDH, -N: N/2 revs from CSI to CDH (N must be odd)
		int CDHIndicator;
		//Minimum safe perilune
		double h_min;
		//Time of TPI (GMT)
		double t_TPI_Coell;

		//Short

		//false = time of tweak DT from insertion, true = input time of tweat
		bool ITWEAK;
		//false = time of TPI is DTPI from insertion, true = input time of TPI
		bool ITPI;
		//Time of tweak (GMT)
		double t_tweak;
		//Delta time of tweak
		double DT;
		//Delta time of TPI
		double DTPI;
		//Phase and height offsets at TPI
		double DTHETA;
		//IMU gimbal angles
		VECTOR3 IMUAngles;
		//body axis reference. false = gimbal angles, true = Axhor
		bool IREF;
		//Angle between line of sight to the forward horizon and the spacecraft X-axis
		double Axhor;
		//Time of TPI (GMT)
		double t_TPI_Short;

		//Both

		//Terminal phase travel angle
		double WT;
		//Desired Delta H
		double DH;
		//Time of insertion (GMT)
		double t_Ins;

		double t_Calc_ARM = -1.0;
		double t_Calc_ShortARM = -1.0;

	} PZMARM;


	struct UMEDSaveTable
	{
		//Block 3
		int SpaceDigVehID = -1;
		int SpaceDigCentralBody = -1;
		//Block 17
		int LandmarkVehID = 0;
		double LandmarkGMT = 0.0;
		double LandmarkDT = 0.0;
		int LandmarkRef = 0;

		//DMT
		int DMT1Vehicle = 0;
		int DMT1Number = 0;
		int DMT1REFSMMATCode = 0;
		bool DMT1HeadsUpDownIndicator = false;
		int DMT2Vehicle = 0;
		int DMT2Number = 0;
		int DMT2REFSMMATCode = 0;
		bool DMT2HeadsUpDownIndicator = false;

		//RET
		int RETPlan = 1;
	} EZETVMED;

	struct GMEDSaveTable
	{
		int G01_Type = RTCC_REFSMMAT_TYPE_MED;
		MATRIX3 G01_REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
		double GMT = 0.0;
		unsigned StartingStar = 0;
		int MTX1 = -1, MTX2 = -1, MTX3 = -1;
		int G14_Vehicle = 1;
		unsigned G14_Star = 0;
		int G14_RB = 0;
		double G14_lat = 0.0, G14_lng = 0.0, G14_height = 0.0;
		double G14_GMT = 0.0;

		double G20_HORIZGET = 0.0;
		int G20_COAS_Axis = 1;
		int G20_AOT_Detent = 2;

		int G23_Option = 1;
	} EZGSTMED;

	struct ExternalDVMakeupBuffer
	{
		int LoadNumber = 0;
		std::string Site1, Site2;
		double GenGET = 0.0;
		int Verb = 71;
		int CMCCoreAddress = 0;
		int Octals[012] = { 0,0,0,0,0,0,0,0,0,0 };
		int VehID = 0;
		double GET = 0.0;
		VECTOR3 DV = _V(0, 0, 0);
		std::string ManeuverCode;
		double GMTID = 0.0;
		std::string StationID;
	} CZAXTRDV, CZLXTRDV;

	struct REFSMMATUpdateMakeupTableBlock
	{
		int UpdateNo = 0;
		int SequenceNo = 0;
		std::string Site1, Site2;
		double GET = 0.0;
		int Verb = 71;
		int Octals[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		int VehID = 0;
		MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
		std::string error;
		//1 = Actual REFSMMAT; 2 = Desired REFSMMAT
		int MatrixType = 2;
		std::string MatrixID;
	};

	struct REFSMMATUpdateMakeupTable
	{
		REFSMMATUpdateMakeupTableBlock Block[2];
	} CZREFMAT;

	struct CMCRetrofireExternalDVUpdateMakeupBuffer
	{
		std::string LoadType;
		int UpdateNo = 0;
		int SequenceNumber = 0;
		double GETLoadGeneration = 0.0;
		double Lat = 0.0;
		double Lng = 0.0;
		int Octals[016] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		VECTOR3 DV = _V(0, 0, 0);
		double GET_TIG = 0.0;
	} CZREXTDV;

	struct CMCEntryUpdateMakeupBuffer
	{
		std::string LoadType;
		int UpdateNo = 0;
		int SequenceNumber = 0;
		double GETLoadGeneration = 0.0;
		double Lat = 0.0;
		double Lng = 0.0;
		int Octals[6] = { 0,0,0,0,0,0};
	} CZENTRY;

	struct NavUpdateMakeupBuffer
	{
		std::string LoadType;
		int SequenceNumber = 0;
		std::string PrimarySite;
		std::string BackupSite;
		double GETofGeneration = 0.0;
		int Octals[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		EphemerisData sv; //For display
		std::string DCCode;
		double AnchorVectorTime = 0.0;
		std::string VehicleID;
	};

	struct CMCLGCNavUpdatesMakeupBuffers
	{
		NavUpdateMakeupBuffer CMCCSMUpdate;
		NavUpdateMakeupBuffer CMCLEMUpdate;
		NavUpdateMakeupBuffer LGCCSMUpdate;
		NavUpdateMakeupBuffer LGCLEMUpdate;
	} CZNAVGEN;

	struct SLVNavigationMakeupTable
	{
		VECTOR3 PosS = _V(0, 0, 0);
		VECTOR3 DotS = _V(0, 0, 0);
		double NUPTIM = 0.0;
	} CZNAVSLV;

	struct LandingSiteMakupBuffer
	{
		std::string LoadType;
		int SequenceNumber = 0;
		std::string PrimarySite;
		std::string BackupSite;
		double GETofGeneration = 0.0;
		int Octals[8] = { 0,0,0,0,0,0,0,0 };
		double lat = 0.0;
		double lng = 0.0;
		double rad = 0.0;
		VECTOR3 R_LS = _V(0, 0, 0);
	};

	struct CMCLGCLandingSiteMakupBuffer
	{
		LandingSiteMakupBuffer CSMLSUpdate;
		LandingSiteMakupBuffer LMLSUpdate;
	} CZLSVECT;

	struct LGCDescentTargetMakupBuffer
	{
		std::string LoadType;
		int SequenceNumber = 0;
		std::string PrimarySite;
		std::string BackupSite;
		double GETofGeneration = 0.0;
		int Octals[5] = { 0,0,0,0,0 };
		double GETTD = 0.0; //Time of landing
	} CZTDTGTU;

	struct AGCLiftoffTimeUpdateMakeupTableBlock
	{
		int UpdateNo = 0;
		int SequenceNumber = 0;
		std::string PrimarySite;
		std::string BackupSite;
		double GETofGeneration = 0.0;
		double TimeIncrement = 0.0;
		int Octals[2] = { 0,0 };
		int VehicleID = 0;
		int VerbSymbol = 70;
	};

	struct AGCLiftoffTimeUpdateMakeupTable
	{
		AGCLiftoffTimeUpdateMakeupTableBlock Blocks[2];
	} CZLIFTFF;

	struct AGCTimeIncrementMakeupTableBlock
	{
		int UpdateNo = 0;
		int SequenceNumber = 0;
		std::string PrimarySite;
		std::string BackupSite;
		double GETofGeneration = 0.0;
		double TimeIncrement = 0.0;
		int Octals[2] = { 0,0 };
		int VehicleID = 0;
		int VerbSymbol = 73;
	};

	struct AGCTimeIncrementMakeupTable
	{
		AGCTimeIncrementMakeupTableBlock Blocks[2];
	} CZTMEINC;

	struct AGCErasableMemoryUpdateData
	{
		int OctalData = 0x8000;
		bool EndOfDataFlag = true;
		int DataType = 0; //0 = address, 1 = single precision, 2 = double precision, 3 = 2nd of double prec, 4 = triple prec, 5 = 2nd of triple prec, 6 = 3rd of triple prec
		double EngineeringUnits = 0.0;
		double ValueLeastSignBit = 0.0;
	};

	struct AGCErasableMemoryUpdateMakeupBlock
	{
		int UpdateNo = 0;
		int SequenceNumber = 0;
		std::string PrimarySite;
		std::string BackupSite;
		double GETofGeneration = 0.0;
		int Index = 0; //Number of uplink values
		bool IsVerb72 = false; //false = V71, true = V72
		AGCErasableMemoryUpdateData Data[19];
	};

	struct AGCErasableMemoryUpdateMakeupTable
	{
		//0 = CMC A, 1 = CMC B, 2 = LGC A, 3 = LGC B
		AGCErasableMemoryUpdateMakeupBlock Blocks[4];
	} CZERAMEM;

	struct FIDOLaunchAnalogNo1DisplayTable
	{
		double LastUpdateTime = -1.0;
		std::vector<double> XVal;
		std::vector<double> YVal;
	} fdolaunchanalog1tab;

	struct FIDOLaunchAnalogNo2DisplayTable
	{
		double LastUpdateTime = -1.0;
		std::vector<double> XVal;
		std::vector<double> YVal;
	} fdolaunchanalog2tab;

	struct MPTManDisplayBuffer
	{
		std::string CSMSTAID;
		std::string LEMSTAID;
		std::string CSMGETAV;
		std::string LEMGETAV;
		std::vector<MPTManDisplay> man;
	} MPTDISPLAY;

	CheckoutMonitor EZCHECKDIS;

	struct VehicleNameTable
	{
		VehicleNameTable() {
			tab.push_back("CSM"); tab.push_back("LEM");
		}

		std::vector<std::string> tab;
	} MHGVNM;

	struct SunMoonEphemerisTable
	{
		int EPOCH;
		//MJD of first entry
		double MJD;
		//71 sets of data, 12 hours apart, covering 35 days, starting 5 days before midnight of launch day
		//0-2: Sun position vector (Er), 3-5: Moon position vector (Er), 6-8: Moon velocity vector (Er/hr), 9-17: Moon libration vector (MCI to MCT)
		double data[71][18];
	} MDGSUN;

	//System parameters for PDI
	LGCDescentConstants RTCCDescentTargets;
	LGCIgnitionConstants RTCCPDIIgnitionTargets;

	//RTCC MED Buffer
	char RTCCMEDBUFFER[256];
	//RTCC On-line Monitor Buffer
	struct OnlineMonitorMessage
	{
		std::vector<std::string> message;
	};

	struct OnlineMonitorBuffer
	{
		std::deque<OnlineMonitorMessage> data;
		std::string TextBuffer[3];
		double DoubleBuffer[8];
		int IntBuffer[2];
		VECTOR3 VectorBuffer[2];
		MATRIX3 MatrixBuffer;
		std::string LastMEDMessage;
	} RTCCONLINEMON;

	//AEG
	PMMAEG pmmaeg;
	PMMLAEG pmmlaeg;

	struct VehicleDataBuffer
	{
		EphemerisData sv;
		double csmmass;
		double lmascmass;
		double lmdscmass;
		double sivbmass;
		std::string config;
	} VEHDATABUF;

	double GetClockTimeFromAGC(agc_t *agc);
	double GetTEPHEMFromAGC(agc_t *agc, bool IsCMC);

private:
	void AP7ManeuverPAD(const AP7ManPADOpt &opt, AP7MNV &pad);
	void navcheck(VECTOR3 R, double GMT, int RBI, double &lat, double &lng, double &alt);
	void AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad);
	void AP11BlockData(AP11BLKOpt *opt, P37PAD &pad);
	void CMCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH);
	void LGCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH);
	void LandingSiteUplink(char *str, int veh);
	void AGCStateVectorUpdate(char *str, int comp, int ves, EphemerisData sv, bool v66 = false);
	void AGCStateVectorUpdate(char *str, SV sv, bool csm, bool v66 = false);
	void AGCDesiredREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, bool cmc = true);
	void AGCREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, bool cmc);
	void CMCRetrofireExternalDeltaVUpdate(char *list, double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH);
	void CMCEntryUpdate(char *list, double LatSPL, double LngSPL);
	void IncrementAGCTime(char *list, int veh, double dt);
	void IncrementAGCLiftoffTime(char *list, int veh, double dt);
	void TLANDUpdate(char *list, double t_land);
	void V7XUpdate(int verb, char *list, int* emem, int n);
	void SunburstAttitudeManeuver(char *list, VECTOR3 imuangles);
	void SunburstLMPCommand(char *list, int code);
	void SunburstMassUpdate(char *list, double masskg);
	void P27PADCalc(const P27Opt &opt, P27PAD &pad);
	int SPSRCSDecision(double a, VECTOR3 dV_LVLH);	//0 = SPS, 1 = RCS
	bool REFSMMATDecision(VECTOR3 Att); //true = everything ok, false = Preferred REFSMMAT necessary
	double FindOrbitalMidnight(SV sv, double t_TPI_guess);
	double FindOrbitalSunrise(SV sv, double t_sunrise_guess);
	void FindRadarAOSLOS(SV sv, double lat, double lng, double &GET_AOS, double &GET_LOS);
	void FindRadarMidPass(SV sv, double lat, double lng, double &GET_Mid);
	void papiWriteScenario_REFS(FILEHANDLE scn, char *item, int tab, int i, REFSMMATData in);
	bool papiReadScenario_REFS(char *line, char *item, int &tab, int &i, REFSMMATData &out);
	void DMissionRendezvousPlan(SV sv_A0, double &t_TPI0);
	void FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double t_TIG, double t_TPI, double &t_Ins, double &CSI);

	bool CalculationMTP_B(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C_PRIME(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_D(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_F(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_G(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_H1(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_SL(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);

	int CapeCrossingRev(int L, double GMT);
	double CapeCrossingGMT(int L, int rev);
	double CapeCrossingFirst(int L);
	double CapeCrossingLast(int L);
	void ECMPAY(EphemerisDataTable2 &EPH, ManeuverTimesTable &MANTIMES, double GMT, bool sun, double &Pitch, double &Yaw);
	//PMMMPT Begin Burn Time Computation Subroutine
	void PCBBT(double *DELT, double *WDI, double *TU, double W, double TIMP, double DELV, int NPHASE, double &T, double &GMTBB, double &GMTI, double &WA);
	//PMMMPT Matrix Utility Subroutine
	void PCMATO(double **A, double *Y, double **B, double *X, int M, int N, double *W1, double lambda, double *W2);
	//PMMMPT Gaussian Elimination Subroutine
	bool PCGAUS(double **A, double *Y, double *X, int N, double eps);
	//RTE Abort Scan Table (AST) Subroutine
	void PMMDAB(EphemerisData2 XIN, ASTInput ARIN, ASTSettings IRIN, ASTData &AST, int &IER, int IPRT);
	//Return-to-Earth Tradeoff Display
	void PMDTRDFF(int med, unsigned page);
	//Mission Plan Table Display
	void PMDMPT();
	//Rendezvous Evaluation Display Load Module
	void PMDRET();
	//Rendezvous Planning Table
	void PMDRPT();
	//Two-Impulse Multiple Solution Display
	void PMDTIMP();
public:
		//Ascent Rendezvous Monitoring Display
		void PMDARM(EphemerisData sv_CSM, EphemerisData sv_LM);
		//Short Ascent Rendezvous Monitoring Display
		void PMDSARM(EphemerisData sv_CSM, EphemerisData sv_LM);
protected:
	//GOST CSM/LM LCV Computation
	void EMMGLCVP(int L, double gmt, int body);
	//Relative Motion Digital Display
	void EMMRMD(int Veh1, int Veh2, double get, double dt, int refs, int axis, int ref_body, int mode, VECTOR3 Att = _V(0, 0, 0), double PYRGET = 0.0);

	//MPT utility functions
	bool MPTConfigSubset(const std::bitset<4> &CfgOld, const std::bitset<4> &CfgNew);
	bool MPTIsRCSThruster(int thruster);
	bool MPTIsPrimaryThruster(int thruster, int i);
	bool MPTIsUllageThruster(int thruster, int i);
	int MPTGetPrimaryThruster(int thruster);
public:
	//Trajectory Update On-line Print
	void EMGPRINT(std::string source, int i);
	void EMGPRINT(std::string source, std::vector<std::string> message);
	void MPTMassUpdate(VESSEL *vessel, MED_M50 &med1, MED_M55 &med2, MED_M49 &med3, bool docked = true);
	PLAWDTOutput GetWeightsTable(VESSEL *v, bool isCSM, bool isDocked) const;
	void MPTGetConfigFromString(const std::string &str, std::bitset<4> &cfg);
	void MPTGetStringFromConfig(const std::bitset<4> &cfg, char *str);
	MissionPlanTable *GetMPTPointer(int L);
protected:

	//Auxiliary subroutines
	int PMMXFRGroundRules(MissionPlanTable * mpt, double GMTI, unsigned ReplaceMan, bool &LastManReplaceFlag, double &LowerLimit, double &UpperLimit, unsigned &CurMan, double &VectorFetchTime);
	int PMMXFRFormatManeuverCode(int Table, int Thruster, int Attitude, unsigned Maneuver, std::string ID, int &TVC, std::string &code);
	int PMMXFRCheckConfigThruster(bool CheckConfig, int CCI, const std::bitset<4> &CCP, int TVC, int Thruster, std::bitset<4> &CC, std::bitset<4> &CCMI);
	int PMMXFRFetchVector(double GMTI, int L, EphemerisData &sv);
	int PMMXFRFetchAnchorVector(int L, EphemerisData &sv);
	void PMMXFRWeightAtInitiation(int CCI, int CCMI, double &weight);
	bool PMMXFRDeleteOption(int L, double GMTI);
	int PMMMCDCallEMSMISS(EphemerisData sv0, double GMTI, EphemerisData &sv1);
	int PMSVCTAuxVectorFetch(int L, double T_F, EphemerisData &sv);
public:
	EphemerisData ConvertSVtoEphemData(SV sv);
	SV ConvertEphemDatatoSV(EphemerisData sv, double mass = 0.0);
protected:

	//RTACF Routines

	struct RTACFGOSTInput
	{
		EphemerisData sv;
		double Weight;
		double get;
		VECTOR3 IMUAttitde; //All options except 4
		MATRIX3 REFSMMAT; //All options except 1

		int option;
		int starid1, starid2; //Options 1 and 3
		double LVLHRoll, LVLHYaw, WindowLine = 31.7*RAD;
		double star_ta1, star_ta2, star_sh1, star_sh2; //Options 1 and 5
		VECTOR3 Att1, Att2; //Option 5
	};

	struct RTACFGOSTOutput
	{
		VECTOR3 IMUAtt; //Option 4
		VECTOR3 LVLHAtt; //Option 4
	};

	void RTACFGuidanceOpticsSupportTable(RTACFGOSTInput in, RTACFGOSTOutput &out);

protected:
	//RTCC CLOCK TIMES
	double RTCC_GreenwichMeanTime;		//Time since midnight
	double RTCC_CSM_GroundElapsedTime;	//Time since MCGMTL (CSM liftoff time)
	double RTCC_LM_GroundElapsedTime;	//Time since MCGMTS (CSM liftoff time)

	double TimeofIgnition;
	double SplashLatitude, SplashLongitude;
	VECTOR3 DeltaV_LVLH;
	OBJHANDLE hEarth, hMoon;
	std::ofstream rtccdebug;

public:

	//Vessel pointers to be used exclusively by the RTCC MFD
	VESSEL *pCSM, *pLM;
	char CSMName[64];
	char LEMName[64];

	RTCCSystemParameters SystemParameters;
	rtcc::RTCCDynamicDisplayData DynamicDisplayData;
};

#endif