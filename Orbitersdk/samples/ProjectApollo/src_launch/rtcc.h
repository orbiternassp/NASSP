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
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/LDPP.h"
#include "../src_rtccmfd/EntryCalculations.h"

#define RTCC_START_STRING	"RTCC_BEGIN"
#define RTCC_END_STRING	    "RTCC_END"

#define RTCC_LAMBERT_MULTIAXIS 0
#define RTCC_LAMBERT_XAXIS 1

#define RTCC_LAMBERT_SPHERICAL 0
#define RTCC_LAMBERT_PERTURBED 1

#define RTCC_ENTRY_DEORBIT 0
#define RTCC_ENTRY_MCC 1
#define RTCC_ENTRY_ABORT 2
#define RTCC_ENTRY_CORRIDOR 3

#define RTCC_ENTRY_MINDV 0
#define RTCC_ENTRY_NOMINAL 1

#define RTCC_CONFIG_CSM 0
#define RTCC_CONFIG_LM 1
#define RTCC_CONFIG_CSM_LM 2
#define RTCC_CONFIG_CSM_SIVB 3
#define RTCC_CONFIG_LM_SIVB 4
#define RTCC_CONFIG_CSM_LM_SIVB 5

#define RTCC_CONFIGCHANGE_NONE 0
#define RTCC_CONFIGCHANGE_UNDOCKING 1
#define RTCC_CONFIGCHANGE_DOCKING 2
#define RTCC_CONFIGCHANGE_LM_STAGING 3

#define RTCC_ENGINETYPE_SPS 0
#define RTCC_ENGINETYPE_APS 1
#define RTCC_ENGINETYPE_DPS 2
#define RTCC_ENGINETYPE_RCSPLUS2 3
#define RTCC_ENGINETYPE_RCSPLUS4 4
#define RTCC_ENGINETYPE_RCSMINUS2 5
#define RTCC_ENGINETYPE_RCSMINUS4 6
#define RTCC_ENGINETYPE_SIVB_APS 7
#define RTCC_ENGINETYPE_LOX_DUMP 8
#define RTCC_ENGINETYPE_SIVB_MAIN 9

#define RTCC_MANVEHICLE_CSM 1
#define RTCC_MANVEHICLE_SIVB 2
#define RTCC_MANVEHICLE_LM 3

#define RTCC_GMP_PCE 1
#define RTCC_GMP_PCL 2
#define RTCC_GMP_PCT 3
#define RTCC_GMP_CRL 4
//Circularization at a specified height
#define RTCC_GMP_CRH 5
#define RTCC_GMP_HOL 6
#define RTCC_GMP_HOT 7
#define RTCC_GMP_HAO 8
#define RTCC_GMP_HPO 9
#define RTCC_GMP_NST 10
#define RTCC_GMP_NSO 11
#define RTCC_GMP_HBT 12
#define RTCC_GMP_HBH 13
#define RTCC_GMP_HBO 14
#define RTCC_GMP_FCT 15
#define RTCC_GMP_FCL 16
#define RTCC_GMP_FCH 17
#define RTCC_GMP_FCA 18
#define RTCC_GMP_FCP 19
#define RTCC_GMP_FCE 20
#define RTCC_GMP_NHT 21
//Combination maneuver to change both apogee and perigee and shift the node at a specified longitude
#define RTCC_GMP_NHL 22
//Maneuver to shift line-of-apsides some angle at a specified longitude
#define RTCC_GMP_SAL 23
//Maneuver to shift line-of-apsides to a specified longitude
#define RTCC_GMP_SAA 24
#define RTCC_GMP_PHL 25
#define RTCC_GMP_PHT 26
#define RTCC_GMP_PHA 27
#define RTCC_GMP_PHP 28
#define RTCC_GMP_CPL 29
#define RTCC_GMP_CPH 30
#define RTCC_GMP_SAT 31
//Maneuver to shift line-of-apsides some angle and keep the same apogee and perigee altitudes
#define RTCC_GMP_SAO 32
//Maneuver to change both apogee and perigee at a specified longitude
#define RTCC_GMP_HBL 33
#define RTCC_GMP_CNL 34
#define RTCC_GMP_CNH 35
//Height maneuver and node shift at a specified longitude
#define RTCC_GMP_HNL 36
#define RTCC_GMP_HNT 37
#define RTCC_GMP_HNA 38
#define RTCC_GMP_HNP 39
#define RTCC_GMP_CRT 40
#define RTCC_GMP_CRA 41
#define RTCC_GMP_CRP 42
#define RTCC_GMP_CPT 43
#define RTCC_GMP_CPA 44
#define RTCC_GMP_CPP 45
#define RTCC_GMP_CNT 46
#define RTCC_GMP_CNA 47
#define RTCC_GMP_CNP 48
#define RTCC_GMP_PCH 49
#define RTCC_GMP_NSH 50
//Node shift at a longitude
#define RTCC_GMP_NSL 51
#define RTCC_GMP_HOH 52
//Maneuver to change both apogee and perigee and place perigee at a certain longitude a certain number of revs later
#define RTCC_GMP_HAS 53

const double LaunchMJD[11] = {//Launch MJD of Apollo missions
	40140.62691,
	40211.535417,
	40283.666667,
	40359.700694,
	40418.563889,
	40539.68194,
	40687.80069,
	40982.849306,
	41158.5652869,
	41423.74583,
	41658.120139
};

//MANUAL ENTRY DEVICES

//Generate Station Contact Table
struct MED_B03
{
	int VEH = 2; //1 = LEM, 2 = CSM
};

//Suppress/Unsuppress C-Band Station Contacts Generation
struct MED_B04
{
	bool FUNCTION = false; //false = unsuppress, true = suppress
};

struct MED_F70
{
	std::string Site;
	double T_V;
	double T_omin;
	double T_omax;
	//1 = Constant G reentry (HB1), 2 = G&N reentry (HGN)
	int EntryProfile;
};

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
	int Vehicle = 1; //1 = LEM, 2 = CSM (Instead of Vector ID)
};

//Initialization for Lunar Descent Planning
struct MED_K17
{
	double Azimuth = 0.0;					//Greater or equal to zero, lower than 360°. If 0, LDPP will compute azimuth
	double DescIgnHeight = 50000.0*0.3048;	//Feet
	bool PoweredDescSimFlag = false;//true = simulate powered descent
	double PoweredDescTime = 0.0;	//Time for powered descent ignition
	int DwellOrbits = 0;			//Number of dwell orbits desired between DOI and PDI
	double DescentFlightTime = 11.0*60.0; //Minutes
	double DescentFlightArc = 15.0*RAD;	//Degrees
};

//Two Impulse Computation
struct MED_K30
{
	int Vehicle = 1; //0 = LEM, 1 = CSM
	int IVFlag = 0; //0 = Time of both maneuvers fixed, 1 = Time of first maneuver fixed, 2 = time of second maneuver fixed

};

//Burn parameters for CSM/LM direct input maneuvers
struct MED_M40
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

//Change Vehicle Weight
struct MED_M50
{
	int Table = 2; //1 = LEM, 2 = CSM
	double CSMWT = -1.0;
	double SIVBWT = -1.0;
	double LMWT = -1.0;
	double LMASCWT = -1.0;
};

//Input initial configuration for Mission Plan Table
struct MED_M55
{
	int Table = 2; //1 = LEM, 2 = CSM
	int ConfigCode = 0;
	double VentingGET = 0.0;
	double DeltaDockingAngle = 0.0;
};

//Transfer a GPM to the MPT
struct MED_M65
{
	int Table = 2; //1 = LEM, 2 = CSM
	int ReplaceCode = 0; //1-15
	int Thruster = RTCC_ENGINETYPE_SPS; //Thruster for the maneuver
	int Attitude = 1;		//Attitude option (0 = AGS External DV, 1 = PGNS External DV, 2 = Manual, 3 = Inertial, 4 = Lambert)
	double UllageDT = 0.0;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool Iteration = false; //false = do not iterate, true = iterate
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
	bool TimeFlag = true;	//false = Start at impulsive time, true = use optimum time
};

//Direct input of a maneuver to the MPT (CSM and LEM)
struct MED_M66
{
	int Table = 2; //1 = LEM, 2 = CSM
	std::string code; //Maneuver code 
	double GETBI = 0; //Time of ignition
	int Thruster = RTCC_ENGINETYPE_SPS; //Thruster for maneuver
	int AttitudeOpt = 0; //Attitude option
	int BurnParamNo = 1; //1 = P1, 2 = P2 etc.
	int CoordInd = 0; //0 = LVLH, 1 = IMU, 2 = FDAI
	VECTOR3 Att = _V(0, 0, 0);
	double UllageDT = 0.0;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool HeadsUp = true; //false = heads down, true = heads up
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	MATRIX3 REFSMMAT;	//Used for IMU and inertial coordinate options
	int ConfigChangeInd = 0; //0 = No change, 1 = Undocking, 2 = Docking
	int FinalConfig = 0;
	double DeltaDA = 0.0; //Delta docking angle
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
};

//TLI Direct Input
struct MED_M68
{
	int Table = 2; //1 = LEM, 2 = CSM
};

//Transfer a DKI, SPQ, or a Descent Plan to the MPT
struct MED_M70
{
	int Plan = 0; //-1 = Descent Plan, 0 = SPQ, 1 = DKI
	double DeleteGET = 0.0;
	int Thruster = RTCC_ENGINETYPE_RCSPLUS4; //Thruster for the maneuver
	int Attitude = 1;		//Attitude option (0 = AGS External DV, 1 = PGNS External DV, 2 = Manual, 3 = Inertial, 4 = Lambert)
	double UllageDT = 0.0;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool Iteration = false; //false = do not iterate, true = iterate
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
	bool TimeFlag = true;	//false = Start at impulsive time, true = use optimum time
};

//Transfer a Two Impulse Maneuver to the MPT
struct MED_M72
{
	bool Table = false; //false = corrective solution, M = multiple solution
	int Plan = 0; // Plan number to be transferred
	double DeleteGET = 0.0; //Deletes all maneuvers in both tables occurring after the input GET (no delete if 0)
	int Thruster = RTCC_ENGINETYPE_RCSPLUS4; //Thruster for the maneuver
	int Attitude = 1;		//Attitude option (0 = AGS External DV, 1 = PGNS External DV, 2 = Manual, 3 = Inertial, 4 = Lambert)
	double UllageDT = 0.0;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool Iteration = false; //false = do not iterate, true = iterate
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
	bool TimeFlag = true;	//false = Start at impulsive time, true = use optimum time
};

//LOI and MCC Transfer
struct MED_M78
{
	int Table = 2; //1 = LEM, 2 = CSM
	bool Type = false; //false = MCC, true = LOI
};

//Direct Input of Lunar Descent Maneuver
struct MED_M86
{
	int Veh = 1; //1 = LEM
	int ReplaceCode = 0;
	bool TimeFlag = false; //false = Nominal landing time to be input, true = Ignition time (only if replacing descent maneuver)
	bool HeadsUp = true; //false = heads down, true = heads up
};

//Space Digitals Initialization
struct MED_U00
{
	int VEH = 2; //1 = LEM, 2 = CSM
	int REF = 0; //0 = Earth, 1 = Moon
};

//Generate Space Digitals
struct MED_U01
{
	int ManualCol = 0;
	int OptionInd = 0; //0 = GET, 1 = MNV
	double GET = 0;
	int MNV = 0;
};

//Initiate C/O Monitor Display
struct MED_U02
{
	int VEH = 1;	//1 = LEM, 2 = CSM
	int IND = 0;	//1 = GMT, 2 = GET, 3 = MVI, 4 = MVE, 5 = RAD, 6 = ALT, 7 = FPA
	double ThresholdTime = 0.0;
	int REF = 0;	//0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT, 4 = EMP
	int FT = 0;		//0 = ER and ER/HR, 1 = ft and ft/s
	double IND_val = 0.0; //For options 0-1, 4-6
	unsigned IND_man = 0; //For options 2-3
};

//Moonrise/Moonset Times Initialization
struct MED_U07
{
	int IND = 0; //0 = REV, 1 = GET
	double PARM = 0.0; //Either GET or REV
};

//Sunrise/Sunset Times Initialization
struct MED_U08
{
	int IND = 0; //0 = REV, 1 = GET
	double PARM = 0.0; //Either GET or REV
	int REF = BODY_EARTH; //Reference body
};

//Predicted Site Acquisition
struct MED_U15
{
	int VEH = 2; //1 = LEM, 2 = CSM
	int IND = 1; //1 = GET, 2 = REV
	double PARAM1 = 0.0; //Begin Time for GET, Begin REV fo REV
	double PARAM2 = 0.0; //Delta Time for GET, End REV for REV
};

//Generate Detailed Maneuver Table
struct MED_U20
{
	int MPT_ID = 2;	//1 = LEM, 2 = CSM
	unsigned ManNo = 0; //Maneuver in table
	MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	bool HeadsUp = true;
};

struct LambertMan //Data for Lambert targeting
{
	double GETbase; //usually MJD at launch
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	int N;		//number of revolutions
	int axis;	//Multi-axis or horizontal burn
	int Perturbation; //Spherical or non-spherical gravity
	VECTOR3 Offset = _V(0, 0, 0); //Offset vector
	double PhaseAngle; //Phase angle to target
	double DH;					//Delta height at arrival
	SV sv_A;		//Chaser state vector
	SV sv_P;		//Target state vector
	bool NCC_NSR_Flag = false;	//true = NCC/NSR combination, false = TPI/TPF combination
	bool use_XYZ_Offset = true;	//true = use offset vector, false = use phase angle and DH
	double Elevation;	//Elevation angle at TPI
	int elevOpt = 0;		//0 = T1 on time, 1 = search for elevation angle
	int TPFOpt = 0;			//0 = T2 on time, 1 = use DT from T1, 2 = use travel angle
	double DT;				//Time between T1 and T2
	double WT;				//Central angle travelled between T1 and T2
};

struct AP7ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime; //time delay for the sextant star check (in case no star is available during the maneuver)
	double navcheckGET; //Time for the navcheck. 0 = no nav check
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct AP11ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime = 0; //time delay for the sextant star check (in case no star is available during the maneuver)
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	double alt = 0.0;	//Altitude above mean radius
};

struct AP11LMManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int enginetype = RTCC_ENGINETYPE_DPS; //Engine type used for the maneuver
	bool HeadsUp = false; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime = 0; //time delay for the sextant star check (in case no star is available during the maneuver)
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	double alt = 0.0;	//Altitude above mean radius
};

struct AP10CSIPADOpt
{
	SV sv0;
	double GETbase;
	double t_CSI;
	double t_TPI;
	VECTOR3 dV_LVLH;
	MATRIX3 REFSMMAT;
	int enginetype = RTCC_ENGINETYPE_RCSPLUS4; //Engine type used for the maneuver
	double KFactor = 0.0;
};

struct AP7TPIPADOpt
{
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	SV sv_A;
	SV sv_P;
};

struct AP9LMTPIPADOpt
{
	SV sv_A; //Chaser state vector
	SV sv_P; //Target state vector
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;	//REFSMMAT
};

struct AP9LMCDHPADOpt
{
	SV sv_A; //Chaser state vector
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;	//REFSMMAT
};

struct EarthEntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double GETbase; //usually MJD at launch
	double TIGguess; //Initial estimate for the TIG
	double ReA = 0; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	bool nominal; //Calculates minimum DV deorbit or nominal 31.7° line deorbit
	int enginetype = RTCC_ENGINETYPE_SPS;		//Engine type used for the maneuver
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool prediction = false;	//0 = Entry maneuver, 1 = Entry prediction
};

struct EntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double GETbase; //usually MJD at launch
	double TIGguess; //Initial estimate for the TIG or baseline TIG for abort and MCC maneuvers
	int type; //Type of reentry maneuver
	double ReA = 0; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	int enginetype;		//Engine type used for the maneuver
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	// relative range override
	double r_rbias = 1285.0;
	//Maximum DV
	double dv_max = 2804.0;
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


struct TLMCCResults
{
	VECTOR3 dV_LVLH;
	double P30TIG;
	double SplashdownLat, SplashdownLng;
	double PericynthionGET;
	double EntryInterfaceGET;
	double NodeLat;
	double NodeLng;
	double NodeAlt;
	double NodeGET;
	double EMPLatitude;
	double FRInclination;
	double t_Rev2Meridian;
	VECTOR3 dV_LVLH_LOI;
	VECTOR3 dV_LVLH_DOI;
	double h_peri_postDOI, h_apo_postDOI;
	SV sv_pre;
	SV sv_post;
};

struct TwoImpulseResuls
{
	VECTOR3 dV;
	VECTOR3 dV2;
	VECTOR3 dV_LVLH;
	double t_TPI;
	double T1;
	double T2;
};

struct SPQResults
{
	double t_CDH;
	double t_TPI;
	double DH;
	VECTOR3 dV_CSI;
	VECTOR3 dV_CDH;
};

struct RTEMoonOpt
{
	VESSEL* vessel;			//Reentry vessel
	double GETbase;			//usually MJD at launch
	double TIGguess = 0.0;	//Initial estimate for the TIG
	double EntryLng;		//Entry longitude
	int returnspeed = -1;	//0 = slow return, 1 = normal return, 2 = fast return
	SV RV_MCC;				//State vector as input
	int RevsTillTEI = 0;	//Revolutions until TEI
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool entrylongmanual = true; //Targeting a landing zone or a manual landing longitude
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver
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

struct REFSMMATOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int REFSMMATopt; //REFSMMAT options: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = Attitude, 8 = LS during TLC
	double REFSMMATTime; //Time for the REFSMMAT calculation
	double LSLng; //longitude for the landing site REFSMMAT
	double LSLat; //latitude for the landing site REFSMMAT
	double LSAzi; //approach azimuth for the landing site REFSMMAT
	int mission; //Just for the launch REFSMMAT
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool HeadsUp = true; //Orientation during the maneuver
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	MATRIX3 PresentREFSMMAT;	//Present REFSMMAT (for option 9)
	VECTOR3 IMUAngles;			//Desired Attitude (for option 9)
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;				//State vector as input
};

struct CDHOpt
{
	VESSEL* vessel; //Vessel executing the burn
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double DH; //Delta Height
	int CDHtimemode; //0 = Fixed Time, 1 = Find GETI
	double TIG; // (Estimated) Time of Ignition
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
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
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct EarthEntryPADOpt
{
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (deorbit maneuver)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (deorbit maneuver)
	MATRIX3 REFSMMAT;
	bool preburn; //
	double lat; //splashdown latitude
	double lng; //splashdown longitude
	SV sv0;
};

struct LunarEntryPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (MCC)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (MCC)
	MATRIX3 REFSMMAT;
	bool direct = true; //0 = with MCC, 1 = without
	double lat; //splashdown latitude
	double lng; //splashdown longitude
	SV sv0;
};

struct TLIManNode
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TLI_TIG; //Initial guess for TLI TIG
	double lat; //selenographic latitude
	double lng; //selenographic longitude
	double PeriGET; //time of pericynthion, initial guess
	double h_peri;	//flyby altitude
	SV RV_MCC;		//State vector as input
};

struct TLIManFR
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TLI_TIG; //Initial guess for TLI TIG
	double lat; //Earth-Moon-Plane
	double PeriGET; //time of pericynthion, initial guess
	double h_peri;	//flyby altitude
	SV RV_MCC;		//State vector as input
};

struct MCCNodeMan
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double MCCGET; //TIG for the MCC
	double lat; //target for MCC, selenographic latitude
	double lng; //selenographic longitude
	double NodeGET; //GET at node
	double h_node;	//node altitude
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver
};

struct MCCFRMan
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	int type; //0 = Fixed LPO, 1 = Free LPO
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver

	//LOI targets for BAP
	double LSlat;			//landing site latitude
	double LSlng;			//landing site longitude
	double alt;			//landing site height
	double azi;			//landing site approach azimuth
	double t_land;		//time of landing
	double LOIh_apo;		//apolune altitude
	double LOIh_peri;		//perilune altitude
};

struct MCCNFRMan
{
	VESSEL* vessel; //vessel
	int type;		//0 = fixed LPO, 1 = free LPO
	double GETbase; //usually MJD at launch
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver

	//LOI targets for BAP
	double LSlat;			//landing site latitude
	double LSlng;			//landing site longitude
	double alt;			//landing site height
	double azi;			//landing site approach azimuth
	double t_land;		//time of landing
	double LOIh_apo;		//apolune altitude
	double LOIh_peri;		//perilune altitude
	int N;				//Revs between DOI and PDI
	int DOIType;		//0 = Normal DOI, 1 = DOI as LOI-2
	double DOIPeriAng;	//Angle between landing site and perilune
	int LOIEllipseRotation = 0;	//0 = Choose the lowest DV solution, 1 = solution 1, 2 = solution 2
	double DOIPeriAlt = 50000.0*0.3048; //perilune altitude above landing site
};

struct MCCFlybyMan
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver
};

struct MCCSPSLunarFlybyMan
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude, initial guess
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	double FRInclination;
	bool AscendingNode;
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int enginetype = RTCC_ENGINETYPE_SPS; //Engine type used for the maneuver
};

struct LOIMan
{
	VESSEL* vessel;		//vessel
	int type = 0;		//0 = fixed approach azimuth, 1 = LOI at pericynthion
	double GETbase;		//usually MJD at launch
	double lat;			//landing site latitude
	double lng;			//landing site longitude
	double alt;			//landing site height
	double azi;			//landing site approach azimuth
	double t_land;		//time of landing
	double h_apo;		//apolune altitude
	double h_peri;		//perilune altitude
	SV RV_MCC;			//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int enginetype = RTCC_ENGINETYPE_SPS;	//Engine type to use for maneuver
	int impulsive = 0;	//0 = finite burntime, 1 = impulsive
	int EllipseRotation = 0;	//0 = Choose the lowest DV solution, 1 = solution 1, 2 = solution 2
};

struct LOI2Man
{
	VESSEL* vessel;			//vessel
	double GETbase;			//usually MJD at launch
	double EarliestGET = 0.0;	//Earliest GET for the LOI-2 maneuver
	double h_circ;			//altitude of circular orbit
	SV RV_MCC;				//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
	double alt = 0.0;		//altitude of the landing site
	int enginetype = RTCC_ENGINETYPE_SPS;	//Engine type to use for maneuver
};

struct DOIMan
{
	int opt;		//0 = DOI from circular orbit, 1 = DOI as LOI-2
	double GETbase; //usually MJD at launch
	double EarliestGET;	//Earliest GET for the DOI maneuver
	double lat; //landing site latitude
	double lng; //landing site longitude
	double alt;	//altitude of the landing site
	SV sv0;		//State vector as input
	int N = 0;	// Revolutions between DOI and PDI
	double PeriAng = 15.0*RAD;	//Angle from landing site to perilune
	double PeriAlt = 50000.0*0.3048; //perilune altitude above landing site
};

struct PCMan
{
	VESSEL* vessel; //vessel
	VESSEL* target; //target
	double GETbase; //usually MJD at launch
	double EarliestGET;	//Earliest GET for the PC maneuver
	double t_A; //time when the orbit is aligned with the landing site
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	bool landed; //0 = use lat/lng/alt to calculate landing site, 1 = target vessel on the surface
	double lat; //landing site latitude
	double lng; //landing site longitude
	double alt;	//altitude of the landing site
};

struct GMPOpt
{
	//0 = Fixed TIG, specify inclination, apoapsis and periapsis altitude
	//1 = Fixed TIG, specify apoapsis altitude
	//2 = Fixed TIG, specify periapsis altitude
	//3 = Fixed TIG, circularize orbit
	//4 = Circularize orbit at specified altitude
	//5 = Rotate velocity vector, specify apoapsis altitude
	//6 = Rotate line of apsides, perigee at specific longitude
	//7 = Optimal node shift maneuver
	int ManeuverCode;
	double GETbase; //usually MJD at launch
	SV RV_MCC;		//State vector as inputn or without
	bool AltRef = 0;	//0 = use mean radius, 1 = use launchpad or landing site radius
	double LSAlt;	//Landing site altitude, if used

	//maneuver parameters

	double TIG_GET;	//Threshold time or time of ignition
	double dW;		//Desired wedge angle change
	double long_D;	//Desired maneuver longitude
	double H_D;		//Desired maneuver height
	double dH_D;	//Desired change in height
	double dLAN;	//Desired change in the ascending node
	double H_A;		//Desired apogee height after the maneuver
	double H_P;		//Desired perigee height after the maneuver
	double dV;		//Input incremental velocity magnitude of the maneuver
	double Pitch;	//Input pitch of the maneuver
	double Yaw;		//Input yaw of the maneuver
	double dLOA;	//Line-of-apsides shift
	int N;			//Number of revolutions
};

struct TLIPADOpt
{
	VESSEL* vessel; //vessel
	SV sv0; //vessel state vector
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	double TLI;	//Time of Injection
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;
	VECTOR3 SeparationAttitude; //LVLH IMU angles
	VECTOR3 R_TLI;
	VECTOR3 V_TLI;
	bool uselvdc;	//LVDC in use/or not
};

struct P27Opt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double SVGET; //GET of the state vector
	double navcheckGET; //GET of the Nav Check
};

struct AGSSVOpt
{
	SV sv;
	MATRIX3 REFSMMAT;
	bool csm;
	double GETbase;
	double AGSbase;
};

struct SkyRendOpt
{
	VESSEL* vessel;		//vessel
	VESSEL* target;		//Target vessel
	double GETbase;		//usually MJD at launch
	int man;			//0 = Presettings, 1 = NC1, 2 = NC2, 3 = NCC, 4 = NSR, 5 = TPI, 6 = TPM, 7 = NPC
	bool PCManeuver;	//0 = NC1 is setting up NPC, 1 = NC2 is setting up NPC
	bool NPCOption;		//0 = NC1 or NC2 with out-of-plane component, setting up a NPC maneuver 90° later
	double TPIGuess;	//Estimate for the TPI time
	double t_TPI;		//Time of TPI
	double E_L;			//Elevation angle at TPI
	double t_C;			//Time of Ignition
	double DH1;			//Delta Height at NCC
	double DH2;			//Delta Height at NSR
	double n_C;
	double t_NC;		//Reference time for the NPC maneuver
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked; //0 = CSM alone, 1 = CSM/DM
};

struct SkylabRendezvousResults
{
	double P30TIG;
	VECTOR3 dV_LVLH;

	double t_NC2;
	double t_NCC;
	double t_NSR;
	double t_TPI;

	double dv_NC2;
	double dv_NCC;

	double dH_NC2;

	VECTOR3 dV_NSR;
};

struct LunarLiftoffTimeOpt
{
	LunarLiftoffTimeOpt();
	double lat;
	double lng;
	double alt;
	double GETbase;		//usually MJD at launch
	double t_hole;		//Threshold time
	SV sv_CSM;			//CSM State vector
	int opt;			// 0 = Concentric Profile, 1 = Direct Profile, 2 = time critical direct profile
	double dt_2;		//Fixed time from insertion to TPI for direct profile
	double theta_1;		//Angle travelled between liftoff and insertion
	double dt_1;		//Ascent burn time (liftoff to insertion)
	double DH;			//Delta height for concentric profile
	bool IsInsVelInput;	//0 = calculate insertion velocity, 1 = use input velocity
	double v_LH;
	double v_LV;
	double theta_F;		//Transfer angle for terminal phase
	double E;			//Elevation angle to initiate terminal phase on
};

struct LunarLiftoffResults
{
	double t_L;
	double t_Ins;
	double t_CSI;
	double t_CDH;
	double t_TPI;
	double t_TPF;
	double v_LH;
	double v_LV;
	double DV_CSI;
	double DV_CDH;
	double DV_TPI;
	double DV_TPF;
	double DV_T;
};

struct PDIPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
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
	double GETbase;
	VECTOR3 R_LS;
	double TIG;
	double v_LH;
	double v_LV;
	SV sv_CSM;
	MATRIX3 Rot_VL;	//Rotation Matrix, vessel to local, left-handed
};

struct LMARKTRKPADOpt
{
	SV sv0; //Input state vector
	double GETbase; //usually MJD at launch
	double LmkTime[4]; //initial guess for time over landmark
	double lat[4];		//landmark latitude
	double lng[4];		//landmark longitude
	double alt[4] = { 0,0,0,0 };	//landmark altitude
	int entries;
};

struct DKIOpt	//Docking Initiation Processor
{
	SV sv_A;
	SV sv_P;
	double GETbase;
	double t_TIG;
	double t_TPI_guess;
	double DH;
	double E;
	int tpimode = 0;	//0 = TPI on time, 1 = TPI at orbital midnight, 2 = TPI at X minutes before sunrise
	//DKI RENDEZVOUS PLANS:
	//0 = Phasing, CSI 0.5 revs later, CDH 0.5 revs later
	//1 = Phasing with +50 ft/s DVZ, at apolune, CDH 0.5 revs later
	//2 = Height, CSI (Phasing) 0.5 revs later, CDH 2xN revs later
	//3 = Just calculate TPI time
	//4 = High Dwell Sequence
	int plan = 0;
	bool maneuverline = true;	//false = use input delta times, true = use 0.5 revolutions
	bool radial_dv = false;		//false = horizontal maneuver, true = 50 ft/s radial component
	int N_HC = 1;				//Number of half revs between CSI and CDH
	int N_PB = 1;				//Number of half revs between Phasing and Boost/Height

	double dt_TPI_sunrise = 16.0*60.0;
	double DeltaT_PBH = 55.0*60.0;	//Delta time between phasing and boost/CSI
	double DeltaT_BHAM = 60.0*60.0;	//Delta time between boost and HAM
	double Delta_HAMH = 60.0*60.0;	//Delta time between HAM and CSI
};

struct DKIResults
{
	VECTOR3 DV_Phasing;
	double t_Boost;
	double dv_Boost;
	double t_HAM;
	double t_CSI;
	double dv_CSI;
	double t_CDH;
	VECTOR3 DV_CDH;
	double t_TPI;
};

struct SPQOpt //Coelliptic Sequence Processor
{
	SV sv_A;
	SV sv_P;
	double GETbase;
	double t_TIG;
	double t_TPI;				// Only for calculation type = 0
	double DH = 15.0*1852.0;	// Only for calculation type = 1
	double E = 26.6*RAD;
	bool K_CSI = true;	//0 = No CSI scheduled, 1 = CSI scheduled
	//bool CDH = true;	//0 = No CDH scheduled, 1 = CDH scheduled 
	//int I_CDH;			//CDH option.
	int K_CDH;			//Height iteration. 0 = fixed TIG at TPI, 1 = fixed DH at CDH
	int K_TPI = 0;		//-1 = Midpoint of darkness, 0 = on time, 1 = on longitude
	bool CalculateTPIParams = true;
};

struct PDAPOpt //Powered Descent Abort Program
{
	SV sv_A;
	SV sv_P;
	double GETbase;
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
	double h_amin = 30.0*1852.0;
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
	double GMTID;	//GMT of the state vector
	double GETID;	//GET of the state vector
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
	char REFR[64];	//Reference planet of requested vector
	double GETBV;	//Time tag of vector from which apogee/perigee values were computed
	double HAR;		//Height of next apogee at GETA, as requested
	double PAR;		//Latitude of next apogee at GETA, as requested
	double LAR;		//Longitude of next apogee at GETA, as requested
	double GETAR;	//Time of arrival at next apogee, as requested
	double HPR;		//Height of next apogee at GETP, as requested
	double PPR;		//Latitude of next apogee at GETP, as requested
	double LPR;		//Longitude of next apogee at GETP, as requested
	double GETPR;	//Time of arrival at next apogee, as requested
};

struct FIDOOrbitDigitalsOpt
{
	SV sv_A;
	double GETbase;
	double MJD;		//MJD to update the state vector to (only used in continuous update)
	double LSAlt = 0.0;
};

struct SpaceDigitals
{
	SpaceDigitals();
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

	bool Init;
};

struct SpaceDigitalsOpt
{
	SV sv_A;
	double GETbase;
	double LSLat;
	double LSLng;
	double LSAlt;
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
	int NV;			//Number of vectors used to interpolate for vector displayed
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
};

struct DetailedManeuverTable
{
	DetailedManeuverTable();
	char C_STA_ID[10];
	double C_GMTV;
	double C_GETV;
	char C_CODE[10];
	char L_STA_ID[10];
	double L_GMTV;
	double L_GETV;
	char REF[10];
	char X_STA_ID[10];
	double X_GMTV;
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
	int REFSMMAT_ID;
	double DEL_P;
	double DEL_Y;
	VECTOR3 VG;
	VECTOR3 IMUAtt;
	VECTOR3 BodyAtt;
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
};

struct MPTManeuver
{
	MPTManeuver();
	MPTSV sv_before;
	MPTSV sv_after;
	int TVC; //Maneuvering vehicle: 1 = CSM, 2 = S-IVB, 3 = LM
	int ID;
	int Thruster;
	VECTOR3 A_T; //Unit thrust vector
	VECTOR3 dV_LVLH; //In P30 coordinates
	MATRIX3 M_B; //Body direction matrix
	int ConfigCodeBefore;
	int ConfigCodeAfter;
	double CSMMassAfter;
	double SIVBMassAfter;
	double LMMassAfter;
	double TotalMassAfter;
	double DV_M;
	double dt;
};

struct MPTManDisplay
{
	MPTManDisplay();
	double AftMJD;
	double BefMJD;
	std::string code;
	double HA;
	double HP;
	double dt;
	double DV;
	int ID;
};

struct MPTable
{
	std::deque<MPTManeuver> mantable;
	double CSMInitMass = 0.0;
	double LMInitMass = 0.0;
	double SIVBInitMass = 0.0;
	double TotalInitMass = 0.0;
	int InitConfigCode = -1;
	double DeltaDockingAngle = 0.0;
	int TUP = 0;
	MPTSV AnchorVector;
};

struct FullMPTable
{
	FullMPTable() { LunarStayTimes[0] = -1; LunarStayTimes[1] = -1; }

	std::deque<MPTManDisplay> fulltable;
	MPTable cmtable;
	MPTable lmtable;
	double LunarStayTimes[2];
};

struct NextStationContact
{
	NextStationContact();
	double GETAOS;
	double GETLOS;
	char StationID[4];
	double DELTAT;
	double MAXELEV;
	bool BestAvailableAOS;
	bool BestAvailableLOS;
	bool BestAvailableEMAX;
	int REV;

	//For sorting
	bool operator<(const NextStationContact& rhs) const;
};

struct OrbitStationContactsTable
{
	NextStationContact Stations[45];
};

struct NextStationContactsTable
{
	NextStationContact NextStations[6];
	double GET = 0.0;
};

struct PredictedSiteAcquisitionTable
{
	NextStationContact Stations[40];
};

struct MANTIMESData
{
	double ManData[2];
};

struct MANTIMES
{
	std::vector<MANTIMESData> Table;
};

struct CapeCrossingTable
{
	CapeCrossingTable();
	int TUP;
	int NumRev;
	int NumRevFirst;
	int NumRevLast;
	double GETEphemFirst;
	double GETEphemLast;
	//Time of last known cape crossing before the time of the update vector (zero if unknown)
	double GETCrossPrev;
	double GETCross[30];
};

struct EphemerisDataTable
{
	int TUP = 0;
	std::vector<MPTSV> table;
	MANTIMES mantimes;
};

struct Station
{
	double lat = 0.0;
	double lng = 0.0;
	double alt = 0.0;
	char code[4];
};

struct StationTable
{
	std::vector<Station> table;
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
	double SN_LK_A; //???
};

struct SunriseSunsetData
{
	//Revolution number
	int REV = 0;
	//GET of terminator rise from liftoff
	double GETTR = 0.0;
	//GET of sunrise from liftoff
	double GETSR = 0.0;
	//Pitch at sunrise
	double theta_SR = 0.0;
	//Yaw at sunrise
	double psi_SR = 0.0;
	//GET of terminator set from liftoff
	double GETTS = 0.0;
	//GET of sunset from liftoff
	double GETSS = 0.0;
	//Pitch at sunset
	double theta_SS = 0.0;
	//Yaw at sunset
	double psi_SS = 0.0;
};

struct SunriseSunsetTable
{
	int num = 0;
	SunriseSunsetData data[8];
};

// Parameter block for Calculation(). Expand as needed.
struct calculationParameters {
	Saturn *src;		// Our ship
	VESSEL *tgt;		// Target ship
	double TLI;			// Time of TLI
	VECTOR3 R_TLI;		//TLI cutoff position vector
	VECTOR3 V_TLI;		//TLI cutoff velocity vector
	double LOI;			// Time of LOI/Pericynthion
	double SEP;			// Time of separation
	double DOI;			// Time of DOI
	double PDI;			// Time of PDI
	double TLAND;		// Time of landing
	double LunarLiftoff;// Time of lunar liftoff
	double LSAlt;		// Height of the lunar landing site relative to mean lunar radius
	double LSAzi;		// Approach azimuth to the lunar landing site
	double LSLat;		// Latitude of the lunar landing site
	double LSLng;		// Longitude of the lunar landing site
	double Insertion;	// Time of Insertion
	double Phasing;		// Time of Phasing
	double CSI;			// Time of CSI
	double CDH;			// Time of CDH
	double TPI;			// Time of TPI
	double TEI;			// Time of TEI
	double EI;			// Time of Entry Interface
	double lat_node;
	double lng_node;
	double alt_node;
	double GET_node;
	MATRIX3 StoredREFSMMAT;
	double TEPHEM;	// MJD of CMC liftoff time
	double PericynthionLatitude;	//Latitude of pericynthion in Earth-Moon Plane coordinates
	double TIGSTORE1;		//Temporary TIG storage
	VECTOR3 DVSTORE1;		//Temporary DV storage
	SV SVSTORE1;			//Temporary state vector storage
};

//For LVDC
struct SevenParameterUpdate
{
	double T_RP;	//Time of Restart Preparation (TB6)
	double C3;
	double Inclination;
	double e;
	double alpha_D;
	double f;
	double theta_N;
};

struct LVDCTLIparam
{
	MATRIX3 MX_A;
	VECTOR3 TargetVector;
	double alpha_TS;
	double Azimuth;
	double beta;
	double C_3;
	double cos_sigma;
	double e_N;
	double f;
	double mu;
	double omega_E;
	double phi_L;
	double R_N;
	double T_2R;
	double theta_EO;
	double TB5;
	double T_L;
	double t_D;
	double T_RG;
	double T_ST;
	double Tt_3R;
	double t_clock;
};

struct GPMPRESULTS
{
	double GET_A;
	double HA;
	double long_A;	//longitude of apoapsis
	double lat_A;	//latitude of apoapsis
	double GET_P;
	double HP;
	double long_P;	//longitude of periapsis
	double lat_P;	//latitude of periapsis
	double A;
	double E;
	double I;
	double Node_Ang;
	double Del_G;
	double Pitch_Man;
	double Yaw_Man;
	double H_Man;
	double long_Man;
	double lat_Man;
};

class RTCC {

	friend class MCC;

public:
	RTCC();
	void Init(MCC *ptr);
	bool Calculation(int mission, int fcn,LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);

	void SetManeuverData(double TIG, VECTOR3 DV);
	void GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD);

	void AP7TPIPAD(const AP7TPIPADOpt &opt, AP7TPI &pad);
	void AP9LMTPIPAD(AP9LMTPIPADOpt *opt, AP9LMTPI &pad);
	void AP9LMCDHPAD(AP9LMCDHPADOpt *opt, AP9LMCDH &pad);
	void TLI_PAD(TLIPADOpt* opt, TLIPAD &pad);
	bool PDI_PAD(PDIPADOpt* opt, AP11PDIPAD &pad);
	void LunarAscentPAD(ASCPADOpt opt, AP11LMASCPAD &pad);
	void EarthOrbitEntry(const EarthEntryPADOpt &opt, AP7ENT &pad);
	void LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad);
	void LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res);
	double TPISearch(SV sv_A, SV sv_P, double GETbase, double elev);
	double FindDH(SV sv_A, SV sv_P, double GETbase, double TIGguess, double DH);
	MATRIX3 REFSMMATCalc(REFSMMATOpt *opt);
	void EntryTargeting(EntryOpt *opt, EntryResults *res);//VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude, double &GET05G, double &RTGO, double &VIO, double &ReA, int &precision);
	void BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res);
	void TranslunarInjectionProcessorNodal(TLIManNode *opt, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut);
	void TranslunarInjectionProcessorFreeReturn(TLIManFR *opt, TLMCCResults *res, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut);
	void TranslunarMidcourseCorrectionTargetingNodal(MCCNodeMan &opt, TLMCCResults &res);
	bool TranslunarMidcourseCorrectionTargetingFreeReturn(MCCFRMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingNonFreeReturn(MCCNFRMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingFlyby(MCCFlybyMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(MCCSPSLunarFlybyMan *opt, TLMCCResults *res, int &step);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node, SV &sv_pre, SV &sv_post);
	void LOI2Targeting(LOI2Man *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void LOI2Targeting(LOI2Man *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_pre, SV &sv_post);
	void DOITargeting(DOIMan *opt, VECTOR3 &DV, double &P30TIG);
	void DOITargeting(DOIMan *opt, VECTOR3 &dv, double &P30TIG, double &t_PDI, double &t_L, double &CR);
	int LunarDescentPlanningProcessor(SV sv, double GETbase, double lat, double lng, double rad, LunarDescentPlanningTable &table);
	void PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_pre, SV &sv_post);
	bool GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG);
	bool GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG, GPMPRESULTS &res);
	OBJHANDLE AGCGravityRef(VESSEL* vessel); // A sun referenced state vector wouldn't be much of a help for the AGC...
	void NavCheckPAD(SV sv, AP7NAV &pad, double GETbase, double GET = 0.0);
	void AGSStateVectorPAD(AGSSVOpt *opt, AP11AGSSVPAD &pad);
	void AP11LMManeuverPAD(AP11LMManPADOpt *opt, AP11LMMNV &pad);
	void AP11ManeuverPAD(AP11ManPADOpt *opt, AP11MNV &pad);
	void AP10CSIPAD(AP10CSIPADOpt *opt, AP10CSI &pad);
	void CSMDAPUpdate(VESSEL *v, AP10DAPDATA &pad);
	void LMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool asc = false);
	void RTEMoonTargeting(RTEMoonOpt *opt, EntryResults *res);
	void LunarOrbitMapUpdate(SV sv0, double GETbase, AP10MAPUPDATE &pad, double pm = -150.0*RAD);
	void LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad);
	SevenParameterUpdate TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double GETbase, double P30TIG, double TB5, double mu, double T_RG);
	void LVDCTLIPredict(LVDCTLIparam lvdc, double m0, SV sv_A, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_IG, SV &sv_TLI);
	SV PMMSPT(LVDCTLIparam lvdc, SV sv_A);
	void LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step);
	void FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, bool agc = true);
	void FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_tig, SV &sv_cut, bool agc = true);
	void EngineParametersTable(int enginetype, double &Thrust, double &Isp);
	VECTOR3 ConvertDVtoLVLH(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_imp);
	VECTOR3 ConvertDVtoInertial(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_LVLH_imp);
	void PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, SV &sv_pre, SV &sv_post, bool agc = true);
	void PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc = true);
	double GetDockedVesselMass(VESSEL *vessel);
	SV StateVectorCalc(VESSEL *vessel, double SVMJD = 0.0);
	SV ExecuteManeuver(SV sv, double GETbase, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster);
	SV ExecuteManeuver(SV sv, double GETbase, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster, MATRIX3 &Q_Xx, VECTOR3 &V_G);
	void TLIFirstGuessConic(SV sv_mcc, double lat, double h_peri, double PeriMJD, VECTOR3 &DV, VECTOR3 &var_converged);
	void TLMCIntegratedXYZT(SV sv_mcc, double lat_node, double lng_node, double h_node, double MJD_node, VECTOR3 DV_guess, VECTOR3 &DV);
	VECTOR3 TLMCEmpiricalFirstGuess(double r, double dt);
	void IntegratedTLMC(SV sv_mcc, double lat, double h, double gamma, double MJD, VECTOR3 var_guess, VECTOR3 &DV, VECTOR3 &var_converged, SV &sv_node);
	void TLMCFirstGuessConic(SV sv_mcc, double lat, double h, double gamma, double MJD_P, VECTOR3 &DV, VECTOR3 &var_converged);
	void TLMCFirstGuess(SV sv_mcc, double lat_EMP, double h_peri, double MJD_P, VECTOR3 &DV, SV &sv_peri);
	bool TLIFlyby(SV sv_TLI, double lat_EMP, double h_peri, SV sv_peri_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry);
	bool TLMCFlyby(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry);
	bool TLMCFlybyConic(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry);
	bool TLMCConicFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP);
	bool TLMCIntegratedFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP);
	bool TLMCConic_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor);
	bool TLMC_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor);
	bool TLMCConic_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor);
	bool TLMC_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor);
	void LaunchTimePredictionProcessor(const LunarLiftoffTimeOpt &opt, LunarLiftoffResults &res);
	bool LunarLiftoffTimePredictionCFP(const LunarLiftoffTimeOpt &opt, VECTOR3 R_LS, SV sv_P, OBJHANDLE hMoon, double h_1, double theta_Ins, double t_L_guess, double t_TPI, LunarLiftoffResults &res);
	bool LunarLiftoffTimePredictionTCDT(const LunarLiftoffTimeOpt &opt, VECTOR3 R_LS, SV sv_P, OBJHANDLE hMoon, double h_1, double t_L_guess, LunarLiftoffResults &res);
	bool LunarLiftoffTimePredictionDT(const LunarLiftoffTimeOpt &opt, VECTOR3 R_LS, SV sv_P, OBJHANDLE hMoon, double h_1, double t_L_guess, LunarLiftoffResults &res);
	void LunarAscentProcessor(VECTOR3 R_LS, double m0, SV sv_CSM, double GETbase, double t_liftoff, double v_LH, double v_LV, double &theta, double &dt_asc, double &dv, SV &sv_IG, SV &sv_Ins);
	bool PoweredDescentProcessor(VECTOR3 R_LS, double TLAND, SV sv, double GETbase, MATRIX3 REFSMMAT, SV &sv_PDI, SV &sv_land, double &dv);
	void EntryUpdateCalc(SV sv0, double GETbase, double entryrange, bool highspeed, EntryResults *res);
	bool DockingInitiationProcessor(DKIOpt opt, DKIResults &res);
	void ConcentricRendezvousProcessor(const SPQOpt &opt, SPQResults &res);
	void AGOPCislunarNavigation(SV sv, MATRIX3 REFSMMAT, int star, double yaw, VECTOR3 &IMUAngles, double &TA, double &SA);
	VECTOR3 LOICrewChartUpdateProcessor(SV sv0, double GETbase, MATRIX3 REFSMMAT, double p_EMP, double LOI_TIG, VECTOR3 dV_LVLH_LOI, double p_T, double y_T);
	SV coast(SV sv0, double dt);
	MPTSV coast(MPTSV sv0, double dt);
	MPTSV coast_conic(MPTSV sv0, double dt);
	VECTOR3 HatchOpenThermalControl(VESSEL *v, MATRIX3 REFSMMAT);
	VECTOR3 PointAOTWithCSM(MATRIX3 REFSMMAT, SV sv, int AOTdetent, int star, double dockingangle);
	void DockingAlignmentProcessor(DockAlignOpt &opt);
	//option: 0 = propagate to specified MJD, 1 = to mean anomaly, 2 = to argument of latitude
	SV GeneralTrajectoryPropagation(SV sv0, int opt, double param);
	void ApsidesDeterminationSubroutine(SV sv0, SV &sv_a, SV &sv_p);
	VECTOR3 HeightManeuverInteg(SV sv0, double dh);
	VECTOR3 ApoapsisPeriapsisChangeInteg(SV sv0, double r_AD, double r_PD);
	VECTOR3 CircularizationManeuverInteg(SV sv0);
	void ApsidesArgumentofLatitudeDetermination(SV sv0, double &u_x, double &u_y);
	bool GETEval(double get);
	bool PDIIgnitionAlgorithm(SV sv, double GETbase, VECTOR3 R_LS, double TLAND, MATRIX3 REFSMMAT, SV &sv_IG, double &t_go, double &CR, VECTOR3 &U_IG);
	bool PoweredDescentAbortProgram(PDAPOpt opt, PDAPResults &res);
	VECTOR3 RLS_from_latlng(double lat, double lng, double alt);

	//Mission Operations Control Room Displays
	void FIDOOrbitDigitalsUpdate(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsCycle(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsApsidesCycle(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsCalculateLongitude(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsCalculateGETL(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsCalculateGETBV(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOSpaceDigitalsUpdate(const SpaceDigitalsOpt &opt, SpaceDigitals &res);
	void FIDOSpaceDigitalsCycle(const SpaceDigitalsOpt &opt, SpaceDigitals &res);
	void FIDOSpaceDigitalsGET1(const SpaceDigitalsOpt &opt, SpaceDigitals &res);
	void FIDOSpaceDigitalsGET2(const SpaceDigitalsOpt &opt, SpaceDigitals &res);
	void FIDOSpaceDigitalsGET3(const SpaceDigitalsOpt &opt, SpaceDigitals &res);
	//Orbit Station Contact Generation Control
	void EMSTAGEN(double GETBase, double GET, OrbitStationContactsTable &res);
	//Next Station Contact Display
	void EMDSTAC(const OrbitStationContactsTable &in, double GET, NextStationContactsTable &out);
	//Predicted and Experimental Site Acquisition Displays

	//Actual RTCC Subroutines
	void EMDPESAD(const OrbitStationContactsTable &in, PredictedSiteAcquisitionTable &out);
	//LM AGS External DV Coordinate Transformation Subroutine
	VECTOR3 PIAEDV(VECTOR3 DV, VECTOR3 R_CSM, VECTOR3 V_CSM, VECTOR3 R_LM, bool i);
	//External DV Coordinate Transformation Subroutine
	VECTOR3 PIEXDV(VECTOR3 R_ig, VECTOR3 V_ig, double WT, double T, VECTOR3 DV, bool i);
	//Generalized Coordinate System Conversion Subroutine
	int ELVCNV(const PZEFEM &ephem, SV sv, int in, int out, SV &sv_out);
	//Checkout Monitor Display
	int EMDCHECK(FullMPTable &mptable, double GETbase, double LSAlt, CheckoutMonitor &res);
	//Detailed Maneuver Table Display
	int PMDDMT(FullMPTable &mptable, double GETbase, double LSAlt, DetailedManeuverTable &res);
	//Lunar Descent Planning Table Display
	void PMDLDPP(const LDPPOptions &opt, const LDPPResults &res, LunarDescentPlanningTable &table);
	//Time of Longitude Crossing Subroutine
	double RLMTLC(const EphemerisDataTable &ephemeris, double GETBase, double long_des, double GET_min, double &GET_cross);
	//Cape Crossing Table Generation
	int RMMEACC(const EphemerisDataTable &ephemeris, double GETBase, CapeCrossingTable &table);
	//Environment Change Calculations
	int EMMENV(const EphemerisDataTable &ephemeris, double GETBase, double GET_begin, bool sun, SunriseSunsetTable &table);
	//Sunrise/Sunset Display
	void EMDSSEMD(double GETBase);
	//Moonrise/Moonset Display
	void EMDSSMMD(double GETBase);
	//Ephemeris Fetch Routine
	int ELFECH(double MJD, int L, unsigned vec_tot, unsigned vec_bef, EphemerisDataTable &EPHEM);
	//RTE Tradeoff Display Sort and Order Routine
	int PMQREAP(const std::vector<TradeoffData> &TOdata);
	//Return to Earth Abort Planning Supervisor
	void PMMREAP(int med);
	//'F' MED Module
	int PMQAFMED(int med);
	//'P' Code MED Processor
	void GMSMED(int med);
	void FDOLaunchAnalog1(MPTSV sv);
	void FDOLaunchAnalog2(MPTSV sv);

	//Skylark
	bool SkylabRendezvous(SkyRendOpt *opt, SkylabRendezvousResults *res);
	bool NC1NC2Program(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh_F, double n_H1, int s, double dh, double n_C, VECTOR3 &dV_NC1_LVLH, double &dh_NC2, double &dv_NC2, double &t_NC2, VECTOR3 &dV_NC2_LVLH, double &dv_NCC, double &t_NCC, double &t_NSR, VECTOR3 &dV_NSR, bool NPC = false);
	void NCCProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh, VECTOR3 &dV_NCC_LVLH, double &t_NSR, VECTOR3 &dV_NSR_LVLH);
	void NSRProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t2, double t3, VECTOR3 &dV_NSR_LVLH);
	void NPCProgram(SV sv_C, SV sv_W, double GETbase, double t, double &t_NPC, VECTOR3 &dV_NPC_LVLH);

	//Mission Planning
	int MPTAddTLI(FullMPTable &mptable, SV sv_IG, SV sv_TLI, double DV, int L);
	int MPTAddDescent(FullMPTable &mptable, SV sv_IG, SV sv_land, double LSAlt, double DV);
	int MPTAddAscent(FullMPTable &mptable, SV sv_IG, SV sv_asc, double LSAlt, double DV);
	int MPTAddManeuver(FullMPTable &mptable, SV sv_ig, SV sv_cut, const char *code, double LSAlt, double DV, int L, int Thruster, int CCI = RTCC_CONFIGCHANGE_NONE, int CC = RTCC_CONFIG_CSM, int opt = 0);
	int MPTDirectInput(MPTManeuver &man, SV sv_ig, SV &sv_cut);
	int MPTDeleteManeuver(FullMPTable &mptable);
	void MPTTrajectoryUpdate(FullMPTable &mptable, SV sv, int L, double GETBase);
	bool MPTTrajectory(FullMPTable &mptable, SV &sv_out, int L);
	bool MPTTrajectory(FullMPTable &mptable, SV &sv_out, int L, unsigned mnv);
	bool MPTTrajectory(FullMPTable &mptable, double GET, double GETbase, SV &sv_out, int L);
	bool MPTHasManeuvers(FullMPTable &mptable, int L);
	int MPTConfirmManeuver(FullMPTable &mptable, int L);
	//Weight Change Module
	int PMMWTC(FullMPTable &mptable, int med);
	//Weight Determination at a Time
	int PLAWDT(FullMPTable &mptable, int L, double mjd, double &cfg_weight);
	int PLAWDT(FullMPTable &mptable, int L, double mjd, int &cfg, double &cfg_weight, double &csm_weight, double &lm_weight, double &sivb_weight);
	int MPTGetVesselConfiguration(const FullMPTable &mptable, int L, double GET, double GETbase);
	int MPTGetVesselConfiguration(const FullMPTable &mptable, int L);
	int MPTCopyEphemeris(FullMPTable &mptable, double GETbase);

	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state

	MCC *mcc;
	struct calculationParameters calcParams;

	//MEDs

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

	struct MED_P08
	{
		double PitchAngle;
	} med_p08;

	struct MED_P10
	{
		//1 = LEM, 2 = CSM
		int VEH;
		//Liftoff time in hours
		double GMTALO;
		//Traj or no traj
		bool TRAJ;
	} med_p10;

	struct MED_P12
	{
		//0 = CSM, 1 = IU1, 2 = IU2
		int VEH;
		double GMTGRR;
		double LaunchAzimuth;
	} med_p12;

	//Enter vector in Keplerian elements (spherical coordinates)
	struct MED_P13
	{
		//0 = CSM, 1 = LEM
		int VEH;
		//Velocity in ft/s
		double Vel;
		//Flight path angle in degrees (-90° to 90°)
		double FPA;
		//Azimuth in degrees (0° to 360°)
		double Azi;
		//Geocentric latitude in degrees (-90° to 90°)
		double Lat;
		//Longitude in degrees (-180° to 180°)
		double Lng;
		//Height above oblate Earth
		double Height;
		//Time in hours
		double Time;
		//Ephemeris Indicator
		int EPHIND = 0;
		//Coordinate system indicator (0 = ECT, 1 = MCT)
		int COORDIND;
	} med_p13;

	//Enter vector in X,Y,Z format
	struct MED_P14
	{
		//0 = CSM, 1 = LEM
		int VEH;
		//Position (-60 to 60 Er)
		double X;
		double Y;
		double Z;
		//Velocity (-60 to 60 Er/hr)
		double X_dot;
		double Y_dot;
		double Z_dot;
		//Time in hours
		double Time;
		//Ephemeris Indicator
		int EPHIND = 0;
		//Coordinate system indicator (0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT, 4 = EMP, 5 = PLUM)
		int COORDIND;
	} med_p14;

	//Update GMTZS for specified vehicle
	struct MED_P15
	{
		//0 = AGC, 1 = LGC, 2 = AGS
		int VEH;
		double GMTZS;
		double DT;
	} med_p15;

	//Generate an ephemeris for one vehicle using a vector from the other vehicle
	struct MED_P16
	{
		int OldVeh = 12; //1 = LEM, 2 = CSM
		int NewVeh = 1; //1 = LEM, 2 = CSM
		double GMT = 0.0;
		unsigned ManNum = 0;
	} med_p16;

	//Cape Crossing Table Update and Limit Change
	struct MED_P17
	{
		int VEH = 1; //1 = LEM, 2 = CSM
		bool IsEarth = true;
		int REV = 1;
	} med_p17;

	//Modify A and E used to determine integration limits
	struct MED_P30
	{
		//In NM
		double SMA;
		double ECC;
	} med_p30;

	//Initialize phase reference time (GET)
	struct MED_P31
	{
		double GET;
	} med_p31;

	//Update scaling factor used by venting model
	struct MED_P33
	{
		double ScaleFactor;
	} med_p33;

	//Offsets and elevation angle for two-impulse solution
	struct MED_P51
	{
		double DH = 0.0; //Delta Height
		double Phase = 0.0; //Phase Angle
		double E = 0.0; //Elevation Angle
		double WT = 0.0; //Travel angle for terminal phase
	} med_p51;

	//Two-Impulse Corrective Combination Nominals
	struct MED_P52
	{
		double GET_NSR = 0.0; //Nominal Time of NSR Maneuver
		double DH_NSR = 0.0; //Nominal Height Difference at NSR
		double PH_NSR = 0.0; //Phase Angle at NSR
	} med_p52;

	//Initialize number of vehicles, first launch vehicle, mission date
	struct MED_P80
	{
		int NumVeh = 1;
		int FirstVeh;
		int Month;
		int Day;
		int Year;
		int DeltaDay;
	} med_p80;

	MED_B03 med_b03;
	MED_B04 med_b04;
	MED_F70 med_f70;
	MED_K16 med_k16;
	MED_K17 med_k17;
	MED_M40 med_m40;
	MED_M50 med_m50;
	MED_M55 med_m55;
	MED_M65 med_m65;
	MED_M66 med_m66;
	MED_M68 med_m68;
	MED_M70 med_m70;
	MED_M72 med_m72;
	MED_M78 med_m78;
	MED_U00 med_u00;
	MED_U01 med_u01;
	MED_U02 med_u02;
	MED_U07 med_u07;
	MED_U08 med_u08;
	MED_U15 med_u15;
	MED_U20 med_u20;

	//Data Tables
	PZEFEM pzefem;
	EphemerisDataTable EZNITCSM;
	EphemerisDataTable EZNITLEM;
	CapeCrossingTable EZCCSM;
	CapeCrossingTable EZCLEM;
	SunriseSunsetTable EZSSTAB;
	SunriseSunsetTable EZMMTAB;
	TradeoffDataDisplay RTETradeoffTable;

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
		std::string RTESite;
		double RTEVectorTime;
		double RTET0Min;
		double RTET0Max;
		double RTETimeOfLanding;
		double RTEUADVMax;
		double RTEPTPMissDistance;
		double RTEInclination;
		int EntryProfile;
		int RTETradeoffRemotePage;

		//Block 13
		int RTETradeoffLabelling[5];
	} PZREAP;

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
private:
	void AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad);
	MATRIX3 GetREFSMMATfromAGC(agc_t *agc, double AGCEpoch, int addroff = 0);
	double GetClockTimeFromAGC(agc_t *agc);
	double GetTEPHEMFromAGC(agc_t *agc);
	void navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt);
	double getGETBase();
	double CalcGETBase();
	void AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad);
	void AP11BlockData(AP11BLKOpt *opt, P37PAD &pad);
	LambertMan set_lambertoptions(SV sv_A, SV sv_P, double GETbase, double T1, double T2, int N, int axis, int Perturbation, VECTOR3 Offset, double PhaseAngle);
	void AGCExternalDeltaVUpdate(char *str, double P30TIG,VECTOR3 dV_LVLH, int DVAddr = 3404);
	void LandingSiteUplink(char *str, double lat, double lng, double alt, int RLSAddr);
	void AGCStateVectorUpdate(char *str, SV sv, bool csm, double AGCEpoch, double GETbase, bool v66 = false);
	void AGCDesiredREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, bool cmc = true, bool AGCCoordSystem = false);
	void AGCREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, int offset = 0, bool AGCCoordSystem = false);
	void CMCRetrofireExternalDeltaVUpdate(char *list, double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH);
	void CMCEntryUpdate(char *list, double LatSPL, double LngSPL);
	void IncrementAGCTime(char *list, double dt);
	void TLANDUpdate(char *list, double t_land, int tlandaddr);
	void V71Update(char *list, int* emem, int n);
	void V72Update(char *list, int *emem, int n);
	void SunburstAttitudeManeuver(char *list, VECTOR3 imuangles);
	void SunburstLMPCommand(char *list, int code);
	void SunburstMassUpdate(char *list, double masskg);
	void P27PADCalc(P27Opt *opt, double AGCEpoch, P27PAD &pad);
	int SPSRCSDecision(double a, VECTOR3 dV_LVLH);	//0 = SPS, 1 = RCS
	bool REFSMMATDecision(VECTOR3 Att); //true = everything ok, false = Preferred REFSMMAT necessary
	double PericynthionTime(VESSEL* vessel);
	SV FindPericynthion(SV sv0);
	void CalcSPSGimbalTrimAngles(double CSMmass, double LMmass, double &ManPADPTrim, double &ManPADYTrim);
	double FindOrbitalMidnight(SV sv, double GETbase, double t_TPI_guess);
	double FindOrbitalSunrise(SV sv, double GETbase, double t_sunrise_guess);
	void FindRadarAOSLOS(SV sv, double GETbase, double lat, double lng, double &GET_AOS, double &GET_LOS);
	void FindRadarMidPass(SV sv, double GETbase, double lat, double lng, double &GET_Mid);
	double GetSemiMajorAxis(SV sv);
	void papiWriteScenario_SV(FILEHANDLE scn, char *item, SV sv);
	bool papiReadScenario_SV(char *line, char *item, SV &sv);
	void DMissionRendezvousPlan(SV sv_A0, double GETbase, double &t_TPI0);
	void FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double GETbase, double t_TIG, double t_TPI, double &t_Ins, double &CSI);

	bool CalculationMTP_B(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C_PRIME(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_D(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_F(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_G(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);

	//Generalized Contact Generator
	void EMGENGEN(const EphemerisDataTable &ephemeris, const StationTable &stationlist, double GETbase, OrbitStationContactsTable &res);
	//Horizon Crossing Subprogram
	bool EMXING(const EphemerisDataTable &ephemeris, const Station & station, double GETbase, std::vector<NextStationContact> &acquisitions);
	//Variable Order Interpolation
	int ELVARY(const std::vector<MPTSV> &EPH, unsigned ORER, double MJD, bool EXTRAP, MPTSV &sv_out);
	//Extended Interpolation Routine
	int ELVCTR(const EphemerisDataTable &EPH, unsigned ORER, double MJD, MPTSV &sv_out, int option, double *LUNRSTAY = NULL);
	//
	int CapeCrossingRev(int L, double GET);
	double CapeCrossingGET(int L, int rev);
	void ECMPAY(const EphemerisDataTable &EPH, double MJD, bool sun, double &Pitch, double &Yaw);
	//Spherical to Inertial Conversion
	int EMMXTR(double vel, double fpa, double azi, double lat, double lng, double h, VECTOR3 &R, VECTOR3 &V);
	//base hour Angle
	double PIGMHA(int E, int Y, int D);

	bool MPTConfigIncludesCSM(int config);
	bool MPTConfigIncludesLM(int config);
	bool MPTConfigIncludesSIVB(int config);
	double MPTConfigMass(int config, double CSMMass, double LMMass, double SIVBMass);

	double GLHTCS(double FLTHRS) { return FLTHRS * 360000.0; }
	double GLCSTH(double FIXCSC) { return FIXCSC / 360000.0; }
	double TJUDAT(int Y, int M, int D);

protected:
	double TimeofIgnition;
	double SplashLatitude, SplashLongitude;
	VECTOR3 DeltaV_LVLH;
	int REFSMMATType;

	struct GeneralConstraintsTable
	{
		//Block 1
		int Year;
		//Block 2
		int RefDayOfYear;
		//Block 3
		int DaysInYear;
		//Block 4
		int MonthofLiftoff;
		int DayofLiftoff;
		int DaysinMonthofLiftoff;
		//Block 5
		double ElevationAngle;
		//Block 8
		double TerminalPhaseAngle;
		//Block 9
		double TIDeltaH;
		//Block 10
		double TIPhaseAngle;
		//Block 11
		double TIElevationAngle;
		//Block 12
		double TITravelAngle;
		//Block 13
		double TINSRNominalTime;
		//Block 14
		double TINSRNominalDeltaH;
		//Block 15
		double TINSRNominalPhaseAngle;
		//Block 18 Bytes 5-8
		int TPFDefinition;
		//Block 25
		double DKIDeltaH;
		//Block 26
		double TPFDefinitionValue;
		//Block 27
		double MinPerigee;
		//Block 28 Bytes 1-4
		double DeltaNSR;
		//Block 29
		double TPIDefinitionValue;
		//Block 30
		double OrbitsFromCSItoTPI;
		//Block 31 Bytes 1-4
		int TPIDefinition;
		//Block 31 Bytes 5-8
		int TPICounterNum;
		//Block 32 Bytes 5-8
		int PhaseAngleSetting;
		//Block 36
		double ActualDH;
		//Block 37
		double ActualPhaseAngle;
		//Block 38
		double ActualWedgeAngle;
		//Block 40
		double LDPPAzimuth;
		//Block 41
		double LDPPHeightofPDI;
		//Block 42 1st word
		int LDPPDwellOrbits;
		//Block 42 2nd word
		bool LDPPPoweredDescentSimFlag;
		//Block 43
		double LDPPTimeofPDI;
		//Block 44
		double LDPPDescentFlightTime;
		//Block 45
		double LDPPDescentFlightArc;
		//Block 46
		double SPQDeltaH;
		//Block 47
		double SPQElevationAngle;
		//Block 48
		double SPQTerminalPhaseAngle;
		//Block 49
		double SPQMinimumPerifocus;

	} GZGENCSN;

	//RTCC System Parameters
	
	//Burnout launch azimuth (rad.)
	double MCLABN;
	//Sine of burnout launch azimuth
	double MCLSBN;
	//Cosine of burnout launch azimuth
	double MCLCBN;
	//Pitch angle from horizon (rad.)
	double MCGHZA;
	//L/O time first vehicle (hrs.)
	double MCGMTL;
	//L/O time second vehicle (hrs.)
	double MCGMTS;
	//L/O time first vehicle (centisec.)
	double MGLGMT;
	//L/O time second vehicle (centisec.)
	double MGGGMT;
	//CSM GMTGRR (hrs.)
	double MCGRAG;
	//IU1 GMTGRR (hrs.)
	double MCGRIC;
	//IU2 GMTGRR (hrs.)
	double MCGRIL;
	//AGC GMTZS (hrs.)
	double MCGZSA;
	//LGC GMTZS (hrs.)
	double MCGZSL;
	//AGS GMTZS (hrs.)
	double MCGZSS;
	//Semimajor axis (Er.)
	double MCGSMA;
	//Eccentricity
	double MCGECC;
	//Phase reference time - GET (hrs.)
	double MCGREF;
	//Venting scale factor
	double MCTVEN;
	//Lambda Zero
	double MCLAMD;

	//MJD of launch day (days)
	double GMTBASE;


	//CONSTANTS
	//Nautical miles per Earth radii
	const double MCCNMC = 3443.93359;
};


#endif