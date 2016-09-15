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
#define RTCC_START_STRING	"RTCC_BEGIN"
#define RTCC_END_STRING	    "RTCC_END"

#define RTCC_LAMBERT_MULTIAXIS 0
#define RTCC_LAMBERT_XAXIS 1

#define RTCC_LAMBERT_SPHERICAL 0
#define RTCC_LAMBERT_PERTURBED 1

#define RTCC_IMPULSIVE 0
#define RTCC_NONIMPULSIVE 1

#define RTCC_ENTRY_DEORBIT 0
#define RTCC_ENTRY_MCC 1
#define RTCC_ENTRY_ABORT 2
#define RTCC_ENTRY_CORRIDOR 3

#define RTCC_ENTRY_MINDV 0
#define RTCC_ENTRY_NOMINAL 1

const double LaunchMJD[11] = {//Launch MJD of Apollo missions
	40140.62691,
	40211.535417,
	40283.666667,
	40359.700694,
	40418.563889,
	40539.68194,
	40687.80069,
	40982.877106,
	41158.5652869,
	41423.74583,
	41658.23125
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
	VESSEL* vessel; //Vessel executing the burn
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	int N;		//number of revolutions
	int axis;	//Multi-axis or horizontal burn
	int Perturbation; //Spherical or non-spherical gravity
	VECTOR3 Offset; //Offset vector
	double PhaseAngle; //Phase angle to target, will overwrite offset
	//bool prograde; //Prograde or retrograde solution
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
	bool csmlmdocked; //0 = CSM alone, 1 = CSM/LM
};

struct AP7ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int engopt; //0 = SPS, 1 = RCS+X, 2 = RCS-X
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime; //time delay for the sextant star check (in case no star is available during the maneuver)
	double navcheckGET; //Time for the navcheck. 0 = no nav check
};

struct AP11ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int engopt; //0 = SPS, 1 = RCS+X, 2 = RCS-X
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime = 0; //time delay for the sextant star check (in case no star is available during the maneuver)
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct AP11LMManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int engopt; //0 = DPS, 1 = RCS+X, 2 = RCS-X
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime = 0; //time delay for the sextant star check (in case no star is available during the maneuver)
	int vesseltype = 2; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct AP7TPIPADOpt
{
	VESSEL* vessel; //vessel
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
};

struct EntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double GETbase; //usually MJD at launch
	double TIGguess; //Initial estimate for the TIG or baseline TIG for abort and MCC maneuvers
	int type; //Type of reentry maneuver
	double ReA; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	double Range;  //Desired range from 0.05g to splashdown, 0 uses AUGEKUGEL function to determine range
	bool nominal; //Calculates minimum DV deorbit or nominal 31.7° line deorbit
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct TEIOpt
{
	VESSEL* vessel;			//Reentry vessel
	double GETbase;			//usually MJD at launch
	double TIGguess = 0.0;	//Initial estimate for the TIG
	int TEItype;			//0 = TEI, 1 = Flyby, 2 = PC+2
	double EntryLng;		//Entry longitude
	int returnspeed;		//0 = slow return, 1 = normal return, 2 = fast return
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;				//State vector as input
	int RevsTillTEI = 0;	//Revolutions until TEI
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
	int REFSMMATopt; //REFSMMAT options: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = LOI-2
	double REFSMMATTime; //Time for the REFSMMAT calculation
	double LSLng; //longitude for the landing site REFSMMAT
	double LSLat; //latitude for the landign site REFSMMAT
	int mission; //Just for the launch REFSMMAT
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
};

struct AP7BLKOpt
{
	int n; //number of PAD entries
	double *lng; //pointer to splashdown longitudes
	double *GETI; //pointer to ignition times
	char **area; //pointer to splashdown areas
};

struct AP11BLKOpt
{
	int n; //number of PAD entries
	double *lng; //pointer to splashdown longitudes
	double *GETI; //pointer to ignition times
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

struct LOIMan
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	int man; //0 = last MCC, 1 = LOI-1 (w/ MCC), 2 = LOI-2 (w/o MCC), 3 = LOI-2, 4 = TLI
	double MCCGET; //GET for the last MCC
	double lat; //target for MCC
	double lng; //target for MCC
	double PeriGET; //time of periapsis (for MCC)
	double h_apo;	//for LOI-1
	double h_peri;	//for MCC and LOI-1, circular orbit for LOI-2
	double inc;		//Inclination (equatorial) for LOI-1
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked; //0 = CSM alone, 1 = CSM/LM
};

struct OrbAdjOpt
{
	VESSEL* vessel;
	OBJHANDLE gravref;
	double SPSGET;
	double GETbase; //usually MJD at launch
	double h_apo;	//
	double h_peri;	//
	double inc;		//
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
	bool csmlmdocked; //0 = CSM alone, 1 = CSM/LM
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
	OBJHANDLE gravref; //
	double GETbase; //usually MJD at launch
	double SVGET; //GET of the state vector
	double navcheckGET; //GET of the Nav Check
};

// Parameter block for Calculation(). Expand as needed.
struct calculationParameters {
	Saturn *src;	// Our ship
	VESSEL *tgt;	// Target ship
	double TEI;		// Time of TEI
	double TLI;		// Time of TLI
	double EI;		// Time of Entry Interface
	double LOI;		// Time of LOI/Pericynthion
	VECTOR3 R_TLI;	//TLI cutoff position vector
	VECTOR3 V_TLI;	//TLI cutoff velocity vector
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
	bool Calculation(int mission, int fcn,LPVOID &pad, char * upString = NULL, char * upDesc = NULL);

	void SetManeuverData(double TIG, VECTOR3 DV);
	void GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD);

	void AP7TPIPAD(AP7TPIPADOpt *opt, AP7TPI &pad);
	void TLI_PAD(TLIPADOpt* opt, TLIPAD &pad);
	void EarthOrbitEntry(EarthEntryPADOpt *opt, AP7ENT &pad);
	void LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad);
	void LambertTargeting(LambertMan *lambert, VECTOR3 &dV_LVLH, double &P30TIG);
	double CDHcalc(CDHOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	MATRIX3 REFSMMATCalc(REFSMMATOpt *opt);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut);
	void OrbitAdjustCalc(OrbAdjOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	OBJHANDLE AGCGravityRef(VESSEL* vessel); // A sun referenced state vector wouldn't be much of a help for the AGC...
	void NavCheckPAD(SV sv, AP7NAV &pad);
	void AP11LMManeuverPAD(AP11LMManPADOpt *opt, AP11LMMNV &pad);
	void AP11ManeuverPAD(AP11ManPADOpt *opt, AP11MNV &pad);
	void TEITargeting(TEIOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude, double &GET05G, double &RTGO, double &VIO);
	SevenParameterUpdate TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double P30TIG, double TB5, double mu, double T_RG);
	void LVDCTLIPredict(LVDCTLIparam lvdc, VESSEL* vessel, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &R_TLI, VECTOR3 &V_TLI, double &T_TLI);

	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state

	MCC *mcc;
	struct calculationParameters calcParams;
private:
	void AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad);
	MATRIX3 GetREFSMMATfromAGC(double AGCEpoch);
	void navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt);
	void StateVectorCalc(VESSEL *vessel, double &SVGET, VECTOR3 &J2000Pos, VECTOR3 &J2000Vel);
	void EntryTargeting(EntryOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude, double &GET05G, double &RTGO, double &VIO);
	double getGETBase();
	void AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad);
	void AP11BlockData(AP11BLKOpt *opt, P37PAD &pad);
	LambertMan set_lambertoptions(VESSEL* vessel, VESSEL* target, double GETbase, double T1, double T2, int N, int axis, int Perturbation, VECTOR3 Offset, double PhaseAngle, int impulsive);
	double lambertelev(VESSEL* vessel, VESSEL* target, double GETbase, double elev);
	char* CMCExternalDeltaVUpdate(double P30TIG,VECTOR3 dV_LVLH);
	char* CMCStateVectorUpdate(SV sv, bool csm, double AGCEpoch);
	char* CMCDesiredREFSMMATUpdate(MATRIX3 REFSMMAT, double AGCEpoch);
	char* CMCREFSMMATUpdate(MATRIX3 REFSMMAT, double AGCEpoch);
	char* CMCRetrofireExternalDeltaVUpdate(double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH);
	char* CMCEntryUpdate(double LatSPL, double LngSPL);
	char* V71Update(int* emem, int n);
	void P27PADCalc(P27Opt *opt, double AGCEpoch, P27PAD &pad);
	int SPSRCSDecision(double a, VECTOR3 dV_LVLH);	//0 = SPS, 1 = RCS
	bool REFSMMATDecision(VECTOR3 Att); //true = everything ok, false = Preferred REFSMMAT necessary
	SV ExecuteManeuver(VESSEL* vessel, double GETbase, double P30TIG, VECTOR3 dV_LVLH, SV sv, double F = 0.0, double isp = 0.0);
	SV coast(SV sv0, double dt);
	void LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step);

	bool CalculationMTP_C(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL);
	bool CalculationMTP_C_PRIME(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL);

protected:
	double TimeofIgnition;
	double SplashLatitude, SplashLongitude;
	VECTOR3 DeltaV_LVLH;
};


#endif