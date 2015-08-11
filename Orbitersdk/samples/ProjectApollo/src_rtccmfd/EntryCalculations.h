#ifndef _ENTRYCALCULATIONS_H
#define _ENTRYCALCULATIONS_H

#include "Orbitersdk.h"
#include "OrbMech.h"

class Entry {
public:
	Entry(VESSEL *v, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, int critical);
	Entry(OBJHANDLE gravref);
	void EntryUpdateCalc();
	void Reentry(VECTOR3 REI, VECTOR3 VEI, double mjd0);
	bool EntryIter();
	void augekugel(double ve, double gammae, double &phie, double &Te);

	double EntryTIGcor;
	double EntryLngcor;
	double EntryLatcor;
	VECTOR3 Entry_DV;
	double EntryLatPred, EntryLngPred;
	double EntryRET, EntryRTGO, EntryVIO;
	double V400k, gamma400k;
	double EntryAng;
	double t2;
private:
	void xdviterator();
	void limitxchange();
	void dvcalc();
	void reentryconstraints();

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
	int critical;
	double p_CON, theta1, theta2, theta3, x, xapo, dv, xmin, xmax, dxmax, beta8, dx, dvapo, beta9, dv_err;
	VECTOR3 V2, U_H, U_R1, R1B, V1B, DV, R11B, V11B;
	int f2;
	double dlngapo,dt0,x2;
	double EMSAlt;
	double D1, D2, D3, D4;
	double k1, k2, k3, k4;
	int n1;
	VECTOR3 VEI;
	double x2_apo;
	int revcor;
};

#endif