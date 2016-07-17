#ifndef _ORBMECH_H
#define _ORBMECH_H

#include "Orbitersdk.h"

const VECTOR3 navstars[37] = { _V(0.874918332, 0.025891699, 0.483577844),
_V(0.934272669, 0.173297383, -0.311612896),
_V(0.477674628, 0.116493556, 0.870779077),
_V(0.491735562, 0.220378448, -0.842395084),
_V(0.013195584, 0.007804379, 0.999882477),
_V(0.545066281, 0.531373849, -0.648494088),
_V(0.703388365, 0.707427419, 0.069218892),
_V(0.410749287, 0.498719061, 0.763259013),
_V(0.350958745, 0.892554545, 0.283150744),
_V(0.201342646, 0.96898881, -0.143254406),
_V(0.137394808, 0.681339885, 0.718956624),
_V(-0.061436077, 0.603170011, -0.795243074),
_V(-0.181895715, 0.940531813, -0.286903917),
_V(-0.411650263, 0.906638731, 0.092467679),
_V(-0.36119376, 0.574807784, -0.734258183),
_V(-0.465616592, 0.477580472, 0.745062468),
_V(-0.774136025, 0.615423403, -0.148214205),
_V(-0.860697047, 0.463812741, 0.209948412),
_V(-0.965624024, 0.052806754, 0.254522476),
_V(-0.952563351, -0.05913135, -0.298540694),
_V(-0.452441663, -0.049270067, -0.890431917),
_V(-0.917106528, -0.350009879, -0.190810644),
_V(-0.58119438, -0.290787715, 0.760036576),
_V(-0.68999017, -0.418081795, -0.590864771),
_V(-0.786255214, -0.52162654, 0.33122272),
_V(-0.532804238, -0.71594486, 0.45115696),
_V(-0.351877285, -0.823996717, -0.444085338),
_V(-0.114790631, -0.33994374, -0.933413823),
_V(-0.112626554, -0.969467959, 0.217823635),
_V(0.121617192, -0.770301475, 0.625975156),
_V(0.206714527, -0.872034942, -0.443648695),
_V(0.45353611, -0.878053717, 0.152730701),
_V(0.551816004, -0.793442209, -0.256804515),
_V(0.320001422, -0.443674048, -0.837109568),
_V(0.454057002, -0.539338315, 0.709187155),
_V(0.81387539, -0.555906349, 0.169041359),
_V(0.834195321, -0.239436257, -0.496778065) 
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
	double h;
	double e;
	double i;
	double RA;
	double w;
	double TA;
};



class CoastIntegrator
{
public:
	CoastIntegrator(VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, OBJHANDLE planet, OBJHANDLE outplanet);
	bool iteration();

	VECTOR3 R2, V2;
	OBJHANDLE outplanet;
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
	int M;
	double r_MP, r_dP, r_SPH;
	OBJHANDLE hEarth, hMoon, hSun, planet;
	double mu_Q, mu_S;
	double mjd0;
	MATRIX3 Rot;
	double rect1, rect2;
	CELBODY *cMoon, *cEarth, *cSun;
	MATRIX3 obli;
	int B, P;
	VECTOR3 R_ES0, V_ES0;
	double W_ES;
};

namespace OrbMech {

	//public:
		//OrbMech(VESSEL *v, OBJHANDLE gravref);

	void rv_from_r0v0_obla(VECTOR3 R1, VECTOR3 V1, double dt, VECTOR3 &R2, VECTOR3 &V2, OBJHANDLE gravref);
	double kepler_E(double e, double M);
	double kepler_H(double e, double M);
	double power(double b, double e);
	void sv_from_coe(OELEMENTS el, double mu, VECTOR3 &R, VECTOR3 &V);
	OELEMENTS coe_from_sv(VECTOR3 R, VECTOR3 V, double mu);
	VECTOR3 elegant_lambert(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double dt, int N, bool prog, double mu);
	VECTOR3 ThreeBodyLambert(double t_I, double t_E, VECTOR3 R_I, VECTOR3 V_init, VECTOR3 R_E, VECTOR3 R_m, VECTOR3 V_m, double r_s, double mu_E, double mu_M, VECTOR3 &R_I_star, VECTOR3 &delta_I_star, VECTOR3 &delta_I_star_dot);
	void INRFV(VECTOR3 R_1, VECTOR3 V_2, double r_2, bool direct, double mu, VECTOR3 &V_1, VECTOR3 &R_2, double &dt_2);
	void SolveQuartic(double *A, double *R, int &N);
	VECTOR3 Vinti(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double mjd0, double dt, int N, bool prog, OBJHANDLE gravref, OBJHANDLE gravin, OBJHANDLE gravout, VECTOR3 V_guess);
	double NSRsecant(VECTOR3 RA, VECTOR3 VA, VECTOR3 RP, VECTOR3 VP, double mjd0, double x, double DH, OBJHANDLE gravref);
	void rv_from_r0v0_ta(VECTOR3 R0, VECTOR3 V0, double dt, VECTOR3 &R1, VECTOR3 &V1, double mu);
	double time_theta(VECTOR3 R, VECTOR3 V, double dtheta, double mu);
	void f_and_g_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &f, double &g, double mu);
	void fDot_and_gDot_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &fdot, double &gdot, double mu);
	void local_to_equ(VECTOR3 R, double &r, double &phi, double &lambda);
	double period(VECTOR3 R, VECTOR3 V, double mu);
	void perifocal(double h, double mu, double e, double theta, double inc, double lambda, double w, VECTOR3 &RX, VECTOR3 &VX);
	double fischer_ellipsoid(VECTOR3 R);
	double timetoperi(VECTOR3 R, VECTOR3 V, double mu);
	double time_radius(VECTOR3 R, VECTOR3 V, double r, double s, double mu);
	double time_radius_integ(VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout, VECTOR3 &RPRE, VECTOR3 &VPRE);
	MATRIX3 GetRotationMatrix(OBJHANDLE plan, double t);
	MATRIX3 GetRotationMatrix2(OBJHANDLE plan, double t);
	MATRIX3 Orbiter2PACSS13(double mjd, double lat, double lng, double azi);
	void PACSS4_from_coe(OELEMENTS coe, double mu, VECTOR3 &R, VECTOR3 &V);
	OELEMENTS coe_from_PACSS4(VECTOR3 R, VECTOR3 V, double mu);
	double GetPlanetCurrentRotation(OBJHANDLE plan, double t);
	double findelev(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double mjd0, double E, OBJHANDLE gravref);
	double findelev_gs(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_gs, double mjd0, double E, OBJHANDLE gravref, double &range);
	VECTOR3 ULOS(MATRIX3 REFSMMAT, MATRIX3 SMNB, double TA, double SA);
	int FindNearestStar(VECTOR3 U_LOS, VECTOR3 R_C, double R_E, double ang_max);
	bool isnotocculted(VECTOR3 S_SM, VECTOR3 R_C, double R_E);
	VECTOR3 CALCGAR(MATRIX3 REFSM, MATRIX3 SMNB);
	MATRIX3 CALCSMSC(VECTOR3 GA);
	VECTOR3 CALCSGTA(MATRIX3 des);
	void CALCCOASA(MATRIX3 SMNB, VECTOR3 S_SM, double &SPA, double &SXP);
	void CALCSXA(MATRIX3 SMNB, VECTOR3 S_SM, double &TA, double &SA);
	VECTOR3 backupgdcalignment(MATRIX3 REFS, VECTOR3 R_C, double R_E, int &set);
	//void rungeinteg(VECTOR3 R0, VECTOR3 V0, double dt, VECTOR3 &R1, VECTOR3 &V1, double mu);
	//void adfunc(double* dfdt, double t, double* f);
	//int rkf45(double*, double**, double*, double*, int, double tol = 1e-15);
	void oneclickcoast(VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, VECTOR3 &R1, VECTOR3 &V1, OBJHANDLE gravref, OBJHANDLE &gravout);
	void periapo(VECTOR3 R, VECTOR3 V, double mu, double &apo, double &peri);
	void umbra(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v1);
	double sunrise(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, OBJHANDLE planet2, bool rise, bool midnight, bool future);
	void orbitmidnight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool night, double &v1);
	bool sight(VECTOR3 R1, VECTOR3 R2, double R_E);
	double P29TimeOfLongitude(VECTOR3 R0, VECTOR3 V0, double MJD, OBJHANDLE gravref, double phi_d);
	void latlong_from_BRCS(VECTOR3 R, double MJD, OBJHANDLE gravref, double &lat, double &lng);
	void latlong_from_r(VECTOR3 R, double &lat, double &lng);
	double findlatitude(VECTOR3 R, VECTOR3 V, double mjd, OBJHANDLE gravref, double lat, bool up, VECTOR3 &Rlat, VECTOR3 &Vlat);
	bool groundstation(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, double lat, double lng, bool rise, double &dt);
	bool gslineofsight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v1);
	int findNextAOS(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet);
	bool vesselinLOS(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet);
	MATRIX3 LaunchREFSMMAT(double lat, double lng, double mjd, double A_Z);
	VECTOR3 DOI_calc(VECTOR3 R, VECTOR3 V, double r_LS, double h_p, double mu);
	void LunarLandingPrediction(VECTOR3 R_D, VECTOR3 V_D, double t_D, double t_E, VECTOR3 R_LSA, double h_DP, double theta_F, double t_F, OBJHANDLE plan, double GETbase, double mu, double &t_DOI, double &t_PDI, double &t_L, VECTOR3 &DV_DOI, double &CR);
	void xaxislambert(VECTOR3 RA1, VECTOR3 VA1, VECTOR3 RP2off, double dt2, int N, bool tgtprograde, double mu, VECTOR3 &VAP2, double &zoff);
	void poweredflight(VECTOR3 R, VECTOR3 V, OBJHANDLE gravref, double f_T, double v_ex, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &t_go);
	//void poweredflight2(VESSEL* vessel, VECTOR3 R, VECTOR3 V, OBJHANDLE gravref, THRUSTER_HANDLE thruster, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &t_go);
	VECTOR3 gravityroutine(VECTOR3 R, OBJHANDLE gravref);
	void impulsive(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_T, double isp, double m, VECTOR3 DV, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff);
	void checkstar(MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &trunnion, double &shaft);
	void coascheckstar(MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &spa, double &sxp);
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
	MATRIX3 transpose_matrix(MATRIX3 a);
	template <typename T> int sign(T val);
	int DoubleToBuffer(double x, double q, int m);
	double cot(double a);
	void fabs_vektor(double* vektor, int n);
	double vektor_max(double* vektor, int n);
	double power(double b, double e);
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
	double stumpC(double z);
	double stumpS(double z);
	void f_and_g(double x, double t, double ro, double a, double &f, double &g, double mu);
	void fDot_and_gDot(double x, double r, double ro, double a, double &fdot, double &gdot, double mu);
	double atan3(double x, double y);
	double imulimit(double a);
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

	double fraction_an(int n);
	double fraction_ad(int n);
	double fraction_a(int n, double x);
	double fraction_b(int n, double x);
	double fraction_delta(int n, double x);
	double fraction_u(int n, double x);
	double fraction_pq(double x);
	double fraction_xi(double x);
	void planeinter(VECTOR3 n1, double h1, VECTOR3 n2, double h2, VECTOR3 &m1, VECTOR3 &m2);
}

MATRIX3 operator+(MATRIX3 a, MATRIX3 b);

//void(*)(double*, double, double*)
#endif