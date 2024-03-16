#ifndef _ORBMECH_H
#define _ORBMECH_H

#include "Orbitersdk.h"
#include <vector>

#define NUMBEROFGROUNDSTATIONS 19

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
	{ "GOLDSTONE" },
	{ "MADRID" }
};

static const char* gsabbreviations[NUMBEROFGROUNDSTATIONS] = {
	{ "BDA" },
	{ "CYI" },
	{ "CRO" },
	{ "HSK" },
	{ "GYM" },
	{ "HAW" },
	{ "VAN" },
	{ "ASC" },
	{ "GWM" },
	{ "MIL" },
	{ "TEX" },
	{ "GBM" },
	{ "ANG" },
	{ "TAN" },
	{ "MER" },
	{ "HTV" },
	{ "RED" },
	{ "GDS" },
	{ "MAD" }
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
	{ 17.137222*RAD, -61.775833*RAD },
	{ -19.00000*RAD, 47.27556*RAD },
	{ 25.0*RAD, 125.0*RAD },
	{ 25.0*RAD, -136.0*RAD },
	{ -24.0*RAD, -118.0*RAD },
	{ 35.33820*RAD, -116.87421*RAD },
	{ 40.45443*RAD, -4.16990*RAD }
};

#define BODY_EARTH 0
#define BODY_MOON 1

struct SV
{
	VECTOR3 R = _V(0, 0, 0);
	VECTOR3 V = _V(0, 0, 0);
	double MJD = 0.0;
	OBJHANDLE gravref = NULL;
	double mass = 0.0;
};

struct ITERSTATE
{
	int s_F = 0;
	double p_H = 0.0;
	double c_I = 0.0;
	double dv = 0.0;
	double dvo = 0.0;
	double err = 0.0;
	double erro = 0.0;
	bool converged = false;
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

//Classical elements
typedef union
{
	double data[6];
	//a: Semi-major axis
	//e: Eccentricity
	//i: Inclination
	//h: Longitude of the ascending node
	//g: Argument of pericenter
	//l: Mean Anomaly
	struct { 
		double a, e, i, h, g, l;
	};
} CELEMENTS;

struct LGCIgnitionConstants
{
	double v_IGG = 5545.46*0.3048;
	double r_IGXG = -130519.86*0.3048;
	double r_IGZG = -1432597.3*0.3048;
	double K_X = 0.617631;
	double K_Y = 0.755e-6 / 0.3048;
	double K_V = 410.0;
};

class CoastIntegrator
{
public:
	CoastIntegrator(int Epoch, VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, int planet, int outplanet);
	~CoastIntegrator();
	bool iteration(bool allow_stop = true);

	void AdjustTF(double t_f) { t_F = t_f; }

	double GetTime();
	VECTOR3 GetPosition();
	VECTOR3 GetVelocity();
	double GetMJD();
	int GetGravRef();

	VECTOR3 R2, V2;
	int outplanet;
	bool soichange;
private:
	VECTOR3 f(VECTOR3 alpha, VECTOR3 R, VECTOR3 a_d);
	double fq(double q);
	VECTOR3 adfunc(VECTOR3 R);
	void SolarEphemeris(double t, VECTOR3 &R_ES, VECTOR3 &V_ES);

	double R_E, mu;
	double K, dt_lim;
	VECTOR3 R00, V00, R0, V0, R_CON, V_CON, R_QC, R_PQ;
	double t_0, t, tau, t_F, x;
	VECTOR3 delta, nu;
	//0 = earth or lunar orbit, 1 = cislunar-midcourse flight
	int M;
	double r_MP, r_dP;
	double mu_Q;
	double mjd0;
	double rect1, rect2;
	CELBODY *cMoon, *cEarth;
	MATRIX3 Rot; //Rotation matrix from J2000 to BRCS
	VECTOR3 U_Z_E, U_Z_M; //Polar unit vectors in BRCS
	int B;
	//Primary Body, 0 = Earth, 1 = Moon
	int P;
	VECTOR3 R_ES0, V_ES0;
	double W_ES;
	static const double r_SPH;
	bool SunEphemerisInit;
};

namespace OrbMech {

	//Constants
	const double mu_Earth = 0.3986032e15;
	const double R_Earth = 6.373338e6;//6378165.0; We use LC-39 launchpad radius, because Orbiter Earth is spherical
	const double w_Earth = 7.29211514667e-5;
	const double mu_Moon = 0.4902778e13;
	const double R_Moon = 1738090.0;
	const double w_Moon = 2.66169948e-6;
	const double mu_Sun = 0.13271244e21; //Different from GSOP. Guess they hadn't properly figured this out yet.
	const double R_Sun = 6.96e8;
	const double J2_Earth = 1082.6269e-6;
	const double J3_Earth = -2.51e-6;
	const double J4_Earth = -1.60e-6;
	const double J5_Earth = -0.15e-6;
	const double J2_Moon = 207.108e-6;
	const double J3_Moon = -2.1e-5;

	//Earth and Moon rotational parameters
	const double LAN_MJD = 51544.5;				//MJD of the LAN in the "beginning", J2000

	const double L_ref_Earth = 0.0;
	const double e_ref_Earth = 0.0;
	const double e_rel_Earth = 0.4090928023;
	const double T_s_Earth = 86164.098904 / 24.0 / 60.0 / 60.0;		//Sidereal Rotational Period
	const double T_p_Earth = -9413040.4;							//Precession Period
	const double L_0_Earth = 0.00001553343;							//LAN in the "beginning"
	const double phi_0_Earth = 4.894942829;							//Sidereal Rotational Offset

	const MATRIX3 R_ref_Earth = mul(_M(cos(L_ref_Earth), 0, -sin(L_ref_Earth), 0, 1, 0, sin(L_ref_Earth), 0, cos(L_ref_Earth)), _M(1, 0, 0, 0, cos(e_ref_Earth), -sin(e_ref_Earth), 0, sin(e_ref_Earth), cos(e_ref_Earth)));
	const MATRIX3 R_obl_Earth = _M(1, 0, 0, 0, cos(e_rel_Earth), -sin(e_rel_Earth), 0, sin(e_rel_Earth), cos(e_rel_Earth));

	const double L_ref_Moon = 0.4643456618;
	const double e_ref_Moon = 7.259562816e-5;
	const double e_rel_Moon  = 0.026699886264850;
	const double T_s_Moon = 2360588.15 / 24.0 / 60.0 / 60.0;	//Sidereal Rotational Period
	const double T_p_Moon = -6793.468728092782;					//Precession Period
	const double L_0_Moon = 1.71817749;							//LAN in the "beginning"
	const double phi_0_Moon = 4.769465382;						//Sidereal Rotational Offset

	const MATRIX3 R_ref_Moon = mul(_M(cos(L_ref_Moon), 0, -sin(L_ref_Moon), 0, 1, 0, sin(L_ref_Moon), 0, cos(L_ref_Moon)), _M(1, 0, 0, 0, cos(e_ref_Moon), -sin(e_ref_Moon), 0, sin(e_ref_Moon), cos(e_ref_Moon)));
	const MATRIX3 R_obl_Moon = _M(1, 0, 0, 0, cos(e_rel_Moon), -sin(e_rel_Moon), 0, sin(e_rel_Moon), cos(e_rel_Moon));

	void rv_from_r0v0_obla(VECTOR3 R1, VECTOR3 V1, double MJD, double dt, VECTOR3 &R2, VECTOR3 &V2);
	double kepler_E(double e, double M, double error2 = 1.e-8);
	double kepler_H(double e, double M);
	double power(double b, double e);
	void sv_from_coe(OELEMENTS el, double mu, VECTOR3 &R, VECTOR3 &V);
	OELEMENTS coe_from_sv(VECTOR3 R, VECTOR3 V, double mu);
	VECTOR3 elegant_lambert(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double dt, int N, bool prog, double mu);
	void SolveQuartic(double *A, double *R, int &N);
	VECTOR3 Vinti(int Epoch, VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double mjd0, double dt, int N, bool prog, int gravref, int gravin, int gravout, VECTOR3 V_guess, double tol = 0.1);
	double NSRsecant(int Epoch, VECTOR3 RA, VECTOR3 VA, VECTOR3 RP, VECTOR3 VP, double mjd0, double x, double DH, OBJHANDLE gravref);
	void ra_and_dec_from_r(VECTOR3 R, double &ra, double &dec);
	void rv_from_r0v0_ta(VECTOR3 R0, VECTOR3 V0, double dt, VECTOR3 &R1, VECTOR3 &V1, double mu);
	int time_theta(VECTOR3 R1, VECTOR3 V1, double dtheta, double mu, double &dt);
	int time_theta(VECTOR3 R1, VECTOR3 V1, double dtheta, double mu, VECTOR3 &R2, VECTOR3 &V2, double &dt);
	void f_and_g_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &f, double &g, double mu);
	void fDot_and_gDot_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &fdot, double &gdot, double mu);
	void local_to_equ(VECTOR3 R, double &r, double &phi, double &lambda);
	double period(VECTOR3 R, VECTOR3 V, double mu);
	double MeanToTrueAnomaly(double meanAnom, double eccdp, double error2 = 1e-12);
	double TrueToMeanAnomaly(double ta, double eccdp);
	double TrueToEccentricAnomaly(double ta, double ecc);
	void perifocal(double h, double mu, double e, double theta, double inc, double lambda, double w, VECTOR3 &RX, VECTOR3 &VX);
	double fischer_ellipsoid(VECTOR3 R);
	double timetoperi(VECTOR3 R, VECTOR3 V, double mu);
	double timetoperi_integ(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, OBJHANDLE ref_peri);
	double timetoperi_integ(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, OBJHANDLE ref_peri, VECTOR3 &R2, VECTOR3 &V2);
	double timetoapo(VECTOR3 R, VECTOR3 V, double mu, int s = 0);
	double time_radius(VECTOR3 R, VECTOR3 V, double r, double s, double mu);
	double time_radius_integ(int Epoch, VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout);
	double time_radius_integ(int Epoch, VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout, VECTOR3 &RPRE, VECTOR3 &VPRE);
	void ReturnPerigee(int Epoch, VECTOR3 R, VECTOR3 V, double mjd0, OBJHANDLE hMoon, OBJHANDLE hEarth, double phi, double &MJD_peri, VECTOR3 &R_peri, VECTOR3 &V_peri);
	MATRIX3 GetRotationMatrix(int plan, double t);
	MATRIX3 Orbiter2PACSS13(double mjd, double lat, double lng, double azi);
	void PACSS13_from_coe(OELEMENTS coe, double lat, double A_Z, double mu, VECTOR3 &R_S, VECTOR3 &V_S);
	MATRIX3 MSGMatrix(double phi_L, double A_Z);
	MATRIX3 MEGMatrix(double theta_E);
	double GetPlanetCurrentRotation(int plan, double t);
	double findelev(int Epoch, VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double mjd0, double E, OBJHANDLE gravref);
	double findelev_conic(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double E, double mu);
	double findelev_gs(int Epoch, MATRIX3 Rot_J_B, VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_gs, double mjd0, double E, OBJHANDLE gravref, double &range);
	VECTOR3 ULOS(MATRIX3 REFSMMAT, MATRIX3 SMNB, double TA, double SA);
	int FindNearestStar(const VECTOR3 *navstars, VECTOR3 U_LOS, VECTOR3 R_C, double R_E, double ang_max);
	VECTOR3 AOTNavigationBase(double AZ, double EL);
	VECTOR3 AOTULOS(MATRIX3 REFSMMAT, MATRIX3 SMNB, double AZ, double EL);
	bool isnotocculted(VECTOR3 S_SM, VECTOR3 R_C, double R_E, double dist = 5.0*RAD);
	MATRIX3 SBNBMatrix();
	MATRIX3 NBSBMatrix();
	VECTOR3 SXTNB(double TA, double SA);
	VECTOR3 CALCGAR(MATRIX3 REFSM, MATRIX3 SMNB);
	MATRIX3 CALCSMSC(VECTOR3 GA);
	VECTOR3 CALCGTA(MATRIX3 des);
	void CALCCOASA(MATRIX3 SMNB, VECTOR3 S_SM, double &SPA, double &SXP);
	void CALCSXA(MATRIX3 SMNB, VECTOR3 S_SM, double &TA, double &SA);
	MATRIX3 AXISGEN(VECTOR3 s_NBA, VECTOR3 s_NBB, VECTOR3 s_SMA, VECTOR3 s_SMB);
	MATRIX3 ROTCOMP(VECTOR3 U_R, double A);
	VECTOR3 backupgdcalignment(const VECTOR3 *navstars, MATRIX3 REFS, VECTOR3 R_C, double R_E, int &set);
	MATRIX3 AGSStarAlignment(const VECTOR3 *navstars, VECTOR3 Att1, VECTOR3 Att2, int star1, int star2, int axis, int detent, double AOTCounter);
	bool oneclickcoast(int Epoch, VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, VECTOR3 &R1, VECTOR3 &V1, OBJHANDLE gravref, OBJHANDLE &gravout);
	bool oneclickcoast(int Epoch, VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, VECTOR3 &R1, VECTOR3 &V1, int gravref, int &gravout);
	SV coast(int Epoch, SV sv0, double dt);
	void periapo(VECTOR3 R, VECTOR3 V, double mu, double &apo, double &peri);
	void umbra(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v1);
	double sunrise(MATRIX3 Rot_J_B, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, OBJHANDLE planet2, bool rise, bool midnight, bool future = false);
	void orbitmidnight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool night, double &v1);
	bool sight(VECTOR3 R1, VECTOR3 R2, double R_E);
	VECTOR3 AdjustApoapsis(VECTOR3 R, VECTOR3 V, double mu, double r_apo_des);
	VECTOR3 AdjustPeriapsis(VECTOR3 R, VECTOR3 V, double mu, double r_peri_des);
	VECTOR3 CircularOrbitDV(VECTOR3 R, VECTOR3 V, double mu);
	VECTOR3 RotateVelocityVector(VECTOR3 R, VECTOR3 V, double ang);
	double P29TimeOfLongitude(MATRIX3 Rot_J_B, VECTOR3 R0, VECTOR3 V0, double MJD, OBJHANDLE gravref, double phi_d);
	void latlong_from_BRCS(MATRIX3 Rot_J_B, VECTOR3 R, double MJD, OBJHANDLE gravref, double &lat, double &lng);
	void latlong_from_BRCS(MATRIX3 Rot_J_B, VECTOR3 R, double MJD, int RefBody, double &lat, double &lng);
	void latlong_from_r(VECTOR3 R, double &lat, double &lng);
	VECTOR3 r_from_latlong(double lat, double lng);
	VECTOR3 r_from_latlong(double lat, double lng, double r);
	double GETfromMJD(double MJD, double GETBase);
	double MJDfromGET(double GET, double GETBase);
	void format_time_HHMMSS(char *buf, double time);
	void format_time_MMSS(char *buf, double time);
	bool groundstation(MATRIX3 Rot_J_B, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, double lat, double lng, bool rise, double &dt);
	bool gslineofsight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v1);
	int findNextAOS(MATRIX3 Rot_J_B, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet);
	void xaxislambert(VECTOR3 RA1, VECTOR3 VA1, VECTOR3 RP2off, double dt2, int N, bool tgtprograde, double mu, VECTOR3 &VAP2, double &zoff);
	void poweredflight(VECTOR3 R, VECTOR3 V, double mjd0, OBJHANDLE gravref, double f_T, double v_ex, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &m_cutoff, double &t_go);
	//void poweredflight2(VESSEL* vessel, VECTOR3 R, VECTOR3 V, OBJHANDLE gravref, THRUSTER_HANDLE thruster, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &t_go);
	VECTOR3 gravityroutine(VECTOR3 R, OBJHANDLE gravref, double mjd0);
	void impulsive(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_av, double isp, double m, VECTOR3 DV, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff);
	void impulsive(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_av, double isp, double m, VECTOR3 R_ref, VECTOR3 V_ref, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff);
	void impulsive2(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_T, double f_av, double isp, double m, VECTOR3 R_ref, VECTOR3 V_ref, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff);
	void checkstar(const VECTOR3 *navstars, MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &trunnion, double &shaft);
	void coascheckstar(const VECTOR3 *navstars, MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &spa, double &sxp);
	bool AOTcheckstar(VECTOR3 navstar, MATRIX3 REFSMMAT, VECTOR3 IMU, double AZ, double EL);
	void AOTStarAcquisition(VECTOR3 navstar, MATRIX3 REFSMMAT, VECTOR3 IMU, double AZ, double EL, double &YROT, double &SROT);
	bool LMCOASCheckStar(VECTOR3 SI, MATRIX3 RMAT, VECTOR3 IMU, int Axis, double &EL, double &SPX);
	void REVUP(VECTOR3 R, VECTOR3 V, double n, double mu, VECTOR3 &R1, VECTOR3 &V1, double &t);
	void RADUP(VECTOR3 R_W, VECTOR3 V_W, VECTOR3 R_C, double mu, VECTOR3 &R_W1, VECTOR3 &V_W1);
	void COE(VECTOR3 R, VECTOR3 V, double h, double mu, VECTOR3 &R_C, VECTOR3 &V_C);
	void ITER(double &c, int &s, double e, double &p, double &x, double &eo, double &xo, double dx0 = 1.0);
	bool QDRTPI(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double mu, double dh, double E_L, int s, VECTOR3 &R_J, VECTOR3 &V_J);
	bool CSIToDH(VECTOR3 R_A1, VECTOR3 V_A1, VECTOR3 R_P2, VECTOR3 V_P2, double DH, double mu, double &dv);
	VECTOR3 CoellipticDV(VECTOR3 R_A2, VECTOR3 R_PC, VECTOR3 V_PC, double mu);
	VECTOR3 ApplyHorizontalDV(VECTOR3 R, VECTOR3 V, double dv);
	double QuadraticIterator(int &c, int &s, double &varguess, double *var, double *obj, double obj0, double initstep, double maxstep);
	bool SolveSystem(int n, double *A, double *b, double *x, double *det);
	bool SolveSeries(double *x, double *y, int ndata, double *out, int m);

	MATRIX3 inverse(MATRIX3 a);
	double determinant(MATRIX3 a);
	MATRIX3 tmat(MATRIX3 a);
	template <typename T> int sign(T val);
	int DoubleToBuffer(double x, double q, int m);
	void AGCSignedValue(int &val);
	int DoubleToDEDA(double x, double q);
	double cot(double a);
	double sec(double a);
	void fabs_vektor(double* vektor, int n);
	double vektor_max(double* vektor, int n);
	double power(double b, double e);
	double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle);
	void local_to_equ(VECTOR3 R, double &r, double &phi, double &lambda);
	MATRIX3 GetObliquityMatrix(int plan, double t);
	//Year, month, day to Julian Date
	double TJUDAT(int Y, int M, int D);
	double MJDOfNBYEpoch(int epoch);
	MATRIX3 J2000EclToBRCSMJD(double mjd);
	MATRIX3 J2000EclToBRCS(int epoch);
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
	VECTOR3 imulimit(VECTOR3 a);
	double imulimit(double a);
	MATRIX3 tensorp(VECTOR3 u, VECTOR3 v);
	MATRIX3 skew(VECTOR3 u);
	VECTOR3 RotateVector(VECTOR3 yaxis, double angle, VECTOR3 pos);
	double OctToDouble(int oct1, int oct2);
	unsigned long long octal_binary(int oct);
	unsigned long long BinToDec(unsigned long long num);
	double DecToDouble(int dec1, int dec2);
	int SingleToBuffer(double x, int SF, bool TwosComplement = false);
	void DoubleToBuffer(double x, int SF, int &c1, int &c2);
	void TripleToBuffer(double x, int SF, int &c1, int &c2, int &c3);
	double round(double number);
	double trunc(double d);
	void normalizeAngle(double &a, bool positive = true);
	double quadratic(double *T, double *DV);
	void adbar_from_rv(double rmag, double vmag, double rtasc, double decl, double fpav, double az, VECTOR3 &R, VECTOR3 &V);
	void rv_from_adbar(VECTOR3 R, VECTOR3 V, double &rmag, double &vmag, double &rtasc, double &decl, double &fpav, double &az);
	VECTOR3 LMDockedCoarseAlignment(VECTOR3 csmang, bool samerefs);
	VECTOR3 LMIMU_from_CSMIMU(MATRIX3 CSM_REFSMMAT, MATRIX3 LM_REFSMMAT, VECTOR3 csmang);
	MATRIX3 CSMBodyToLMBody(double da);
	VECTOR3 LMDockedFineAlignment(VECTOR3 lmang, VECTOR3 csmang, bool samerefs = true);
	VECTOR3 finealignLMtoCSM(VECTOR3 lmn20, VECTOR3 csmn20, MATRIX3 LM_REFSMMAT, MATRIX3 CSM_REFSMMAT);
	VECTOR3 GimbalAngleConversion(MATRIX3 REFSMMAT1, VECTOR3 GimbalAngles, MATRIX3 REFSMMAT2, bool isCSM);
	//Rotation matrix from inertial to LVLH
	MATRIX3 LVLH_Matrix(VECTOR3 R, VECTOR3 V);
	MATRIX3 GetVesselToLocalRotMatrix(VESSEL *v);
	double GetSemiMajorAxis(VECTOR3 R, VECTOR3 V, double mu);
	double GetMeanMotion(VECTOR3 R, VECTOR3 V, double mu);
	double CMCEMSRangeToGo(MATRIX3 Rot_J_B, VECTOR3 R05G, double MJD05G, double lat, double lng);
	//RTCC EMXING support routine, calculate direction vectors and sine of elevation angle
	void EMXINGElev(VECTOR3 R, VECTOR3 R_S, VECTOR3 &N, VECTOR3 &rho, double &sinang);
	//RTCC EMXING support routine, calculates elevation slope function
	double EMXINGElevSlope(VECTOR3 R, VECTOR3 V, VECTOR3 R_S, int body);

	//AEG
	CELEMENTS LyddaneMeanToOsculating(CELEMENTS arr, int body);
	CELEMENTS LyddaneOsculatingToMean(CELEMENTS arr_osc, int body);
	CELEMENTS KeplerToEquinoctial(CELEMENTS kep);
	CELEMENTS EquinoctialToKepler(CELEMENTS aeq);
	void BrouwerSecularRates(CELEMENTS coe_osc, CELEMENTS coe_mean, int body, double &l_dot, double &g_dot, double &h_dot);
	SV PMMAEGS(int Epoch, SV sv0, int opt, double param, bool &error, double DN = 0.0);
	SV PMMAEG(int Epoch, SV sv0, int opt, double param, bool &error, double DN = 0.0);
	SV PMMLAEG(int Epoch, SV sv0, int opt, double param, bool &error, double DN = 0.0);
	//Inertial to Keplerian Conversion Subroutine
	CELEMENTS GIMIKC(VECTOR3 R, VECTOR3 V, double mu);
	//Keplerian to Inertial Conversion Subroutine
	void GIMKIC(CELEMENTS elem, double mu, VECTOR3 &R, VECTOR3 &V);
	SV PositionMatch(int Epoch, SV sv_A, SV sv_P, double mu);
	//Phase angle determination
	double THETR(double u1, double u2, double i1, double i2, double h1, double h2);
	double PHSANG(VECTOR3 R, VECTOR3 V, VECTOR3 R_D);
	VECTOR3 PROJCT(VECTOR3 U1, VECTOR3 U2, VECTOR3 X);

	double fraction_an(int n);
	double fraction_ad(int n);
	double fraction_a(int n, double x);
	double fraction_b(int n, double x);
	double fraction_delta(int n, double x);
	double fraction_u(int n, double x);
	double fraction_pq(double x);
	double fraction_xi(double x);
	void planeinter(VECTOR3 n1, double h1, VECTOR3 n2, double h2, VECTOR3 &m1, VECTOR3 &m2);
	double LinearInterpolation(double x0, double y0, double x1, double y1, double x);
	void CubicInterpolation(double *x, double *y, double *a);
	void VandermondeMatrix(double *x, int N, double **V);
	int LUPDecompose(double **A, int N, double Tol, int *P);
	void LUPSolve(double **A, int *P, double *b, int N, std::vector<double> &x);
	void LUPInvert(double **A, int *P, int N, double **IA);
	void LinearLeastSquares(std::vector<double> &x, std::vector<double> &y, double &b1, double &b2);
	double Sum(double *x, int N);
	double SumProd(double *x, double *y, int N);
	double SumQuad(double *x, int N);
	double QuadSum(double *x, int N);
	void DROOTS(double A, double B, double C, double D, double E, int N, double *x, int &M, int &I);

	//Time formatting functions
	double HHMMSSToSS(int H, int M, int S);
	double HHMMSSToSS(double H, double M, double S);
	double round_to(double value, double precision = 1.0);
	void SStoHHMMSS(double time, int &hours, int &minutes, double &seconds, double precision = 1.0);
	void format_time(char *buf, double time);
	void format_time_prec(char *buf, double time);
}

inline CELEMENTS operator+(const CELEMENTS &a, const CELEMENTS &b)
{
	CELEMENTS c;
	c.a = a.a + b.a;
	c.e = a.e + b.e;
	c.i = a.i + b.i;
	c.h = a.h + b.h;
	c.g = a.g + b.g;
	c.l = a.l + b.l;
	return c;
}

inline CELEMENTS operator-(const CELEMENTS &a, const CELEMENTS &b)
{
	CELEMENTS c;
	c.a = a.a - b.a;
	c.e = a.e - b.e;
	c.i = a.i - b.i;
	c.h = a.h - b.h;
	c.g = a.g - b.g;
	c.l = a.l - b.l;
	return c;
}

MATRIX3 operator+(MATRIX3 a, MATRIX3 b);
VECTOR3 rhmul(const MATRIX3 &A, const VECTOR3 &b);
VECTOR3 rhtmul(const MATRIX3 &A, const VECTOR3 &b);
MATRIX3 MatrixRH_LH(MATRIX3 A);
double acos2(double _X);
double asin2(double _X);
double factorial(unsigned n);

//void(*)(double*, double, double*)
#endif