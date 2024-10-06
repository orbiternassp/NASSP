/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2019

  RTCC Tables

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
#include "Orbitersdk.h"

struct EphemerisData
{
	double GMT = 0.0;
	VECTOR3 R = _V(0, 0, 0);
	VECTOR3 V = _V(0, 0, 0);
	int RBI = -1; //0 = Earth, 1 = Moon
};

struct EphemerisData2
{
	double GMT = 0.0;
	VECTOR3 R = _V(0, 0, 0);
	VECTOR3 V = _V(0, 0, 0);
};

struct SV2
{
	EphemerisData sv;
	double Mass = 0.0;
	double AttachedMass = 0.0;
};

struct EphemerisHeader
{
	//Update number
	int TUP = 0;
	//Vehicle code
	int VEH = 0;
	//Coordinate indicator
	int CSI = 0;
	//Vector offset
	unsigned Offset = 0;
	//Number of vectors returned
	unsigned NumVec = 0;
	//Status indicator
	int Status = 0;
	//Time of first returned vector
	double TL = 0.0;
	//Time of last returned vector
	double TR = 0.0;
};

struct EphemerisDataTable2
{
	EphemerisHeader Header;
	std::vector<EphemerisData2> table;
};

struct RTCCNIInputTable
{
	//Word 1
	//Open
	//Word 2-4, R and V at maneuver initiation (first phase)
	VECTOR3 R;
	//Word 5-7
	VECTOR3 V;
	//Word 8, maximum time between ephemeris vectors
	double DTOUT;
	//Word 9, Weight loss rate multiplier (normally 1.0)
	double WDMULT = 1.0;
	//Word 10, density multiplier
	double DENSMULT = 1.0;
	//Word 11 (Bytes 1,2), ephemeris output option: 0 for none, 1 for R and V, 2 for R and V and weights
	int IEPHOP = 0;
	//Word 11 (Bytes 3,4), auxiliary output option (false = table not desired, true = table desired)
	bool KAUXOP = false;
	//Word 11 (Bytes 5,6), maximum no. of ephemeris vectors
	unsigned MAXSTO = 9999999;
	//Word 16
	double Area = 0.0;
	//Word 19, S-IVB weight at maneuver initiation
	double SIVBWT;
	//Word 20, Total configuration weight at maneuver initiation
	double CAPWT;
	//Word 21 (Bytes 1,2), attitude control mode (6 = IGM)
	int MANOP;
	//Word 21 (Bytes 3,4), thruster code (36 = S-IVB main thruster)
	int ThrusterCode;
	//Word 22 (Bytes 1,2), configuration at maneuver initiation (3 = CSM/S-IVB, 14 = LM/S-IVB, 15 = CSM/LM/S-IVB)
	unsigned IC;
	//Word 22 (Bytes 3,4), maneuvering vehicle code (2 = S-IVB)
	int MVC;
	//Word 22 (Bytes 7,8), frozen maneuver indicator (true = frozen)
	bool IFROZN;
	//Word 23 (Bytes 1,2), reference body indicator (0 = Earth, 1 = Moon)
	int IREF;
	//Word 23 (Bytes 3,4), coordinate system indicator (0 = ECI)
	int ICOORD;
	//Word 25, Time to begin maneuver (1st phase), GMT
	double GMTI;
	//Word 28, dt of maneuver excluding ullage and tailoff. DT < 0 generates a zero maneuver
	double DTINP = 0.0;
	//Word 29-41
	//Open
	//Word 42
	double GMTI_frozen;
	//Word 43-45
	VECTOR3 R_frozen;
	//Word 46-48
	VECTOR3 V_frozen;
	//Word 53-67
	double Params[15];
	union
	{
		double Word68d;
		int Word68i[2];
	};
	//Word 69-74
	double Params2[6];

	double GMTBASE;
	double CSMWT, LMAWT, LMDWT;
};

struct RTCCNIAuxOutputTable
{
	//SV at main engine on
	//Word 1
	double GMT_BI;
	//Word 2-4
	VECTOR3 R_BI;
	//Word 5-7
	VECTOR3 V_BI;
	//SV at end of maneuver (after tailoff)
	//Word 8
	double GMT_BO;
	//Word 9-11
	VECTOR3 R_BO;
	//Word 12-14
	VECTOR3 V_BO;
	//Word 15-17, unit thrust vector at main engine on
	VECTOR3 A_T;
	//Word 18-26, body axes
	VECTOR3 X_B, Y_B, Z_B;
	//Word 30, duration of main engine burn to cutoff signal
	double DT_B;
	//Word 31, DT of tailoff
	double DT_TO;
	//Word 33-34, Pitch and Yaw gimbal at main engine on
	double P_G;
	double Y_G;
	//Word 35, Total tailoff DV
	double DV_TO;
	//Word 36, total DV (includes ullage and tailoff)
	double DV;
	//Word 37, DV along X-body axis (includes ullage, excludes tailoff)
	double DV_C;
	//Word 38, DV along X-body axis of tailoff
	double DV_cTO;
	//Word 39, DV of pure ullage
	double DV_U;
	//Words 40-41: Open (CSM and LM weight?)
	double W_CSM, W_LMA, W_LMD;
	//Word 42: S-IVB weighr at maneuver initiation
	double W_SIVB;
	//Word 43, total configuration weight at maneuver initiation
	double WTINIT;
	//Word 45, weight at main engine on
	double WTENGON;
	//Word 46, weight at end of maneuver
	double WTEND;
	//Word 48, main fuel weight loss during maneuver
	double MainFuelUsed;
	//Word 49 (Bytes 1-2), reference body indicator (0 = Earth)
	bool RBI;
	//Word 49 (Byptes 3-4), coordinate system indicator (0 = ECI)
	int CSI;
	//Words 50-56, SV at beginning of thrust (first phase)
	VECTOR3 R_1;
	VECTOR3 V_1;
	double GMT_1;
	//RCS fuel weight loss during maneuver
	double RCSFuelUsed;
	//Velocity-to-be-gained
	VECTOR3 V_G;
	//SV at main engine on, without ullage (free flight)
	EphemerisData sv_FF;
	//Eccentricity of target conic (TLI)
	double Word60;
	//Energy of target conic (TLI)
	double Word61;
	//True anomaly of descending node of target conic (TLI)
	double Word62;
	//Estimate true anomaly of target conic (TLI)
	double Word63;
	//Semi-latus rectum of target conic (TLI)
	double Word64;
	//K5 (mu/P) (TLI)
	double Word65;
	//Desired terminal radius (TLI)
	double Word66;
	//Desired terminal velocity (TLI)
	double Word67;
	//Desired terminal flight-path angle (TLI)
	double Word68;
	//Gravitational acceleration at maneuver burnout (TLI)
	double Word69;
};

struct MANTIMESData
{
	MANTIMESData() { ManData[0] = 0.0;ManData[1] = 0.0; }
	double ManData[2];
};

struct ManeuverTimesTable
{
	int TUP = 0;
	std::vector<MANTIMESData> Table;
};

struct EMSMISSAuxOutputTable
{
	EphemerisData sv_cutoff;
	bool landed;
	//0 = no errors detected, 1 = input time cannot be referenced on sun/moon ephemeris, 2 = MPT is being updated, 3 = error from maneuver integrator, 4 = PLAWDT error
	//5 = maneuver in interval maybe preventing the minimum number of points from being satisfied, 6 = ephemeris space filled before request was satisfied
	int ErrorCode;
	double InputArea;
	double InputWeight;
	double CutoffArea;
	double CutoffWeight;
	//1 = maximum time, 2 = radius, 3 = altitude, 4 = flight path angle, 5 = reference switch, 6 = beginning of maneuver, 7 = end of maneuver, 8 = ascending node
	int TerminationCode;
	//Maneuver number of last processed maneuver
	unsigned ManeuverNumber;
	double LunarStayBeginGMT;
	double LunarStayEndGMT;
};

struct PLAWDTInput
{
	double T_UP;				//Option 1: Time of desired, areas/weights. Option 2: Time to stop adjustment
	int Num = 0;				//Option 1: Maneuver number of last maneuver to be ignored (zero to consider all maneuvers). Option 2: Configuration code associated with input values (same format as MPT code)
	bool KFactorOpt = false;	//0 = No K-factor desired, 1 = K-factor desired
	int TableCode;		//1 = CSM, 3 = LM (MPT and Expandables Tables). Negative for option 2.
	bool VentingOpt = false;	//0 = No venting, 1 = venting
	double CSMArea;
	double SIVBArea;
	double LMAscArea;
	double LMDscArea;
	double CSMWeight;
	double SIVBWeight;
	double LMAscWeight;
	double LMDscWeight;
	//Time of input areas/weights
	double T_IN;
};

struct PLAWDTOutput
{
	PLAWDTOutput()
	{
		Err = 0;
		ConfigArea = 0.0;
		ConfigWeight = 0.0;
		CSMArea = 0.0;
		SIVBArea = 0.0;
		LMAscArea = 0.0;
		LMDscArea = 0.0;
		CSMWeight = 0.0;
		SIVBWeight = 0.0;
		LMAscWeight = 0.0;
		LMDscWeight = 0.0;
		KFactor = 1.0;
	}

	//0: No error
	//1: Request time within a maneuver - previous maneuver values used
	//2: Maneuver not current - last current values used
	//3: Time to stop adjustment is before time of input areas/weights - input values returned as output
	int Err;
	std::bitset<4> CC;
	double ConfigArea;
	double ConfigWeight;
	double CSMArea;
	double SIVBArea;
	double LMAscArea;
	double LMDscArea;
	double CSMWeight;
	double SIVBWeight;
	double LMAscWeight;
	double LMDscWeight;
	double KFactor;
};

struct EMSLSFInputTable
{
	bool ECIEphemerisIndicator = false;
	bool ECTEphemerisIndicator = false;
	bool MCIEphemerisIndicator = false;
	bool MCTEphemerisIndicator = false;
	//Left limit of ephemeris (time to begin ephemeris)
	double EphemerisLeftLimitGMT;
	//Right limit of ephemeris (time to end ephemeris)
	double EphemerisRightLimitGMT;
	EphemerisDataTable2 *ECIEphemTableIndicator = NULL;
	EphemerisDataTable2 *ECTEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCIEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCTEphemTableIndicator = NULL;
	//Storage interval for lunar surface ephemeris
	double LunarEphemDT = 3.0*60.0;
};

struct ReferenceSwitchTable
{
	EphemerisData InputVector;
	EphemerisData BeforeRefSwitchVector;
	EphemerisData AfterRefSwitchVector;
	EphemerisData LastVector;
};

struct EMSMISSInputTable
{
	EphemerisData AnchorVector;
	bool landed = false;
	//Desired value of stopping parameter relative to the Earth
	double EarthRelStopParam = 0.0;
	//Desired value of stopping parameter relative to the Moon
	double MoonRelStopParam = 0.0;
	//Maximum time of integration
	double MaxIntegTime = 10.0*24.0*3600.0;
	//Storage interval for maneuver ephemeris
	double ManEphemDT = 10.0;
	//Storage interval for lunar surface ephemeris
	double LunarEphemDT = 3.0*60.0;
	//Density multiplier value
	double DensityMultiplier = 1.0;
	//Left limit of ephemeris (time to begin ephemeris)
	double EphemerisLeftLimitGMT;
	//Right limit of ephemeris (time to end ephemeris)
	double EphemerisRightLimitGMT;
	//Minimum time between ephemeris points
	double MinEphemDT = 0.0;
	//Reference frame of desired stopping parameter (0 = Earth, 1 = Moon, 2 = both)
	int StopParamRefFrame = 2;
	//Minimum number of points desired in ephemeris
	unsigned MinNumEphemPoints = 9;
	bool ECIEphemerisIndicator = false;
	bool ECTEphemerisIndicator = false;
	bool MCIEphemerisIndicator = false;
	bool MCTEphemerisIndicator = false;
	//Ephemeris build indicator
	bool EphemerisBuildIndicator = false;
	//Maneuver cut-off indicator (0 = cut at begin of maneuver, 1 = cut at end of maneuver, 2 = don't cut off)
	int ManCutoffIndicator;
	//Descent burn indicator
	bool DescentBurnIndicator = false;
	//Cut-off indicator (1 = Time, 2 = radial distance, 3 = altitude above Earth or moon, 4 = flight-path angle, 5 = first reference switch, 6 = first ascending node relative to the Earth, 7 = longitude, 8 = latitude)
	int CutoffIndicator = 1;
	//Integration direction indicator (+X-forward, -X-backward)
	double IsForwardIntegration = 1.0;
	//Coordinate system indicator (Part of Anchor Vector)
	//Maneuver indicator (true = consider maneuvers, false = don't consider maneuvers)
	bool ManeuverIndicator = false;
	//Vehicle code (1 = LEM, 3 = CSM)
	int VehicleCode;
	//Density multiplication override indicator
	bool DensityMultOverrideIndicator = false;
	//Table of ephemeris addresses indicator
	EphemerisDataTable2 *ECIEphemTableIndicator = NULL;
	EphemerisDataTable2 *ECTEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCIEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCTEphemTableIndicator = NULL;
	//Reference switch table indicator
	ReferenceSwitchTable *RefSwitchTabIndicator = NULL;
	//Maneuver times table indicator
	ManeuverTimesTable *ManTimesIndicator = NULL;
	//Runge-Kutta auxiliary output table indicator
	RTCCNIAuxOutputTable *AuxTableIndicator = NULL;
	//Runge-Kutta dense ephemeris table indicator
	//Update in process override indicator (true = override, false = don't override)
	bool UIPOverrideIndicator = false;
	//Maneuver number of last maneuver to be ignored
	unsigned IgnoreManueverNumber = 10000U;
	EMSMISSAuxOutputTable NIAuxOutputTable;
	bool useInputWeights = false;
	PLAWDTOutput *WeightsTable = NULL;
};

struct RMMYNIInputTable
{
	VECTOR3 R0, V0;
	double GMT0;
	double lat_T, lng_T;
	//Backup mode G-level
	double g_c_BU = 0.05;
	//G&N mode G-level
	double g_c_GN = 0.05;
	double LAD = 0.27;
	double LOD = 0.207;
	double CGBIAS = 0.0;
	//Initial bank angle for G&N simulation
	double C10 = 0.0;
	double H_EMS = -1.0;
	//Mode: 1 = Zero lift, 2 = Max Lift, 3 = G&N, 4 = Bank angle - time to reverse bank angle, 5 = Constant bank angle, 6 = Constant bank to G, then roll, 7 = Bank angle to G-level then maximum lift
	//8 = Bank angle to a G-level then bank angle-time to reverse bank angle, 9 = Bank angle to a G-level then another bank angle to impact prediction, 10 = constant G
	int KSWCH;
	//Initial reentry bank angle
	double K1;
	//Second bank angle to be flown after g = g_c
	double K2;
	//Desired constant g level (m/s^2)
	double D0 = 4.0*9.80665;
	//Roll direction for the constant g mode
	double RLDIR = 1.0;
	//Time to reverse bank angle
	double t_RB = 0.0;
	double CMArea = -1.0;
	double CMWeight = -1.0;
};

struct RMMYNIOutputTable
{
	double lat_IP = 0.0;
	double lng_IP = 0.0;
	double t_drogue = 0.0;
	double t_main = 0.0;
	double t_lc = 0.0;
	double t_05g = 0.0;
	double t_2g = 0.0;
	double DRE_2g = 0.0;
	double t_gc = 0.0;
	double gmax = 0.0;
	double t_gmax = 0.0;
	double t_BBO = 0.0;
	double t_EBO = 0.0;
	double R_EMS = 0.0;
	double V_EMS = 0.0;
	double t_V_Circ = 0.0;
	//1 = time limit, 2 = impact, 3 = skipout
	int IEND;
};

struct ReentryManeuverDefinition
{
	//State vector at main engine on
	EphemerisData sv0;
	//Vehicle area
	double A;
	//Configuration weight
	double WT;
	//1 = fixed inertial, 2 = manual holding body orientation, 3 = Lambert Guidance, 4 = External DV (primary), 5 = External DV (LM AGS)
	int Attitude;
	int Thruster;
	//0 = two thrusters, 1 = four thrusters
	bool UllageOption;
	//Configuration code at maneuver initiation
	unsigned Config;
	//DT of ullage
	double dt_ullage;
	//DT of maneuver
	double DTMAN;
	//DV desired
	double DVMAN;
	//Unit thrust vector at burn initiate
	VECTOR3 AT;
	//Velocity-to-be-gained for External DV
	VECTOR3 VG;
};

struct ReentryConstraintsDefinition
{
	int LiftMode = 0;
	double GNInitialBank = 0.0;
	double InitialBankAngle = 0.0;
	double FinalBankAngle = 0.0;
	double GMTReverseBank = 0.0;
	double GLevel = 0.0;
	double BackupLiftMode = 0.0;
	double lat_T = 0.0;
	double lng_T = 0.0;
	//1 or -1
	double RollDirection = 0.0;
	double ConstantGLevel = 0.0;
};

struct ReentryConstraintsTable
{
	ReentryConstraintsDefinition entry;
};

struct RetrofireDisplayParametersTableData
{
	//0 = good data, +1 = no data, -1 = bad data
	int Indicator = 1;
	//Ullage quad (2 or 4)
	int UllageQuads;
	std::string BurnCode;
	std::string Area;
	std::string RefsID;
	double CSMWeightRetro;
	double TrueAnomalyRetro;
	VECTOR3 Att_LVLH;
	VECTOR3 Att_IMU;
	//Velocity counter - tailoff
	double DVC;
	double BurnTime;
	//Total velocity + tailoff
	double DVT;
	double UllageDT;
	double GMTI;
	double GETI;
	double RET400k;
	double V400k;
	double Gamma400k;
	double BankAngle;
	//Elapsed time from GETI to reverse bank angle
	double RETRB;
	//Maximum lift
	double lat_ML, lng_ML;
	//Target
	double lat_T, lng_T;
	//Impact point
	double lat_IP, lng_IP;
	//Zero lift
	double lat_ZL, lng_ZL;
	//Miss distance (NM)
	double dlat_NM, dlng_NM;
	//Height at retrofire above the oblate Earth
	double H_Retro;
	//Ballistic indicator
	int Ballistic;
	//Rev number of impact
	int Rev_IP;
	double P_G, Y_G;
	MATRIX3 REFSMMAT;
	double DV_TO;
	double DT_TO;
	//Biased and unbiased DV?
	VECTOR3 VG_XDX;
	VECTOR3 VGX_THR;
	double H_apo, H_peri;

	//Sep/shaping maneuver
	//0 = good data, +1 = no data, -1 = bad data
	int Indicator_Sep = 1;
	//Ullage quad (2 or 4)
	int UllageQuads_Sep;
	double CSMWeightSep;
	double TrueAnomalySep;
	VECTOR3 Att_LVLH_Sep;
	VECTOR3 Att_IMU_Sep;
	//Velocity counter - tailoff
	double DVC_Sep;
	double BurnTime_Sep;
	//Total velocity + tailoff
	double DVT_Sep;
	double UllageDT_Sep;
	double GMTI_Sep;
	double GETI_Sep;
	//Height at sep/shaping above the oblate Earth
	double H_Sep;
	double H_apo_sep, H_peri_sep;
	double P_G_Sep, Y_G_Sep;
};

struct RetrofireDisplayParametersTable
{
	//Primary, Contingency, Manual
	RetrofireDisplayParametersTableData data[3];
};

struct TimeConstraintsTable
{
	EphemerisData sv_present;
	double a = 0.0;
	double e = 0.0;
	double i = 0.0;
	double gamma = 0.0;
	double lat = 0.0;
	double lng = 0.0;
	double h = 0.0;
	double T0 = 0.0;
	double TA = 0.0; //True anomaly
	double MA = 0.0; //Mean anomaly
	double V = 0.0;
	double azi = 0.0;
	double AoP = 0.0;
	double RA = 0.0;
	double l = 0.0;	//Semi-latus rectum
	int OrbitNum = 0;
	int RevNum = 0;
	//EI time?
	double GMTPI = 0.0;
	std::string StationID;
	int TUP = 0;
	double h_a = 0.0;
	double h_p = 0.0;
};

struct RetrofireTransferTableEntry
{
	double GMTI = 0.0;
	VECTOR3 DeltaV = _V(0, 0, 0);
	int Thruster = 33;
	double dt_ullage = 0.0;
	bool UllageThrusterOption = true;

	ReentryConstraintsDefinition entry;
};

struct RetrofireTransferTable
{
	RetrofireTransferTableEntry Primary;
	RetrofireTransferTableEntry Manual;
};

struct SpacecraftSettingTable
{
	SpacecraftSettingTable();
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	int Indicator; //-1 = bad data, 0 = good data, 1 = no data
	bool IsRTE; //true = RTE, false = TTF
	double lat_T; //Latitude of splashdown target
	double lng_T; //Longitude of splashdown target
};

struct REFSMMATData
{
	MATRIX3 REFSMMAT;
	int ID = 0;
	double GMT = 0.0;
};

struct REFSMMATLocker
{
	REFSMMATData data[12];
};

struct StateVectorTableEntry
{
	EphemerisData Vector;
	int ID = -1;
	std::string VectorCode;
	bool LandingSiteIndicator = false;
};

struct SLVTargetingParametersTable
{
	double GMTLO = 0.0;
	double TINS = 0.0;
	double GSTAR = 0.0;
	double DN = 0.0;
	double TYAW = 0.0;
	double TPLANE = 0.0;
	double TGRR = 0.0;
	double AZL = 0.0;
	double VIGM = 0.0;
	double H = 0.0;
	double AZP = 0.0;
	double RIGM = 0.0;
	double GIGM = 0.0;
	double IIGM = 0.0;
	double TIGM = 0.0;
	double TDIGM = 0.0;
	double DELNO = 0.0;
	double DELNOD = 0.0;
	double PA = 0.0;
	double HA_C = 0.0;
	double HP_C = 0.0;
	double TA_C = 0.0;
	double DH = 0.0;
	double HA_T = 0.0;
	double HP_T = 0.0;
	double I_T = 0.0;
	double DN_T = 0.0;
	double BIAS = 0.0;
	double LATLS = 0.0;
	double LONGLS = 0.0;
};

struct StationData
{
	std::string code;
	//Geodetic longitude
	double lng;
	//Geodetic latitude
	double lat_geod;
	//Geocentric latitude
	double lat_geoc;
	//sin((lng))
	double sin_lng;
	//cos((lng))
	double cos_lng;
	//sin((lat_geod))
	double sin_lat_geod;
	//cos((lat_geod))
	double cos_lat_geod;
	//sin((lat_geoc))
	double sin_lat_geoc;
	//cos((lat_geoc))
	double cos_lat_geoc;
	//R_E*sin((lat_geod)-(lat_geoc))
	double R_sin_dlat;
	//R_E*cos((lat_geod)-(lat_geoc))
	double R_cos_dlat;
	//Inertial longitude at reference time
	double lng_iner;
	//Altitude above ellipsoid (H)
	double H;
	//Station radius R(S)
	double R_S;
	//Ellipsoid radius R(E)
	double R_E;
	//R(E)*cos((lat_geoc)) + H*cos(lat_geod))
	double R_E_cos_lat;
	//R(E)*sin((lat_geoc)) + H*sin(lat_geod))
	double R_E_sin_lat;
};

struct StationTable
{
	std::vector<StationData> table;
};