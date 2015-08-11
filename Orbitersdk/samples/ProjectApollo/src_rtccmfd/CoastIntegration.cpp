#include "CoastIntegration.h"

void local_to_equ(VECTOR3 R, double &r, double &phi, double &lambda)
{
	double l, m, n;
	//OBJHANDLE gravref;
	//VESSEL* vessel;
	//VECTOR3 Recl;

	//gravref = oapiGetObjectByName("Earth");
	//vessel = oapiGetFocusInterface();
	//vessel->GetRelativePos(gravref, Recl);

	//oapiLocalToEqu(gravref, _V(R.x,R.z,R.y), &lambda, &phi, &r);

	r = length(R);
	l = R.x / r;
	m = R.y / r;
	n = R.z / r;
	phi = asin(n);
	if (m > 0)
	{
		lambda = acos(l / cos(phi));
	}
	else
	{
		lambda = 2.0 * PI - acos(l / cos(phi));
	}
}

void fabs_vektor(double* vektor, int n)
{
	for (int i = 0; i<n; i++)
	{
		vektor[i] = fabs(vektor[i]);
	}
}

double vektor_max(double* vektor, int n)
{
	double max = vektor[0];
	for (int i = 1; i<n; i++)
	{
		if (max<vektor[n])
		{
			max = vektor[n];
		}
	}
	return max;
}

double power(double b, double e)
{
	double ergebnis;
	if (b<0 && e != ceil(e))
	{
		ergebnis = -pow(-b, e);
	}
	else
	{
		ergebnis = pow(b, e);
	}
	return ergebnis;
}



CoastIntegrator2::CoastIntegrator2(VECTOR3 R1, VECTOR3 V1, double mjd0, double deltat, OBJHANDLE planet)
{
	double K, dt_lim, r, dt_max;

	this->R1 = R1;
	this->V1 = V1;
	this->deltat = mjd0*24.0*3600.0+deltat;
	this->planet = planet;
	this->mu = GGRAV*oapiGetMass(planet);
	this->equ_rad = oapiGetSize(planet);
	this->t = mjd0*24.0*3600.0;

	K = 0.3;
	dt_lim = 400;
	r = length(R1);
	dt_max = min(dt_lim, K*power(r, 1.5) / sqrt(mu));
	dt = 0.1*dt_max;

	y = R1;
	z = V1;
}

bool CoastIntegrator2::iteration()
{
	if (t + dt > deltat)
	{
		dt = deltat - t;
	}
	k1 = f(y, t);
	k2 = f(y + z*dt*0.5 + k1*dt*dt*0.125, t + 0.5*dt);
	k3 = f(y + z*dt + k2*dt*dt*0.5, t + dt);
	phi = z + (k1 + k2*2.0)*dt*1.0 / 6.0;
	psi = (k1 + k2*4.0 + k3)*1.0 / 6.0;
	y += phi*dt;
	z += psi*dt;
	t += dt;
	if (t == deltat)
	{
		R2 = y;
		V2 = z;
		return true;
	}
	else
	{
		return false;
	}
}

bool CoastIntegrator2::iterationnum(int n)
{
	int m = 0;
	bool test = false;
	while (m < n && !test)
	{
		test = iteration();
		m += 1;
	}
	return test;
}

VECTOR3 CoastIntegrator2::f(VECTOR3 R, double t)
{
	return _V(0,0,0);
}
	/*double R_E, r, phi, lambda, a0r, a2r, a2theta, a3r, a4r, a3theta, a4theta;
	int JCoeffCount;
	double *JCoeff;
	VECTOR3 rdach, thetadach, phidach, a_G;
	/*VECTOR3 R_SP, R_PQ, R_QC, a_dQ, R_PS, R_SC, a_dS;
	OBJHANDLE hMoon, hSun, hEarth;
	double *MoonPos, *EarthPos, *SunPos;
	MoonPos = new double[12];
	EarthPos = new double[12];
	SunPos = new double[12];
	hMoon = oapiGetObjectByName("Moon");
	hSun = oapiGetObjectByName("Sun");
	hEarth = oapiGetObjectByName("Earth");
	CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);
	CELBODY *cEarth = oapiGetCelbodyInterface(hEarth);
	CELBODY *cSun = oapiGetCelbodyInterface(hSun);
	int ispolar;
	MATRIX3 Rot;
	VECTOR3 EarthVec;
	double MJD, mu_Q, mu_S, r_QC, r_PQ, q_Q, r_PS, q_S, r_SC;

	R_E = equ_rad;

	JCoeffCount = oapiGetPlanetJCoeffCount(planet);

	JCoeff = new double[JCoeffCount];

	for (int i = 0; i < JCoeffCount; i++)
	{
		JCoeff[i] = oapiGetPlanetJCoeff(planet, i);
	}

	//local_to_equ(R, r, phi, lambda);
	//phi * 180 / pi
	//rdach = R / length(R);
	//phidach = unit(crossp(_V(0, 0, 1), rdach));
	//thetadach = unit(crossp(rdach, phidach));
	/*if (JCoeffCount == 1 || planet == oapiGetObjectByName("Moon"))
	{
		a0r = -mu / (power(r, 2.0));

		a_G = rdach*a0r;
	}
	else if (JCoeffCount == 2)
	{
		a0r = -mu / (power(r, 2.0));
		a2r = -3.0 / 2.0 * mu*power(R_E, 2.0) * JCoeff[0] / (power(r, 4.0))*(3.0 * power(sin(phi), 2.0) - 1.0);
		a2theta = 3.0 * mu*power(R_E, 2.0) * JCoeff[0] / (power(r, 4.0))*sin(phi)*cos(phi);

		a_G = rdach*a0r - (rdach*a2r + thetadach*a2theta);
	}
	else if (JCoeffCount == 3)
	{
		a0r = -mu / (power(r, 2.0));
		a2r = -3.0 / 2.0 * mu*power(R_E, 2.0) * JCoeff[0] / (power(r, 4.0))*(3.0 * power(sin(phi), 2.0) - 1.0);
		a2theta = 3.0 * mu*power(R_E, 2.0) * JCoeff[0] / (power(r, 4.0))*sin(phi)*cos(phi);
		a3r = -2.0 * mu*power(R_E, 3) * JCoeff[1] / power(r, 5.0)*(5.0 * power(sin(phi), 3.0) - 3.0 * sin(phi));
		a3theta = 3.0 / 2.0 * mu*power(R_E, 3) * JCoeff[1] / power(r, 5)*(5.0 * power(sin(phi), 2.0) * cos(phi) - cos(phi));

		a_G = rdach*a0r - (rdach*a2r + thetadach*a2theta + rdach*a3r + thetadach*a3theta);
	}
	else if (JCoeffCount > 3)
	{
		a0r = -mu / (power(r, 2.0));
		a2r = -3.0 / 2.0 * mu*power(R_E, 2.0) * JCoeff[0] / (power(r, 4.0))*(3.0 * power(sin(phi), 2.0) - 1.0);
		a2theta = 3.0 * mu*power(R_E, 2.0) * JCoeff[0] / (power(r, 4.0))*sin(phi)*cos(phi);
		a3r = -2.0 * mu*power(R_E, 3) * JCoeff[1] / power(r, 5.0)*(5.0 * power(sin(phi), 3.0) - 3.0 * sin(phi));
		a3theta = 3.0 / 2.0 * mu*power(R_E, 3) * JCoeff[1] / power(r, 5)*(5.0 * power(sin(phi), 2.0) * cos(phi) - cos(phi));
		a4r = -5.0 / 8.0 * mu*power(R_E, 4) * JCoeff[2] / power(r, 6.0)*(35.0 * power(sin(phi), 4.0) - 30.0 * power(sin(phi), 2.0) + 3.0);
		a4theta = 5.0 / 2.0 * mu*power(R_E, 4.0) * JCoeff[2] / power(r, 6.0)*(7.0 * power(sin(phi), 3.0) * cos(phi) - 3.0 * sin(phi)*cos(phi));

		a_G = rdach*a0r - (rdach*a2r + thetadach*a2theta + rdach*a3r + thetadach*a3theta + rdach*a4r + thetadach*a4theta);
	}
	else
	{
	VECTOR3 U_R, U_Z, adE2,adE3,adE4,adE;
	double costheta, P2, P3, P4, P5;
		U_R = unit(R);
		U_Z = _V(0, 0, 1.0);
		costheta = dotp(U_R, U_Z);
		P2 = 3.0 * costheta;
		P3 = 0.5*(15.0*costheta*costheta - 3.0);
		P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
		P5 = 0.25*(9.0*costheta*P4 - 5.0 * P3);
		adE2 = (U_R*P3*costheta - U_Z*P2*costheta)*JCoeff[0] * power(R_E / r, 2.0);
		adE3 = (U_R*P4*costheta - U_Z*P3*costheta)*JCoeff[1] * power(R_E / r, 3.0);
		adE4 = (U_R*P5*costheta - U_Z*P4*costheta)*JCoeff[2] * power(R_E / r, 4.0);
		adE = (adE2 + adE3 + adE4)*mu / power(r, 2.0);
		a_G = 
	//}

	/*mu_S = GGRAV*oapiGetMass(hSun);
	MJD = t / 86400.0;
	Rot = J2000EclToBRCS(40222.525);

	cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_BARYPOS, MoonPos);
	ispolar = cEarth->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_BARYPOS, EarthPos);
	cSun->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_BARYPOS, SunPos);

	EarthVec = Polar2Cartesian(EarthPos[2] * AU, EarthPos[1], EarthPos[0]);

	if (planet == oapiGetObjectByName("Earth"))
	{
		mu_Q = GGRAV*oapiGetMass(hMoon);

		//R_SQ = _V(MoonPos[0], MoonPos[1], MoonPos[2]) - _V(SunPos[0], SunPos[1], SunPos[2]);
		R_SP = mul(Rot, _V(EarthVec.x, EarthVec.z, EarthVec.y));// -_V(SunPos[0], SunPos[1], SunPos[2]);
		R_PS = -R_SP;
		R_PQ = mul(Rot,_V(MoonPos[0], MoonPos[2], MoonPos[1]));// R_SQ - R_SP;
		r_PQ = length(R_PQ);
		r_PS = length(R_PS);
		R_QC = R - R_PQ;
		R_SC = R - R_PS;
		r_QC = length(R_QC);
		r_SC = length(R_SC);
		q_Q = dotp(R - R_PQ*2.0, R) / power(r_PQ, 2);
		q_S = dotp(R - R_PS*2.0, R) / power(r_PS, 2);

		a_dQ = -(R_PQ*ff(q_Q) + R)*mu_Q / power(r_QC, 3);
		a_dS = -(R_PS*ff(q_S) + R)*mu_S / power(r_SC, 3);
	}
	else
	{
		mu_Q = GGRAV*oapiGetMass(hEarth);

		R_SP = mul(Rot, _V(EarthVec.x, EarthVec.z, EarthVec.y))+mul(Rot,_V(MoonPos[0], MoonPos[2], MoonPos[1]));
		R_PS = -R_SP;
		R_PQ = -mul(Rot, _V(MoonPos[0], MoonPos[2], MoonPos[1]));
		r_PQ = length(R_PQ);
		r_PS = length(R_PS);
		R_QC = R - R_PQ;
		R_SC = R - R_PS;
		r_QC = length(R_QC);
		r_SC = length(R_SC);
		q_Q = dotp(R - R_PQ*2.0, R) / power(r_PQ, 2);
		q_S = dotp(R - R_PS*2.0, R) / power(r_PS, 2);

		a_dQ = -(R_PQ*ff(q_Q) + R)*mu_Q / power(r_QC, 3);
		a_dS = -(R_PS*ff(q_S) + R)*mu_S / power(r_SC, 3);
	}

	return a_G+a_dQ+a_dS;
	return a_G;
}*/

double CoastIntegrator2::ff(double q)
{
	return q*(3.0 + 3.0*q + q*q) / (1.0 + power(1 + q, 1.5));
}

