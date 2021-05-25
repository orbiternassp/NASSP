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

struct EphemerisDataTable
{
	EphemerisHeader Header;
	std::vector<EphemerisData> table;
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
	int ErrorCode;
	//1 = maximum time, 2 = radius, 3 = altitude, 4 = flight path angle, 5 = reference switch, 6 = beginning of maneuver, 7 = end of maneuver, 8 = ascending node
	int TerminationCode;
	//Maneuver number of last processed maneuver
	unsigned ManeuverNumber;
	double LunarStayBeginGMT;
	double LunarStayEndGMT;
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
	double MinEphemDT;
	//Reference frame of desired stopping parameter (0 = Earth, 1 = Moon, 2 = both)
	int StopParamRefFrame = 2;
	//Minimum number of points desired in ephemeris
	unsigned MinNumEphemPoints;
	bool ECIEphemerisIndicator = false;
	bool ECTEphemerisIndicator = false;
	bool MCIEphemerisIndicator = false;
	bool MCTEphemerisIndicator = false;
	//Ephemeris build indicator
	bool EphemerisBuildIndicator = false;
	//Maneuver cut-off indicator (0 = cut at begin of maneuver, 1 = cut at end of maneuver, 2 = don't cut off)
	int ManCutoffIndicator;
	//Descent burn indicator
	bool DescentBurnIndicator;
	//Cut-off indicator (1 = Time, 2 = radial distance, 3 = altitude above Earth or moon, 4 = flight-path angle, 5 = first reference switch)
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
	EphemerisDataTable *EphemTableIndicator = NULL; //Delete me
	EphemerisDataTable2 *ECIEphemTableIndicator = NULL;
	EphemerisDataTable2 *ECTEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCIEphemTableIndicator = NULL;
	EphemerisDataTable2 *MCTEphemTableIndicator = NULL;
	//Reference switch table indicator
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
	double CMWT;
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
	double t_gc = 0.0;
	double gmax = 0.0;
	double t_gmax = 0.0;
	//1 = time limit, 2 = impact, 3 = skipout
	int IEND;
};

struct ReentryConstraintsTable
{
	//R31
	int Thruster = 33;		//1 = RCS+2, 2 = RCS+4, 3 = RCS-2, 4 = RCS-4, 33 = SPS
	int GuidanceMode = 4;	//1 = Inertial, 4 = Guided (G&N)
	int BurnMode = 3;		//1 = DV, 2 = DT, 3 = V, Gamma Target (only SPS)
	double dt = 0.0;
	double dv = 0.0;
	int AttitudeMode = 2;	//1 = LVLH, 2 = 31.7° window line on horizon
	VECTOR3 LVLHAttitude = _V(0.0, -48.5*RAD, PI);
	double UllageTime = 15.0;
	bool Use4UllageThrusters = true;	//0 = two thrusters, 1 = four thrusters
	int REFSMMAT = 1;		//1 = CUR...
	int GimbalIndicator = -1; //-1 = compute, 1 = use system parameters
	double InitialBankAngle = 0.0;
	double GLevel = 0.2;
	double FinalBankAngle = 55.0*RAD;
};

struct RetrofireDisplayParametersTable
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
	double TA = 0.0;
	double V = 0.0;
	double azi = 0.0;
	double AoP = 0.0;
	double RA = 0.0;
	double l = 0.0;
	int OrbitNum = 0;
	int RevNum = 0;
	//EI time?
	double GMTPI = 0.0;
	std::string StationID;
	int TUP = 0;
};

struct RetrofireTransferTableEntry
{
	double GMTI = 0.0;
	VECTOR3 DeltaV = _V(0, 0, 0);
	int Thruster = 33;
	double dt_ullage = 0.0;
	bool UllageThrusterOption = true;
	double lat_T = 0.0;
	double lng_T = 0.0;
};

struct RetrofireTransferTable
{
	RetrofireTransferTableEntry Primary;
	RetrofireTransferTableEntry Manual;
};

struct SpacecraftSettingTable
{
	int Indicator = 1; //-1 = bad data, 0 = good data, 1 = no data
	int EnryMode = 0;
	int REFSMMATID = 0;
	double GMTI = 0.0;
	double lat_T = 0.0;
	double lng_T = 0.0;
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