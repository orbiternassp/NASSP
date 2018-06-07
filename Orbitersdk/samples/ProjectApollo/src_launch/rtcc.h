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

#define RTCC_VESSELTYPE_CSM 0
#define RTCC_VESSELTYPE_LM 1

#define RTCC_ENGINETYPE_RCS 0
#define RTCC_ENGINETYPE_SPSDPS 1
#define RTCC_ENGINETYPE_APS 2

#define RTCC_DIRECTIONTYPE_PLUSX 0
#define RTCC_DIRECTIONTYPE_MINUSX 1

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
};

struct TwoImpulseResuls
{
	VECTOR3 dV;
	VECTOR3 dV_LVLH;
	double t_TPI;
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
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double EarliestGET;	//Earliest GET for the DOI maneuver
	double lat; //landing site latitude
	double lng; //landing site longitude
	double alt;	//altitude of the landing site
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
	int N = 0;	// Revolutions between DOI and PDI
	double PeriAng = 15.0*RAD;	//Angle from landing site to perilune
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
	int type = 0;
	double GETbase; //usually MJD at launch
	VESSEL* vessel;
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	int impulsive = 1; //Calculated with nonimpulsive maneuver compensation or without
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	bool AltRef = 0;	//0 = use mean radius, 1 = use launchpad or landing site radius
	double LSAlt;	//Landing site altitude, if used
	int vesseltype = 0;			//0 = CSM, 1 = LM

	//maneuver parameters

	double TIG_GET;	//time of ignition, used for options 0 and 1
	double h_apo;	//apoapsis altitude, used for options 0 and 1
	double h_peri;	//periapsis altitude, used for option 0
	double inc;		//orbital inclination, used for option 0
	double rot_ang;	//rotate velocity vector, used for option 5
	double lng;		//Longitude, used for option 6
	int N;			//Number of orbits, used for option 6
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
	double lat;
	double lng;
	double alt;
	VESSEL* target;		//Target vessel
	double GETbase;		//usually MJD at launch
	double t_TPIguess;		//GET of TPI maneuver
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	int opt;		// 0 = Concentric Profile, 1 = Direct Profile, 2 = time critical direct profile
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
	int plan = 0;
	bool maneuverline = true;	//false = use input delta times, true = use 0.5 revolutions
	bool radial_dv = false;		//false = horizontal maneuver, true = 50 ft/s radial component
	int N_HC = 1;			//Number of half revs between CSI and CDH

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
	double t_TPI;	// Only for calculation type = 0
	double DH;		// Only for calculation type = 1
	double E;
	int type;		//0 = fixed TIG at TPI, 1 = fixed DH at CDH
	int maneuver;	//0 = CSI, 1 = CDH
};

// Parameter block for Calculation(). Expand as needed.
struct calculationParameters {
	Saturn *src;	// Our ship
	VESSEL *tgt;	// Target ship
	double TLI;		// Time of TLI
	VECTOR3 R_TLI;	//TLI cutoff position vector
	VECTOR3 V_TLI;	//TLI cutoff velocity vector
	double LOI;		// Time of LOI/Pericynthion
	double DOI;		// Time of DOI
	double PDI;		// Time of PDI
	double TLAND;	// Time of landing
	double Insertion; // Time of Insertion
	double Phasing;	// Time of Phasing
	double CSI;		// Time of CSI
	double CDH;		// Time of CDH
	double TPI;		// Time of TPI
	double TEI;		// Time of TEI
	double EI;		// Time of Entry Interface
	double lat_node;
	double lng_node;
	double alt_node;
	double GET_node;
	MATRIX3 StoredREFSMMAT;
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
	double theta_EO;
	double TB5;
	double T_L;
	double t_D;
	double T_RG;
	double T_ST;
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
	void EarthOrbitEntry(EarthEntryPADOpt *opt, AP7ENT &pad);
	void LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad);
	void LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res);
	double CDHcalc(CDHOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);
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
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node);
	void LOI2Targeting(LOI2Man *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void DOITargeting(DOIMan *opt, VECTOR3 &dV_LVLH_imp, double &P30TIG_imp, VECTOR3 &dV_LVLH, double &P30TIG, double &t_PDI, double &t_L, double &CR);
	void PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG, double &TOA);
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
	void LunarOrbitMapUpdate(SV sv0, double GETbase, AP10MAPUPDATE &pad);
	void LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad);
	SevenParameterUpdate TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double GETbase, double P30TIG, double TB5, double mu, double T_RG);
	void LVDCTLIPredict(LVDCTLIparam lvdc, VESSEL* vessel, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &R_TLI, VECTOR3 &V_TLI, double &T_TLI);
	void LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step);
	void FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip);
	void FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_out);
	void PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int vesseltype, int enginetype, double attachedMass, VECTOR3 DV, double &GET_TIG, VECTOR3 &dV_LVLH);
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
	void EntryUpdateCalc(SV sv0, double GETbase, double entryrange, bool highspeed, EntryResults *res);
	bool DockingInitiationProcessor(DKIOpt opt, DKIResults &res);
	void ConcentricRendezvousProcessor(SPQOpt *opt, VECTOR3 &DV_coe, double &t_TPI);
	SV coast(SV sv0, double dt);
	VECTOR3 HatchOpenThermalControl(VESSEL *v, MATRIX3 REFSMMAT);

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
	void OrbitAdjustCalc(SV sv_tig, double r_apo, double r_peri, double inc, VECTOR3 &DV);
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
	VECTOR3 PointAOTWithCSM(MATRIX3 REFSMMAT, SV sv, int AOTdetent, int star, double dockingangle);
	void DMissionRendezvousPlan(SV sv_A0, double GETbase, double &t_TPI0);
	void FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double GETbase, double t_TIG, double t_TPI, double &t_Ins, double &CSI);

	bool CalculationMTP_B(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C_PRIME(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_D(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_F(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);

protected:
	double TimeofIgnition;
	double SplashLatitude, SplashLongitude;
	VECTOR3 DeltaV_LVLH;
	int REFSMMATType;
};


#endif