#ifndef _ENTRYCALCULATIONS_H
#define _ENTRYCALCULATIONS_H

#include "Orbitersdk.h"
#include "OrbMech.h"

class Entry {
public:
	Entry(VESSEL *v, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, int critical, double entryrange, bool entrynominal, bool entrylongmanual);
	Entry(OBJHANDLE gravref);
	void EntryUpdateCalc();
	void Reentry(VECTOR3 REI, VECTOR3 VEI, double mjd0);
	bool EntryIter();
	//bool EntryIter2();
	void augekugel(double ve, double gammae, double &phie, double &Te);
	double MPL(double lat);
	double EPL(double lat);
	double AOL(double lat);
	double IOL(double lat);
	double WPL(double lat);

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
private:
	void coniciter(VECTOR3 R1B, VECTOR3 V1B, double t1, double &theta_long, double &theta_lat, VECTOR3 &V2, double &x, double &dx, double &t21);
	void precisioniter(VECTOR3 R1B, VECTOR3 V1B, double t1, double t21, double &x, double &theta_long, double &theta_lat, VECTOR3 &V2);
	void precomputations(bool x2set, VECTOR3 R1B, VECTOR3 V1B, VECTOR3 &U_R1, VECTOR3 &U_H, double &MA2, double &C_FPA);
	void conicreturn(int f1, VECTOR3 R1B, VECTOR3 V1B, double MA2, double C_FPA, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, double &x, int &n1);
	void conicinit(VECTOR3 R1B, double MA2, double &xmin, double &xmax, double &theta1, double &theta2, double &theta3);
	void xdviterator(VECTOR3 R1B,VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x);
	void xdviterator2(int f1, VECTOR3 R1B, VECTOR3 V1B, double theta1, double theta2, double theta3, VECTOR3 U_R1, VECTOR3 U_H, double dx, double xmin, double xmax, double &x);
	void limitxchange(double theta1, double theta2, double theta3, VECTOR3 V1B, VECTOR3 U_R1, VECTOR3 U_H, double xmin, double xmax, double &x);
	void dvcalc(VECTOR3 V1B, double theta1, double theta2, double theta3, double x, VECTOR3 U_R1, VECTOR3 U_H, VECTOR3 &V2, VECTOR3 &DV, double &p_CON);
	void reentryconstraints(int n1, VECTOR3 R1B, VECTOR3 VEI);
	void newxt2(int n1, double xt2err, double &xt2_apo, double &xt2, double &xt2err_apo);
	void landingsite(VECTOR3 REI, VECTOR3 VEI, double t2, double &lambda, double &phi);
	void finalstatevector(VECTOR3 R1B, VECTOR3 V2, double beta1, double &t21, VECTOR3 &RPRE, VECTOR3 &VPRE);
	void newrcon(int n1, double RD, double rPRE, double R_ERR, double &dRCON, double &rPRE_apo);
	double landingzonelong(int zone, double lat);

	OBJHANDLE gravref, hEarth;
	CoastIntegrator* coast;
	//OrbMech* mech;
	double MA1, C0, C1, C2, C3;
	VESSEL* vessel;
	VECTOR3 R0B, V0B;
	MATRIX3 Rot;
	double mjd;
	double GETbase, get;
	double RCON, RD;
	double mu;
	double Tguess;
	double tigslip;
	int ii;
	double EntryLng;
	int entryphase;
	int critical; //0 = Earth orbit reentry, 1 = MCC calculation, 2 = TLC or TEC abort
	double xapo, dv_err;
	VECTOR3 R11B, V11B;
	int f2;
	double dlngapo,dt0, x2, x2_apo;
	double EMSAlt;
	double D1, D2, D3, D4;
	double k1, k2, k3, k4;
	int revcor;
	double entryrange;
	int rangeiter;
	double phi2;
	double earthorbitangle; //31.7° nominal angled reentry DV vector
	double R_E;
	bool entrynominal; //0 = minimum DV entry, 1 = 31.7° line
	double dt1; //time between estimated maneuver time and actual (currently iterated) maneuver time
	double x, dx, dxmax;
	bool xislimited;
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	bool entrylongmanual;
};

#endif