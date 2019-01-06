#ifndef _ORBMECH_H
#define _ORBMECH_H

#include "Orbitersdk.h"
#include <vector>

const VECTOR3 navstars[37] = { _V(0.87325707, 0.222717753, 0.433380771),
_V(0.933983515, 0.0421048982, -0.354826677),
_V(0.474230235, 0.456854026, 0.75258892),
_V(0.492731712, -0.129171941, -0.860540568),
_V(0.010128911, 0.40497893, 0.914269912),
_V(0.543321089, 0.233682862, -0.806346398),
_V(0.698256178, 0.681882483, -0.217886645),
_V(0.404981298, 0.764255442, 0.501900156),
_V(0.343908924, 0.93415959, -0.0952497353),
_V(0.195051858, 0.833544498, -0.516873623),
_V(0.130502244, 0.912114029, 0.388609268),
_V(-0.0632195158, 0.236597063, -0.969548928),
_V(-0.187545554, 0.747401122, -0.637352514),
_V(-0.418201486, 0.865470289, -0.27580554),
_V(-0.362955763, 0.232568134, -0.902316561),
_V(-0.471158209, 0.73100027, 0.493607686),
_V(-0.777933252, 0.49981843, -0.380790484),
_V(-0.864519888, 0.502532948, 0.00812396612),
_V(-0.96672924, 0.142390103, 0.212507964),
_V(-0.951226902, -0.180199054, -0.250391056),
_V(-0.449404749, -0.402805961, -0.797359849),
_V(-0.914079394, -0.403947962, -0.0358455889),
_V(-0.581451821, 0.0311390298, 0.812984712),
_V(-0.685292494, -0.623812256, -0.375809082),
_V(-0.783614901, -0.352762709, 0.511376728),
_V(-0.529185544, -0.481419898, 0.698711344),
_V(-0.344817825, -0.935282024, -0.0796756118),
_V(-0.109619054, -0.684031025, -0.721169342),
_V(-0.106559622, -0.803652501, 0.585480746),
_V(0.125067322, -0.456806946, 0.88073014),
_V(0.214089696, -0.974959511, -0.0601627197),
_V(0.459149324, -0.741396708, 0.489400469),
_V(0.558073669, -0.825925685, 0.0800033956),
_V(0.325589471, -0.737606353, -0.591547432),
_V(0.455646022, -0.209297007, 0.865206025),
_V(0.817196194, -0.436629484, 0.376224766),
_V(0.837327731, -0.410968743, -0.360537048)
};

#define NUMBEROFGROUNDSTATIONS 18

static const char* gsnames[NUMBEROFGROUNDSTATIONS] = {
	{ "BERMUDA" },
	{ "GRAND CANARY" },
	{ "CARNARVON" },
	{ "HONEYSUCKLE" },
	{ "GUAYMAS" },
	{ "HAWAII" },
	{ "USNS VANGUARD" },
	{ "ASCENSION" },
	{ "GUAM" },
	{ "MERRIT" },
	{ "CORPUS CHRISTI" },
	{ "GRAND BAHAMA" },
	{ "ANTIGUA" },
	{ "TANANARIVE" },
	{ "USNS MERCURY" },
	{ "USNS HUNTSVILLE" },
	{ "USNS REDSTONE" },
	{ "GOLDSTONE" }
};

const double groundstations[NUMBEROFGROUNDSTATIONS][2] = {
	{ 32.3528*RAD, -64.6592*RAD},
	{ 27.74055*RAD, -15.60077*RAD},
	{-24.90619*RAD,  113.72595*RAD},
	{-35.40282*RAD,  148.98144*RAD},
	{ 27.95029*RAD, -110.90846*RAD},
	{ 21.44719*RAD, -157.76307*RAD},
	{ 32.7*RAD, -48.0*RAD },
	{ -7.94354*RAD, -14.37105*RAD },
	{ 13.30929*RAD, 144.73694*RAD },
	{ 28.40433*RAD, -80.60192*RAD },
	{ 27.65273*RAD, -97.37588*RAD },
	{ 26.62022*RAD, -78.35825*RAD },
	{ 28.40433*RAD, -80.60192*RAD },
	{ -19.00000*RAD, 47.27556*RAD },
	{ 25.0*RAD, 125.0*RAD },
	{ 25.0*RAD, -136.0*RAD },
	{ -24.0*RAD, -118.0*RAD },
	{ 35.33820*RAD, -116.87421*RAD }
};

struct OELEMENTS
{
	double h = 0.0;
	double e = 0.0;
	double i = 0.0;
	double RA = 0.0;
	double w = 0.0;
	double TA = 0.0;
};

struct TLMCConstants
{
	double r;
	double lat;
	double gamma;
	double MJD;
	double dt;
	OBJHANDLE hMoon;
	OBJHANDLE gravout;
};

struct TLMCXYZTConstants
{
	VECTOR3 R1;
	VECTOR3 V1;
	double MJD;
	double dt;
	OBJHANDLE gravin;
};

struct TLMCFlybyConstants
{
	VECTOR3 R1;
	VECTOR3 V1;
	double mjd0;
	OBJHANDLE gravin;
};

class CoastIntegrator
{
public:
	CoastIntegrator(VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, OBJHANDLE planet, OBJHANDLE outplanet);
	~CoastIntegrator();
	bool iteration();

	VECTOR3 R2, V2;
	OBJHANDLE outplanet;
	bool soichange;
private:
	VECTOR3 f(VECTOR3 alpha, VECTOR3 R, VECTOR3 a_d);
	double fq(double q);
	VECTOR3 adfunc(VECTOR3 R);
	void SolarEphemeris(double t, VECTOR3 &R_ES, VECTOR3 &V_ES);

	double R_E, mu;
	double K, dt_lim;
	int jcount;
	double *JCoeff;
	VECTOR3 R00, V00, R0, V0, R_CON, V_CON, R_QC, R_PQ;
	double t_0, t, tau, t_F, x;
	VECTOR3 delta, nu;
	//0 = earth or lunar orbit, 1 = cislunar-midcourse flight
	int M;
	double r_MP, r_dP, r_SPH;
	OBJHANDLE hEarth, hMoon, hSun, planet;
	double mu_Q, mu_S;
	double mjd0;
	double rect1, rect2;
	CELBODY *cMoon, *cEarth, *cSun;
	VECTOR3 U_Z_E, U_Z_M;
	int B;
	//Primary Body, 0 = Earth, 1 = Moon
	int P;
	VECTOR3 R_ES0, V_ES0;
	double W_ES;
};

namespace OrbMech {

	//public:
		//OrbMech(VESSEL *v, OBJHANDLE gravref);

	void rv_from_r0v0_obla(VECTOR3 R1, VECTOR3 V1, double MJD, double dt, VECTOR3 &R2, VECTOR3 &V2, OBJHANDLE gravref);
	double kepler_E(double e, double M);
	double kepler_H(double e, double M);
	double power(double b, double e);
	void sv_from_coe(OELEMENTS el, double mu, VECTOR3 &R, VECTOR3 &V);
	OELEMENTS coe_from_sv(VECTOR3 R, VECTOR3 V, double mu);
	VECTOR3 elegant_lambert(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double dt, int N, bool prog, double mu);
	VECTOR3 ThreeBodyLambert(double t_I, double t_E, VECTOR3 R_I, VECTOR3 V_init, VECTOR3 R_E, VECTOR3 R_m, VECTOR3 V_m, double r_s, double mu_E, double mu_M, VECTOR3 &R_I_star, VECTOR3 &delta_I_star, VECTOR3 &delta_I_star_dot, double tol = 1000.0);
	void INRFV(VECTOR3 R_1, VECTOR3 V_2, double r_2, bool direct, double mu, VECTOR3 &V_1, VECTOR3 &R_2, double &dt_2);
	void SolveQuartic(double *A, double *R, int &N);
	VECTOR3 GeneralizedIterator(VECTOR3(*state_evaluation)(VECTOR3, void*), bool(*endcondition)(VECTOR3), VECTOR3 Target, VECTOR3 var_guess, VECTOR3 stepsizes, void *constants);
	VECTOR3 GeneralizedIterator2(VECTOR3(*state_evaluation)(VECTOR3, void*), bool(*endcondition)(VECTOR3), VECTOR3 Target, VECTOR3 var_guess, VECTOR3 stepsizes, void *constants);
	VECTOR3 Vinti(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double mjd0, double dt, int N, bool prog, OBJHANDLE gravref, OBJHANDLE gravin, OBJHANDLE gravout, VECTOR3 V_guess, double tol = 0.1);
	VECTOR3 TLMCConicFirstGuessIterator(double r_peri, double lat_EMP, double gamma, VECTOR3 var_guess, VECTOR3 R2, VECTOR3 step, double MJD0, double dt, OBJHANDLE hMoon, OBJHANDLE gravout);
	VECTOR3 IntegratedTLMCIterator(double r_peri, double lat_EMP, double gamma, VECTOR3 var_guess, VECTOR3 R2, VECTOR3 step, double mjd0, double dt, OBJHANDLE hMoon, OBJHANDLE gravout);
	VECTOR3 TLMCIntegratedXYZTIterator(VECTOR3 R1, VECTOR3 V1, double mjd0, OBJHANDLE gravin, VECTOR3 DV_guess, VECTOR3 target, VECTOR3 step, double dt);
	VECTOR3 TLMCConicFlybyIterator(VECTOR3 R1, VECTOR3 V1, double mjd0, OBJHANDLE gravin, VECTOR3 DV_guess, VECTOR3 target, VECTOR3 step);
	VECTOR3 TLMCIntegratedFlybyIterator(VECTOR3 R1, VECTOR3 V1, double mjd0, OBJHANDLE gravin, VECTOR3 DV_guess, VECTOR3 target, VECTOR3 step);
	VECTOR3 TLMCConicSPSLunarFlybyIterator(VECTOR3 R1, VECTOR3 V1, double mjd0, OBJHANDLE gravin, VECTOR3 DV_guess, VECTOR3 target, VECTOR3 step);
	VECTOR3 TLMCIntegratedSPSLunarFlybyIterator(VECTOR3 R1, VECTOR3 V1, double mjd0, OBJHANDLE gravin, VECTOR3 DV_guess, VECTOR3 target, VECTOR3 step);
	void ThirdBodyConic(VECTOR3 R1, OBJHANDLE grav1, VECTOR3 R2, OBJHANDLE grav2, double mjd0, double dt, VECTOR3 V_guess, VECTOR3 &V1_apo, VECTOR3 &V2_apo, double tol = 0.1);
	double NSRsecant(VECTOR3 RA, VECTOR3 VA, VECTOR3 RP, VECTOR3 VP, double mjd0, double x, double DH, OBJHANDLE gravref);
	void ra_and_dec_from_r(VECTOR3 R, double &ra, double &dec);
	void rv_from_r0v0_ta(VECTOR3 R0, VECTOR3 V0, double dt, VECTOR3 &R1, VECTOR3 &V1, double mu);
	double time_theta(VECTOR3 R, VECTOR3 V, double dtheta, double mu);
	void f_and_g_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &f, double &g, double mu);
	void fDot_and_gDot_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &fdot, double &gdot, double mu);
	void rv_from_r0v0_tb(VECTOR3 R0, VECTOR3 V0, double mjd0, OBJHANDLE hMoon, OBJHANDLE gravout, double t, VECTOR3 &R1, VECTOR3 &V1);
	void local_to_equ(VECTOR3 R, double &r, double &phi, double &lambda);
	double period(VECTOR3 R, VECTOR3 V, double mu);
	void perifocal(double h, double mu, double e, double theta, double inc, double lambda, double w, VECTOR3 &RX, VECTOR3 &VX);
	double fischer_ellipsoid(VECTOR3 R);
	double timetoperi(VECTOR3 R, VECTOR3 V, double mu);
	double timetoapo_integ(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref);
	double timetoapo_integ(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, VECTOR3 &R2, VECTOR3 &V2);
	double timetoperi_integ(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, OBJHANDLE ref_peri);
	double timetoperi_integ(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, OBJHANDLE ref_peri, VECTOR3 &R2, VECTOR3 &V2);
	double timetoapo(VECTOR3 R, VECTOR3 V, double mu, int s = 0);
	double timetonode_integ(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, VECTOR3 u_node, VECTOR3 &R2, VECTOR3 &V2);
	double time_radius(VECTOR3 R, VECTOR3 V, double r, double s, double mu);
	double time_radius_integ(VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout);
	double time_radius_integ(VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout, VECTOR3 &RPRE, VECTOR3 &VPRE);
	double findpatchpoint(VECTOR3 R1, VECTOR3 V1, double mjd1, double mu_E, double mu_M, VECTOR3 &RP_M, VECTOR3 &VP_M);
	void GetLunarEphemeris(double MJD, VECTOR3 &R_EM, VECTOR3 &V_EM);
	void ReturnPerigee(VECTOR3 R, VECTOR3 V, double mjd0, OBJHANDLE hMoon, OBJHANDLE hEarth, double phi, double &MJD_peri, VECTOR3 &R_peri, VECTOR3 &V_peri);
	void ReturnPerigeeConic(VECTOR3 R, VECTOR3 V, double mjd0, OBJHANDLE hMoon, OBJHANDLE hEarth, double &MJD_peri, VECTOR3 &R_peri, VECTOR3 &V_peri);
	double PATCH(VECTOR3 R, VECTOR3 V, double mjd0, bool earthsoi, VECTOR3 &R3, VECTOR3 &V3, bool Q = true);
	MATRIX3 GetRotationMatrix(OBJHANDLE plan, double t);
	//MATRIX3 GetRotationMatrix2(OBJHANDLE plan, double t);
	MATRIX3 Orbiter2PACSS13(double mjd, double lat, double lng, double azi);
	void PACSS4_from_coe(OELEMENTS coe, double mu, VECTOR3 &R, VECTOR3 &V);
	void PACSS13_from_coe(OELEMENTS coe, double lat, double A_Z, double mu, VECTOR3 &R_S, VECTOR3 &V_S);
	OELEMENTS coe_from_PACSS4(VECTOR3 R, VECTOR3 V, double mu);
	MATRIX3 MSGMatrix(double phi_L, double A_Z);
	MATRIX3 MEGMatrix(double theta_E);
	double GetPlanetCurrentRotation(OBJHANDLE plan, double t);
	double findelev(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double mjd0, double E, OBJHANDLE gravref);
	double findelev_conic(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double E, double mu);
	double findelev_gs(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_gs, double mjd0, double E, OBJHANDLE gravref, double &range);
	VECTOR3 ULOS(MATRIX3 REFSMMAT, MATRIX3 SMNB, double TA, double SA);
	int FindNearestStar(VECTOR3 U_LOS, VECTOR3 R_C, double R_E, double ang_max);
	VECTOR3 AOTULOS(MATRIX3 REFSMMAT, MATRIX3 SMNB, double AZ, double EL);
	bool isnotocculted(VECTOR3 S_SM, VECTOR3 R_C, double R_E);
	VECTOR3 CALCGAR(MATRIX3 REFSM, MATRIX3 SMNB);
	MATRIX3 CALCSMSC(VECTOR3 GA);
	VECTOR3 CALCGTA(MATRIX3 des);
	void CALCCOASA(MATRIX3 SMNB, VECTOR3 S_SM, double &SPA, double &SXP);
	void CALCSXA(MATRIX3 SMNB, VECTOR3 S_SM, double &TA, double &SA);
	MATRIX3 AXISGEN(VECTOR3 s_NBA, VECTOR3 s_NBB, VECTOR3 s_SMA, VECTOR3 s_SMB);
	MATRIX3 ROTCOMP(VECTOR3 U_R, double A);
	VECTOR3 backupgdcalignment(MATRIX3 REFS, VECTOR3 R_C, double R_E, int &set);
	//void rungeinteg(VECTOR3 R0, VECTOR3 V0, double dt, VECTOR3 &R1, VECTOR3 &V1, double mu);
	//void adfunc(double* dfdt, double t, double* f);
	//int rkf45(double*, double**, double*, double*, int, double tol = 1e-15);
	bool oneclickcoast(VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, VECTOR3 &R1, VECTOR3 &V1, OBJHANDLE gravref, OBJHANDLE &gravout);
	void periapo(VECTOR3 R, VECTOR3 V, double mu, double &apo, double &peri);
	void umbra(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v1);
	double sunrise(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, OBJHANDLE planet2, bool rise, bool midnight, bool future = false);
	void orbitmidnight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool night, double &v1);
	bool sight(VECTOR3 R1, VECTOR3 R2, double R_E);
	VECTOR3 AdjustApoapsis(VECTOR3 R, VECTOR3 V, double mu, double r_apo_des);
	VECTOR3 AdjustPeriapsis(VECTOR3 R, VECTOR3 V, double mu, double r_peri_des);
	VECTOR3 CircularOrbitDV(VECTOR3 R, VECTOR3 V, double mu);
	VECTOR3 RotateVelocityVector(VECTOR3 R, VECTOR3 V, double ang);
	double P29TimeOfLongitude(VECTOR3 R0, VECTOR3 V0, double MJD, OBJHANDLE gravref, double phi_d);
	void latlong_from_J2000(VECTOR3 R, double MJD, OBJHANDLE gravref, double &lat, double &lng);
	void latlong_from_r(VECTOR3 R, double &lat, double &lng);
	VECTOR3 r_from_latlong(double lat, double lng);
	VECTOR3 r_from_latlong(double lat, double lng, double r);
	double findlatitude(VECTOR3 R, VECTOR3 V, double mjd, OBJHANDLE gravref, double lat, bool up, VECTOR3 &Rlat, VECTOR3 &Vlat);
	double FindNextEquatorialCrossing(VECTOR3 R, VECTOR3 V, double mjd, OBJHANDLE gravref);
	double GETfromMJD(double MJD, double GETBase);
	double MJDfromGET(double GET, double GETBase);
	void format_time_HHMMSS(char *buf, double time);
	void format_time_MMSS(char *buf, double time);
	bool groundstation(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, double lat, double lng, bool rise, double &dt);
	bool gslineofsight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v1);
	int findNextAOS(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet);
	bool vesselinLOS(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet);
	MATRIX3 LaunchREFSMMAT(double lat, double lng, double mjd, double A_Z);
	void LunarLandingPrediction(VECTOR3 R_D, VECTOR3 V_D, double t_D, double t_E, VECTOR3 R_LSA, double h_DP, double theta_F, double t_F, OBJHANDLE plan, double GETbase, double mu, int N, double &t_DOI, double &t_PDI, double &t_L, VECTOR3 &DV_DOI, double &CR);
	void LunarLandingPrediction2(VECTOR3 R_0, VECTOR3 V_0, double t_0, double t_E, VECTOR3 R_LSA, double h_P, double h_A, double theta_F, double t_F, OBJHANDLE plan, double GETbase, double mu, int N, double & t_DOI, double &t_PDI, double &t_L, VECTOR3 &DV_DOI, double &CR);
	void xaxislambert(VECTOR3 RA1, VECTOR3 VA1, VECTOR3 RP2off, double dt2, int N, bool tgtprograde, double mu, VECTOR3 &VAP2, double &zoff);
	void poweredflight(VECTOR3 R, VECTOR3 V, double mjd0, OBJHANDLE gravref, double f_T, double v_ex, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &m_cutoff, double &t_go);
	//void poweredflight2(VESSEL* vessel, VECTOR3 R, VECTOR3 V, OBJHANDLE gravref, THRUSTER_HANDLE thruster, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &t_go);
	VECTOR3 gravityroutine(VECTOR3 R, OBJHANDLE gravref, double mjd0);
	void impulsive(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_T, double f_av, double isp, double m, VECTOR3 DV, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff, bool agc = true);
	void impulsive(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_T, double f_av, double isp, double m, VECTOR3 R_ref, VECTOR3 V_ref, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff);
	double DVFromBurnTime(double bt, double thrust, double isp, double mass);
	void checkstar(MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &trunnion, double &shaft);
	void coascheckstar(MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &spa, double &sxp);
	void AOTcheckstar(MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct);
	void REVUP(VECTOR3 R, VECTOR3 V, double n, double mu, VECTOR3 &R1, VECTOR3 &V1, double &t);
	void RADUP(VECTOR3 R_W, VECTOR3 V_W, VECTOR3 R_C, double mu, VECTOR3 &R_W1, VECTOR3 &V_W1);
	void COE(VECTOR3 R, VECTOR3 V, double h, double mu, VECTOR3 &R_C, VECTOR3 &V_C);
	void ITER(double &c, int &s, double e, double &p, double &x, double &eo, double &xo, double dx0 = 1.0);
	bool QDRTPI(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double mu, double dh, double E_L, int s, VECTOR3 &R_J, VECTOR3 &V_J);
	bool CSIToDH(VECTOR3 R_A1, VECTOR3 V_A1, VECTOR3 R_P2, VECTOR3 V_P2, double DH, double mu, double &dv);
	VECTOR3 CoellipticDV(VECTOR3 R_A2, VECTOR3 R_PC, VECTOR3 V_PC, double mu);
	VECTOR3 ApplyHorizontalDV(VECTOR3 R, VECTOR3 V, double dv);
	double QuadraticIterator(int &c, int &s, double &varguess, double *var, double *obj, double obj0, double initstep, double maxstep);
	bool SolveSystem(int n, double *A, double *b, double *x, double *det);
	bool SolveSeries(double *x, double *y, int ndata, double *out, int m);
	void GetLunarEquatorialCoordinates(double MJD, double &ra, double &dec, double &radius);
	void EMPToEcl(VECTOR3 R_EMP, VECTOR3 V_EMP, double MJD, VECTOR3 &R_Ecl, VECTOR3 &V_Ecl);
	void EclToEMP(VECTOR3 R_Ecl, VECTOR3 V_Ecl, double MJD, VECTOR3 &R_EMP, VECTOR3 &V_EMP);
	void RotatePerigeeToSpecifiedLongitude(VECTOR3 R, VECTOR3 V, double mjd, OBJHANDLE plan, double lng_des, int N, double mu, double &dv, double &dTIG, double &dt);
	OELEMENTS PlaneChange(OELEMENTS coe_b, double dW);
	OELEMENTS NodeShift(OELEMENTS coe_b, double dLAN);
	OELEMENTS ApoapsisPeriapsisChange(OELEMENTS coe_b, double mu, double r_A, double r_P);
	VECTOR3 HeightManeuver(VECTOR3 R, VECTOR3 V, double dh, double mu);
	//private:
		//VESSEL* vessel;
		//double mu;
		//int JCoeffCount;
		//double* JCoeff;
		//CoastIntegrator* coast;
		//OBJHANDLE gravref;
		//double R_b;


	//typedef void (OrbMech::*adfuncptr)(double* dfdt, double t, double* f);

	MATRIX3 inverse(MATRIX3 a);
	double determinant(MATRIX3 a);
	MATRIX3 tmat(MATRIX3 a);
	template <typename T> int sign(T val);
	int DoubleToBuffer(double x, double q, int m);
	int DoubleToDEDA(double x, double q);
	double cot(double a);
	double sec(double a);
	void fabs_vektor(double* vektor, int n);
	double vektor_max(double* vektor, int n);
	double power(double b, double e);
	double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle);
	void local_to_equ(VECTOR3 R, double &r, double &phi, double &lambda);
	MATRIX3 GetObliquityMatrix(OBJHANDLE plan, double t);
	MATRIX3 J2000EclToBRCS(double mjd);
	MATRIX3 _MRx(double a);
	MATRIX3 _MRy(double a);
	MATRIX3 _MRz(double a);
	VECTOR3 Polar2Cartesian(double r, double lat, double lng);
	VECTOR3 Polar2CartesianVel(double r, double lat, double lng, double r_dot, double lat_dot, double lng_dot);
	int decimal_octal(int n);
	void rv_from_r0v0(VECTOR3 R0, VECTOR3 V0, double t, VECTOR3 &R1, VECTOR3 &V1, double mu, double x = 0.0);
	double kepler_U(double dt, double ro, double vro, double a, double mu, double x0);
	double kepler_U_equation(double x, double ro, double vro, double a, double mu);
	double stumpC(double z);
	double stumpS(double z);
	void f_and_g(double x, double t, double ro, double a, double &f, double &g, double mu);
	void fDot_and_gDot(double x, double r, double ro, double a, double &fdot, double &gdot, double mu);
	double atan3(double x, double y);
	double imulimit(double a);
	MATRIX3 tensorp(VECTOR3 u, VECTOR3 v);
	MATRIX3 skew(VECTOR3 u);
	VECTOR3 RotateVector(VECTOR3 yaxis, double angle, VECTOR3 pos);
	double OctToDouble(int oct1, int oct2);
	unsigned long long octal_binary(int oct);
	unsigned long long BinToDec(unsigned long long num);
	double DecToDouble(int dec1, int dec2);
	double round(double number);
	double trunc(double d);
	double quadratic(double *T, double *DV);
	double HHMMSSToSS(int H, int M, int S);
	double HHMMSSToSS(double H, double M, double S);
	void adbar_from_rv(double rmag, double vmag, double rtasc, double decl, double fpav, double az, VECTOR3 &R, VECTOR3 &V);
	void rv_from_adbar(VECTOR3 R, VECTOR3 V, double &rmag, double &vmag, double &rtasc, double &decl, double &fpav, double &az);
	VECTOR3 LMDockedCoarseAlignment(VECTOR3 csmang, bool samerefs);
	VECTOR3 LMIMU_from_CSMIMU(MATRIX3 CSM_REFSMMAT, MATRIX3 LM_REFSMMAT, VECTOR3 csmang);
	MATRIX3 CSMBodyToLMBody(double da);
	VECTOR3 LMDockedFineAlignment(VECTOR3 lmang, VECTOR3 csmang, bool samerefs = true);
	VECTOR3 finealignLMtoCSM(VECTOR3 lmn20, VECTOR3 csmn20, MATRIX3 LM_REFSMMAT, MATRIX3 CSM_REFSMMAT);
	//Earth-Moon-Plane Matrix, converts ecliptic coordinates to EMP coordinates.
	MATRIX3 EMPMatrix(double MJD);
	//Rotation matrix from inertial to LVLH
	MATRIX3 LVLH_Matrix(VECTOR3 R, VECTOR3 V);
	MATRIX3 GetVesselToLocalRotMatrix(MATRIX3 Rot_VG, MATRIX3 Rot_LG);
	MATRIX3 GetVesselToGlobalRotMatrix(MATRIX3 Rot_VL, MATRIX3 Rot_LG);

	double fraction_an(int n);
	double fraction_ad(int n);
	double fraction_a(int n, double x);
	double fraction_b(int n, double x);
	double fraction_delta(int n, double x);
	double fraction_u(int n, double x);
	double fraction_pq(double x);
	double fraction_xi(double x);
	void planeinter(VECTOR3 n1, double h1, VECTOR3 n2, double h2, VECTOR3 &m1, VECTOR3 &m2);
	void CubicInterpolation(double *x, double *y, double *a);
	void VandermondeMatrix(double *x, int N, double **V);
	int LUPDecompose(double **A, int N, double Tol, int *P);
	void LUPSolve(double **A, int *P, double *b, int N, double *x);
	void LinearLeastSquares(std::vector<double> &x, std::vector<double> &y, double &b1, double &b2);
	double Sum(double *x, int N);
	double SumProd(double *x, double *y, int N);
	double SumQuad(double *x, int N);
	double QuadSum(double *x, int N);
}

MATRIX3 operator+(MATRIX3 a, MATRIX3 b);
VECTOR3 rhmul(const MATRIX3 &A, const VECTOR3 &b);
VECTOR3 rhtmul(const MATRIX3 &A, const VECTOR3 &b);
double acos2(double _X);

//void(*)(double*, double, double*)
#endif