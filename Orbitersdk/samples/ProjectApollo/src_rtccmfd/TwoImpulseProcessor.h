/****************************************************************************
This file is part of Project Apollo - NASSP

Two Impulse Processor, RTCC Module PMSTICN (Header)

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

struct TwoImpulseOpt
{
	TwoImpulseOpt();

	//1 = Corrective Combination (NCC), 2 = Multiple Solution/Two-Impulse Computation (TPI), 3 = Single Solution, 4 = Transfer Plan, 5 = DKI/SPQ
	int mode;
	// 1 = CSM, 3 = LEM
	int ChaserVehicle;
	//Mode 1: 0 = use TimeStep as time increment of second maneuver, 1 = use TimeStep as terminal phase slip time increment
	//Mode 2: 0 = Time of both maneuvers fixed, 1 = time of first maneuver fixed, 2 = time of second maneuver fixed
	int RequestIndicator;
	double ChaserVectorTime;
	double TargetVectorTime;
	//Time of first maneuver (GMT)
	double T1;	
	//Time of second maneuver (GMT)
	double T2;
	VehicleDataBlock sv_C;		//Chaser state vector
	std::string ChaserStationID;
	VehicleDataBlock sv_T;		//Target state vector
	std::string TargetStationID;
	//Time increment of variable maneuver
	double TimeStep;

	//For single solution and transfer
	//1 = Multiple, 2 = Corrective Combination
	int TwoImpulseTableIndicator;
	//1 to 13
	int PlanNumber;

	//Mode 1 (Corrective Combination) only options
	//Minimum height difference
	double DH_min;
	//Maximum height difference
	double DH_max;
	//Height increment
	double DH_inc;
	//Minimum time of second (NSR) maneuver
	double T2_min;
	//Maximum time of second (NSR) maneuver
	double T2_max;
	//Limit of TPI slip time
	double dt_TPI_slip;
	//Mode 2 (Multiple Solution/Two Impulse) only options
	//Time range of variable maneuver
	double TimeRange;

	//Mode 3 (Single Solution) only options
	//false = 2 quads, true = 4 quads
	bool UllageQuads;
	//1 = Target, 2 = Horizon
	int LOSMode;
	//Pitch angle (i.e. the angle between astronauts line-of-sight and the X body axis)
	double DeltaPitch;

	//Mode 5 (DKI/SPQ) only options
	double DH;
	double PhaseAngle;
	double WT;
	double Elev;
};

struct TwoImpulseResuls
{
	TwoImpulseResuls();

	VehicleDataBlock sv_tig;		//State vector before NCC/TPI
	VehicleDataBlock sv_tig_apo;	//State vector after NCC/TPI
	VehicleDataBlock sv_tig2;		//State vector before NSR/TPF
	VehicleDataBlock sv_tig2_apo;	//State vector after NSR/TPF
	VECTOR3 dV;
	VECTOR3 dV2;
	VECTOR3 dV_LVLH;
	VECTOR3 dV_LVLH2;
	double T1;					//GET of NCC/TPI
	double T2;					//GET of NSR/TPF
	bool SolutionFound;
};

struct CorrectiveCombinationSolutionTableEntry
{
	//Impulsive GMT of the second maneuver of the plan
	double GMT_NSR = 0.0;
	//Sum of the impulsive DVs for both maneuvers plus an approximation of the DV required for terminal phase
	double DV_T = 0.0;
	//Height offset after the second maneuver
	double DH = 0.0;
	//Phase angle after the second maneuver
	double PhaseAngle = 0.0;
	//Time slip
	double T_SLIP = 0.0;
};

struct CorrectiveCombinationSolutionTable
{
	bool Updating = false;
	int Solutions = 0;
	int MAN_VEH = 0;
	std::string CSMSTAID, LMSTAID;
	double CSM_GMTTH = 0.0;
	double LM_GMTTH = 0.0;
	double T_NCC = 0.0;
	CorrectiveCombinationSolutionTableEntry data[13];
};

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
	double CSM_GMTTH = 0.0;
	double LM_GMTTH  = 0.0;
	//Number of seconds until the environment change indicated from the frozen maneuver of the first solution (negative for darkness)
	double DT_Light = 0.0;
	double DH = 0.0;
	double PhaseAngle = 0.0;
	TwoImpulseMultipleSolutionTableEntry data[13];
};

struct TwoImpulseSingleSolutionTableManeuverData
{
	double TIG = 0.0;			// Time of maneuver (approach time increment is pre-set in this location for 1st maneuver)
	double E_HOR = 0.0;			// E_HOR of maneuver
	double DV = 0.0;			// DV of maneuver
	double Pitch = 0.0;			// Pitch angle of maneuver
	double Yaw = 0.0;			// Yaw angle of maneuver
	VECTOR3 DV_LVLH = _V(0, 0, 0);
	double Pitch_LOS = 0.0;		// LOS pitch
	double Yaw_LOS = 0.0;		// LOS yaw
	VECTOR3 DV_LOS = _V(0, 0, 0); // DV along axes
	VECTOR3 BT_LOS = _V(0, 0, 0); // Burn times along axes
	double MinEnvironChange = 0.0;	// Minutes until next environment change
	double HA = 0.0;				// Apogee height
	double HP = 0.0;				// Perigee height
};

struct TwoImpulseSingleSolutionTableApproachData
{
	double GMT = 0.0;	// Time of approach data
	double Azi = 0.0;	// Azimuth
	double Elev = 0.0;	// Elevation
	double Range = 0.0;
	double RangeRate = 0.0;
	VECTOR3 DX = _V(0, 0, 0);	// Offset in curvilinear system
};

struct TwoImpulseSingleSolutionTable
{
	TwoImpulseSingleSolutionTable();

	std::string LMSTAID;
	std::string CSMSTAID;
	double LM_GMTTH;
	double CSM_GMTTH;
	int MAN_VEH; //1 = CSM, 3 = LEM
	int PointingMode; //1 = Target, 2 = Horizon
	int PlanNumber;
	int TwoImpulseTableIndicator;	// 1 = Multiple Solution, 2 = CC
	double ActualPhase;		// Actual phase after second maneuver
	double ActualDH;		// Actual height offset after second maneuver
	double ActualWT;		// Actual target terminal phase transfer angle between the two maneuvers (no. of firing quads is preset in this location)
	double DeltaPitch;		// Delta pitch angle
	
	TwoImpulseSingleSolutionTableManeuverData man[2];
	//3 for maneuver 1, 4 for maneuver 2
	TwoImpulseSingleSolutionTableApproachData app[7];
};

struct TwoImpulseMultipleSolutionDisplay
{
	std::string ErrorMessage = "NO TWO IMPULSE PLANS AVAILABLE";
	std::string CSMSTAID;
	std::string LMSTAID;
	double GETTH_CSM = 0.0;
	double GETTH_LM = 0.0;
	std::string MAN_VEH;
	char GETFRZ = ' ';
	char GMTFRZ = ' ';
	char GETVAR = ' ';
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
};

struct TwoImpulseCorrectiveCombinationDisplayEntry
{
	int Code = 0;
	double GET_NSR = 0.0;
	double GMT_NSR = 0.0;
	double DVT = 0.0;
	double DH = 0.0;
	double PhaseAngle = 0.0;
	double DT = 0.0;
	double TSLIP = 0.0;
};

struct TwoImpulseCorrectiveCombinationDisplay
{
	std::string ErrorMessage = "NO CORRECTIVE COMBINATION PLANS AVAILABLE";
	std::string CSMSTAID;
	std::string LMSTAID;
	double GETTH_CSM = 0.0;
	double GETTH_LM = 0.0;
	std::string MAN_VEH;
	double GET_NCC = 0.0;
	double GMT_NCC = 0.0;
	int Solutions = 0;
	TwoImpulseCorrectiveCombinationDisplayEntry data[13];
};

struct TwoImpulseSingleSolutionDisplayManeuverData
{
	double GET = 0.0;
	double GMT = 0.0;
	double E_HOR = 0.0;			// E_HOR of maneuver
	double DV = 0.0;			// DV of maneuver
	double Pitch = 0.0;			// Pitch angle of maneuver
	double Yaw = 0.0;			// Yaw angle of maneuver
	VECTOR3 DV_LVLH = _V(0, 0, 0);
	double Pitch_LOS = 0.0;		// LOS pitch
	double Yaw_LOS = 0.0;		// LOS yaw
	VECTOR3 DV_LOS = _V(0, 0, 0); // DV along axes
	VECTOR3 BT_LOS = _V(0, 0, 0); // Burn times along axes
	char BT_LOS_DIR[3] = {' ', ' ', ' '};
	double MinEnvironChange = 0.0;	// Minutes until next environment change
	std::string Condition;			// Darkness or daylight
	double HA = 0.0;				// Apogee height
	double HP = 0.0;				// Perigee height
};

struct TwoImpulseSingleSolutionDisplayApproachData
{
	double GET = 0.0;	// Time of approach data
	double TGT_AZ = 0.0;
	char TGT_AZ_DIR = ' ';
	double TGT_EL = 0.0;
	char TGT_EL_DIR = ' ';
	double RANGE = 0.0;
	double RDOT = 0.0;
	VECTOR3 OFF = _V(0, 0, 0);
	char X = ' ';
	char Y = ' ';
	char Z = ' ';
};

struct TwoImpulseSingleSolutionDisplay
{
	std::string ErrorMessage = "NO INFORMATION AVAILABLE AT THIS TIME";
	std::string LMSTAID;
	std::string CSMSTAID;
	double LM_GETTH = 0.0;
	double CSM_GETTH = 0.0;
	std::string MAN_VEH;
	std::string PointingMode;
	std::string TwoImpulseTableIndicator;
	int ID = 0;
	double DTR = 0.0; //Time between the two maneuvers
	double WT = 0.0;
	double PHASE = 0.0;
	double DELH = 0.0;
	double DELPITCH = 0.0;
	TwoImpulseSingleSolutionDisplayManeuverData man[2];
	TwoImpulseSingleSolutionDisplayApproachData app[7];
};

class TwoImpulseProcessor : public RTCCModule
{
public:
	TwoImpulseProcessor(RTCC *r);
	void PMSTICN(const TwoImpulseOpt &opt, TwoImpulseResuls &res);

	//DISPLAYS
	//Two Impulse Multiple Solution
	void PMDTIMP();
	//Two Impulse Corrective Combination
	void PMDDTVCC();
	//Two Impulse Multiple Solution
	void PMDTIPSS();
protected:
	//Common code for CC and ML
	void CCAndML();
	//Mode 1: Corrective Combination
	void CorrectiveCombination();
	//Mode 2: Multiple Solution
	void MultipleSolution();
	//Common code for single solution and transfer plan
	void SingleSolutionTransferPlan();
	//Mode 3: Single Solution
	void SingleSolution(TwoImpulseSingleSolutionTable &tab);
	//Mode 4: Transfer Plan
	void TransferPlan();
	//Mode 5: External Request
	void ExternalRequest(TwoImpulseResuls &res);

	//Two Impulse Impulsive Maneuver Calculation
	int PMMTIS(VehicleDataBlock sv_A1, VehicleDataBlock sv_P1, double dt, double DH, double theta, VehicleDataBlock &sv_A1_apo, VehicleDataBlock &sv_A2, VehicleDataBlock &sv_A2_apo) const;

	//Utility functions
	bool ElevationAngleSearch(VehicleDataBlock sv_A0, VehicleDataBlock sv_P0, double Elev, VehicleDataBlock &sv_A1, VehicleDataBlock &sv_P1, double &T1) const;
	double T2Search() const;
	void PMSTICN_PY(VECTOR3 R_A, VECTOR3 V_A, VECTOR3 R_B, VECTOR3 V_B, double &Pitch, double &Yaw, VECTOR3 &DV_LVLH) const;
	bool coast(VehicleDataBlock sv0, double dt, VehicleDataBlock &sv1) const;
	void SecularRates(EphemerisData sv0, double &l_dot, double &g_dot, double &h_dot) const;

	//Time of first maneuver
	double T1;
	//Time of second maneuver
	double T2;
	double DH;
	double PhaseAngle;
	double Elev;
	double WT;
	//Chaser state vector before first maneuver
	VehicleDataBlock sv_C1;
	//Target state vector at time of first maneuver
	VehicleDataBlock sv_T1;
	//Chaser state vector after first maneuver
	VehicleDataBlock sv_C1_apo;
	//Chaser state vector before second maneuver
	VehicleDataBlock sv_C2;
	//Chaser state vector after second maneuver
	VehicleDataBlock sv_C2_apo;
	//Target state vector at time of second maneuver
	VehicleDataBlock sv_T2;

	TwoImpulseOpt opt;
};