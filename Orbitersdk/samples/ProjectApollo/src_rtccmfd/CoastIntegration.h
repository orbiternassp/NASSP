#ifndef _COASTINGINTEGRATION_H
#define _COASTINGINTEGRATION_H

#include "Orbitersdk.h"
//#include "OrbMech.h"



class CoastIntegrator2
{
public:
	CoastIntegrator2(VECTOR3 R1, VECTOR3 V1, double mjd0, double dt, OBJHANDLE planet);
	bool iteration();
	bool iterationnum(int n);
	VECTOR3 R2, V2;
private:
	VECTOR3 f(VECTOR3 R, double t);
	double ff(double q);

	VECTOR3 R1, V1, k1, k2, k3, phi, psi, y, z;
	double deltat, mu, t, dt;
	OBJHANDLE planet;
	double equ_rad;
};



#endif