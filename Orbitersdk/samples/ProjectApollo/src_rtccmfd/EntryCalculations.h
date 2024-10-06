/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2014-2018

RTCC Entry Calculations (Header)

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

#include "OrbMech.h"
#include "RTCCModule.h"
#include "RTCCTables.h"

struct ATPData
{
	double lat;
	double lng;
};

namespace EntryCalculations
{
	const double RTE_VEL_LINE[26] = { 25.5, 26.0, 26.5, 27.0, 27.5, 28.0, 28.5, 29.0, 29.5, 30.0, 30.5, 31.0, 31.5, 32.0, 32.5, 33.0, 33.5, 34.0, 34.5, 35.0, 35.5, 36.0, 36.5, 37.0, 37.5, 38.0 };
	const double RTE_MSFN_LINE[26] = { 91.35, 91.91, 92.47, 92.91, 93.29, 93.62, 93.91, 94.17, 94.39, 94.59, 94.78, 94.96, 95.12, 95.27, 95.41, 95.54, 95.67, 95.8, 95.92, 96.03, 96.14, 96.24, 96.35, 96.44, 96.54, 96.58 };
	const double RTE_CONT_LINE[26] = { 93.3, 93.74, 94.04, 94.31, 94.55, 94.78, 94.96, 95.13, 95.28, 95.41, 95.55, 95.66, 95.77, 95.86, 95.96, 96.05, 96.14, 96.21, 96.29, 96.36, 96.43, 96.48, 96.55, 96.61, 96.66, 96.74 };

	const double RTE_b[6] = { 0.61974454e4, -0.26277752, -0.38675677e-5, 0.15781674e-9, 0.67856872e-14, -0.14887772e-18 };
	const double RTE_cc = 0.105;
	const double RTE_ee[3] = { 0.18957317e3, 0.17640466, 0.19321074e-2 };
	const double RTE_ff[3] = { 0.64623407e2, 0.57834928e-1,-0.48255307e-3 };
	const double RTE_jj[2][6] = { {0.10718858e7, -0.1627124e3, 0.98775571e-2, -0.29943037e-6, 0.45325217e-11, -0.27404876e-16},
									 {0.18262030e6, -0.27810612e2, 0.16998821e-2, -0.51884802e-7, 0.79087925e-12, -0.48128071e-17} };
	const double RTE_pp[2][2] = { {0.59e2, 0.3006}, {0.193e3, 0.1795} };
	const double RTE_q[2][6] = { {0.0, 0.555e-4, -0.1025e1, 0.4e3, 0.335e3, -0.4215e2},{0.0, 0.555e-4, -0.1025e1, 0.7e3, 0.31e3, -0.45e2} };

	double ReentryTargetLine(double vel, bool msfn);
	double ReentryTargetLineTan(double vel, bool msfn);
	void augekugel(double ve, double gammae, double &phie, double &Te);
	void landingsite(MATRIX3 Rot_J_B, VECTOR3 REI, VECTOR3 VEI, double MJD_EI, double &lambda, double &phi);
	void Reentry(MATRIX3 Rot_J_B, VECTOR3 REI, VECTOR3 VEI, double mjd0, bool highspeed, double &EntryLatPred, double &EntryLngPred, double &EntryRTGO, double &EntryVIO, double &EntryRET);
	double XDOTX2(VECTOR3 a, VECTOR3 b, double DIFG);
	void RA2XYZ4(VECTOR3 R, VECTOR3 V, double &beta, double &A_Z);
	double landingzonelong(int zone, double lat);

	//Actual RTE processor routines
	void REENTRYNew(double LD, int ICRNGG, double v_i, double i_r, double A_Z, double mu, double r_rbias, double &eta_rz1, double &theta_cr, double &T);
	VECTOR3 TVECT(VECTOR3 a, VECTOR3 b, double alpha, double gamma);
	void EGTR(VECTOR3 R_geoc, VECTOR3 V_geoc, double GMT, double alpha_SID0, VECTOR3 &R_geogr, VECTOR3 &V_geogr);
	double INTER(const double *X, const double *Y, int IMAX, double x);
	double URF(double T, double x);
	void AESR(double r1, double r2, double beta1, double T, double R, double mu, double eps, double &a, double &e, bool &k2, int &info, double &V1);
	int MINMIZ(VECTOR3 &X, VECTOR3 &Y, VECTOR3 &Z, bool opt, VECTOR3 CUR, double TOL, double &XMIN, double &YMIN);
	double LNDING(VECTOR3 REI, VECTOR3 VEI, double GMT_EI, double alpha_SIDO0, double LD, int ICRNGG, double r_rbias, double &lambda, double &phi, double &GMT_L);
	void SIDCOM(double JD0, double DT, double N, double &alpha_go, double &T);

	double MPL(double lat);
	double MPL2(double lat);
	double EPL(double lat);
	double AOL(double lat);
	double IOL(double lat);
	double WPL(double lat);
	bool TBLOOK(double *LINE, double lat, double &lng);
}

//RTCC task RTSDBMP
class RetrofirePlanning : public RTCCModule
{
public:
	RetrofirePlanning(RTCC *r);
	//Retrofire Planning Control Module
	bool RMSDBMP(EphemerisData sv, double GETI, double lat_T, double lng_T, double CSMmass, double Area);
protected:
	//Retrofire Planning Boundary Computation
	void RMMDBF();
	//Retrofire Maneuvers Computations
	void RMMDBM();
	//Retrofire Convergence
	void RMMDBN(int entry);
	//Thrust Direction and Body Attitude Routine
	void RMMATT(int entry, int opt, bool calcDesired, VECTOR3 Att, MATRIX3 REFSMMAT, int thruster, VECTOR3 R, VECTOR3 V, int TrimIndicator, double mass, VECTOR3 &U_T, VECTOR3 &OtherAtt);
	//Retrofire Output Control
	void RMSTTF();
	//Retrofire On-Line Printing
	void RMGTTF(std::string source, int i);

	bool WasGETIInput;
	//Time left and right for the ephemeris
	double TL, TR;
	//Time of ignition of main burn
	double GMTI;
	//Time of propulsion initiation (either main burn or sep/shaping, always main engine)
	double GMT_TI;
	//Time of shaping maneuver
	double GMTI_SH;
	double lat_T;
	double lng_T;
	//Initial CSM mass
	double CSMmass;
	//CSM mass after sep/shaping, if applicable
	double CSMmass_Sep;
	int refsid;
	REFSMMATData refsdata;
	int Thruster;
	double SQMU;
	double F, mdot, DVBURN;
	VECTOR3 U_T;
	//1 = DV, 2 = DT, 3 = V, Gamma Target (only SPS)
	int BurnMode;
	//-1 = compute, 1 = use system parameters
	int GimbalIndicator = -1;
	//Ullage time
	double dt_ullage;
	//Ullage time of sep/shaping maneuver
	double dt_ullage_sep;
	//Gradient of DV vs. flight path angle for V, gamma targeting
	double p_gam;
	//Partials
	double p_dlng_dtf, p_dlat_dtf, p_dlat_dtRB, p_dlng_dtRB;
	double dlat, dlng;
	//Main iteration counter
	int MAINITER;
	//Main error indicator
	int ERR;
	//Error return from LLBRTD
	int LLBRTDERR;
	//Downrange error in NM
	double MD_lng;
	//Crossrange error in NM
	double MD_lat;
	//Total miss distance in NM
	double MD_total;
	//Maximum miss distance in NM
	double MD_max;
	//Main loop convergence flag
	bool HASCONVERGED;
	double dlng2, lng_old, GMTI_old, ddt;
	//Thrust value in CMC
	double TCMC;
	//Burn attitude in LVLH coordinates (thrust direction, not body)
	VECTOR3 LVLHAtt;
	VECTOR3 BodyAtt;
	VECTOR3 IMUAtt;
	MATRIX3 DesREFSMMAT;
	//Time to reverse bank angle
	double t_RB;
	//Partials status. 0 = not yet calculated, 1 = TTF varied, 2 = GMTRB varied
	int PARTSTAT;
	//Variation of the time to fire
	const double DT_TTF = 10.0;
	//Variation of the time to reverse bank angle
	const double DT_TRB = 10.0;
	//Stored landing errors
	double dlat_0, dlng_0, dlat_TRB, dlng_TRB, partialprod;
	//G-max and time of G-max
	double gmax, gmt_gmax;
	//0 = Retro maneuver only, 1 = Sep + Retro, 2 = Shaping + Retro
	int ManeuverType;

	//Input state vector
	EphemerisData sv0;
	//State vector to be used as input for ephemeris creation
	EphemerisData sv0_apo;
	//State vector at sep/shaping burn initiation (ullage on)
	EphemerisData sv_BI_SEP;
	//State vector at sep/shaping main engine on
	EphemerisData sv_TIG_SEP;
	//State vector at sep/shaping burnout (tailoff end)
	EphemerisData sv_BO_SEP;
	//State vector at burn initiation (ullage on)
	EphemerisData sv_BI;
	//State vector at main engine on
	EphemerisData sv_TIG;
	//State vector at burnout (tailoff end)
	EphemerisData sv_BO;
	//ECI state vector at entry interface (400k altitude)
	EphemerisData sv_EI;
	//Actual latitude of landing
	double lat_IP;
	//Actual longitude of landing
	double lng_IP;
	//Zero lift landing point
	double lat_ZL, lng_ZL;
	//Maximum lift landing point
	double lat_ML, lng_ML;

	//Body axes as output from RMMATT
	VECTOR3 X_B, Y_B, Z_B;

	//Area
	double Area;

	EphemerisDataTable2 ephem;
	RTCCNIAuxOutputTable burnaux, burnaux_sep;
	RMMYNIOutputTable reentryout;
};

class EarthEntry : public RTCCModule {
public:
	EarthEntry(RTCC *r, VECTOR3 R0B, VECTOR3 V0B, double mjd, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, bool entrynominal, bool entrylongmanual);
	bool EntryIter();

	double EntryTIGcor; //Corrected Time of Ignition for the Reentry Maneuver
	double EntryLngcor;	//Corrected Splashdown Longitude
	double EntryLatcor;	//Corrected Splashdown Latitude

	double EntryLatPred, EntryLngPred;	//Predicted Splashdown Latitude and Longitude
	double EntryRET, EntryRTGO, EntryVIO;
	double V400k, gamma400k;
	double EntryAng;
	double t2;
	VECTOR3 Entry_DV; //Entry DV vector in LVLH coordinates
	int precision; //0 = only conic, 1 = precision, 2 = PeA=-30 solution
	int errorstate;
private:
	void coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21);
	void precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2);
	void precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2, double &C_FPA);
	void conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1);
	void conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3);
	void xdviterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x);
	void xdviterator2(int f1, VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x);
	void xdviterator3(VECTOR3 R1B, VECTOR3 V1B, double min, double xmax, double &x);
	void precisionperi(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2);
	void dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON);
	void reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 REI, VECTOR3 VEI);
	void finalstatevector(VECTOR3 R1B, VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE);
	void newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo);

	OBJHANDLE gravref, hEarth;
	double MA1, C0, C1, C2, C3;
	VECTOR3 R0B, V0B;
	double mjd;
	double GETbase, get;
	double RCON, RD;
	double mu;
	double Tguess;
	double tigslip;
	double EntryTIGcor_old, dlng_old;
	int ii;
	double EntryLng;
	int entryphase;
	double xapo, dv_err;
	VECTOR3 R11B, V11B;
	int f2;
	double dlngapo, dt0, x2, x2_apo;
	double EMSAlt;
	double k1, k2, k3, k4;
	double phi2; //+1 near apogee solution, -1 near perigee solution
	double earthorbitangle; //31.7° nominal angled reentry DV vector
	double R_E;
	bool entrynominal; //0 = minimum DV entry, 1 = 31.7° line
	double dt1; //time between estimated maneuver time and actual (currently iterated) maneuver time
	double x, dx, dxmax;
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	bool entrylongmanual;
	double xlim;
	double t21;
	double EntryInterface;
};

class RTEEarth : public RTCCModule
{
public:
	RTEEarth(RTCC *r, EphemerisData sv0, double GMTbase, double alpha_SID0, double EntryTIG, double t_Z, int critical);
	void READ(double RRBI, double DVMAXI, int EPI, double URMAX);
	void ATP(double *line);
	bool EntryIter();

	//OUTPUT
	//State vector at ignition
	EphemerisData sv_ig, sv_ig_apo;

	double EntryRET, EntryRTGO, EntryVIO;
	double V400k, gamma400k;
	double EntryAng;
	VECTOR3 DV; //Inertial DV
	VECTOR3 Entry_DV; //Entry DV vector in LVLH coordinates
	int precision; //0 = only conic, 1 = precision, 2 = PeA=-30 solution
	int errorstate;

	//State vector at reentry
	VECTOR3 R_r, V_r;
	double t2;
	//State at landing
	double EntryLngcor;	//Corrected Splashdown Longitude
	double EntryLatcor;	//Corrected Splashdown Latitude
	double t_Z;			//Time of splashdown
private:
	void coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21);
	void precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2);
	void precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2);
	void conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1);
	void conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3);
	//Find independent variable x for which the maximum allowed DV is expended
	double dvmaxiterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double dxmax, double dv_des);
	double dtiterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double xmax,double dxmax, double dt_des);
	void xdviterator(VECTOR3 R1B,VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x);
	void limitxchange(double theta1, double theta2, double theta3, VECTOR3 V1B, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double xmax, double &x);
	void dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON);
	void reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 REI, VECTOR3 VEI);
	void newxt2(int n1, double xt2err, double &xt2_apo, double &xt2, double &xt2err_apo);
	void finalstatevector(VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE);
	void newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo);

	//Maximum allowable major axis of return trajectories with a negative radial component
	double MA1;
	//Polynomial coefficients used to determine the maximum allowable major axis of return trajectories with a positive radial component based on the radius magnitude
	double C0, C1, C2, C3;
	double GMTbase;
	double RCON, RD;
	double mu;
	double EntryTIGcor_old, dlng_old;
	//Iteration counter. First iteration is conic calculations
	int ii;
	double EntryLng;
	//1 = Alternate Target Point (ATP), 2 = Time critical, 3 = Fuel critical
	int critical;
	double xapo;
	int f2;
	double dlngapo, x2, x2_apo;
	double EMSAlt;
	//Radius used to determine which estimate of reentry angle should be used
	double k1;
	//Initial estimate of reentry radius
	double k2;
	//Initial estimate of the cotangent of the reentry angle used when the initial radius is less than k1 (equivalent to -3° 29.5')
	double k3;
	//Initial estimate of the cotangent of the reentry angle used when the initial radius is greater than k1 (equivalent to -5° 58')
	double k4;
	int revcor;
	double phi2;
	//Independent variable, cotangent of the post-return flight path angle
	double x;
	//Change in x
	double dx;
	//Maximum allowed change in x
	double dxmax;
	//Limit on the cotangent of the post-return flight path angle
	double xlim;
	double t21;
	// relative range override (unit is nautical miles!)
	double r_rbias;
	//Maximum DV for the burn
	double dv_max;
	//Estimate time between maneuver and landing (ATP mode)
	double dt_z;
	//Alternate target line (lat, lng, lat, lng etc.)
	double LINE[10];
	// 0: guide reentry to the steep target line
	// 1: manual reentry to the shallow target line
	// 2: manual reentry to the steep target line
	int ICRNGG;
	//Maximum allowable reentry speed
	double u_rmax;
	//Cosine of flight path angle
	double C_FPA;
	//Coast integrator status
	int ITS;
	//Right ascension at GMT zero
	double alpha_SID0;
};

struct TradeoffData
{
	double lat = 0.0;
	double T0 = 0.0;
	double T_Z = 0.0;
	double DV = 0.0;
};

struct DiscreteData
{
	VECTOR3 DV;
	VECTOR3 RR;
	VECTOR3 U_r;
	double T_r;
	double lambda_z;
	double delta_z;
	double eta_rz;
	double theta_cr;
	double T_rz;
	int NOSOLN = 1;
};

class ConicRTEEarthNew : public RTCCModule
{
public:
	ConicRTEEarthNew(RTCC *r, std::vector<EphemerisData2> &SVArray);
	void MAIN();
	void Init(double dvm, int icrngg, double irmax, double urmax, double rrbi, int imsfn);
	void READ(int Mode, double gmtbase, double tzmin, double tzmax);
	void ATP(std::vector<ATPData> line);

	std::vector<TradeoffData> TOData;
	DiscreteData SolData;
protected:

	//SUBROUTINES:
	void INITAL();
	void DVMINQ(int FLAG, int QE, int Q0, double beta_r, double &DV, int &QA, double &V_a, double &beta_a);
	void FCUA(int FLAG, VECTOR3 R_a, double &beta_r, double &DV, double &U_r, double &V_a, double &beta_a);
	void MSDS(double VR_a, double VT_a, double beta_r, double theta, double &delta, double &phi, double &phi_z, double &lambda, double &theta_z);
	bool RUBR(int QA, int QE, double R_a, double U_0, double U_r, double beta_r, double &AINV, double &DV, double &e, double &T, double &V_a, double &beta_a);
	void VELCOM(double T, double R_a, double &beta_r, double &dt, double &p, bool &QA, int &sw6, double &U_r, double &VR_a, double &VT_a, double &beta_a, double &eta_ar, double &DV);
	void VARMIN();
	void TCOMP(double dv, double delta, double &T, double &TP);
	void TMIN(double &dv, int &FLAG, double &T, double &U_r, double &VT_a, double &VR_a, double &beta_r);
	double PRTIAL(int FLAG, double R_a, double U_r);
	void RENTRY(double LD, double U_r, double eta_ar, double theta, double &T_rz, double &eta_rz, double &theta_cr);
	void SCAN();
	void VACOMP(double VR_a, double VT_a, double beta_r, double theta, VECTOR3 &DV, double &T_z, VECTOR3 &V_a, double &alpha, double &delta, double &lambda);
	void VUP2(VECTOR3 R_a, VECTOR3 V_a, double T_ar, double beta_r, VECTOR3 &V_a2);
	double TripTime(double v_a, double beta_a);
	void StoreSolution(VECTOR3 dv, double lat, double t0, double t, double tz);

	//INPUTS:

	//Program mode
	//0 = TCUA, 1 = FCUA, 2 = PTP tradeoff, 3 = PTP discrete, 4 = ATP tradeoff, 5 = ATP discrete
	int Mode;
	//State vector array
	std::vector<EphemerisData2> &XArray;
	//Maximum DV to be used for the abort manuever
	double DVMAX;
	//Time at which maneuver is to be computed
	double T0;
	//Flag that defines the reentry mode which is to be simulated
	//1: constant G reentry
	//2: G&N reentry
	//10: Compute R0 only; R0 is that portion of the down-range distance that depends on reentry speed only
	int ICRNGG;
	//Maximum inclination computed at reentry
	double I_rmax;
	//Maximum reentry speed
	double U_rmax;
	//Constant reentry relative range
	double RRBI;
	//Flag that selects the reentry target line
	//1: Use shallow reentry target line
	//2: Use steep reentry target line
	int IMSFN;
	//Earliest abort time to be considered
	double T_omin;
	//Latest abort time to be considered
	double T_omax;
	double lambda_z;
	double delta_z;
	double lambda_apo[5];
	double delta_apo[5];
	int mm;
	//Minimum time of landing
	double T_zmin;
	//Maximum time of landing
	double T_zmax;
	double MDM;
	double GMTbase;

	//PROGRAM SYMBOLS

	//Constants
	double mu;
	//Radial distance at reentry
	double RR;
	double R_E;
	double w_E;
	double T_rz_avg;
	const double DPR = DEG;
	const double KMPER = 6378.165;
	const double FTPER = KMPER * 1000.0 / 0.3048;
	const double NMPER = KMPER * 1000.0 / 1852.0;
	const double SCPHR = 3600.0;
	const double C3PIO2 = 3.0*PI05;
	const double RE = 1.0;


	//Global variables

	//Trip time for which motion of the impact point switches from easterly to westerly
	double TSW6;
	//Indicates whether the impact point moves easterly or westerly with increasing trip time
	int SW6;
	double VR_0;
	double VT_0;
	//Abort position magnitude
	double r0;
	//Preabort velocity
	double u0;
	//Unit vector in direction of abort point
	VECTOR3 R0;
	//Unit vector in direction of preabort angular momentum
	VECTOR3 R1;
	//Unit vector in direction of R1 x R0
	VECTOR3 R2;
	//
	double eta_rz_avg;
	//Preabort motion direction flag
	int Q0;
	//Trip time increment used in searching for PTP and ATP solutions
	double DT;
	//Fraction of trip time increment to be taken during the search for the PTP and ATP solutions
	double DDT;
	//Flag that indicates whether solution has been found
	//0 = TCUA or FCUA solution found
	//1 = no solution found
	//2 = PTP or ATP solution found
	int NOSOLN;
	//Indicates whether the function is being minimized or maximized
	int XK;
	//Indicates whether the possibility of a zero has been detected in the function being minimized
	int TEST;
	//Control flag used in PTP, ATP and FCUA modes.
	//0: Solution is sought to the entry target line
	//1: Solution is sought with DV < DVM
	//2. Solution is sought with DV = DVM
	int SW2;
	//Preabort flight-path angle
	double beta_0;
	//Maximum allowable trip time
	double T_max;
	//Minimum trip time
	double T_min;
	VECTOR3 X0, U0;
	//Azimuth change to nearest inclination constraint
	double theta_0;
	//Right ascension at Greenwich at abort time
	double alpha_g;
	//Maximum allowable DV
	double DVM;
	//Miss distance
	double MD;
	//Previous miss distance
	double MDP;
	//Minimum possible trip time, computed in subroutine TMIN
	double T_mt;
	bool END;
	//Indicates whether a solution has been found
	bool SOL;
	bool STORE;
	double T_ar_stored;
	OBJHANDLE hEarth;
	//
	VECTOR3 RR_vec, VV_vec;
};

struct RTEMoonStoredSolution
{
	double dv;
	double i_r;
	double INTER;
	bool q_m;
	double t_z;
};

struct RTEMoonInputsArray
{
	//BASIC INPUTS
	//Mode indicator
	//12 = PTP discrete, 14 = ATP discrete, 16 = UA discrete, 22 = PTP tradeoff, 24 = ATP tradeoff, 32 = PTP search, 34 = ATP search, 36 = UA search
	int SMODEI = 0;
	//An array of premaneuver states spaced along the preabort trajectory
	EphemerisDataTable2 STAVEC;
	//Sidereal angle
	double alpha_SID0 = 0.0;
	//Maximum allowable landing time in GMT; this is used in tradeoff displays and the UA submode
	double TZMAXI = 0.0;
	//Approaximate landing time in GMT for PTP and ATP discrete and search cases; also used as the minimum allowable landing time for tradeoff displays and all UA submodes
	double TZMINI = 0.0;
	//GENERAL PROGRAM OPTIONS
	//Maximum allowable return inclination, degrees
	double IRMAXI = 40.0;
	//Maximum allowable reentry speed, feet per second
	double URMAXI = 36323.0;
	//Relative range override, nautical miles
	double RRBI = 0.0;
	//Motion flag. 0 = postmaneuver direction of motion is selected internal to the program, 1 = only noncircumlinar motion is allowed, 2 = only circumlinar motion is allowed
	int CIRI = 0;
	//Minimum altitude allowed at closest approach to the moon, nautical miles
	double HMINI = 50.0;
	//Reentry flag. 0 = guided reentry to the steep target line, 1 = manual reentry to the shallow target line, 2 = manual reentry to the steep target line
	int EPI = 2;
	//Lift-to-drag ratio
	double L2DI = 0.29332;
	//Maximum allowable DV, feet per second
	double DVMAXI = 10000.0;
	//LANDING AREA IDENTIFICATION
	//PTP submode options
	//Longitude of the desired site, radians
	double LAMZI = 0.0;
	//Latitude of the desired site, radians
	double MUZI = 0.0;
	//Maximum allowable miss distance from the target site, nautical miles
	double MDMAXI = 0.0;
	//ATP submode options
	//The latitude, longitude coordinates of the target line; the order of input is latitude of point one, longitude of point one, latitude of point two etc; as many as five points maybe be used
	double LINE[10];
	//The desired return inclination is abs(IRKI); when IRKI < 0 the azimuth at reentry is greater than the azimuth at reentry for the minimum possible return inclination
	//When IRKI > 0, the reentry azimuth is less than that for the minimum possible return inclination
	//In degrees
	double IRKI = 0.0;
};

class RTEMoon : public RTCCModule
{
public:
	RTEMoon(RTCC *r);
	//Moon-Centered Return-to-Earth Subprocessor main control routine
	bool MASTER(const RTEMoonInputsArray &opt);

	//OUTPUTS:
	//Actual landing coordinates
	double EntryLatcor, EntryLngcor;
	//State vector at ignition
	EphemerisData2 sv0;
	//Velocity vector after the maneuver
	VECTOR3 Vig_apo;
	//Inertial Delta V vector
	VECTOR3 DV;
	//LVLH Delta V vector
	VECTOR3 Entry_DV;
	//Lunar flyby altitude
	double FlybyPeriAlt;
	//Reentry state
	VECTOR3 R_EI, V_EI;
	double t_R;
	//Reentry flight-path angle
	double EntryAng;
	//Earth-centered return inclination (negative if azimuth is TBD)
	double ReturnInclination;
	//Splashdown time
	double t_Landing;
private:

	struct RTEMoonSEARCHArray
	{
		//IPART: Flag to control interval of search. 1 = initial call, 2 = searching for local minimum, 3 = testing between left limit and current minimum, 4 = testing between current minimum and right limit
		int IPART = 1;
		//Arrays::
		//0 = left limit, 2 = current best, 4 = right limit
		//1 = test between left limit and local minimum, 3 = test between local minimum and right limit
		//5 = local minimum, 6 == global minimum?
		//Array of DV values
		double TEST[7];
		//Array of state vectors
		EphemerisData2 STAVEX[7];
		//Convergence flag
		bool IOUT = false;
		unsigned J;
		//Indicator for continuing the local minimum search before the 3rd state vector in the ephemeris table
		int K;
		//State vector counter
		int L;
		double TEST5;
		EphemerisData2 STAVC5;
		double dt;
	};

	//Read program inputs and transfers them to internal variables
	void READ(const RTEMoonInputsArray &opt);

	//PTP control logic
	void MCSS();
	//PTP accessibility logic
	void MCSSLM(bool &REP, double t_z_apo);
	//ATP control logic
	bool CLL(double &i_r, double &INTER, bool &q_m, double &t_z, double &dv);
	//UA control logic
	bool MCUA(double &i_r, double &INTER, bool &q_m, double &t_z, double &dv);

	VECTOR3 ThreeBodyAbort(VECTOR3 R_I, VECTOR3 V_I, double t_I, double t_EI, bool q_m, double Incl, double INTER, VECTOR3 &R_EI, VECTOR3 &V_EI);
	int MCDRIV(VECTOR3 Y_0, VECTOR3 V_0, double t_0, double var, bool q_m, double Incl, double INTER, bool KIP, double t_zmin, VECTOR3 &V_a, VECTOR3 &R_EI, VECTOR3 &V_EI, double &T_EI, bool &NIR, double &Incl_apo, double &y_p, bool &q_d);
	void SEARCH(RTEMoonSEARCHArray &arr, double dv);
	bool FINDUX(VECTOR3 X_x, double t_x, double r_r, double u_r, double beta_r, double i_r, double INTER, bool q_a, double mu, VECTOR3 &U_x, VECTOR3 &R_EI, VECTOR3 &V_EI, double &T_EI, double &Incl_apo) const;
	void INRFV(VECTOR3 R_1, VECTOR3 V_2, double r_2, double mu, bool k3, double &ainv, double &e, double &p, double &theta, VECTOR3 &V_1, VECTOR3 &R_2, double &dt_2, bool &q_m, bool &k_1, double &beta_1, double &beta_2) const;
	void STORE(int opt, double &dv, double &i_r, double &INTER, double &t_z, bool &q_m);
	void PSTATE(double ainv_H, double e_H, double p_H, double t_0, double T_x, VECTOR3 Y_0, VECTOR3 &Y_a_apo, VECTOR3 V_x, double theta, double beta_a, double beta_x, double T_a, VECTOR3 &V_a, double &t_x_aaapo, VECTOR3 &Y_x_apo, double &Dy_0, double &deltat, VECTOR3 &X_mx, VECTOR3 &U_mx) const;

	int hMoon;
	double mu_E, mu_M, w_E, R_E, R_M;
	//double r_s; //Pseudostate sphere
	double EntryLng;
	double dlngapo, dtapo;
	double i_rmax, u_rmax;
	//12 = PTP discrete, 14 = ATP discrete, 16 = UA discrete
	//22 = PTP tradeoff, 24 = ATP tradeoff
	//32 = PTP search, 34 = ATP search, 36 = UA search
	int SMODE;
	// 2: primary target point mode
	// 4: alternate target point mode
	// 6: fuel critical unspecified area mode
	int CRIT;
	// 1: abort at a discrete time
	// 2: produce tradeoff display
	// 3: optimize DV as a function of maneuver time
	int LETSGO;
	// 1: print flag for tradeoff display
	// 2: print flag for discrete and search cases
	int LFLAG;
	int CENT;
	// Minimum altitude allowed at closest approach to the moon
	double h_min;
	// the maximum allowable miss distance from the target site
	double MD_max;
	// 0: postmaneuver direction of motion is selected internal to the program
	// 1: only noncircumlunar motion is allowed
	// 2: only circumlunar motion is allowed
	int CIRCUM;
	// lift-to-drag ratio
	double LD;
	// maximum allowable DV
	double DV_max;
	// relative range override (unit is nautical miles!)
	double r_rbias;
	// 0: guide reentry to the steep target line
	// 1: manual reentry to the shallow target line
	// 2: manual reentry to the steep target line
	int ICRNGG;
	// desired inclination. Signed for the two azimuth options
	double i_rk;
	// true if postmaneuver direction of motion determined internally
	bool bRTCC;
	double t_zmin, t_zmax;
	double mu_z, mu_z1;
	double lambda_z, lambda_z1;
	//Right ascension at GMT zero
	double alpha_SID0;
	//0 = use circumlunar or noncircumlunar, 1 = use retrograde motion
	bool QDFLG;
	//Reentry radius
	double r_r;
	//Radius of the pseudostate transformation sphere
	double r_s;
	//Position and velocity vector of the Moon at abort time
	VECTOR3 X_m0, U_m0;
	//Alternate target line (lat, lng, lat, lng etc.)
	double LINE[10];
	//State vector array
	EphemerisDataTable2 STAVEC;

	RTEMoonStoredSolution solution;

	//Maximum number of iterations for MCDRIV
	const int k_max = 50;
};