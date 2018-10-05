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

#define RTCC_START_STRING	"RTCC_BEGIN"
#define RTCC_END_STRING	    "RTCC_END"

#define RTCC_LAMBERT_MULTIAXIS 0
#define RTCC_LAMBERT_XAXIS 1

#define RTCC_LAMBERT_SPHERICAL 0
#define RTCC_LAMBERT_PERTURBED 1

#define RTCC_IMPULSIVE 0
#define RTCC_NONIMPULSIVE 1
#define RTCC_NONIMPULSIVERCS 2

#define RTCC_ENTRY_DEORBIT 0
#define RTCC_ENTRY_MCC 1
#define RTCC_ENTRY_ABORT 2
#define RTCC_ENTRY_CORRIDOR 3

#define RTCC_ENTRY_MINDV 0
#define RTCC_ENTRY_NOMINAL 1

#define RTCC_VESSELTYPE_CSM		0
#define RTCC_VESSELTYPE_LM		1
#define RTCC_VESSELTYPE_SIVB	2

#define RTCC_ENGINETYPE_RCS 0
#define RTCC_ENGINETYPE_SPSDPS 1
#define RTCC_ENGINETYPE_APS 2

#define RTCC_DIRECTIONTYPE_PLUSX 0
#define RTCC_DIRECTIONTYPE_MINUSX 1

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

struct SV
{
	VECTOR3 R = _V(0, 0, 0);
	VECTOR3 V = _V(0, 0, 0);
	double MJD = 0.0;
	OBJHANDLE gravref = NULL;
	double mass = 0.0;
};

struct LambertMan //Data for Lambert targeting
{
	double GETbase; //usually MJD at launch
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	int N;		//number of revolutions
	int axis;	//Multi-axis or horizontal burn
	int Perturbation; //Spherical or non-spherical gravity
	VECTOR3 Offset; //Offset vector
	double PhaseAngle; //Phase angle to target
	double DH;					//Delta height at arrival
	SV sv_A;		//Chaser state vector
	SV sv_P;		//Target state vector
	bool NCC_NSR_Flag = false;	//true = NCC/NSR combination, false = TPI/TPF combination
	bool use_XYZ_Offset = true;	//true = use offset vector, false = use phase angle and DH
	double Elevation;	//Elevation angle at TPI
};

struct AP7ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int enginetype = 1; //0 = RCS, 1 = SPS
	int directiontype = 0; //0 = +X, 1 = -X (RCS only)
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
	int enginetype = 1; //0 = RCS, 1 = SPS
	int directiontype = 0; //0 = +X, 1 = -X (RCS only)
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
	int enginetype = 1; //0 = RCS, 1 = DPS, 2 = APS
	int directiontype = 0; //0 = +X, 1 = -X (RCS only)
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
	int enginetype;
	double KFactor = 0.0;
};

struct AP7TPIPADOpt
{
	VESSEL* vessel; //vessel
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
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
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
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
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
};

struct EntryResults
{
	VECTOR3 dV_LVLH;
	double P30TIG;
	double latitude, longitude;
	double GET05G, GET400K, RTGO, VIO, ReA, Incl, FlybyAlt;
	int precision;
	SV sv_postburn;
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
};

struct TwoImpulseResuls
{
	VECTOR3 dV;
	VECTOR3 dV_LVLH;
	double t_TPI;
};

struct SPQResults
{
	double t_CDH;
	double t_TPI;
	double DH;
	VECTOR3 dV_CSI;
	VECTOR3 dV_CDH;
};

struct TEIOpt
{
	VESSEL* vessel;			//Reentry vessel
	double GETbase;			//usually MJD at launch
	double TIGguess = 0.0;	//Initial estimate for the TIG
	double EntryLng;		//Entry longitude
	int returnspeed;		//0 = slow return, 1 = normal return, 2 = fast return
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;				//State vector as input
	int RevsTillTEI = 0;	//Revolutions until TEI
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool entrylongmanual = true; //Targeting a landing zone or a manual landing longitude
	int vesseltype = 0;			//0 = CSM, 1 = LM
	double Inclination = 0.0;	//Specified return inclination
	bool Ascending = true;		//Ascending or descending node
};

struct RTEFlybyOpt
{
	VESSEL* vessel;			//Reentry vessel
	double GETbase;			//usually MJD at launch
	double TIGguess;		//Initial estimate for the TIG
	int FlybyType;			//0 = Flyby, 1 = PC+2
	double EntryLng;		//Entry longitude
	int returnspeed;		//0 = slow return, 1 = normal return, 2 = fast return
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;				//State vector as input
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool entrylongmanual = true; //Targeting a landing zone or a manual landing longitude
	int vesseltype = 0;			//0 = CSM, 1 = LM
	double Inclination = 0.0;	//Specified return inclination
	bool Ascending = true;		//Ascending or descending node
};

struct REFSMMATOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	bool REFSMMATdirect = true; //if false, there is a maneuver between "now" and the relevant time of the REFSMMAT calculation
	double P30TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	double P30TIG2; //Time of Ignition 2nd maneuver
	VECTOR3 dV_LVLH2; //Delta V in LVLH coordinates 2nd maneuver
	int REFSMMATopt; //REFSMMAT options: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = LOI-2, 8 = LS during TLC, 9 = Attitude
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
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (deorbit maneuver)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (deorbit maneuver)
	MATRIX3 REFSMMAT;
	bool preburn; //
	double lat; //splashdown latitude
	double lng; //splashdown longitude
};

struct LunarEntryPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (MCC)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (MCC)
	MATRIX3 REFSMMAT;
	bool direct; //0 = with MCC, 1 = without
	double lat; //splashdown latitude
	double lng; //splashdown longitude
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
	bool useSV = false;		//true if state vector is to be used
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
	bool useSV = false;		//true if state vector is to be used
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
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
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
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM

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
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM

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
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
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
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
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
	bool useSV = false;	//true if state vector is to be used
	SV RV_MCC;			//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
	int impulsive = RTCC_NONIMPULSIVE;	//Calculated with nonimpulsive maneuver compensation or without
	int EllipseRotation = 0;	//0 = Choose the lowest DV solution, 1 = solution 1, 2 = solution 2
};

struct LOI2Man
{
	VESSEL* vessel;			//vessel
	double GETbase;			//usually MJD at launch
	double h_circ;			//altitude of circular orbit
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;				//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
	double alt = 0.0;		//altitude of the landing site
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
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
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
	LunarLiftoffTimeOpt() :theta_1(17.0*RAD), dt_1(7.0*60.0 + 15.0) {};
	double lat;
	double lng;
	double alt;
	double GETbase;		//usually MJD at launch
	double t_hole;		//Threshold time
	SV sv_CSM;			//CSM State vector
	int opt;			// 0 = Concentric Profile, 1 = Direct Profile, 2 = time critical direct profile
	double dt_2;		//Fixed time from insertion to TPI for direct profile
	double theta_1;	//Angle travelled between liftoff and insertion
	double dt_1;		//Ascent burn time (liftoff to insertion)
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
	VESSEL* vessel; //vessel
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
	int type;		//0 = fixed TIG at TPI, 1 = fixed DH at CDH
	int maneuver;	//0 = CSI, 1 = CDH
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
	char REF[64];	//Reference planet
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
	double L;		//The longitude associated with GETL
	double TO;		//Orbital period
	double K;		//K-Factor
	double ORBWT;	//Total current weight
};

struct FIDOOrbitDigitalsOpt
{
	SV sv_A;
	double GETbase;
	double MJD;		//MJD to update the state vector to (only used in continuous update)
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
	double MJD;		//MJD to update the state vector to (only used in continuous and GET update)
};

struct MPTManeuver
{
	SV sv_before;
	SV sv_after;
	std::string code;
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

	void AP7TPIPAD(AP7TPIPADOpt *opt, AP7TPI &pad);
	void AP9LMTPIPAD(AP9LMTPIPADOpt *opt, AP9LMTPI &pad);
	void AP9LMCDHPAD(AP9LMCDHPADOpt *opt, AP9LMCDH &pad);
	void TLI_PAD(TLIPADOpt* opt, TLIPAD &pad);
	bool PDI_PAD(PDIPADOpt* opt, AP11PDIPAD &pad);
	void LunarAscentPAD(ASCPADOpt opt, AP11LMASCPAD &pad);
	void EarthOrbitEntry(EarthEntryPADOpt *opt, AP7ENT &pad);
	void LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad);
	void LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res);
	double CDHcalc(CDHOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	double FindDH(SV sv_A, SV sv_P, double GETbase, double TIGguess, double DH);
	MATRIX3 REFSMMATCalc(REFSMMATOpt *opt);
	void EntryTargeting(EntryOpt *opt, EntryResults *res);//VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude, double &GET05G, double &RTGO, double &VIO, double &ReA, int &precision);
	void BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res);
	void TranslunarInjectionProcessorNodal(TLIManNode *opt, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut);
	void TranslunarInjectionProcessorFreeReturn(TLIManFR *opt, TLMCCResults *res, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut);
	void TranslunarMidcourseCorrectionTargetingNodal(MCCNodeMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	bool TranslunarMidcourseCorrectionTargetingFreeReturn(MCCFRMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingNonFreeReturn(MCCNFRMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingFlyby(MCCFlybyMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(MCCSPSLunarFlybyMan *opt, TLMCCResults *res, int &step);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node, SV &sv_post);
	void LOI2Targeting(LOI2Man *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void DOITargeting(DOIMan *opt, VECTOR3 &DV, double &P30TIG);
	void DOITargeting(DOIMan *opt, VECTOR3 &dv, double &P30TIG, double &t_PDI, double &t_L, double &CR);
	void PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
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
	void TEITargeting(TEIOpt *opt, EntryResults *res);
	void RTEFlybyTargeting(RTEFlybyOpt *opt, EntryResults *res);
	void LunarOrbitMapUpdate(SV sv0, double GETbase, AP10MAPUPDATE &pad, double pm = -150.0*RAD);
	void LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad);
	SevenParameterUpdate TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double GETbase, double P30TIG, double TB5, double mu, double T_RG);
	void LVDCTLIPredict(LVDCTLIparam lvdc, VESSEL* vessel, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &R_TLI, VECTOR3 &V_TLI, double &T_TLI);
	void LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step);
	void FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, bool agc = true);
	void FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_out, bool agc = true);
	VECTOR3 ConvertDVtoLVLH(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_imp);
	VECTOR3 ConvertDVtoInertial(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_LVLH_imp);
	void PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int vesseltype, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc = true);
	double GetDockedVesselMass(VESSEL *vessel);
	SV StateVectorCalc(VESSEL *vessel, double SVMJD = 0.0);
	SV ExecuteManeuver(VESSEL* vessel, double GETbase, double P30TIG, VECTOR3 dV_LVLH, SV sv, double attachedMass, double F = 0.0, double isp = 0.0);
	SV ExecuteManeuver(VESSEL* vessel, double GETbase, double P30TIG, VECTOR3 dV_LVLH, SV sv, double attachedMass, MATRIX3 &Q_Xx, VECTOR3 &V_G, double F = 0.0, double isp = 0.0);
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
	void LaunchTimePredictionProcessor(LunarLiftoffTimeOpt *opt, LunarLiftoffResults *res);
	bool LunarLiftoffTimePredictionCFP(VECTOR3 R_LS, SV sv_P, double GETbase, OBJHANDLE hMoon, double dt_1, double h_1, double theta_1, double theta_Ins, double DH, double E, double t_L_guess, double t_TPI, double theta_F, LunarLiftoffResults &res);
	bool LunarLiftoffTimePredictionTCDT(VECTOR3 R_LS, SV sv_P, double GETbase, OBJHANDLE hMoon, double dt_1, double h_1, double theta_1, double t_L_guess, LunarLiftoffResults &res);
	bool LunarLiftoffTimePredictionDT(VECTOR3 R_LS, SV sv_P, double GETbase, OBJHANDLE hMoon, double dt_1, double h_1, double theta_1, double dt_2, double DH, double E, double t_L_guess, double theta_F, LunarLiftoffResults &res);
	void LunarAscentProcessor(VECTOR3 R_LS, double m0, SV sv_CSM, double GETbase, double t_liftoff, double v_LH, double v_LV, double &theta, double &dt_asc, SV &sv_Ins);
	void EntryUpdateCalc(SV sv0, double GETbase, double entryrange, bool highspeed, EntryResults *res);
	bool DockingInitiationProcessor(DKIOpt opt, DKIResults &res);
	void ConcentricRendezvousProcessor(SPQOpt *opt, SPQResults &res);
	void AGOPCislunarNavigation(SV sv, MATRIX3 REFSMMAT, int star, double yaw, VECTOR3 &IMUAngles, double &TA, double &SA);
	VECTOR3 LOICrewChartUpdateProcessor(SV sv0, double GETbase, MATRIX3 REFSMMAT, double p_EMP, double LOI_TIG, VECTOR3 dV_LVLH_LOI, double p_T, double y_T);
	SV coast(SV sv0, double dt);
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
	void FIDOOrbitDigitalsUpdate(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsCycle(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsApsidesCycle(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsCalculateLongitude(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOOrbitDigitalsCalculateGETL(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res);
	void FIDOSpaceDigitalsUpdate(const SpaceDigitalsOpt &opt, SpaceDigitals &res);
	void FIDOSpaceDigitalsCycle(const SpaceDigitalsOpt &opt, SpaceDigitals &res);
	void FIDOSpaceDigitalsGET(const SpaceDigitalsOpt &opt, SpaceDigitals &res);

	//Skylark
	bool SkylabRendezvous(SkyRendOpt *opt, SkylabRendezvousResults *res);
	bool NC1NC2Program(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh_F, double n_H1, int s, double dh, double n_C, VECTOR3 &dV_NC1_LVLH, double &dh_NC2, double &dv_NC2, double &t_NC2, VECTOR3 &dV_NC2_LVLH, double &dv_NCC, double &t_NCC, double &t_NSR, VECTOR3 &dV_NSR, bool NPC = false);
	void NCCProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh, VECTOR3 &dV_NCC_LVLH, double &t_NSR, VECTOR3 &dV_NSR_LVLH);
	void NSRProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t2, double t3, VECTOR3 &dV_NSR_LVLH);
	void NPCProgram(SV sv_C, SV sv_W, double GETbase, double t, double &t_NPC, VECTOR3 &dV_NPC_LVLH);

	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state

	MCC *mcc;
	struct calculationParameters calcParams;
private:
	void AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad);
	MATRIX3 GetREFSMMATfromAGC(agc_t *agc, double AGCEpoch, int addroff = 0);
	double GetClockTimeFromAGC(agc_t *agc);
	double GetTEPHEMFromAGC(agc_t *agc);
	void navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt);
	double getGETBase();
	void AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad);
	void AP11BlockData(AP11BLKOpt *opt, P37PAD &pad);
	LambertMan set_lambertoptions(SV sv_A, SV sv_P, double GETbase, double T1, double T2, int N, int axis, int Perturbation, VECTOR3 Offset, double PhaseAngle);
	double lambertelev(VESSEL* vessel, VESSEL* target, double GETbase, double elev);
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

protected:
	double TimeofIgnition;
	double SplashLatitude, SplashLongitude;
	VECTOR3 DeltaV_LVLH;
	int REFSMMATType;
};


#endif