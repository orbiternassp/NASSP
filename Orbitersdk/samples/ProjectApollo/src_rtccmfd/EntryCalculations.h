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
	void landingsite(VECTOR3 REI, VECTOR3 VEI, double MJD_EI, double &lambda, double &phi);
	void Reentry(VECTOR3 REI, VECTOR3 VEI, double mjd0, bool highspeed, double &EntryLatPred, double &EntryLngPred, double &EntryRTGO, double &EntryVIO, double &EntryRET);
	VECTOR3 ThreeBodyAbort(double t_I, double t_EI, VECTOR3 R_I, VECTOR3 V_I, double mu_E, double mu_M, bool INRFVsign, VECTOR3 &R_EI, VECTOR3 &V_EI, double Incl = 0, bool asc = true);
	void Abort(VECTOR3 R0, VECTOR3 V0, double RCON, double dt, double mu, VECTOR3 &DV, VECTOR3 &R_EI, VECTOR3 &V_EI);
	bool Abort_plane(VECTOR3 R0, VECTOR3 V0, double MJD0, double RCON, double dt, double mu, double Incl, double INTER, VECTOR3 &DV, VECTOR3 &R_EI, VECTOR3 &V_EI, double &Incl_apo);
	void time_reentry(VECTOR3 R0, VECTOR3 V0, double r1, double x2, double dt, double mu, VECTOR3 &V, VECTOR3 &R_EI, VECTOR3 &V_EI);
	void time_reentry_plane(VECTOR3 R0, VECTOR3 eta, double r1, double x2, double dt, double mu, VECTOR3 &V, VECTOR3 &R_EI, VECTOR3 &V_EI);
	double landingzonelong(int zone, double lat);

	//Actual RTE processor routines
	void REENTRYNew(double LD, int ICRNGG, double v_i, double i_r, double A_Z, double mu, double r_rbias, double &eta_rz1, double &theta_cr, double &T);
	VECTOR3 TVECT(VECTOR3 a, VECTOR3 b, double alpha, double gamma);
	void EGTR(VECTOR3 R_geoc, VECTOR3 V_geoc, double MJD, VECTOR3 &R_geogr, VECTOR3 &V_geogr);
	double INTER(const double *X, const double *Y, int IMAX, double x);
	double URF(double T, double x);
	void TFPCR(double mu, int k, double a_apo, double e, double r, double &T, double &P);
	void AESR(double r1, double r2, double beta1, double T, double R, double mu, double eps, double &a, double &e, int &k2, int &info, double &V1);
	VECTOR3 MCDRIV(double t_I, double var, VECTOR3 R_I, VECTOR3 V_I, double mu_E, double mu_M, bool INRFVsign, double Incl, double INTER, bool KIP, double t_zmin, VECTOR3 &R_EI, VECTOR3 &V_EI, double &MJD_EI, bool &NIR, double &Incl_apo, double &r_p);
	bool FINDUX(VECTOR3 R0, VECTOR3 V0, double MJD0, double r_r, double u_r, double beta_r, double i_r, double INTER, bool q_a, double mu, VECTOR3 &DV, VECTOR3 &R_EI, VECTOR3 &V_EI, double &MJD_EI, double &Incl_apo);
	int MINMIZ(VECTOR3 &X, VECTOR3 &Y, VECTOR3 &Z, bool opt, VECTOR3 CUR, double TOL, double &XMIN, double &YMIN);
	double LNDING(VECTOR3 REI, VECTOR3 VEI, double MJD_EI, double LD, int ICRNGG, double r_rbias, double &lambda, double &phi, double &MJD_L);
	double SEARCH(int &IPART, VECTOR3 &DVARR, VECTOR3 &TIGARR, double tig, double dv, bool &IOUT);
	void SIDCOM(double JD0, double DT, double N, double &alpha_go, double &T);

	double MPL(double lat);
	double MPL2(double lat);
	double EPL(double lat);
	double AOL(double lat);
	double IOL(double lat);
	double WPL(double lat);
}

class EarthEntry {
public:
	EarthEntry(VECTOR3 R0B, VECTOR3 V0B, double mjd, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, bool entrynominal, bool entrylongmanual);
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
	double phi2;
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

class RTEEarth {
public:
	RTEEarth(VECTOR3 R0B, VECTOR3 V0B, double mjd, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, int critical, bool entrylongmanual, double RRBI, double DVMAXI);
	bool EntryIter();

	double EntryTIGcor; //Corrected Time of Ignition for the Reentry Maneuver
	double EntryLngcor;	//Corrected Splashdown Longitude
	double EntryLatcor;	//Corrected Splashdown Latitude
	//VECTOR3 Entry_DV;
	double EntryLatPred, EntryLngPred;	//Predicted Splashdown Latitude and Longitude
	double EntryRET, EntryRTGO, EntryVIO;
	double V400k, gamma400k;
	double EntryAng;
	double t2;
	VECTOR3 Entry_DV; //Entry DV vector in LVLH coordinates
	int precision; //0 = only conic, 1 = precision, 2 = PeA=-30 solution
	OBJHANDLE SOIplan; //maneuver in earth or moon SOI
	int errorstate;
private:
	void coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21);
	void precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2);
	void precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2, double &C_FPA);
	void conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1);
	void conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3);
	double dvmaxiterator(VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double dxmax, double dv);
	void xdviterator(VECTOR3 R1B,VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x);
	void limitxchange(double theta1, double theta2, double theta3, VECTOR3 V1B, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double xmax, double &x);
	void dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON);
	void reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 REI, VECTOR3 VEI);
	void newxt2(int n1, double xt2err, double &xt2_apo, double &xt2, double &xt2err_apo);
	void finalstatevector(VECTOR3 R1B, VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE);
	void newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo);
	OBJHANDLE AGCGravityRef(VESSEL *vessel);

	OBJHANDLE gravref, hEarth;
	double MA1, C0, C1, C2, C3;
	VECTOR3 R0B, V0B;
	double mjd;
	double GETbase, get;
	double RCON, RD;
	double mu;
	double EntryTIGcor_old, dlng_old;
	int ii;
	double EntryLng;
	int entryphase;
	int critical; //1 = MCC calculation, 2 = TLC or TEC abort, 3 = MCC calculation (corridor control)
	double xapo, dv_err;
	VECTOR3 R11B, V11B;
	int f2;
	double dlngapo,dt0, x2, x2_apo;
	double EMSAlt;
	double k1, k2, k3, k4;
	int revcor;
	double phi2;
	double R_E;
	double dt1; //time between estimated maneuver time and actual (currently iterated) maneuver time
	double x, dx, dxmax;
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	bool entrylongmanual;
	double xlim;
	double t21;
	double EntryInterface;
	// relative range override (unit is nautical miles!)
	double r_rbias;
	double dv_max;
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
	int NOSOLN;
};

class ConicRTEEarthNew
{
public:
	ConicRTEEarthNew(std::vector<MPTSV> &SVArray, PZEFEM &ephemeris, std::vector<TradeoffData> &todata);
	void MAIN();
	void Init(double dvm, int icrngg, double irmax, double urmax, double rrbi, int imsfn);
	void READ(int Mode, double getbase, double tzmin, double tzmax);
	void ATP(std::vector<ATPData> line);
protected:

	//SUBROUTINES:
	void INITAL();
	void DVMINQ(int FLAG, int QE, int Q0, double beta_r, double &DV, int &QA, double &V_a, double &beta_a);
	void FCUA(int FLAG, VECTOR3 R_a, double &beta_r, double &DV, double &U_r, double &V_a, double &beta_a);
	void MSDS(double VR_a, double VT_a, double beta_r, double theta, double &delta, double &phi, double &phi_z, double &lambda, double &theta_z);
	void RUBR(int QA, int QE, double R_a, double U_0, double U_r, double beta_r, double &A, double &DV, double &e, double &T, double &V_a, double &beta_a);
	void VELCOM(double T, double R_a, double &beta_r, double &dt, double &p, int &QA, int &sw6, double &U_r, double &VR_a, double &VT_a, double &beta_a, double &eta_ar, double &DV);
	void VARMIN();
	void TCOMP(double dv, double delta, double &T, double &TP);
	void TMIN(double &dv, int &FLAG, double &T, double &U_r, double &VT_a, double &VR_a, double &beta_r);
	double PRTIAL(int FLAG, double R_a, double U_r);
	void RENTRY(double LD, double U_r, double eta_ar, double theta, double &T_rz, double &eta_rz, double &theta_cr);
	void SCAN();
	void VACOMP(double VR_a, double VT_a, double beta_r, double theta, VECTOR3 &DV, double &T_z, VECTOR3 &V_a, double &alpha, double &delta, double &lambda);
	void VUP2(VECTOR3 R_a, VECTOR3 V_a, double T_ar, double beta_r, VECTOR3 &V_a2);
	double TripTime(double v_a, double beta_a);
	void StoreSolution(VECTOR3 dv, double lat, double t0, double tz);

	//INPUTS:

	//Program mode
	//0 = TCUA, 1 = FCUA, 2 = PTP tradeoff, 3 = PTP discrete, 4 = ATP tradeoff, 5 = ATP discrete
	int Mode;
	//State vector array
	std::vector<MPTSV> &XArray;
	PZEFEM &ephem;
	std::vector<TradeoffData> &TOData;
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
	double GETbase;

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
};

class RTEMoon
{
public:
	RTEMoon(VECTOR3 R0M, VECTOR3 V0M, double mjd0, OBJHANDLE gravref, double GETbase, double *line);
	void READ(int SMODEI, double IRMAXI, double URMAXI, double RRBI, int CIRI, double HMINI, int EPI, double L2DI, double DVMAXI, double MUZI, double IRKI, double MDMAXI, double TZMINI, double TZMAXI);
	bool MASTER();
	void MCSS();
	bool CLL(double &i_r, double &INTER, double &dv);
	bool MCUA(double &i_r, double &INTER, double &dv);
	
	void MCSSLM(bool &REP, double t_z_apo);

	int precision;
	double EntryLatcor, EntryLngcor;
	VECTOR3 Entry_DV;
	VECTOR3 R_EI, V_EI;
	double EIMJD;
	double EntryAng;
	VECTOR3 Rig, Vig, Vig_apo;
	double TIG;
	double ReturnInclination;
	double FlybyPeriAlt;
private:

	bool TBLOOK(double lat, double &lng);

	OBJHANDLE hMoon, hEarth;
	VECTOR3 DV;
	double mu_E, mu_M, w_E, R_E, R_M;
	//double r_s; //Pseudostate sphere
	CELBODY *cMoon;
	double EntryLng;
	double dlngapo, dtapo;
	bool INRFVsign;
	double dTIG, mjd0, GETbase;
	double i_rmax, u_rmax;
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
	double t_zmin, t_zmax;
	double mu_z, mu_z1;
	double lambda_z, lambda_z1;
	
	//Alternate target line (lat, lng, lat, lng etc.)
	double LINE[10];
};