#ifndef _ENTRYCALCULATIONS_H
#define _ENTRYCALCULATIONS_H

#include "Orbitersdk.h"
#include "OrbMech.h"

namespace EntryCalculations
{
	void augekugel(double ve, double gammae, double &phie, double &Te);
	void landingsite(VECTOR3 REI, VECTOR3 VEI, double MJD_EI, double &lambda, double &phi);
	void Reentry(VECTOR3 REI, VECTOR3 VEI, double mjd0, bool highspeed, double &EntryLatPred, double &EntryLngPred, double &EntryRTGO, double &EntryVIO, double &EntryRET);
	VECTOR3 ThreeBodyAbort(double t_I, double t_EI, VECTOR3 R_I, VECTOR3 V_I, double mu_E, double mu_M, bool INRFVsign, VECTOR3 &R_EI, VECTOR3 &V_EI, double Incl = 0, bool asc = true);
	void Abort(VECTOR3 R0, VECTOR3 V0, double RCON, double dt, double mu, VECTOR3 &DV, VECTOR3 &R_EI, VECTOR3 &V_EI);
	void Abort_plane(VECTOR3 R0, VECTOR3 V0, double MJD0, double RCON, double dt, double mu, double Incl, bool asc, VECTOR3 &DV, VECTOR3 &R_EI, VECTOR3 &V_EI);
	void time_reentry(VECTOR3 R0, VECTOR3 V0, double r1, double x2, double dt, double mu, VECTOR3 &V, VECTOR3 &R_EI, VECTOR3 &V_EI);
	void time_reentry_plane(VECTOR3 R0, VECTOR3 eta, double r1, double x2, double dt, double mu, VECTOR3 &V, VECTOR3 &R_EI, VECTOR3 &V_EI);
	double landingzonelong(int zone, double lat);

	double MPL(double lat);
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
	double D1, D2, D3, D4;
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

class Entry {
public:
	Entry(VECTOR3 R0B, VECTOR3 V0B, double mjd, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, int critical, bool entrylongmanual);
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
	double D1, D2, D3, D4;
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
};

class Flyby
{
public:
	Flyby(VECTOR3 R0M, VECTOR3 V0M, double mjd0, OBJHANDLE gravref, double MJDguess, double EntryLng, bool entrylongmanual, int returnspeed, int FlybyType, double Inclination = 0.0, bool Ascending = true);
	bool Flybyiter();

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
	OBJHANDLE hMoon, hEarth;
	VECTOR3 DV;
	double DT_TEI_EI;	//Tiem between TEI and EI
	double EntryLng;
	double mu_E, mu_M;
	//double r_s; //Pseudostate sphere
	CELBODY *cMoon;
	double dlngapo, dtapo;
	int ii;
	bool entrylongmanual;
	int landingzone;
	int FlybyType;	//0 = Flyby, 1 = PC+2
	bool INRFVsign;
	double mjd0;
	double dv[3], TIGvar[3];
	double IncDes;
	bool Asc;
};

class TEI
{
public:
	TEI(VECTOR3 R0M, VECTOR3 V0M, double mjd0, OBJHANDLE gravref, double MJDguess, double EntryLng, bool entrylongmanual, int returnspeed, int RevsTillTEI, double Inclination = 0.0, bool Ascending = true);
	bool TEIiter();

	int precision;
	double EntryLatcor, EntryLngcor;
	VECTOR3 Entry_DV;
	VECTOR3 R_EI, V_EI;
	double EIMJD;
	double EntryAng;
	VECTOR3 Rig, Vig, Vig_apo;
	double TIG;
	double ReturnInclination;
private:
	OBJHANDLE hMoon, hEarth;
	VECTOR3 DV;
	double DT_TEI_EI;	//Time between TEI and EI
	double EntryLng;
	double mu_E, mu_M;
	//double r_s; //Pseudostate sphere
	CELBODY *cMoon;
	double dlngapo, dtapo;
	int ii, jj;
	bool entrylongmanual;
	int landingzone;
	bool INRFVsign;
	double dTIG, mjd0;
	double dv[3], TIGvar[3];
	double IncDes;
	bool Asc;
};

#endif