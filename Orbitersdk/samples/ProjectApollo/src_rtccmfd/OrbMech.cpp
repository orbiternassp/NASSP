#include "OrbMech.h"
#include <limits>

inline double acosh(double z) { return log(z + sqrt(z + 1.0)*sqrt(z - 1.0)); }
inline double atanh(double z){ return 0.5*log(1.0 + z) - 0.5*log(1.0 - z); }

namespace OrbMech{

	double period(VECTOR3 R, VECTOR3 V, double mu)
	{
		double a, epsilon;

		epsilon = power(length(V), 2.0) / 2.0 - mu / length(R);
		a = -mu / (2.0*epsilon);
		return PI2*sqrt(power(a, 3.0) / mu);
	}

	double fraction_an(int n)
	{
		if (n == 0)
		{
			return 4.0;
		}
		else
		{
			return fraction_an(n - 1) + (2.0*n + 1.0);
		}
	}

	double fraction_ad(int n)
	{
		if (n == 0)
		{
			return 15.0;
		}
		else
		{
			return fraction_ad(n - 1) + 4.0*(2.0*n + 1.0);
		}
	}

	double fraction_a(int n, double x)
	{
		if (n == 0)
		{
			return 1.0 / 5.0;
		}
		else
		{
			return -fraction_an(n) / fraction_ad(n)*x;
		}
	}

	double fraction_b(int n, double x)
	{
		return 1.0;
	}

	double fraction_delta(int n, double x)
	{
		if (n == 0)
		{
			return 1.0;
		}
		else
		{
			return 1.0 / (1.0 - fraction_a(n, x)*fraction_delta(n - 1, x));
		}
	}

	double fraction_u(int n, double x)
	{
		if (n == 0)
		{
			return 1.0 / 5.0;
		}
		else
		{
			return fraction_u(n - 1, x)*(fraction_delta(n, x) - 1.0);
		}
	}

	double fraction_pq(double x)
	{
		int n = 0;
		double u = 100.0;
		double pq = 0.0;

		while (abs(u) > 1e-9)
		{
			u = fraction_u(n, x);
			pq += u;
			n++;
		}
		return pq;
	}

	double fraction_xi(double x)
	{
		double eta, xi_eta, xi_x;

		eta = (sqrt(1.0 + x) - 1.0) / (sqrt(1.0 + x) + 1.0);
		xi_eta = fraction_pq(eta);
		xi_x = 1.0 / (8.0*(sqrt(1.0 + x) + 1.0))*(3.0 + xi_eta / (1.0 + eta*xi_eta));
		return xi_x;
	}

	double HHMMSSToSS(int H, int M, int S)
	{
		return (double)H*3600.0 + (double)M*60.0 + (double)S;
	}

	double HHMMSSToSS(double H, double M, double S)
	{
		return H*3600.0 + M*60.0 + S;
	}

	void adbar_from_rv(double rmag, double vmag, double rtasc, double decl, double fpav, double az, VECTOR3 &R, VECTOR3 &V)
	{
		R = _V(cos(decl)*cos(rtasc), cos(decl)*sin(rtasc), sin(decl))*rmag;
		V.x = vmag*(cos(rtasc)*(-cos(az)*sin(fpav)*sin(decl) + cos(fpav)*cos(decl)) - sin(az)*sin(fpav)*sin(rtasc));
		V.y = vmag*(sin(rtasc)*(-cos(az)*sin(fpav)*sin(decl) + cos(fpav)*cos(decl)) + sin(az)*sin(fpav)*cos(rtasc));
		V.z = vmag*(cos(az)*cos(decl)*sin(fpav) + cos(fpav)*sin(decl));
	}

void perifocal(double h, double mu, double e, double theta, double inc, double lambda, double w, VECTOR3 &RX, VECTOR3 &VX)	//Creates a velocity vector from orbital elements
{
	//INPUTS:
	//h: specific relative angular momentum of the orbit
	// mu: gravitational parameter of the celestial body
	//e: orbital eccentricity
	//theta: true anomaly
	//inc: equatorial inclination
	//lambda: Longitude of the Ascending Node
	//w: Argument of Periapsis

	//OUTPUT:
	//VX: Velocity vector in the equatorial geocentric coordinate system


	VECTOR3 Rx, Vx;
	MATRIX3 Q_xX;

	Rx = _V(cos(theta),sin(theta),0.0)*h*h / mu*1.0 / (1.0 + e*cos(theta));
	Vx = _V(-sin(theta), e + cos(theta), 0.0)*(mu / h);
	Q_xX = _M(-sin(lambda)*cos(inc)*sin(w) + cos(lambda)*cos(w), -sin(lambda)*cos(inc)*cos(w) - cos(lambda)*sin(w), sin(lambda)*sin(inc), cos(lambda)*cos(inc)*sin(w) + sin(lambda)*cos(w), cos(lambda)*cos(inc)*cos(w) - sin(lambda)*sin(w), -cos(lambda)*sin(inc), sin(inc)*sin(w), sin(inc)*cos(w), cos(inc));
	RX = mul(Q_xX, Rx);
	VX = mul(Q_xX, Vx);
}

double fischer_ellipsoid(VECTOR3 R)		//Used in the AGC to calculate the radius of the ellipsoid Earth. Not used for the calculated altitude in P30?
{
	double r, phi, lambda, a, b, sinl;
	local_to_equ(R, r, phi, lambda);				//Calculates the radius, latitude and longitude from the position vector
	a = 6378166.0;									//Semi-major axis of the ellipsoid (not of an orbit!)
	b = 6356784.0;									//Semi-minor axis of the ellipsoid (not of an orbit!)
	sinl = sin(phi);								//Saves one sinus calculation
	return sqrt(b*b / (1 - (1 - b*b / a / a)*(1 - sinl*sinl)));	//Calculates the radius dependent on the latitude
}


/*OrbMech::OrbMech(VESSEL *v, OBJHANDLE gravref)
{
	vessel = v;
	mu = GGRAV*oapiGetMass(gravref);
	this->gravref = gravref;
	this->JCoeffCount = oapiGetPlanetJCoeffCount(gravref);
	this->JCoeff = new double[JCoeffCount];
	for (int i = 0; i < JCoeffCount; i++)
	{
		JCoeff[i] = oapiGetPlanetJCoeff(gravref, i);
	}
	this->R_b = oapiGetSize(gravref);
}*/

void rv_from_r0v0_ta(VECTOR3 R0, VECTOR3 V0, double dt, VECTOR3 &R1, VECTOR3 &V1, double mu)
{
	double f, g, fdot, gdot;

	f_and_g_ta(R0, V0, dt, f, g, mu);
	fDot_and_gDot_ta(R0, V0, dt, fdot, gdot, mu);

	R1 = R0*f + V0*g;
	V1 = R0*fdot + V0*gdot;
}

void f_and_g_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &f, double &g, double mu)
{
	double h, vr0, r0, s, c, r;

	h = length(crossp(R0, V0));
	vr0 = dotp(V0, R0) / length(R0);
	r0 = length(R0);
	s = sin(dt);
	c = cos(dt);
	r = h*h / mu / (1 + (h*h / mu / r0 - 1)*c - h*vr0*s / mu);
	f = 1 - mu*r*(1 - c) / (h*h);
	g = r*r0*s / h;
}

void fDot_and_gDot_ta(VECTOR3 R0, VECTOR3 V0, double dt, double &fdot, double &gdot, double mu)
{
	double h, vr0, r0, c, s;
	h = length(crossp(R0, V0));
	vr0 = dotp(V0, R0) / length(R0);
	r0 = length(R0);
	c = cos(dt);
	s = sin(dt);
	fdot = mu / h*(vr0 / h*(1 - c) - s / r0);
	gdot = 1 - mu*r0 / (h*h) * (1 - c);
}

double time_theta(VECTOR3 R, VECTOR3 V, double dtheta, double mu)
{
	double r, v, alpha, a, f, g, fdot, gdot, sigma0, r1, dt, h, p;

	//double h, v_r, cotg, x, p;

	r = length(R);
	v = length(V);
	alpha = 2.0 / r - v*v / mu;
	a = 1.0 / alpha;
	f_and_g_ta(R, V, dtheta, f, g, mu);
	fDot_and_gDot_ta(R, V, dtheta, fdot, gdot, mu);
	sigma0 = dotp(R, V) / sqrt(mu);

	h = length(crossp(R, V));
	p = h*h / mu;

	r1 = r*p / (r + (p - r)*cos(dtheta) - sqrt(p)*sigma0*sin(dtheta));

	if (alpha > 0)
	{
		double dE, cos_dE, sin_dE;

		cos_dE = 1.0 - r / a*(1.0 - f);
		sin_dE = -r*r1*fdot / sqrt(mu*a);
		dE = atan2(sin_dE, cos_dE);
		
		dt = g + sqrt(power(a, 3.0) / mu)*(dE - sin_dE);
	}
	else if (alpha == 0.0)
	{
		double c, s;

		c = sqrt(r*r + r1*r1 - 2 * r*r1*cos(dtheta));
		s = (r + r1 + c) / 2.0;

		dt =  2.0 / 3.0*sqrt(power(s, 3.0) / 2.0 / mu)*(1.0 - power((s - c) / s, 3.0 / 2.0));
	}
	else
	{
		double dH;

		dH = acosh(1.0 - r / a*(1.0 - f));

		dt = g + sqrt(power(-a, 3.0) / mu)*(sinh(dH) - dH);
	}

	return dt;
}

void rv_from_r0v0(VECTOR3 R0, VECTOR3 V0, double t, VECTOR3 &R1, VECTOR3 &V1, double mu, double x)	//computes the state vector (R,V) from the initial state vector (R0,V0) and the elapsed time
{
	double r0, v0, vr0, alpha, f, g, fdot, gdot, r, xx, paratol, x0;

	//If absolute value of alpha is smaller than paratol, then the parabolic initial guess is used
	paratol = 0.00000001;

	r0 = length(R0);
	v0 = length(V0);
	vr0 = dotp(R0, V0) / r0;
	alpha = 2.0 / r0 - v0*v0 / mu;

	//Initial guess supplied by the calling function
	if (x != 0.0)
	{
		x0 = x;
	}
	//Calculate initial guess
	else
	{
		//Initial guess for elliptical and hyperbolic orbits (and nonsensical orbits)
		if (abs(alpha) > paratol || v0 == 0.0)
		{
			x0 = sqrt(mu)*abs(alpha)*t;
		}
		//Initial guess for (near) parabolic orbits
		else
		{
			VECTOR3 H = crossp(R0, V0);
			double hmag = length(H);
			double p = hmag*hmag / mu;
			double s = 0.5  * (PI05 - atan(3.0 *sqrt(mu / (p*p*p))* t));
			double w = atan(power(tan(s), 1.0 / 3.0));
			x0 = sqrt(p) * (2.0 *cot(2.0 *w));
		}
	}

	xx = kepler_U(t, r0, vr0, alpha, mu, x0);
	f_and_g(xx, t, r0, alpha, f, g, mu);
	R1 = R0*f + V0*g;
	r = length(R1);
	fDot_and_gDot(xx, r, r0, alpha, fdot, gdot, mu);
	V1 = R0*fdot + V0*gdot;
}

double kepler_U(double dt, double ro, double vro, double a, double mu, double x0) //This function uses Newton's method to solve the universal Kepler equation for the universal anomaly.
{
	double error2, ratio, C, S, F, dFdx, x;
	int n, nMax;

	error2 = 1e-8;
	nMax = 1000;
	n = 0;
	ratio = 1;
	C = 0;
	S = 0;
	F = 0;
	dFdx = 0;
	x = x0;
	while ((abs(ratio) > error2) && (n <= nMax)) {
		n = n + 1;
		C = stumpC(a*x*x);
		S = stumpS(a*x*x);
		F = ro*vro / sqrt(mu)*x*x*C + (1.0 - a*ro)*OrbMech::power(x, 3.0)*S + ro*x - sqrt(mu)*dt;
		dFdx = ro*vro / sqrt(mu)*x*(1.0 - a*x*x*S) + (1.0 - a*ro)*x*x*C + ro;
		ratio = F / dFdx;
		x = x - ratio;
	}
	return x;
}

double stumpS(double z)
{
	double s;

	s = 0;
	if (z > 0) {
		s = (sqrt(z) - sin(sqrt(z))) / OrbMech::power(sqrt(z), 3);
	}
	else if (z < 0) {
		s = (sinh(sqrt(-z)) - sqrt(-z)) / OrbMech::power(sqrt(-z), 3);
	}
	else {
		s = 1.0 / 6.0;
	}
	return s;
}

double stumpC(double z)
{
	double c;
	c = 0;
	if (z > 0) {
		c = (1.0 - cos(sqrt(z))) / z;
	}
	else if (z < 0) {
		c = (cosh(sqrt(-z)) - 1.0) / (-z);
	}
	else {
		c = 0.5;
	}
	return c;
}

double power(double b, double e)
{
	double res;

	res = 0;
	if (b < 0 && e != ceil(e)) {
		res = -pow(-b, e);
	}
	else {
		res = pow(b, e);
	}
	return res;
}

void f_and_g(double x, double t, double ro, double a, double &f, double &g, double mu)	//calculates the Lagrange f and g coefficients
{
	double z;

	z = a*x*x;
	f = 1 - x*x / ro*stumpC(z);
	g = t - 1 / sqrt(mu)*OrbMech::power(x, 3)*stumpS(z);
}

void fDot_and_gDot(double x, double r, double ro, double a, double &fdot, double &gdot, double mu)	//calculates the time derivatives of the Lagrange f and g coefficients
{
	double z;
	z = a*x*x;
	fdot = sqrt(mu) / r / ro*(z*stumpS(z) - 1.0)*x;
	gdot = 1.0 - x*x / r*stumpC(z);
}

double kepler_E(double e, double M)
{
	double error2, ratio, E;
	//{
	//	This function uses Newton's method to solve Kepler's
	//		equation E - e*sin(E) = M for the eccentric anomaly,
	//		given the eccentricity and the mean anomaly.
	//		E - eccentric anomaly(radians)
	//		e - eccentricity, passed from the calling program
	//		M - mean anomaly(radians), passed from the calling program
	//}
	// ----------------------------------------------

	error2 = 1.e-8;
	ratio = 1.0;

	if (M < PI)
	{
		E = M + e / 2.0;
	}
	else
	{
		E = M - e / 2.0;
	}

	while (abs(ratio) > error2)
	{
		ratio = (E - e*sin(E) - M) / (1.0 - e*cos(E));
		E = E - ratio;
	}
	return E;
} //kepler_E

double kepler_H(double e, double M)
{
	double error2, F, ratio;
	//{
	//This function uses Newton's method to solve Kepler's equation
	//	for the hyperbola e*sinh(F) - F = M for the hyperbolic
	//		eccentric anomaly, given the eccentricity and the hyperbolic
	//		mean anomaly.
	//		F - hyperbolic eccentric anomaly(radians)
	//		e - eccentricity, passed from the calling program
	//		M - hyperbolic mean anomaly(radians), passed from the
	//		calling program
	//		User M - functions required : none
	//		%}
	// ----------------------------------------------
	//...Set an error tolerance :
	error2 = 1.0e-8;
	//...Starting value for F:
	F = M;
	ratio = 1.0;

	while (abs(ratio) > error2)
	{
		ratio = (e*sinh(F) - F - M) / (e*cosh(F) - 1.0);
		F = F - ratio;
	}
	return F;
}

void rv_from_r0v0_obla(VECTOR3 R1, VECTOR3 V1, double MJD, double dt, VECTOR3 &R2, VECTOR3 &V2, OBJHANDLE gravref)
{
	OELEMENTS coe, coe2;
	MATRIX3 Rot;
	VECTOR3 R1_equ, V1_equ, R2_equ, V2_equ;
	double h, e, Omega_0, i, omega_0, theta0, a, T, n, E_0, t_0, t_f, n_p, t_n, M_n, E_n, theta_n, Omega_dot, omega_dot, Omega_n, omega_n,mu,JCoeff;

	mu = GGRAV*oapiGetMass(gravref);

	if (oapiGetPlanetJCoeffCount(gravref) > 0)
	{
		JCoeff = oapiGetPlanetJCoeff(gravref, 0);
	}

	Rot = GetObliquityMatrix(gravref, MJD);

	R1_equ = rhtmul(Rot, R1);
	V1_equ = rhtmul(Rot, V1);

	coe = coe_from_sv(R1_equ, V1_equ, mu);
	h = coe.h;
	e = coe.e;
	Omega_0 = coe.RA;
	i = coe.i;
	omega_0 = coe.w;
	theta0 = coe.TA;

	a = h*h / mu * 1.0 / (1.0 - e*e);
	T = 2.0 * PI / sqrt(mu)*OrbMech::power(a, 3.0 / 2.0);
	n = 2.0 * PI / T;
	E_0 = 2.0 * atan(sqrt((1.0 - e) / (1.0 + e))*tan(theta0 / 2.0));
	t_0 = (E_0 - e*sin(E_0)) / n;
	t_f = t_0 + dt;
	n_p = t_f / T;
	t_n = (n_p - floor(n_p))*T;
	M_n = n*t_n;
	E_n = kepler_E(e, M_n);
	theta_n = 2.0 * atan(sqrt((1.0 + e) / (1.0 - e))*tan(E_n / 2.0));
	if (theta_n < 0)
	{
		theta_n += 2 * PI;
	}

	Omega_dot = -(3.0 / 2.0 * sqrt(mu)*JCoeff * OrbMech::power(oapiGetSize(gravref), 2.0) / (OrbMech::power(1.0 - OrbMech::power(e, 2.0), 2.0) * OrbMech::power(a, 7.0 / 2.0)))*cos(i);
	omega_dot = -(3.0 / 2.0 * sqrt(mu)*JCoeff * OrbMech::power(oapiGetSize(gravref), 2.0) / (OrbMech::power(1.0 - OrbMech::power(e, 2.0), 2.0) * OrbMech::power(a, 7.0 / 2.0)))*(5.0 / 2.0 * sin(i)*sin(i) - 2.0);

	Omega_n = Omega_0 + Omega_dot*dt;
	omega_n = omega_0 + omega_dot*dt;

	coe2.h = h;
	coe2.e = e;
	coe2.RA = Omega_n;
	coe2.i = i;
	coe2.w = omega_n;
	coe2.TA = theta_n;

	sv_from_coe(coe2, mu, R2_equ, V2_equ);

	R2 = rhmul(Rot, R2_equ);
	V2 = rhmul(Rot, V2_equ);
}

void sv_from_coe(OELEMENTS el, double mu, VECTOR3 &R, VECTOR3 &V)	//computes the state vector(R, V) from the classical orbital elements
{
	double e, h, RA, incl, TA, w, rp_factor, vp_factor;
	VECTOR3 rp, vp;
	MATRIX3 R3_W, R1_i, R3_w, R2, Q_Xp;

	e = el.e;
	//a = el.a;
	//r_p = (1 - e)*a;
	h = el.h;// sqrt(r_p*mu*(1 + e));
	RA = el.RA;
	incl = el.i;
	TA = el.TA;
	w = el.w;// el.omegab - el.theta;
	rp_factor = h*h / (mu*(1 + e*cos(TA)));
	rp.x = rp_factor*cos(TA);
	rp.y = rp_factor*sin(TA);
	rp.z = 0;
	vp_factor = mu / h;
	vp.x = -vp_factor*sin(TA);
	vp.y = vp_factor*(e + cos(TA));
	vp.z = 0;
	R3_W = _M(cos(RA), sin(RA), 0, -sin(RA), cos(RA), 0, 0, 0, 1);
	R1_i = _M(1, 0, 0, 0, cos(incl), sin(incl), 0, -sin(incl), cos(incl));
	R3_w = _M(cos(w), sin(w), 0, -sin(w), cos(w), 0, 0, 0, 1);
	R2 = mul(R3_w, R1_i);
	Q_Xp = mul(R2, R3_W);
	R = tmul(Q_Xp, rp);
	V = tmul(Q_Xp, vp);
}

OELEMENTS coe_from_sv(VECTOR3 R, VECTOR3 V, double mu)
{
	/*% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	%{
	% This function computes the classical orbital elements(coe)
	% from the state vector(R, V) using Algorithm 4.1.
	%
	mu - gravitational parameter(km ^ 3 / s ^ 2)
	R - position vector in the geocentric equatorial frame(km)
	V - velocity vector in the geocentric equatorial frame(km)
	r, v - the magnitudes of R and V
	vr - radial velocity component(km / s)
	H - the angular momentum vector(km ^ 2 / s)
	h - the magnitude of H(km ^ 2 / s)
	incl - inclination of the orbit(rad)
	N - the node line vector(km ^ 2 / s)
	n - the magnitude of N
	cp - cross product of N and R
	RA - right ascension of the ascending node(rad)
	E - eccentricity vector
	e - eccentricity(magnitude of E)
	eps - a small number below which the eccentricity is considered
	to be zero
	w - argument of perigee(rad)
	TA - true anomaly(rad)
	a - semimajor axis(km)
	pi - 3.1415926...
	coe - vector of orbital elements[h e RA incl w TA a]
	User M - functions required : None
	%}
	% -------------------------------------------- -*/
	double eps, r, v, vr, h, incl, n, RA, e, w, TA;
	VECTOR3 H, N, E, cp;
	OELEMENTS coe;

	eps = 1.e-10;
	r = length(R);
	v = length(V);
	vr = dotp(R, V) / r;
	H = crossp(R, V);
	h = length(H);
	incl = acos(H.z / h);
	N = crossp(_V(0, 0, 1), H);
	n = length(N);
	if (n != 0)
	{
		RA = acos(N.x / n);
		if (N.y < 0)
		{
			RA = PI2 - RA;
		}
	}
	else
	{
		RA = 0;
	}

	E = (R*(OrbMech::power(v, 2) - mu / r) - V*r*vr) * 1 / mu;
	e = length(E);

	if (n != 0)
	{
		if (e > eps)
		{
			w = acos(dotp(N, E) / n / e);
			if (E.z < 0)
			{
				w = 2 * PI - w;
			}
		}
		else
		{
			w = 0;
		}
	}
	else
	{
		w = 0;
	}
	if (e > eps)
	{
		TA = acos(dotp(E, R) / e / r);
		if (vr < 0)
		{
			TA = 2 * PI - TA;
		}
	}
	else
	{
		cp = crossp(N, R);
		if (cp.z >= 0)
		{
			TA = acos(dotp(N, R) / n / r);
		}
		else
		{
			TA = 2 * PI - acos(dotp(N, R) / n / r);
		}
	}
	//a = OrbMech::power(h,2) / mu / (1 - OrbMech::power(e,2));
	coe.e = e;
	coe.h = h;
	coe.i = incl;
	coe.RA = RA;
	coe.TA = TA;
	coe.w = w;
	return coe;
}

OELEMENTS coe_from_PACSS4(VECTOR3 R, VECTOR3 V, double mu)
{
	double r, v, C3, e, inc, alpha_D, f, theta_N;
	VECTOR3 HH, E, K, N;
	OELEMENTS coe;

	r = length(R);
	v = length(V);
	C3 = v*v - 2.0*mu / r;
	HH = crossp(R, V);
	E = crossp(V, HH) / mu - unit(R);
	e = length(E);
	K = _V(0.0, 0.0, 1.0);
	N = crossp(HH, K);
	inc = acos(HH.z / length(HH));
	alpha_D = acos(dotp(N, E) / e / length(N));

	if (E.z < 0)
	{
		alpha_D = PI2 - alpha_D;
	}
	f = acos(dotp(E, R) / length(R) / length(E));
	theta_N = acos(N.x / length(N));

	if (N.y > 0)
	{
		theta_N = PI2 - theta_N;
	}
	theta_N = theta_N - -80.6041140*RAD;
	if (theta_N > PI2)
	{
		theta_N = theta_N - PI2;
	}

	coe.e = e;
	coe.h = C3;
	coe.i = inc;
	coe.RA = theta_N;
	coe.TA = f;
	coe.w = alpha_D;
		//coe = [C3 inc theta_N e alpha_D f];
	return coe;
}

void PACSS4_from_coe(OELEMENTS coe, double mu, VECTOR3 &R, VECTOR3 &V)
{
	double lng, C3, inc, theta_N, e, f, a, h, alpha_D;
	OELEMENTS coe2;

	lng = -80.604133*RAD;
	C3 = coe.h;
	inc = coe.i;
	theta_N = coe.RA;
	e = coe.e;
	alpha_D = coe.w;
	f = coe.TA;
	a = -mu / C3;
	h = sqrt(mu*a*(1.0 - e*e));
	//coe2 = [h e pi - theta_N - lng inc pi - alpha_D f];
	coe2.e = e;
	coe2.h = h;
	coe2.i = inc;
	coe2.RA = PI - theta_N - lng;
	coe2.TA = f;
	coe2.w = PI - alpha_D;

	sv_from_coe(coe2, mu, R, V);
}

VECTOR3 elegant_lambert(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double dt, int N, bool prog, double mu)
{
	double tol, ratio, r1, r2, c, s, theta, lambda, T, l, m, x, h1, h2, B, y, z, x_new, A, root;
	int nMax, n;
	VECTOR3 c12, Vt1, Vt2;

	tol = 1e-8;
	nMax = 1000;
	n = 0;
	ratio = 1;

	r1 = length(R1);
	r2 = length(R2);
	c = length(R2 - R1);
	s = (r1 + r2 + c) / 2;

	c12 = crossp(unit(R1), unit(R2));

	theta = acos(dotp(R1, R2) / r1 / r2);
	if ((prog == true && c12.z < 0) || (prog == false && c12.z >= 0))
	{
		theta = PI2 - theta;
	}

	lambda = sqrt(r1*r2) / s*cos(theta / 2.0);
	T = sqrt(8.0 * mu / OrbMech::power(s, 3.0))*dt;
	l = OrbMech::power((1.0 - lambda) / (1.0 + lambda), 2.0);
	m = OrbMech::power(T, 2.0) / (OrbMech::power(1.0 + lambda, 6.0));

	//T_m = pi*((2 * N + 1) - lambda ^ 5);

	//Low Energy
	x = 1.0 + 4.0 * l;

	while (abs(ratio) > tol && nMax >= n)
	{
		n = n + 1;
		if (N == 0)
		{
			double xi = fraction_xi(x);
			h1 = OrbMech::power(l + x, 2.0)*(1.0 + xi*(1.0 + 3.0*x)) / ((1.0 + 2.0*x + l)*(3.0 + x*(1.0 + 4.0*xi)));
			h2 = m*(1.0 + (x - l)*xi) / ((1.0 + 2.0*x + l)*(3.0 + x*(1.0 + 4.0*xi)));
		}
		else
		{
			h1 = OrbMech::power(l + x, 2.0) / (4.0 * OrbMech::power(x, 2.0) * (1.0 + 2.0 * x + l))*(3.0 * OrbMech::power(1.0 + x, 2.0) * (N*PI / 2.0 + atan(sqrt(x))) / sqrt(x) - (3.0 + 5.0 * x));
			h2 = m / (4.0 * OrbMech::power(x, 2.0) * (1.0 + 2.0 * x + l))*((OrbMech::power(x, 2.0) - (1.0 + l)*x - 3.0 * l)*(N*PI / 2.0 + atan(sqrt(x))) / sqrt(x) + (3.0 * l + x));
		}
		B = 27 * h2 / (4 * OrbMech::power(1 + h1, 3));
		if (B >= 0)
		{
			z = 2.0 * cosh(1.0 / 3.0 * acosh(sqrt(B + 1.0)));
		}
		else
		{
			z = 2.0 * cos(1.0 / 3.0 * acos(sqrt(B + 1.0)));
		}
		y = 2.0 / 3.0 * (1.0 + h1)*(sqrt(B + 1.0) / z + 1.0);
		x_new = sqrt(OrbMech::power((1.0 - l) / 2.0, 2) + m / y / y) - (1.0 + l) / 2.0;
		ratio = x - x_new;
		x = x_new;
	}

	Vt1 = ((R2 - R1) + R1*s*OrbMech::power(1.0 + lambda, 2.0) * (l + x) / (r1*(1.0 + x))) * 1.0 / (lambda*(1 + lambda))*sqrt(mu*(1.0 + x) / (2.0 * OrbMech::power(s, 3.0) * (l + x)));
	if (N == 0)
	{
		return Vt1;
	}

	//High Energy
	root = OrbMech::power((OrbMech::power((2.0 * m) / (OrbMech::power(N, 2.0)*OrbMech::power(PI, 2.0)), 1.0 / 3.0) - (1.0 + l) / 2.0), 2.0) - 4.0 * l;
	if (root >= 0)
	{
		x = (OrbMech::power(2.0 * m / (OrbMech::power(N, 2.0)*OrbMech::power(PI, 2.0)), 1.0 / 3.0) - (1.0 + l) / 2.0) - sqrt(root);
	}
	else
	{
		x = 0.0001;
	}

	ratio = 1;
	n = 0;
	while (abs(ratio) > tol && nMax >= n)
	{
		n = n + 1;
		h1 = (l + x)*(1.0 + 2.0 * x + l) / (2.0 * (l - OrbMech::power(x, 2.0)));
		h2 = m*sqrt(x) / (2.0 * (l - OrbMech::power(x, 2)))*((l - OrbMech::power(x, 2.0))*(N*PI / 2.0 + atan(sqrt(x))) / sqrt(x) - (l + x));
		B = 27.0 * h2 / (4.0 * OrbMech::power(sqrt(x)*(1 + h1), 3));
		if (B >= 0)
		{
			z = 2.0 * cosh(1.0 / 3.0 * acosh(sqrt(B + 1.0)));
		}
		else
		{
			z = 2.0 * cos(1.0 / 3.0 * acos(sqrt(B + 1.0)));
		}
		A = OrbMech::power((sqrt(B) + sqrt(B + 1.0)), 1.0 / 3.0);
		y = 2.0 / 3.0 * sqrt(x)*(1.0 + h1)*(sqrt(B + 1.0) / (A + 1.0 / A) + 1.0);
		//y = 2 / 3 * (1 + h1)*(sqrt(B + 1) / z + 1);
		x_new = 0.5*((m / y / y - (1.0 + l)) - sqrt(OrbMech::power(m / y / y - (1.0 + l), 2) - 4.0 * l));

		ratio = x - x_new;
		x = x_new;
	}

	Vt2 = ((R2 - R1) + R1*s*OrbMech::power(1.0 + lambda, 2.0) * (l + x) / (r1*(1.0 + x))) * 1.0 / (lambda*(1.0 + lambda))*sqrt(mu*(1.0 + x) / (2.0 * OrbMech::power(s, 3.0) * (l + x)));

	if (length(V1 - Vt1) > length(V1 - Vt2))
	{
		return Vt2;
	}
	else
	{
		return Vt1;
	}
}

void oneclickcoast(VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, VECTOR3 &R1, VECTOR3 &V1, OBJHANDLE gravref, OBJHANDLE &gravout)
{
	bool stop;
	CoastIntegrator* coast;
	coast = new CoastIntegrator(R0, V0, mjd0, dt, gravref, gravout);
	stop = false;
	while (stop == false)
	{
		stop = coast->iteration();
	}
	R1 = coast->R2;
	V1 = coast->V2;
	gravout = coast->outplanet;
	delete coast;
	stop = false;
}

VECTOR3 ThreeBodyLambert(double t_I, double t_E, VECTOR3 R_I, VECTOR3 V_init, VECTOR3 R_E, VECTOR3 R_m, VECTOR3 V_m, double r_s, double mu_E, double mu_M, VECTOR3 &R_I_star, VECTOR3 &delta_I_star, VECTOR3 &delta_I_star_dot)
{
	VECTOR3 R_I_sstar, V_I_sstar, V_I_star, R_S, R_I_star_apo, R_E_apo, V_E_apo, V_I;
	double t_S, tol, dt_S;
	OBJHANDLE hMoon, hEarth;
	//R_I_star, delta_I_star, delta_I_star_dot, 

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	tol = 1000.0;

	//R_I_star = delta_I_star = delta_I_star_dot = _V(0.0, 0.0, 0.0);

	do
	{
		do
		{
			R_I_sstar = R_m + R_I_star + delta_I_star;

			if (t_I - t_E < 0)
			{
				V_I_sstar = elegant_lambert(R_I_sstar, _V(0.0, 0.0, 0.0), R_E, (t_E - t_I) * 24.0 * 3600.0, 0, true, mu_E);
				V_I_star = V_I_sstar - V_m - delta_I_star_dot;
				INRFV(R_I, V_I_star, r_s, true, mu_M, V_I, R_S, dt_S);
			}
			else
			{

				V_I_sstar = elegant_lambert(R_I_sstar, _V(0.0, 0.0, 0.0), R_E, -(t_E - t_I) * 24.0 * 3600.0, 0, false, mu_E);
				V_I_sstar = -V_I_sstar;
				V_I_star = V_I_sstar - V_m - delta_I_star_dot;
				INRFV(R_I, -V_I_star, r_s, true, mu_M, V_I, R_S, dt_S);
				V_I = -V_I;
				dt_S *= -1.0;
			}
			t_S = t_I + dt_S / 24.0 / 3600.0;
			R_I_star_apo = R_I_star;
			R_I_star = R_S + V_I_star*(t_I - t_S) * 24.0 * 3600.0;

		} while (length(R_I_star - R_I_star_apo) > tol);

		OrbMech::oneclickcoast(R_I, V_I, t_I, (t_E - t_I) * 24.0 * 3600.0, R_E_apo, V_E_apo, hMoon, hEarth);
		rv_from_r0v0(R_E_apo, V_E_apo, (t_I - t_E) * 24.0 * 3600.0, R_I_sstar, V_I_sstar, mu_E);
		delta_I_star = R_I_sstar - R_m - R_I_star;
		delta_I_star_dot = V_I_sstar - V_m - V_I_star;

	} while (length(R_E - R_E_apo) > tol);

	return V_I;
}

void INRFV(VECTOR3 R_1, VECTOR3 V_2, double r_2, bool direct, double mu, VECTOR3 &V_1, VECTOR3 &R_2, double &dt_2)
{
	VECTOR3 r_1_cf, v_2_cf, c;
	double cos_psi, sin_psi, A, B, C, r_1, v_2, theta, *AA, *RR, sin_beta_2, v_1, f, g, p, sgn;
	int N;

	if (direct)
	{
		sgn = 1.0;
	}
	else
	{
		sgn = -1.0;
	}

	AA = new double[5];
	RR = new double[4];

	r_1 = length(R_1);
	v_2 = length(V_2);
	r_1_cf = unit(R_1);
	v_2_cf = unit(V_2);

	cos_psi = dotp(unit(R_1), unit(V_2));
	sin_psi = sgn*sqrt(1.0 - cos_psi*cos_psi);

	A = r_2*r_2*v_2*v_2 / mu / r_1;
	B = (1.0 - r_2*v_2*v_2 / mu)*sin_psi;
	C = cos_psi*cos_psi;
	
	//solve quartic
	AA[0] = A*A;
	AA[1] = 2.0*A*B;
	AA[2] = B*B + C - 2.0*A;
	AA[3] = -2.0*B;
	AA[4] = 1.0 - C;

	SolveQuartic(AA, RR, N);

	if (cos_psi > 0)
	{
		sin_beta_2 = RR[N-2];
	}
	else
	{
		sin_beta_2 = RR[N-1];
	}

	c = unit(crossp(r_1_cf, v_2_cf)/sin_psi);
	theta = atan2(sin_psi, cos_psi) - sin_beta_2;
	R_2 = (r_1_cf*cos(theta)+crossp(c,r_1_cf)*sin(theta))*r_2;
	v_1 = sqrt(v_2 - 2.0*mu / r_2 + 2.0*mu / r_1);
	p = r_2*r_2*v_2*v_2*sin_beta_2*sin_beta_2 / mu;
	f = 1.0 - r_2*(1.0 - cos(theta)) / p;
	g = r_2*r_1*sin(theta) / sqrt(mu*p);
	V_1 = (R_2 - R_1*f) / g;

	dt_2 = time_theta(R_1, V_1, theta, mu);
}

void SolveQuartic(double *A, double *R, int &N)
{
	double a, b, c, d, e, p, q, delta, delta0, delta1, S;

	a = A[0];
	b = A[1];
	c = A[2];
	d = A[3];
	e = A[4];

	p = (8.0*a*c - 3.0*b*b) / (8.0*a*a);
	q = (b*b*b - 4.0*a*b*c + 8.0*a*a*d) / (8.0*a*a*a);

	delta0 = c*c - 3.0*b*d + 12.0*a*e;
	delta1 = 2.0*c*c*c - 9.0*b*c*d + 27.0*b*b*e + 27.0*a*d*d - 72.0*a*c*e;

	delta = (delta1*delta1 - 4.0*delta0*delta0*delta0) / (-27.0);

	if (delta > 0)
	{
		double P, D;

		P = 8.0*a*c - 3.0 * b*b;
		D = 64.0*a*a*a*e - 16.0*a*a*c*c + 16.0*a*b*b*c - 16.0*a*a*b*d - 3.0*b*b*b*b;
		if (P < 0 && D < 0)
		{
			N = 4;

			double x1, x2, x3, x4, phi;
			
			phi = acos(delta1 / (2.0*sqrt(delta0*delta0*delta0)));
			S = 0.5*sqrt(-2.0 / 3.0*p + 2.0 / 3.0 / a*sqrt(delta0)*cos(phi / 3.0));

			x1 = -b / 4.0 / a - S + 0.5*sqrt(-4.0*S*S - 2.0*p + q / S);
			x2 = -b / 4.0 / a - S - 0.5*sqrt(-4.0*S*S - 2.0*p + q / S);
			x3 = -b / 4.0 / a + S + 0.5*sqrt(-4.0*S*S - 2.0*p - q / S);
			x4 = -b / 4.0 / a + S - 0.5*sqrt(-4.0*S*S - 2.0*p - q / S);

			if (x4 < x2)
			{
				R[0] = x4;

				if (x3 < x2)
				{
					R[1] = x3;
					R[2] = x2;
					R[3] = x1;
				}
				else
				{
					R[1] = x2;

					if (x3 < x1)
					{
						R[2] = x3;
						R[3] = x1;
					}
					else
					{
						R[2] = x1;
						R[3] = x3;
					}
				}
			}
			else
			{
				R[0] = x2;

				if (x1 < x4)
				{
					R[1] = x1;
					R[2] = x4;
					R[3] = x3;
				}
				else
				{
					R[1] = x4;

					if (x1 < x3)
					{
						R[2] = x1;
						R[3] = x3;
					}
					else
					{
						R[2] = x3;
						R[3] = x1;
					}
				}
			}
		}
		else
		{
			N = 0;
		}
	}
	else
	{
		N = 2;

		double Q;

		Q = power((delta1 + sqrt(-27.0*delta)) / 2.0, 1.0 / 3.0);
		S = 0.5*sqrt(-2.0 / 3.0*p + 1.0 / 3.0 / a*(Q + delta0 / Q));

		if (-4.0*S*S - 2.0*p + q / S > 0)
		{
			R[0] = -b / 4.0 / a - S - 0.5*sqrt(-4.0*S*S - 2.0*p + q / S);
			R[1] = -b / 4.0 / a - S + 0.5*sqrt(-4.0*S*S - 2.0*p + q / S);
			R[2] = -b / 4.0 / a + S;
			R[3] = 0.5*sqrt((-1.0)*(-4.0*S*S - 2.0*p - q / S));
		}
		else
		{
			R[0] = -b / 4.0 / a + S - 0.5*sqrt(-4.0*S*S - 2.0*p - q / S);
			R[1] = -b / 4.0 / a + S + 0.5*sqrt(-4.0*S*S - 2.0*p - q / S);
			R[2] = -b / 4.0 / a - S;
			R[3] = 0.5*sqrt((-1.0)*(-4.0*S*S - 2.0*p + q / S));
		}
	}
}

VECTOR3 Vinti(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double mjd0, double dt, int N, bool prog, OBJHANDLE gravref, OBJHANDLE gravin, OBJHANDLE gravout, VECTOR3 V_guess)
{
	double h, rho, error2, error3, mu, max_dr;
	int nMax, nMax2, n;
	VECTOR3 Vt1, V1_star, dr2, R2_star, V2_star, R1_ref, V1_ref, R2_ref;
	VECTOR3 v_l[3][4];
	VECTOR3 R2l[3][4];
	VECTOR3 V2l[3][4];
	VECTOR3 T[3];
	MATRIX3 T2;
	OBJHANDLE hEarth;

	hEarth = oapiGetObjectByName("Earth");

	h = 10e-3;
	rho = 0.5;
	error3 = 100.0;
	error2 = 0.1;// 1e-8;
	dr2 = _V(1.0, 1.0, 1.0);
	n = 0;
	nMax = 100;
	nMax2 = 10;

	mu = GGRAV*oapiGetMass(gravref);

	double hvec[4] = { h / 2, -h / 2, rho*h / 2, -rho*h / 2 };

	if (gravref != gravin)
	{
		oneclickcoast(R1, V1, mjd0, 0.0, R1_ref, V1_ref, gravin, gravref);
		R2_ref = R2;
	}
	else if (gravref != gravout)
	{
		VECTOR3 V2_ref;
		R1_ref = R1;
		V1_ref = V1;
		oneclickcoast(R2, V1, mjd0 + dt / 24.0 / 3600.0, 0.0, R2_ref, V2_ref, gravout, gravref);
	}
	else
	{
		R1_ref = R1;
		V1_ref = V1;
		R2_ref = R2;
	}

	if (dt > 0)
	{
		Vt1 = elegant_lambert(R1_ref, V1_ref, R2_ref, dt, N, prog, mu);
	}
	else
	{
		Vt1 = elegant_lambert(R1_ref, V1_ref, R2_ref, -dt, N, !prog, mu);
	}
	if (gravref != gravin)
	{
		VECTOR3 R1_unused;
		oneclickcoast(R1_ref, Vt1, mjd0, 0.0, R1_unused, Vt1, gravref, gravin);
	}
	if (length(V_guess) == 0.0)
	{
		V1_star = Vt1*sign(dt);
	}
	else
	{
		V1_star = V_guess;
	}

	if (gravref == hEarth && gravin == gravout && dt>0)
	{

		rv_from_r0v0_obla(R1, V1_star, mjd0, dt, R2_star, V2_star, gravref);
		dr2 = R2 - R2_star;

		while (length(dr2) > error3 && nMax2 >= n)
		{
			n += 1;
			for (int i = 0; i < 4; i++)
			{
				v_l[0][i] = V1_star + _V(1, 0, 0)*hvec[i];
				v_l[1][i] = V1_star + _V(0, 1, 0)*hvec[i];
				v_l[2][i] = V1_star + _V(0, 0, 1)*hvec[i];
			}
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					rv_from_r0v0_obla(R1, v_l[i][j], mjd0, dt, R2l[i][j], V2l[i][j], gravref);
				}
			}
			for (int i = 0; i < 3; i++)
			{
				T[i] = (R2l[i][2] - R2l[i][3] - (R2l[i][0] - R2l[i][1])*OrbMech::power(rho, 3.0)) * 1.0 / (rho*h*(1.0 - OrbMech::power(rho, 2.0)));
			}
			T2 = _M(T[0].x, T[1].x, T[2].x, T[0].y, T[1].y, T[2].y, T[0].z, T[1].z, T[2].z);
			V1_star = V1_star + mul(inverse(T2), dr2);
			rv_from_r0v0_obla(R1, V1_star, mjd0, dt, R2_star, V2_star, gravref);
			dr2 = R2 - R2_star;
		}
		//return V1_star;

		if (n == nMax2 || _isnan(R2_star.x))// || isinf(R2_star.x))
		{
			return _V(0, 0, 0);
		}

		dr2 = _V(1.0, 1.0, 1.0);
		n = 0;
	}

	oneclickcoast(R1, V1_star, mjd0, dt, R2_star, V2_star, gravin, gravout);
	dr2 = R2 - R2_star;

	while (length(dr2) > error2 && nMax >= n)
	{
		n += 1;
		for (int i = 0; i < 4; i++)
		{
			v_l[0][i] = V1_star + _V(1, 0, 0)*hvec[i];
			v_l[1][i] = V1_star + _V(0, 1, 0)*hvec[i];
			v_l[2][i] = V1_star + _V(0, 0, 1)*hvec[i];
		}
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				oneclickcoast(R1, v_l[i][j], mjd0, dt, R2l[i][j], V2l[i][j], gravin, gravout);
			}
		}
		for (int i = 0; i < 3; i++)
		{
			T[i] = (R2l[i][2] - R2l[i][3] - (R2l[i][0] - R2l[i][1])*OrbMech::power(rho, 3.0)) * 1.0 / (rho*h*(1.0 - OrbMech::power(rho, 2.0)));
		}
		T2 = _M(T[0].x, T[1].x, T[2].x, T[0].y, T[1].y, T[2].y, T[0].z, T[1].z, T[2].z);
		V1_star = V1_star + mul(inverse(T2), dr2);
		oneclickcoast(R1, V1_star, mjd0, dt, R2_star, V2_star, gravin, gravout);
		dr2 = R2 - R2_star;
		max_dr = 0.5*length(R2_star);
		if (length(dr2) > max_dr)
		{
			dr2 = unit(dr2)*max_dr;
		}
	}
	return V1_star;
}

void planeinter(VECTOR3 n1, double h1, VECTOR3 n2, double h2, VECTOR3 &m1, VECTOR3 &m2)
{
	double c1, c2;
	c1 = (h1 - h2*dotp(n1, n2)) / (1.0 - power(dotp(n1, n2), 2.0));
	c2 = (h2 - h1*dotp(n1, n2)) / (1.0 - power(dotp(n1, n2), 2.0));
	m1 = n1*c1 + n2*c2;
	m2 = crossp(n1, n2);
}

double NSRsecant(VECTOR3 RA, VECTOR3 VA, VECTOR3 RP, VECTOR3 VP, double mjd0, double x, double DH, OBJHANDLE gravref)
{
	double theta, SW, dh_CDH, mu;
	VECTOR3 RA2, VA2, RP2, VP2, u, RA2_alt, VA2_alt, RPC, VPC;
	bool stop;

	stop = false;

	mu = GGRAV*oapiGetMass(gravref);

	//rv_from_r0v0(RA, VA, x, RA2, VA2, mu);
	//rv_from_r0v0(RP, VP, x, RP2, VP2, mu);

	CoastIntegrator *coast;
	coast = new CoastIntegrator(RA, VA, mjd0, x, gravref, gravref);

	while (stop == false)
	{
		stop = coast->iteration();
	}
	RA2 = coast->R2;
	VA2 = coast->V2;

	stop = false;

	delete coast;

	coast = new CoastIntegrator(RP, VP, mjd0, x, gravref, gravref);

	while (stop == false)
	{
		stop = coast->iteration();
	}
	RP2 = coast->R2;
	VP2 = coast->V2;

	delete coast;

	u = unit(crossp(RP2, VP2));
	RA2_alt = RA2;
	VA2_alt = VA2;
	RA2 = unit(RA2 - u*dotp(RA2, u))*length(RA2);
	VA2 = unit(VA2 - u*dotp(VA2, u))*length(VA2);

	theta = acos(dotp(RA2, RP2) / length(RA2) / length(RP2));
	SW = sign(dotp(u, crossp(RP2, RA2)));
	theta = SW*theta;
	rv_from_r0v0_ta(RP2, VP2, theta, RPC, VPC, mu);
	dh_CDH = length(RPC) - length(RA2);
	return DH - dh_CDH;
}

MATRIX3 GetRotationMatrix(OBJHANDLE plan, double t)
{
	double t0, T_p, L_0, e_rel, phi_0, T_s, e_ref, L_ref, L_rel, phi;
	MATRIX3 Rot1, Rot2, R_ref, Rot3, Rot4, R_rel, R_rot, R, Rot;

	if (plan == oapiGetObjectByName("Earth"))
	{
		t0 = 51544.5;								//LAN_MJD, MJD of the LAN in the "beginning"
		T_p = -9413040.4;							//Precession Period
		L_0 = 0.00001553343;						//LAN in the "beginning"
		e_rel = 0.4090928023;						//Obliquity / axial tilt of the earth in radians
		phi_0 = 4.894942829;						//Sidereal Rotational Offset
		T_s = 86164.098904 / 24.0 / 60.0 / 60.0;	//Sidereal Rotational Period
		e_ref = 0;									//Precession Obliquity
		L_ref = 0;									//Precession LAN
	}
	else if (plan == oapiGetObjectByName("Moon"))
	{
		t0 = 51544.5;							//LAN_MJD, MJD of the LAN in the "beginning"
		T_p = -6793.468728092782;				//Precession Period
		L_0 = 1.71817749;						//LAN in the "beginning"
		e_rel = 0.026699886264850;				//Obliquity / axial tilt of the earth in radians
		phi_0 = 4.769465382;					//Sidereal Rotational Offset
		T_s = 2360588.15 / 24.0 / 60.0 / 60.0;	//Sidereal Rotational Period
		e_ref = 7.259562816e-005;				//Precession Obliquity
		L_ref = 0.4643456618;					//Precession LAN
	}

	Rot1 = _M(cos(L_ref), 0, -sin(L_ref), 0, 1, 0, sin(L_ref), 0, cos(L_ref));
	Rot2 = _M(1, 0, 0, 0, cos(e_ref), -sin(e_ref), 0, sin(e_ref), cos(e_ref));
	R_ref = mul(Rot1, Rot2);
	L_rel = L_0 + PI2*(t - t0) / T_p;
	Rot3 = _M(cos(L_rel), 0, -sin(L_rel), 0, 1, 0, sin(L_rel), 0, cos(L_rel));
	Rot4 = _M(1, 0, 0, 0, cos(e_rel), -sin(e_rel), 0, sin(e_rel), cos(e_rel));
	R_rel = mul(Rot3, Rot4);
	phi = phi_0 + PI2*(t - t0) / T_s + (L_0 - L_rel)*cos(e_rel);
	R_rot = _M(cos(phi), 0, -sin(phi), 0, 1, 0, sin(phi), 0, cos(phi));
	Rot = mul(R_rel, R_rot);
	R = mul(R_ref, Rot);
	return R;
}

/*MATRIX3 GetRotationMatrix2(OBJHANDLE plan, double t)
{
	MATRIX3 Ra;
	double w;

	Ra = GetObliquityMatrix(plan, t);
	w = GetPlanetCurrentRotation(plan, t);
	return mul(Ra, _M(cos(w), 0, -sin(w), 0, 1.0, 0.0, sin(w), 0, cos(w)));
}*/

MATRIX3 Orbiter2PACSS13(double mjd, double lat, double lng, double azi)
{
	OBJHANDLE hEarth;
	MATRIX3 Rot1, Rot2, Rot3, Rot4;
	VECTOR3 R_P, UX10, UY10, UZ10;

	hEarth = oapiGetObjectByName("Earth");

	Rot1 = GetRotationMatrix(hEarth, mjd);
	Rot2 = _M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)));
	UX10 = R_P;
	UY10 = unit(crossp(_V(0.0, 0.0, 1.0), UX10));
	UZ10 = crossp(UX10, UY10);

	Rot3 = _M(UX10.x, UX10.y, UX10.z, UY10.x, UY10.y, UY10.z, UZ10.x, UZ10.y, UZ10.z);
	Rot4 = _M(1.0, 0.0, 0.0, 0.0, cos(-azi), -sin(-azi), 0.0, sin(-azi), cos(-azi));

	return mul(transpose_matrix(Rot4), mul(Rot3, mul(Rot2, transpose_matrix(Rot1))));
}

double GetPlanetCurrentRotation(OBJHANDLE plan, double t)
{
	double t0, T_p, L_0, e_rel, phi_0, T_s, e_ref, L_ref, L_rel, phi, e_ecl, L_ecl, phi_off,r;
	MATRIX3 Rot1, Rot2, R_ref, Rot3, Rot4, Rot5, Rot6, R_rel, R_rot, Rot, R_ecl, R_off;
	VECTOR3 s;

	if (plan == oapiGetObjectByName("Earth"))
	{
		t0 = 51544.5;								//LAN_MJD, MJD of the LAN in the "beginning"
		T_p = -9413040.4;							//Precession Period
		L_0 = 0.00001553343;						//LAN in the "beginning"
		e_rel = 0.4090928023;						//Obliquity / axial tilt of the earth in radians
		phi_0 = 4.894942829;						//Sidereal Rotational Offset
		T_s = 86164.098904 / 24.0 / 60.0 / 60.0;	//Sidereal Rotational Period
		e_ref = 0;									//Precession Obliquity
		L_ref = 0;									//Precession LAN
	}
	else if (plan == oapiGetObjectByName("Moon"))
	{
		t0 = 51544.5;							//LAN_MJD, MJD of the LAN in the "beginning"
		T_p = -6793.468728092782;				//Precession Period
		L_0 = 1.71817749;						//LAN in the "beginning"
		e_rel = 0.026699886264850;				//Obliquity / axial tilt of the earth in radians
		phi_0 = 4.769465382;					//Sidereal Rotational Offset
		T_s = 2360588.15 / 24.0 / 60.0 / 60.0;	//Sidereal Rotational Period
		e_ref = 7.259562816e-005;				//Precession Obliquity
		L_ref = 0.4643456618;					//Precession LAN
	}

	Rot1 = _M(cos(L_ref), 0.0, -sin(L_ref), 0.0, 1.0, 0.0, sin(L_ref), 0.0, cos(L_ref));
	Rot2 = _M(1.0, 0.0, 0.0, 0.0, cos(e_ref), -sin(e_ref), 0.0, sin(e_ref), cos(e_ref));
	R_ref = mul(Rot1, Rot2);
	L_rel = L_0 + PI2*(t - t0) / T_p;
	Rot3 = _M(cos(L_rel), 0.0, -sin(L_rel), 0.0, 1.0, 0.0, sin(L_rel), 0.0, cos(L_rel));
	Rot4 = _M(1.0, 0.0, 0.0, 0.0, cos(e_rel), -sin(e_rel), 0.0, sin(e_rel), cos(e_rel));
	R_rel = mul(Rot3, Rot4);
	phi = phi_0 + PI2*(t - t0) / T_s + (L_0 - L_rel)*cos(e_rel);
	R_rot = _M(cos(phi), 0.0, -sin(phi), 0.0, 1.0, 0.0, sin(phi), 0.0, cos(phi));
	Rot = mul(R_ref,mul(R_rel, R_rot));
	s = mul(Rot, _V(0.0, 1.0, 0.0));
	e_ecl = acos(s.y);
	L_ecl = atan(-s.x / s.z);
	Rot5 = _M(cos(L_ecl), 0.0, -sin(L_ecl), 0.0, 1.0, 0.0, sin(L_ecl), 0.0, cos(L_ecl));
	Rot6 = _M(1.0, 0.0, 0.0, 0.0, cos(e_ecl), -sin(e_ecl), 0.0, sin(e_ecl), cos(e_ecl));
	R_ecl = mul(Rot5, Rot6);
	R_off = mul(transpose_matrix(R_ecl), mul(R_ref, R_rel));
	phi_off = atan(-R_off.m13 / R_off.m11);
	r = phi + phi_off;
	return r;
}

double findelev(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double mjd0, double E, OBJHANDLE gravref)
{
	double w_A, w_P, r_A, v_A, r_P, v_P, alpha, t, dt, E_err, E_A;
	VECTOR3 u, R_A, V_A, R_P, V_P, U_L, U_P;

	t = 0;
	E_err = 1.0;
	dt = 10.0;
	R_A = R_A0;
	V_A = V_A0;
	R_P = R_P0;
	V_P = V_P0;

	r_A = length(R_A);
	v_A = length(V_A);
	r_P = length(R_P);
	v_P = length(V_P);

	while (abs(E_err) > 0.005*RAD || abs(dt)>1.0)
	{
		u = unit(crossp(R_A, V_A));
		w_A = dotp(V_A, unit(crossp(u, R_A)) / r_A);
		w_P = dotp(V_P, unit(crossp(crossp(R_P, V_P), R_P)) / r_P);
		alpha = E + sign(dotp(crossp(R_A, R_P), u))*acos(dotp(R_A / r_A, R_P / r_P));
		dt = (alpha - PI + sign(r_P - r_A)*(PI - acos(r_A*cos(E) / r_P))) / (w_A - w_P);

		oneclickcoast(R_A, V_A, mjd0+t/24.0/3600.0, dt, R_A, V_A, gravref, gravref);
		oneclickcoast(R_P, V_P, mjd0+t/24.0/3600.0, dt, R_P, V_P, gravref, gravref);
		t += dt;
		r_A = length(R_A);
		v_A = length(V_A);
		r_P = length(R_P);
		v_P = length(V_P);
		U_L = unit(R_P - R_A);
		U_P = unit(U_L - R_A*dotp(U_L, R_A) / r_A / r_A);
		E_A = acos(dotp(U_L, U_P*sign(dotp(U_P, crossp(u, R_A)))));
		E_err = E - E_A;
	}
	return t;
}

double findelev_gs(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_gs, double mjd0, double E, OBJHANDLE gravref, double &range)
{
	double w_A, w_P, r_A, v_A, r_P, alpha, t, dt, E_err, E_A, dE, dE_0, dt_0, dt_max, t_S, theta_0;
	VECTOR3 R_A, V_A, R_P, U_L, U_P, U_N, U_LL, R_proj;
	MATRIX3 Rot2;
	int i;

	t = 0;
	E_err = 1.0;
	dt = 10.0;
	R_A = R_A0;
	V_A = V_A0;
	dE = 0;
	dE_0 = 0;
	i = 0;
	dt_max = 150.0;
	dt_0 = 0;

	w_A = PI2 / oapiGetPlanetPeriod(gravref);
	if (gravref == oapiGetObjectByName("Moon"))
	{
		w_A *= -1.0;
	}
	r_P = length(R_gs);

	Rot2 = OrbMech::GetRotationMatrix(gravref, mjd0);
	R_P = mul(Rot2, R_gs);
	R_P = _V(R_P.x, R_P.z, R_P.y);

	U_N = unit(crossp(R_A, V_A));
	U_LL = unit(crossp(U_N, R_P));
	R_proj = unit(crossp(U_LL, U_N))*r_P;

	r_A = length(R_A);
	v_A = length(V_A);

	t_S = t;
	U_L = unit(R_A - R_proj);
	//u = unit(crossp(R_A, V_A));
	U_P = unit(U_L - R_proj*dotp(U_L, R_proj) / r_P / r_P);
	E_A = acos(dotp(U_L, U_P*sign(dotp(U_P, crossp(U_N, R_proj)))));
	if (dotp(U_L, R_proj) < 0)
	{
		E_A = PI2 - E_A;
	}

	while ((abs(E_err) > 0.005*RAD || abs(dt)>1.0) && i < 30)
	{
		dE_0 = dE;
		dE = E_A - E;
		w_P = dotp(V_A, unit(crossp(U_N, R_A)) / r_A);
		alpha = E + sign(dotp(crossp(R_proj, R_A), U_N))*acos(dotp(R_proj / r_P, R_A / r_A));
		dt = (alpha - acos(min(1.0, r_P*cos(E) / r_A))) / (w_A - w_P);

		if (abs(dt) > dt_max)
		{
			dt = dt_max*sign(dt);
		}
		if (i > 0)
		{
			if (dE*dE_0 < 0)
			{
				dt = -sign(dt_0)*abs(dt) / 2.0;
				dt_max = dt_max / 3.0;
				t += dt;
				dt_0 = dt;
			}
			else if (abs(dE_0)>abs(dE))
			{
				dt = sign(dt_0)*abs(dt);
				t += dt;
				dt_0 = dt;
			}
			else
			{
				t = t - 1.5*dt_0;
				dt_0 = -dt_0 / 2.0;
			}
		}
		else
		{
			t += dt;
			dt_0 = dt;
		}
		oneclickcoast(R_A, V_A, mjd0 + t_S / 24.0 / 3600.0, t - t_S, R_A, V_A, gravref, gravref);
		Rot2 = OrbMech::GetRotationMatrix(gravref, mjd0 + t / 24.0 / 3600.0);
		R_P = mul(Rot2, R_gs);
		R_P = _V(R_P.x, R_P.z, R_P.y);

		U_N = unit(crossp(R_A, V_A));
		U_LL = unit(crossp(U_N, R_P));
		R_proj = unit(crossp(U_LL, U_N))*r_P;
		
		t_S = t;
		r_A = length(R_A);
		v_A = length(V_A);
		U_L = unit(R_A - R_proj);
		//u = unit(crossp(R_A, V_A));
		U_P = unit(U_L - R_proj*dotp(U_L, R_proj) / r_P / r_P);
		E_A = acos(dotp(U_L, U_P*sign(dotp(U_P, crossp(U_N, R_proj)))));
		if (dotp(U_L, R_proj) < 0)
		{
			E_A = PI2 - E_A;
		}
		E_err = E - E_A;
		i++;
	}

	theta_0 = acos(dotp(unit(R_proj), unit(R_P)));
	if (dotp(U_N, R_P) < 0)
	{
		theta_0 = -theta_0;
	}
	if (gravref == oapiGetObjectByName("Moon"))
	{
		theta_0 *= -1.0;
	}
	range = theta_0*r_P;

	return t;
}

double timetoperi(VECTOR3 R, VECTOR3 V, double mu)
{
	OELEMENTS coe;
	double a, chi, alpha, r0, vr0;

	coe = coe_from_sv(R, V, mu);
	//[h e RA incl w TA a]

	a = coe.h*coe.h / mu / (1.0 - coe.e*coe.e);

	if (coe.e > 1.0)
	{
		double F;
		F = log((sqrt(coe.e + 1.0) + sqrt(coe.e - 1.0)*tan(coe.TA / 2.0)) / (sqrt(coe.e + 1.0) - sqrt(coe.e - 1.0)*tan(coe.TA / 2.0)));
		chi = -sqrt(-a)*F;
	}
	else if (coe.e == 1)
	{
		chi = -coe.h / sqrt(mu)*tan(coe.TA / 2.0);
	}
	else
	{
		double E_0;
		E_0 = 2 * atan(sqrt((1.0 - coe.e) / (1.0 + coe.e))*tan(coe.TA / 2.0));
		chi = -sqrt(a)*E_0;
	}

	alpha = 1.0 / a;

	r0 = length(R);
	vr0 = dotp(R, V) / r0;
	return 1.0 / sqrt(mu)*(r0*vr0 / sqrt(mu)*chi*chi*stumpC(alpha*chi*chi) + (1.0 - alpha*r0)*OrbMech::power(chi, 3.0) * stumpS(alpha*chi*chi) + r0*chi);
}

double time_radius(VECTOR3 R, VECTOR3 V, double r, double s, double mu)
{
	double r0, v0, vr0, alpha, a, e, x, dt;
	VECTOR3 Ex;
	

	r0 = length(R);
	v0 = length(V);
	vr0 = dotp(R, V) / r0;
	alpha = 2.0 / r0 - v0*v0 / mu;
	a = 1.0 / alpha;
	Ex = (R*(v0*v0 - mu / r0) - V*r0*vr0)*1.0 / mu;
	e = length(Ex);
	if (e > 1)
	{
		double coshF, sinhF, F, coshF0, sinhF0, F0;

		coshF = (r / -a + 1.0) / e;

		if (coshF*coshF < 1.0)
		{
			sinhF = 0.0;
			coshF = 1.0;
		}
		else
		{
			sinhF = s*sqrt(coshF*coshF - 1);
		}
		F = atanh(sinhF / coshF);
		coshF0 = (r0 / -a + 1.0) / e;
		if (vr0 >= 0)
		{
			sinhF0 = sqrt(coshF0*coshF0 - 1.0);
		}
		else
		{
			sinhF0 = -sqrt(coshF0*coshF0 - 1.0);
		}
		F0 = atanh(sinhF0 / coshF0);
		if (F < F0 && s == -1)
		{
			F = F + PI2;
		}
		x = sqrt(-a)*(F - F0);
	}
	else
	{
		double cosE, sinE, E;
		double sinE0, cosE0, E0;

		cosE = (1.0 - r / a) / e;
		if (cosE*cosE > 1)
		{
			sinE = 0.0;
			if (cosE < 0)
			{
				cosE = -1.0;
			}
			else
			{
				cosE = 1.0;
			}
		}
		else
		{
			sinE = s*sqrt(1.0 - cosE*cosE);
		}
		E = atan2(sinE, cosE);
		cosE0 = (1.0 - r0 / a) / e;
		if (cosE0*cosE0 > 1)
		{
			sinE0 = 0.0;
			if (cosE0 < 0)
			{
				cosE0 = -1.0;
			}
			else
			{
				cosE0 = 1.0;
			}
		}
		else
		{
			if (vr0 >= 0)
			{
				sinE0 = sqrt(1.0 - cosE0*cosE0);
			}
			else
			{
				sinE0 = -sqrt(1.0 - cosE0*cosE0);
			}
		}
		E0 = atan2(sinE0, cosE0);
		if (E < E0 && s == -1)
		{
			E = E + PI2;
		}
		x = sqrt(a)*(E - E0);
	}

	dt = (r0*vr0 / sqrt(mu)*x*x*stumpC(alpha*x*x) + (1.0 - alpha*r0)*x*x*x*stumpS(alpha*x*x) + r0*x) / sqrt(mu);
	return dt;
}

double time_radius_integ(VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout, VECTOR3 &RPRE, VECTOR3 &VPRE)
{
	double dt1, sing, cosg, x2PRE, dt21,beta12,beta4,RF,phi4,dt21apo,beta13,dt2,beta14,mu;
	VECTOR3 N, R0out, V0out;

	mu = GGRAV*oapiGetMass(gravout);
	beta12 = 1.0;
	dt21apo = 100000000.0;
	dt2 = 0.0;
	dt21 = 1.0;

	oneclickcoast(R, V, mjd0, 0.0, R0out, V0out, gravref, gravout);
	dt1 = time_radius(R0out, V0out, r, s, mu);

	oneclickcoast(R, V, mjd0, dt1, RPRE, VPRE, gravref, gravout);

	while (abs(beta12) > 0.000007 && abs(dt21)>0.01)
	{
		N = crossp(unit(RPRE), unit(VPRE));
		sing = length(N);
		cosg = dotp(unit(RPRE), unit(VPRE));
		x2PRE = cosg / sing;
		beta4 = r / length(RPRE);
		beta12 = beta4 - 1.0;
		RF = beta4*length(RPRE);
		if (beta12 > 0)
		{
			phi4 = -1.0;
		}
		else if (x2PRE > 0)
		{
			phi4 = -1.0;
		}
		else
		{
			phi4 = 1.0;
		}
		dt21 = time_radius(RPRE, VPRE*phi4, RF, -phi4, mu);
		dt21 = phi4*dt21;
		beta13 = dt21 / dt21apo;

		if (beta13 > 0)
		{
			beta14 = 1.0;
		}
		else
		{
			beta14 = -0.6;
		}
		if (beta13 / beta14 > 1.0)
		{
			dt21 = beta14*dt21apo;
		}
		dt21apo = dt21;
		if (abs(dt21) != 0.0)
		{
			oneclickcoast(RPRE, VPRE, mjd0 + (dt1 + dt2) / 24.0 / 3600.0, dt21, RPRE, VPRE, gravout, gravout);
			dt2 += dt21;
		}
	}
	return dt1 + dt2;
}

void orbitmidnight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool night, double &v1)
{
	double tol, swit, mu, R_E,g1,g2,beta1,beta2,aa,SS,p;
	OELEMENTS coe;
	VECTOR3 P, Q, h, h_proj, r_proj;

	if (night)
	{
		swit = -1.0;
	}
	else
	{
		swit = 1.0;
	}
	tol = 1e-6;
	mu = GGRAV*oapiGetMass(planet);
	R_E = oapiGetSize(planet);

	coe = coe_from_sv(R, V, mu);

	P = _V(cos(coe.w)*cos(coe.RA) - sin(coe.w)*sin(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.RA) + sin(coe.w)*cos(coe.RA)*cos(coe.i), sin(coe.w)*sin(coe.i));
	Q = _V(-sin(coe.w)*cos(coe.RA) - cos(coe.w)*sin(coe.RA)*cos(coe.i), -sin(coe.w)*sin(coe.RA) + cos(coe.w)*cos(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.i));

	beta1 = dotp(sun, P) / length(sun);

	aa = coe.h*coe.h / (mu*(1 - coe.e*coe.e));

	//if (beta1*beta1 > 1.0 - pow(R_E / (aa*(1.0 - coe.e)), 2) && beta1*beta1 < 1.0 - pow(R_E / (aa*(1.0 + coe.e)), 2))
	//{
	//	v1 = 0;
	//	return;
	//}
	beta2 = dotp(sun, Q) / length(sun);
	p = coe.h*coe.h / mu;
	
	h = unit(crossp(R, V));
	h_proj = unit(crossp(unit(sun), h));
	r_proj = unit(crossp(h, h_proj));
	g1 = dotp(r_proj, P);
	g2 = dotp(r_proj, Q);

	v1 = 2.0*atan(g2 / (g1 + swit*1.0));

	SS = cos(v1)*g1 + sin(v1)*g2;

	if (abs(SS - swit)>tol)
	{
		v1 += PI;
	}
	if (v1 > PI)
	{
		v1 -= PI2;
	}
	else if (v1 < -PI)
	{
		v1 += PI2;
	}
	return;
}

void umbra(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v1)
{
	OELEMENTS coe;
	VECTOR3 P, Q;
	//double beta1, beta2, p, A,B, C, D, tol, f, f_dot, f_ddot,x, R_E, sinv, cosv, v, x_alt,mu,f0,f1;
	double tol, R_E, f, beta1, beta2, a, b, c, d, e, p, q, QQ, D0, D1, S, DD, phi, SS,sinx,pp, alpha, cond, aa, mu;
	double x[4];
	double cosv[2], sinv[2];
	int j;

	tol = 1e-6;
	mu = GGRAV*oapiGetMass(planet);
	R_E = oapiGetSize(planet);
	f = 1;

	coe = coe_from_sv(R, V, mu);

	//if (coe.i > PI05)
	//{
	//	swit = -1.0;
	//}
	//else
	//{
	//	swit = 1.0;
	//}

	P = _V(cos(coe.w)*cos(coe.RA) - sin(coe.w)*sin(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.RA) + sin(coe.w)*cos(coe.RA)*cos(coe.i), sin(coe.w)*sin(coe.i));
	Q = _V(-sin(coe.w)*cos(coe.RA) - cos(coe.w)*sin(coe.RA)*cos(coe.i), -sin(coe.w)*sin(coe.RA) + cos(coe.w)*cos(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.i));
	beta1 = dotp(sun, P) / length(sun);

	aa = coe.h*coe.h / (mu*(1 - coe.e*coe.e));

	if (beta1*beta1 > 1.0 - pow(R_E / (aa*(1.0 - coe.e)), 2) && beta1*beta1 < 1.0 - pow(R_E / (aa*(1.0 + coe.e)), 2))
	{
		v1 = 0;
		return;
	}

	beta2 = dotp(sun, Q) / length(sun);
	p = coe.h*coe.h / mu;
	/*A = coe.e*coe.e*R_E*R_E + p*p*beta1*beta1 - p*p*beta2*beta2;
	B = 2.0 * coe.e*R_E;
	C = 2.0*beta1*beta2*p*p;
	D = R_E*R_E + p*p*beta2*beta2 - p*p;*/

	alpha = R_E / p;

	a = pow(alpha,4)*pow(coe.e, 4) - 2.0*pow(alpha, 2)*(beta2*beta2 - beta1*beta1)*coe.e*coe.e + pow(beta1*beta1 + beta2*beta2, 2);
	b = 4.0*pow(alpha, 4)*pow(coe.e, 3) - 4.0*pow(alpha, 2)*(beta2*beta2 - beta1*beta1)*coe.e;
	c = 6.0*pow(alpha, 4)*coe.e*coe.e - 2.0*pow(alpha, 2)*(beta2*beta2 - beta1*beta1) - 2.0*pow(alpha, 2)*pow(1.0 - beta2*beta2, 1)*coe.e*coe.e + 2.0*(beta2*beta2 - beta1*beta1)*(1.0 - beta2*beta2) - 4.0*beta1*beta1*beta2*beta2;
	d = 4.0*pow(alpha, 4)*coe.e - 4.0*pow(alpha, 2)*(1.0 - beta2*beta2)*coe.e;
	e = pow(alpha, 4) - 2.0*pow(alpha, 2)*(1.0 - beta2*beta2) + pow(1.0 - beta2*beta2,2);

	pp = (8.0*a*c - 3.0 * b*b) / (8.0*a*a);
	q = (OrbMech::power(b, 3.0) - 4.0*a*b*c + 8.0*a*a*d) / (8.0 * a*a*a);
	D0 = c*c - 3.0*b*d + 12.0 * a*e;
	D1 = 2.0*c*c*c - 9.0*b*c*d + 27.0*b*b*e + 27.0*a*d*d - 72.0*a*c*e;
	DD = -(D1*D1 - 4.0*D0*D0*D0)/27.0;
	if (DD > 0)
	{
		phi = acos(D1 / (2.0*sqrt(D0*D0*D0)));
		S = 0.5*sqrt(-2.0 / 3.0*pp + 2.0 / 3.0 / a*sqrt(D0)*cos(phi / 3.0));
	}
	else
	{
		QQ = OrbMech::power((D1 + sqrt(D1*D1 - 4.0*D0*D0*D0)) / 2.0, 1.0 / 3.0);
		S = 0.5*sqrt(-2.0 / 3.0*pp + 1.0 / (3.0*a)*(QQ + D0 / QQ));
	}
	x[0] = -b / (4.0*a) - S + 0.5*sqrt(-4.0*S*S - 2.0 * pp + q / S);
	x[1] = -b / (4.0*a) - S - 0.5*sqrt(-4.0*S*S - 2.0 * pp + q / S);
	x[2] = -b / (4.0*a) + S + 0.5*sqrt(-4.0*S*S - 2.0 * pp - q / S);
	x[3] = -b / (4.0*a) + S - 0.5*sqrt(-4.0*S*S - 2.0 * pp - q / S);

	j = 0;

	for (int i = 0; i < 4; i++)
	{
		sinx = sqrt(1.0 - x[i] * x[i]);
		SS = R_E*R_E*pow(1.0 + coe.e*x[i], 2) + p*p*pow(beta1*x[i] + beta2*sinx, 2) - p*p;
		if (abs(SS) < 1.0)
		{
			cond = beta1*x[i] + beta2*sinx;
			if (cond < 0)
			{
				cosv[j] = x[i];
				sinv[j] = sinx;
				j++;
			}
		}
		sinx = -sinx;
		SS = R_E*R_E*pow(1.0 + coe.e*x[i], 2) + p*p*pow(beta1*x[i] + beta2*sinx, 2) - p*p;
		if (abs(SS) < 1.0)
		{
			cond = beta1*x[i] + beta2*sinx;
			if (cond < 0)
			{
				cosv[j] = x[i];
				sinv[j] = sinx;
				j++;
			}
		}
	}
	if (rise)
	{
		if (4.0*a*pow(cosv[0], 3) + 3.0*b*pow(cosv[0], 2) + 2.0*c*cosv[0] + d < 0)
		{
			v1 = atan2(sinv[0], cosv[0]);
		}
		else
		{
			v1 = atan2(sinv[1], cosv[1]);
		}
	}
	else
	{
		if (4.0*a*pow(cosv[0], 3) + 3.0*b*pow(cosv[0], 2) + 2.0*c*cosv[0] + d > 0)
		{
			v1 = atan2(sinv[0], cosv[0]);
		}
		else
		{
			v1 = atan2(sinv[1], cosv[1]);
		}
	}

	/*x = 0;
	x_alt = 1;



	f0 = A - B + D;
	f1 = A + B + D;
	//if (f0*f1 <= 0)
	//{
		while (abs(f) > tol && abs(x - x_alt) > 1e-12)
		{
			f = A*x*x + B*x + C*x*sqrt(1.0 - x*x) + D;
			f_dot = 2.0*A*x + B + C*(1.0 - 2.0*x*x) / sqrt(1.0 - x*x);
			f_ddot = 2.0*A + C*x*(2.0*x*x*x - 3.0) / OrbMech::power(1.0 - x*x, 1.5);
			x_alt = x;
			x = x - f / (f*f_ddot / (2.0*f_dot));
			if (x > 1.0)
			{
				x -= 2.0;
			}
			else if (x < -1.0)
			{
				x += 2.0;
			}
		}
		cosv = x;
		sinv = sqrt(1.0 - cosv*cosv);
		if (beta1*cosv + beta2*sinv<0)
		{
			v = atan2(sinv, cosv);
		}
		else
		{
			v = atan2(-sinv, cosv);
		}
		if (f_dot>0)
		{
			v1 = v;
		}
		else
		{
			v2 = v;
		}
		f = 1;
		x = 0.999;
		x_alt = 1;

		while (abs(f) > tol && abs(x - x_alt) > 1e-12)
		{
			f = A*x*x + B*x + C*x*sqrt(1.0 - x*x) + D;
			f_dot = 2.0*A*x + B + C*(1.0 - 2.0*x*x) / sqrt(1.0 - x*x);
			f_ddot = 2.0*A + C*x*(2.0*x*x*x - 3.0) / OrbMech::power(1.0 - x*x, 1.5);
			x_alt = x;
			x = x - f / (f*f_ddot / (2.0*f_dot));//
			if (x > 1.0)
			{
				x -= 2.0;
			}
			else if (x < -1.0)
			{
				x += 2.0;
			}
		}
		cosv = x;
		sinv = sqrt(1.0 - cosv*cosv);
		if (beta1*cosv + beta2*sinv<0)
		{
			v = atan2(sinv, cosv);
		}
		else
		{
			v = atan2(-sinv, cosv);
		}
		if (f_dot>0)
		{
			v1 = v;
		}
		else
		{
			v2 = v;
		}
	//}
	//else
	//{
	//	v1 = v2 = 0;
	//}*/
}

bool sight(VECTOR3 R1, VECTOR3 R2, double R_E)
{
	VECTOR3 R1n, R2n;
	bool los;
	double tau_min;

	R1n = R1 / R_E;
	R2n = R2 / R_E;

	tau_min = (length(R1n)*length(R1n) - dotp(R1n, R2n)) / (length(R1n)*length(R1n) + length(R2n)*length(R2n) - 2.0*dotp(R1n,R2n));
	los = false;
	if (tau_min < 0 || tau_min>1)
	{
		los = true;
	}
	else if ((1.0 - tau_min)*length(R1n)*length(R1n) + dotp(R1n, R2n)*tau_min >= 1.0)
	{
		los = true;
	}
	return los;
}

double P29TimeOfLongitude(VECTOR3 R0, VECTOR3 V0, double MJD, OBJHANDLE gravref, double phi_d)
{
	MATRIX3 Rot2;
	VECTOR3 mu_N, mu_S, mu_Z, U_Z, mu_E, mu_C, R, V, mu_D, mu_E_apo;
	double mu, F, dphi, t, phi, lambda, eps_phi, absphidminphi, phidminphi, phi_0, theta_P, theta, t_F;
	int n, s_G;
	OBJHANDLE hEarth;

	mu = GGRAV*oapiGetMass(gravref);
	n = 0;
	eps_phi = 0.0001*RAD;
	hEarth = oapiGetObjectByName("Earth");
	absphidminphi = 1.0;

	U_Z = _V(0.0, 1.0, 0.0);
	Rot2 = OrbMech::GetRotationMatrix(gravref, MJD);
	mu_Z = mul(Rot2, U_Z);
	mu_Z = _V(mu_Z.x, mu_Z.z, mu_Z.y);

	mu_N = unit(crossp(R0, V0));
	mu_S = unit(crossp(mu_N, R0));
	mu_N = mu_N*sign(dotp(mu_N, mu_Z));
	mu_E = unit(crossp(mu_Z, R0));
	mu_C = unit(crossp(mu_Z, mu_E));

	R = R0;
	V = V0;
	t = MJD;
	dphi = 0.0;

	if (gravref == hEarth)
	{
		F = 16.0 / 15.0;
	}
	else
	{
		F = 327.8 / 328.8;
	}

	while (absphidminphi > eps_phi && absphidminphi < PI2 - eps_phi)
	{
		latlong_from_J2000(R, t, gravref, lambda, phi);

		absphidminphi = abs(phi_d - phi);
		phidminphi = phi_d - phi;

		while (absphidminphi >= PI2)
		{
			absphidminphi -= PI2;
		}

		if (n == 0)
		{
			phi_0 = phi;
			if (gravref == hEarth)
			{
				if (phidminphi < 0)
				{
					phidminphi = phidminphi - (sign(phidminphi))*PI2;
				}
			}
			else
			{
				if (phidminphi >= 0)
				{
					phidminphi = phidminphi - (sign(phidminphi))*PI2;
				}
			}
		}
		else
		{
			if (sign((phi - phi_0)*(phi_0 - phi)) == 1)
			{
				if (sign(phi) + sign(phi_d) != 0)
				{
					phidminphi = phidminphi - (sign(phidminphi))*PI2;
				}
			}
			else
			{
				if (sign(phi_d*phi) == -1)
				{
					phidminphi = phidminphi - (sign(phidminphi))*PI2;
				}
			}
		}

		dphi += F*phidminphi;
		if (abs(dphi) > PI2)
		{
			theta_P = PI2;
		}
		else
		{
			theta_P = 0.0;
		}
		mu_E_apo = mu_E*cos(dphi) + mu_C*sin(dphi);
		mu_D = unit(crossp(mu_E_apo, mu_N));
		s_G = sign(dotp(mu_D, mu_S));
		theta = PI*(1 - s_G) + s_G*acos(dotp(mu_D, R0) / length(R0)) + theta_P;

		t_F = time_theta(R0, V0, theta, mu);
		if (t_F < 0.0)
		{
			t_F += period(R0, V0, mu);
		}
		t = MJD + t_F / 24.0 / 3600.0;
		rv_from_r0v0(R0, V0, (t - MJD)*24.0*3600.0, R, V, mu);
		n++;
	}

	return t;
}

void latlong_from_J2000(VECTOR3 R, double MJD, OBJHANDLE gravref, double &lat, double &lng)
{
	MATRIX3 Rot2;
	VECTOR3 R_ecl, R_equ;

	Rot2 = OrbMech::GetRotationMatrix(gravref, MJD);

	R_ecl = _V(R.x, R.z, R.y);
	R_equ = tmul(Rot2, R_ecl);

	latlong_from_r(_V(R_equ.x, R_equ.z, R_equ.y), lat, lng);
}

void latlong_from_r(VECTOR3 R, double &lat, double &lng)
{
	VECTOR3 u;

	u = unit(R);
	lat = atan2(u.z, sqrt(u.x*u.x + u.y*u.y));
	lng = atan2(u.y, u.x);
}

bool groundstation(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, double lat, double lng, bool rise, double &dt)
{
	double v1, dt_old,h, e, theta0,a,T,n, E_0, t_0, E_1, t_f, R_E, dt_max, rev, T_p, mu;
	VECTOR3 R_GS, gndst;
	MATRIX3 Rot;
	OELEMENTS coe;
	bool los;
	int nn, nmax;

	nn = 0;
	dt = 0;
	dt_max = 100.0;
	nmax = 100;
	dt_old = 1;
	R_E = oapiGetSize(planet);
	mu = GGRAV*oapiGetMass(planet);
	rev = 0.0;
	T_p = oapiGetPlanetPeriod(planet);

	while (abs(dt_old - dt) > 0.5 && nn <= nmax)
	{

		Rot = GetRotationMatrix(planet, MJD+dt/24.0/3600.0);
		R_GS = unit(_V(cos(lng)*cos(lat), sin(lat), sin(lng)*cos(lat)))*R_E;
		gndst = mul(Rot, R_GS);
		gndst = _V(gndst.x, gndst.z, gndst.y);
		los = gslineofsight(R, V, gndst, planet, rise, v1);
		if (!los)
		{
			return false;
		}

		coe = coe_from_sv(R, V, mu);
		h = coe.h;
		e = coe.e;
		theta0 = coe.TA;

		a = h*h / mu * 1.0 / (1.0 - e*e);
		T = PI2 / sqrt(mu)*OrbMech::power(a, 3.0 / 2.0);
		n = PI2 / T;
		E_0 = 2.0 * atan(sqrt((1.0 - e) / (1.0 + e))*tan(theta0 / 2.0));
		t_0 = (E_0 - e*sin(E_0)) / n;
		E_1 = 2.0 * atan(sqrt((1.0 - e) / (1.0 + e))*tan(v1 / 2.0));
		t_f = (E_1 - e*sin(E_1)) / n;
		dt_old = dt;
		dt = t_f - t_0;

		if (dt < 0)
		{
			dt += T;
		}
		if (dt*(1.0 + T / T_p)>T && rev == 0.0)
		{
			rev = 1.0;
		}
		/*if (abs(dt_des - dt_old)>dt_max)
		{
			dt += dt_max;
		}
		else
		{
			dt = dt_des;
		}*/
		if (dt > 0 && abs(dt - dt_old) > 0.5*T)
		{
			dt += T;
		}
		nn++;
	}

	return true;
}

bool gslineofsight(VECTOR3 R, VECTOR3 V, VECTOR3 sun, OBJHANDLE planet, bool rise, double &v)
{
	double R_E, beta1, beta2, root, deno, a, b, c, e, f, d, g, aa, p, alpha, fact, root2, v1, v2, mu;
	OELEMENTS coe;
	VECTOR3 P, Q;

	if (rise)
	{
		fact = -1.0;
	}
	else
	{
		fact = 1.0;
	}

	R_E = oapiGetSize(planet);
	mu = GGRAV*oapiGetMass(planet);

	coe = coe_from_sv(R, V, mu);

	P = _V(cos(coe.w)*cos(coe.RA) - sin(coe.w)*sin(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.RA) + sin(coe.w)*cos(coe.RA)*cos(coe.i), sin(coe.w)*sin(coe.i));
	Q = _V(-sin(coe.w)*cos(coe.RA) - cos(coe.w)*sin(coe.RA)*cos(coe.i), -sin(coe.w)*sin(coe.RA) + cos(coe.w)*cos(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.i));
	beta1 = dotp(sun, P) / length(sun);

	aa = coe.h*coe.h / (mu*(1.0 - coe.e*coe.e));

	beta2 = dotp(sun, Q) / length(sun);
	p = coe.h*coe.h / mu;
	alpha = R_E / p;

	root2 = alpha*alpha*beta2*beta2*coe.e*coe.e - alpha*alpha*beta2*beta2 - 2.0*alpha*beta1*beta2*beta2*coe.e + beta1*beta1*beta2*beta2 + pow(beta2, 4);
	if (root2 < 0)
	{
		return false;
	}
	root = sqrt(root2);
	deno = alpha*alpha*coe.e*coe.e - 2.0*alpha*beta1*coe.e + beta1*beta1 + beta2*beta2;
	a = (-root - coe.e*alpha*alpha + alpha*beta1) / deno;
	b = (2.0*coe.e*alpha*alpha*beta1) / deno;
	c = alpha*beta1*beta1 / deno;
	d = coe.e*alpha*root / deno;
	e = beta1*root / deno;
	f = coe.e*coe.e*pow(alpha, 3) / deno;

	v1 = PI05-atan2(a, -1.0 / beta2*(-b + c + d - e + f - alpha));
	v = v1;

	//test1 = cos(v1)*beta1 + sin(v1)*beta2;
	//test2 = alpha*(1.0 + coe.e*cos(v1));

	if (fact*(2.0*(-sin(v1)*beta1 + cos(v1)*beta2)*(cos(v1)*beta1 + sin(v1)*beta2) - 2.0*alpha*alpha*(-coe.e*sin(v1))*(1.0+coe.e*cos(v1)))>0)
	//if (fact*(pow(-cos(PI05-v1)*beta2 + sin(PI05-v1)*beta1,2) + pow(-alpha*sin(PI05-v1),2)) > 0)
	{
		g = (root - coe.e*alpha*alpha + alpha*beta1) / deno;
		v2 = PI05 - atan2(g, -1.0 / beta2*(-b + c - d + e + f - alpha));
		v = v2;
	}

	
	return true;
}

int findNextAOS(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet)
{
	double lat, lng, dt, dtmin;
	int gsmin;
	bool los;

	dtmin = 1000000;

	for (int i = 0; i < NUMBEROFGROUNDSTATIONS; i++)
	{
		lat = groundstations[i][0];
		lng = groundstations[i][1];
		los = groundstation(R, V, MJD, planet, lat, lng, 1, dt);
		if (los && dt < dtmin)
		{
			gsmin = i;
			dtmin = dt;
		}
	}
	return gsmin;
}

bool vesselinLOS(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet)
{
	VECTOR3 R_GS, gndst;
	MATRIX3 Rot;
	double lat, lng, R_E;
	bool los;

	Rot = GetRotationMatrix(planet, MJD);
	R_E = oapiGetSize(planet);

	for (int i = 0; i < NUMBEROFGROUNDSTATIONS; i++)
	{
		lat = groundstations[i][0];
		lng = groundstations[i][1];
		R_GS = unit(_V(cos(lng)*cos(lat), sin(lat), sin(lng)*cos(lat)))*R_E;
		gndst = mul(Rot, R_GS);
		gndst = _V(gndst.x, gndst.z, gndst.y);
		los = sight(R, gndst, R_E);
		if (los)
		{
			return false;
		}
	}
	return true;
}

double sunrise(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, OBJHANDLE planet2, bool rise, bool midnight, bool future)
{
	//midnight = 0-> rise=0:sunset, rise=1:sunrise
	//midnight = 1-> rise=0:midday, rise=1:midnight
	double *PlanPos, *SunPos;
	PlanPos = new double[12];
	SunPos = new double[12];
	VECTOR3 PlanVec, R_EM, R_SE;
	OBJHANDLE hEarth, hMoon, hSun;
	double mu, v1;
	unsigned char options;



	mu = GGRAV*oapiGetMass(planet);

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	hSun = oapiGetObjectByName("Sun");

	CELBODY *cPlan = oapiGetCelbodyInterface(planet);
	//CELBODY *cSun = oapiGetCelbodyInterface(hSun);

	OELEMENTS coe;
	double h, e, theta0, a, T, n, E_0, t_0, E_1, dt, t_f, dt_alt;

	dt = 0;
	dt_alt = 1;

	while (abs(dt_alt-dt)>0.5)
	{
		if (planet == hMoon && planet2 == hSun)
		{
			CELBODY *cEarth = oapiGetCelbodyInterface(hEarth);
			options = cPlan->clbkEphemeris(MJD + dt / 24.0 / 3600.0, EPHEM_TRUEPOS, PlanPos);
			if (options & EPHEM_POLAR)
			{
				R_EM = Polar2Cartesian(PlanPos[2] * AU, PlanPos[1], PlanPos[0]);
			}
			else
			{
				R_EM = _V(PlanPos[0], PlanPos[2], PlanPos[1]);
				//R_ES = -mul(Rot, _V(EarthVec.x, EarthVec.z, EarthVec.y));
			}
			options = cEarth->clbkEphemeris(MJD + dt / 24.0 / 3600.0, EPHEM_TRUEPOS, PlanPos);
			if (options & EPHEM_POLAR)
			{
				R_SE = Polar2Cartesian(PlanPos[2] * AU, PlanPos[1], PlanPos[0]);
			}
			else
			{
				R_SE = _V(PlanPos[0], PlanPos[2], PlanPos[1]);
				//R_ES = -mul(Rot, _V(EarthVec.x, EarthVec.z, EarthVec.y));
			}
			PlanVec = -(R_EM + R_SE);
		}
		else
		{
			options = cPlan->clbkEphemeris(MJD + dt / 24.0 / 3600.0, EPHEM_TRUEPOS, PlanPos);

			if (options & EPHEM_POLAR)
			{
				PlanVec = -Polar2Cartesian(PlanPos[2] * AU, PlanPos[1], PlanPos[0]);
			}
			else
			{
				PlanVec = -_V(PlanPos[0], PlanPos[2], PlanPos[1]);
				//R_ES = -mul(Rot, _V(EarthVec.x, EarthVec.z, EarthVec.y));
			}
		}

		if (midnight)
		{
			orbitmidnight(R, V, PlanVec, planet, rise, v1);
		}
		else
		{
			umbra(R, V, PlanVec, planet, rise, v1);
		}

		coe = coe_from_sv(R, V, mu);
		h = coe.h;
		e = coe.e;
		theta0 = coe.TA;

		a = h*h / mu * 1.0 / (1.0 - e*e);
		T = PI2 / sqrt(mu)*OrbMech::power(a, 3.0 / 2.0);
		n = PI2 / T;
		E_0 = 2.0 * atan(sqrt((1.0 - e) / (1.0 + e))*tan(theta0 / 2.0));
		t_0 = (E_0 - e*sin(E_0)) / n;
		E_1 = 2.0 * atan(sqrt((1.0 - e) / (1.0 + e))*tan(v1 / 2.0));
		t_f = (E_1 - e*sin(E_1)) / n;
		dt_alt = dt;
		dt = t_f - t_0;

		if (dt < 0 && future)
		{
			dt += T;
		}
	}

	return dt;
}

VECTOR3 ULOS(MATRIX3 REFSMMAT, MATRIX3 SMNB, double TA, double SA)
{
	MATRIX3 SBNB;
	VECTOR3 U_NB, S_SM, U_LOS;
	double a;

	a = -0.5676353234;
	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
	U_NB = mul(SBNB, _V(sin(TA)*cos(SA), sin(TA)*sin(SA), cos(TA)));
	S_SM = mul(transpose_matrix(SMNB), U_NB);
	U_LOS = mul(transpose_matrix(REFSMMAT), S_SM);
	return U_LOS;
}

int FindNearestStar(VECTOR3 U_LOS, VECTOR3 R_C, double R_E, double ang_max)
{
	VECTOR3 ustar;
	double dotpr, last;
	int star;
	star = -1;
	last = -2.0;

	for (int i = 0; i < 37; i++)
	{
		ustar = navstars[i];
		dotpr = dotp(ustar, U_LOS);
		if (dotpr>last && isnotocculted(U_LOS,R_C,R_E) && dotpr>cos(ang_max))
		{
			star = i;
			last = dotpr;
		}
	}
	return star;
}

VECTOR3 backupgdcalignment(MATRIX3 REFS, VECTOR3 R_C, double R_E, int &set)
{
	double a,SA,TA,dTA;
	VECTOR3 s_NBA, s_NBB,s_SMB, s_SMA,X_NB,Y_NB,Z_NB,X,Y,Z,X_SM,Y_SM,Z_SM,imuang;
	MATRIX3 SBNB,SMNB;

	a = -0.5676353234;
	TA = 7.5*RAD;
	SA = PI;

	int** starset;
	starset = new int*[2];
	for (int i = 0; i < 3;i++)
	{
		starset[i] = new int[2];
	}
	starset[0][0] = 29;
	starset[0][1] = 34;

	starset[1][0] = 2;
	starset[1][1] = 4;

	starset[2][0] = 20;
	starset[2][1] = 27;

	for (set = 0; set < 3; set++)
	{
		s_SMA = navstars[starset[set][0]];
		s_SMB = navstars[starset[set][1]];

		dTA = dotp(s_SMA, s_SMB);

		SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));

		s_NBA = mul(SBNB, _V(sin(TA)*cos(SA), sin(TA)*sin(SA), cos(TA)));
		s_NBB = mul(SBNB, _V(sin(TA - dTA)*cos(SA), sin(TA - dTA)*sin(SA), cos(TA - dTA)));

		//s_SMA = navstars[29];// mul(REFS, navstars[29]);
		//s_SMB = navstars[34]; //mul(REFS, navstars[34]);

		X_NB = s_NBA;
		Y_NB = unit(crossp(s_NBA, s_NBB));
		Z_NB = crossp(X_NB, Y_NB);

		X_SM = s_SMA;
		Y_SM = unit(crossp(s_SMA, s_SMB));
		Z_SM = crossp(X_SM, Y_SM);

		X = X_SM*X_NB.x + Y_SM*Y_NB.x + Z_SM*Z_NB.x;
		Y = X_SM*X_NB.y + Y_SM*Y_NB.y + Z_SM*Z_NB.y;
		Z = X_SM*X_NB.z + Y_SM*Y_NB.z + Z_SM*Z_NB.z;

		SMNB = _M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z);

		imuang = CALCGAR(REFS, SMNB);

		if (cos(imuang.z*2.0) + 0.5>0 && isnotocculted(s_SMA, R_C, R_E) && isnotocculted(s_SMB, R_C, R_E))
		{
			return imuang;
		}
	}
	return _V(0, 0, 0);
}

bool isnotocculted(VECTOR3 S_SM, VECTOR3 R_C, double R_E)
{
	double c,dote;

	c = cos(5.0*RAD + asin(R_E / length(R_C)));

	dote = dotp(S_SM, unit(-R_C));
	if (dote < c)
	{
		return true;
	}
	else
	{
		return false;
	}

}

MATRIX3 inverse(MATRIX3 a)
{
	double det;

	det = determinant(a);
	return _M(a.m22*a.m33 - a.m23*a.m32, a.m13*a.m32 - a.m12*a.m33, a.m12*a.m23 - a.m13*a.m22, a.m23*a.m31 - a.m21*a.m33, a.m11*a.m33 - a.m13*a.m31, a.m13*a.m21 - a.m11*a.m23, a.m21*a.m32 - a.m22*a.m31, a.m12*a.m31 - a.m11*a.m32, a.m11*a.m22 - a.m12*a.m21) / det;

}

double determinant(MATRIX3 a)
{
	return a.m11*a.m22*a.m33 + a.m12*a.m23*a.m31 + a.m13*a.m21*a.m32 - a.m13*a.m22*a.m31 - a.m12*a.m21*a.m33 - a.m11*a.m23*a.m32;
}

MATRIX3 transpose_matrix(MATRIX3 a)
{
	MATRIX3 b;

	b = _M(a.m11, a.m21, a.m31, a.m12, a.m22, a.m32, a.m13, a.m23, a.m33);
	return b;
}

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

int DoubleToBuffer(double x, double q, int m)
{
	int c = 0, out = 0, f = 1;

	x = x * (268435456.0 / pow(2.0, fabs(q)));

	if (m) c = 0x3FFF & (((int)fabs(x)) >> 14);	// High word
	else   c = 0x3FFF & ((int)fabs(x));		// Low word

	if (x<0.0) c = 0x7FFF & (~c); // Polarity change

	while (c != 0) {
		out += (c & 7) * f;
		f *= 10;	c = c >> 3;
	}
	return out;
}

double DecToDouble(int dec1, int dec2)
{
	double val = OrbMech::power(2.0, -14.0)*dec1 + OrbMech::power(2.0, -28.0)*dec2;
	if (val > 0.5)
	{
		val = 2.0 - val;
		val = -val;
	}
	return val;
}

double OctToDouble(int oct1, int oct2)
{
	unsigned long long bin1, bin2;
	double dec1, dec2,dubb;
	bin1 = octal_binary(oct1);
	bin2 = octal_binary(oct2);
	dec1 = BinToDec(bin1);
	dec2 = BinToDec(bin2);
	dubb = dec1*OrbMech::power(2.0, -14.0) + dec2*OrbMech::power(2.0, -28.0);
	if (dubb > 0.5)
	{
		dubb = 0.5 - dubb;
	}
	return dubb;
}

unsigned long long octal_binary(int n)  /* Function to convert octal to binary.*/
{
	unsigned long long decimal = 0, binary = 0, i = 0;
	while (n != 0)
	{
		decimal += (n % 10)*OrbMech::power(8, i);
		++i;
		n /= 10;
	}
	/* At this point, the decimal variable contains corresponding decimal value of that octal number. */
	i = 1;
	while (decimal != 0)
	{
		binary += (decimal % 2)*i;
		decimal /= 2;
		i *= 10;
	}
	return binary;
}

unsigned long long BinToDec(unsigned long long num)
{
	unsigned long long dec = 0, rem, base = 1;
	while (num > 0)
	{
		rem = num % 10;
		dec = dec + rem * base;
		base = base * 2;
		num = num / 10;
	}
	return dec;
}

double cot(double a)
{
	return cos(a) / sin(a);
}



/*void rungeinteg(VECTOR3 R0, VECTOR3 V0, double dt, VECTOR3 &R1, VECTOR3 &V1, double mu)
{
	int n = 6;
	double f0[6] = { 0.0 };
	f0[0] = R0.x;
	f0[1] = R0.y;
	f0[2] = R0.z;
	f0[3] = V0.x;
	f0[4] = V0.y;
	f0[5] = V0.z;
	double t0 = 0;
	double tf = dt;
	double tspan[2] = { 0.0 };
	tspan[0] = t0;
	tspan[1] = tf;
	//void(*func_ptr)(double*, double, double*) = &this->adfunc;
	int laenge = 10000;
	double* tout = NULL;
	tout = new double[laenge];
	double** yout = NULL;
	yout = new double*[n];

	double tol = 1e-13;

	for (int i = 0; i<n; i++)
	{
		yout[i] = new double[laenge];
	}
	double* yptr[6] = { yout[0], yout[1], yout[2], yout[3], yout[4], yout[5] };

	int ganzzahl = rkf45(tout, yptr, tspan, f0, n, tol);

	R1.x = yout[0][ganzzahl];
	R1.y = yout[1][ganzzahl];
	R1.z = yout[2][ganzzahl];
	V1.x = yout[3][ganzzahl];
	V1.y = yout[4][ganzzahl];
	V1.z = yout[5][ganzzahl];
}*/

/*void adfunc(double* dfdt, double t, double* f)
{
	VECTOR3 R, V, rdach, phidach, thetadach, a_G;
	double phi, a0r, a2r, a2theta, a3r, a3theta, a4theta, a4r, r;

	R = _V(f[0], f[1], f[2]);
	V = _V(f[3], f[4], f[5]);

	phi = asin(dotp(unit(R),_V(0.0, 0.0, 1.0)));
	rdach = unit(R);
	r = length(R);
	phidach = unit(crossp(_V(0.0, 0.0, 1.0),rdach));
	thetadach = unit(crossp(rdach, phidach));

	if (JCoeffCount == 1)
	{
		a0r = -mu / (OrbMech::power(r, 2.0));

		a_G = rdach*a0r;
	}
	else if (JCoeffCount == 2)
	{
		a0r = -mu / (OrbMech::power(r, 2.0));
		a2r = -3.0 / 2.0 * mu*OrbMech::power(R_b, 2.0) * JCoeff[0] / (OrbMech::power(r, 4.0))*(3.0 * OrbMech::power(sin(phi), 2.0) - 1.0);
		a2theta = 3.0 * mu*OrbMech::power(R_b, 2.0) * JCoeff[0] / (OrbMech::power(r, 4.0))*sin(phi)*cos(phi);

		a_G = rdach*a0r - (rdach*a2r + thetadach*a2theta);
	}
	else if (JCoeffCount == 3)
	{
		a0r = -mu / (OrbMech::power(r, 2.0));
		a2r = -3.0 / 2.0 * mu*OrbMech::power(R_b, 2.0) * JCoeff[0] / (OrbMech::power(r, 4.0))*(3.0 * OrbMech::power(sin(phi), 2.0) - 1.0);
		a2theta = 3.0 * mu*OrbMech::power(R_b, 2.0) * JCoeff[0] / (OrbMech::power(r, 4.0))*sin(phi)*cos(phi);
		a3r = -2.0 * mu*OrbMech::power(R_b, 3.0) * JCoeff[1] / OrbMech::power(r, 5.0)*(5.0 * OrbMech::power(sin(phi), 3.0) - 3.0 * sin(phi));
		a3theta = 3.0 / 2.0 * mu*OrbMech::power(R_b, 3.0) * JCoeff[1] / OrbMech::power(r, 5)*(5.0 * OrbMech::power(sin(phi), 2.0) * cos(phi) - cos(phi));

		a_G = rdach*a0r - (rdach*a2r + thetadach*a2theta + rdach*a3r + thetadach*a3theta);
	}
	else if (JCoeffCount > 3)
	{
		a0r = -mu / (OrbMech::power(r, 2.0));
		a2r = -3.0 / 2.0 * mu*OrbMech::power(R_b, 2.0) * JCoeff[0] / (OrbMech::power(r, 4.0))*(3.0 * OrbMech::power(sin(phi), 2.0) - 1.0);
		a2theta = 3.0 * mu*OrbMech::power(R_b, 2.0) * JCoeff[0] / (OrbMech::power(r, 4.0))*sin(phi)*cos(phi);
		a3r = -2.0 * mu*OrbMech::power(R_b, 3.0) * JCoeff[1] / OrbMech::power(r, 5.0)*(5.0 * OrbMech::power(sin(phi), 3.0) - 3.0 * sin(phi));
		a3theta = 3.0 / 2.0 * mu*OrbMech::power(R_b, 3.0) * JCoeff[1] / OrbMech::power(r, 5)*(5.0 * OrbMech::power(sin(phi), 2.0) * cos(phi) - cos(phi));
		a4r = -5.0 / 8.0 * mu*OrbMech::power(R_b, 4.0) * JCoeff[2] / OrbMech::power(r, 6.0)*(35.0 * OrbMech::power(sin(phi), 4.0) - 30.0 * OrbMech::power(sin(phi), 2.0) + 3.0);
		a4theta = 5.0 / 2.0 * mu*OrbMech::power(R_b, 4.0) * JCoeff[2] / OrbMech::power(r, 6.0)*(7.0 * OrbMech::power(sin(phi), 3.0) * cos(phi) - 3.0 * sin(phi)*cos(phi));

		a_G = rdach*a0r - (rdach*a2r + thetadach*a2theta + rdach*a3r + thetadach*a3theta + rdach*a4r + thetadach*a4theta);
	}

	dfdt[0] = V.x;
	dfdt[1] = V.y;
	dfdt[2] = V.z;
	dfdt[3] = a_G.x;
	dfdt[4] = a_G.y;
	dfdt[5] = a_G.z;
}*/

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
		lambda = PI2 - acos(l / cos(phi));
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

MATRIX3 GetObliquityMatrix(OBJHANDLE plan, double t)
{
	double t0, T_p, L_0, e_rel, phi_0, T_s, e_ref, L_ref, L_rel, phi, e_ecl, L_ecl;
	MATRIX3 Rot1, Rot2, Rot3, Rot4, Rot5, Rot6, R_ref, R_rel, R_rot, Rot;
	VECTOR3 s;

	if (plan == oapiGetObjectByName("Earth"))
	{
		t0 = 51544.5;								//LAN_MJD, MJD of the LAN in the "beginning"
		T_p = -9413040.4;							//Precession Period
		L_0 = 0.00001553343;						//LAN in the "beginning"
		e_rel = 0.4090928023;						//Obliquity / axial tilt of the earth in radians
		phi_0 = 4.894942829;						//Sidereal Rotational Offset
		T_s = 86164.098904 / 24.0 / 60.0 / 60.0;	//Sidereal Rotational Period
		e_ref = 0;									//Precession Obliquity
		L_ref = 0;									//Precession LAN
	}
	else if (plan == oapiGetObjectByName("Moon"))
	{
		t0 = 51544.5;							//LAN_MJD, MJD of the LAN in the "beginning"
		T_p = -6793.468728092782;				//Precession Period
		L_0 = 1.71817749;						//LAN in the "beginning"
		e_rel = 0.026699886264850;				//Obliquity / axial tilt of the earth in radians
		phi_0 = 4.769465382;					//Sidereal Rotational Offset
		T_s = 2360588.15 / 24.0 / 60.0 / 60.0;	//Sidereal Rotational Period
		e_ref = 7.259562816e-005;				//Precession Obliquity
		L_ref = 0.4643456618;					//Precession LAN
	}

	L_rel = L_0 + PI2*(t - t0) / T_p;
	Rot1 = _M(cos(L_ref), 0.0, -sin(L_ref), 0.0, 1.0, 0.0, sin(L_ref), 0.0, cos(L_ref));
	Rot2 = _M(1.0, 0.0, 0.0, 0.0, cos(e_ref), -sin(e_ref), 0.0, sin(e_ref), cos(e_ref));
	R_ref = mul(Rot1, Rot2);
	Rot3 = _M(cos(L_rel), 0.0, -sin(L_rel), 0.0, 1.0, 0.0, sin(L_rel), 0.0, cos(L_rel));
	Rot4 = _M(1.0, 0.0, 0.0, 0.0, cos(e_rel), -sin(e_rel), 0.0, sin(e_rel), cos(e_rel));
	R_rel = mul(Rot3, Rot4);
	phi = phi_0 + PI2*(t - t0) / T_s + (L_0 - L_rel)*cos(e_rel);
	R_rot = _M(cos(phi), 0.0, -sin(phi), 0.0, 1.0, 0.0, sin(phi), 0.0, cos(phi));
	Rot = mul(R_ref, mul(R_rel, R_rot));
	s = mul(Rot, _V(0.0, 1.0, 0.0));
	e_ecl = acos(s.y);
	L_ecl = atan(-s.x / s.z);
	Rot5 = _M(cos(L_ecl), 0.0, -sin(L_ecl), 0.0, 1.0, 0.0, sin(L_ecl), 0.0, cos(L_ecl));
	Rot6 = _M(1.0, 0.0, 0.0, 0.0, cos(e_ecl), -sin(e_ecl), 0.0, sin(e_ecl), cos(e_ecl));
	return mul(Rot5, Rot6);
}

MATRIX3 J2000EclToBRCS(double mjd)
{
	double t1 = (mjd - 51544.5) / 36525.0;
	double t2 = t1*t1;
	double t3 = t2*t1;

	t1 *= 4.848136811095359e-6;
	t2 *= 4.848136811095359e-6;
	t3 *= 4.848136811095359e-6;

	double i = 2004.3109*t1 - 0.42665*t2 - 0.041833*t3;
	double r = 2306.2181*t1 + 0.30188*t2 + 0.017998*t3;
	double L = 2306.2181*t1 + 1.09468*t2 + 0.018203*t3;

	double rot = -r - PI05;
	double lan = PI05 - L;
	double inc = i;
	double obl = 0.4090928023;

	return mul(mul(_MRz(rot), _MRx(inc)), mul(_MRz(lan), _MRx(-obl)));
}

MATRIX3 _MRx(double a)
{
	double ca = cos(a), sa = sin(a);
	return _M(1.0, 0, 0, 0, ca, sa, 0, -sa, ca);
}

MATRIX3 _MRy(double a)
{
	double ca = cos(a), sa = sin(a);
	return _M(ca, 0, -sa, 0, 1.0, 0, sa, 0, ca);
}

MATRIX3 _MRz(double a)
{
	double ca = cos(a), sa = sin(a);
	return _M(ca, sa, 0, -sa, ca, 0, 0, 0, 1.0);
}

VECTOR3 Polar2Cartesian(double r, double lat, double lng)
{
	return _V(r*cos(lat)*cos(lng), r*cos(lat)*sin(lng), r*sin(lat));
}

VECTOR3 Polar2CartesianVel(double r, double lat, double lng, double r_dot, double lat_dot, double lng_dot)
{
	return _V(r_dot*cos(lat)*cos(lng) - lat_dot*r*sin(lat)*cos(lng) - r*lng_dot*cos(lat)*sin(lng), r_dot*cos(lat)*sin(lng) - r*lat_dot*sin(lat)*sin(lng) + r*lng_dot*cos(lat)*cos(lng), r_dot*sin(lat) + r*lat_dot*cos(lat));
}

int decimal_octal(int n) /* Function to convert decimal to octal */
{
	int rem, i = 1, octal = 0;
	while (n != 0)
	{
		rem = n % 8;
		n /= 8;
		octal += rem*i;
		i *= 10;
	}
	return octal;
}

double atan3(double x, double y)
{
	double a;

	a = atan2(x, y);
	if (a < 0)
	{
		a += PI2;
	}
	return a;
}

/*int OrbMech::rkf45(double* tout, double** yout, double* tspan, double* y0, int n, double tolerance)
{
	int ganzzahl = 0;
	double* a = NULL;
	double* c = NULL;
	double* c4 = NULL;
	double* c5 = NULL;
	double** b = NULL;
	double** f = NULL;
	a = new double[6];
	c = new double[6];
	c4 = new double[6];
	c5 = new double[6];
	b = new double*[6];
	f = new double*[n];
	for (int ac = 0; ac<n; ac++)
	{
		f[ac] = new double[6];
	}
	for (int i = 0; i<6; i++)
	{
		a[i] = 0.0;
		c[i] = 0.0;
		c4[i] = 0.0;
		c5[i] = 0.0;
		b[i] = new double[5];
		for (int j = 0; j<5; j++)
		{
			b[i][j] = 0.0;
		}
	}
	a[0] = 0;
	a[1] = 0.25;
	a[2] = 3;
	a[2] = a[2] / 8;
	a[3] = 12;
	a[3] = a[3] / 13;
	a[4] = 1;
	a[5] = 0.5;
	b[0][0] = 0.0;
	b[0][1] = 0.0;
	b[0][2] = 0.0;
	b[0][3] = 0.0;
	b[0][4] = 0.0;
	b[1][0] = 0.25;
	b[1][1] = 0.0;
	b[1][2] = 0.0;
	b[1][3] = 0.0;
	b[1][4] = 0.0;
	b[2][0] = 3;
	b[2][0] = b[2][0] / 32;
	b[2][1] = 9;
	b[2][1] = b[2][1] / 32;
	b[2][2] = 0.0;
	b[2][3] = 0.0;
	b[2][4] = 0.0;
	b[3][0] = 1932;
	b[3][0] = b[3][0] / 2197;
	b[3][1] = -7200;
	b[3][1] = b[3][1] / 2197;
	b[3][2] = 7296;
	b[3][2] = b[3][2] / 2197;
	b[3][3] = 0.0;
	b[3][4] = 0.0;
	b[4][0] = 439;
	b[4][0] = b[4][0] / 216;
	b[4][1] = -8;
	b[4][2] = 3680;
	b[4][2] = b[4][2] / 513;
	b[4][3] = -845;
	b[4][3] = b[4][3] / 4104;
	b[4][4] = 0.0;
	b[5][0] = -8;
	b[5][0] = b[5][0] / 27;
	b[5][1] = 2;
	b[5][2] = -3544;
	b[5][2] = b[5][2] / 2565;
	b[5][3] = 1859;
	b[5][3] = b[5][3] / 4104;
	b[5][4] = -11;
	b[5][4] = b[5][4] / 40;
	c4[0] = 25;
	c4[0] = c4[0] / 216;
	c4[1] = 0.0;
	c4[2] = 1408;
	c4[2] = c4[2] / 2565;
	c4[3] = 2197;
	c4[3] = c4[3] / 4104;
	c4[4] = -1;
	c4[4] = c4[4] / 5;
	c4[5] = 0.0;
	c5[0] = 16;
	c5[0] = c5[0] / 135;
	c5[1] = 0.0;
	c5[2] = 6656;
	c5[2] = c5[2] / 12825;
	c5[3] = 28561;
	c5[3] = c5[3] / 56430;
	c5[4] = -9;
	c5[4] = c5[4] / 50;
	c5[5] = 2;
	c5[5] = c5[5] / 55;
	double t0 = tspan[0];
	double tf = tspan[1];
	double tol = tolerance;
	double t = t0;
	double* y = NULL;
	y = new double[n];
	double* yi = NULL;
	yi = new double[n];
	double* y_inner = NULL;
	y_inner = new double[n];
	double* dfdt = NULL;
	dfdt = new double[n];
	double* vektor = NULL;
	vektor = new double[n];
	double* vektor2 = NULL;
	vektor2 = new double[n];
	double* te = NULL;
	te = new double[n];
	double* y_abs = NULL;
	y_abs = new double[n];
	double* te_abs = NULL;
	te_abs = new double[n];
	for (int k = 0; k<n; k++)
	{
		y_abs[k] = 0.0;
		te_abs[k] = 0.0;
		y[k] = 0.0;
		y_inner[k] = 0.0;
		dfdt[k] = 0.0;
		vektor[k] = 0.0;
		vektor2[k] = 0.0;
		te[n] = 0.0;
		for (int z = 0; z<6; z++)
		{
			f[k][z] = 0.0;
		}
	}
	for (int l = 0; l<n; l++)
	{
		y[l] = y0[l];
	}
	tout[ganzzahl] = t;
	for (int m = 0; m<n; m++)
	{
		yout[m][ganzzahl] = y[m];
	}
	double h = (tf - t0) / 100;
	double hmin, ti, t_inner, te_max, ymax, te_allowed, delta;
	for (int ab = 0; ab<6; ab++)
	{
		c[ab] = c4[ab] - c5[ab];
	}
	while (t<tf)
	{
		hmin = 16 * DBL_EPSILON;
		ti = t;
		for (int o = 0; o<n; o++)
		{
			yi[o] = y[o];
		}
		for (int p = 0; p<6; p++)
		{
			t_inner = ti + a[p] * h;
			for (int q = 0; q<n; q++)
			{
				y_inner[q] = yi[q];
			}
			for (int r = 0; r<p; r++)
			{
				for (int s = 0; s<n; s++)
				{
					y_inner[s] = y_inner[s] + h*b[p][r] * f[s][r];
				}
			}
			adfunc(dfdt, t_inner, y_inner);
			for (int u = 0; u<n; u++)
			{
				f[u][p] = dfdt[u];
			}
		}
		for (int ba = 0; ba<n; ba++)
		{
			for (int bb = 0; bb<6; bb++)
			{
				vektor2[ba] += f[ba][bb] * c[bb];
			}
		}
		for (int v = 0; v<n; v++)
		{
			te[v] = h*vektor2[v];
			te_abs[v] = te[v];
			y_abs[v] = y[v];
		}
		fabs_vektor(te_abs, n);
		fabs_vektor(y_abs, n);
		te_max = vektor_max(te_abs, n);
		ymax = vektor_max(y_abs, n);
		if (ymax>1)
		{
			te_allowed = tol*ymax;
		}
		else
		{
			te_allowed = tol*1.0;
		}
		delta = OrbMech::power((te_allowed / (te_max + DBL_EPSILON)), 0.2);
		if (te_max <= te_allowed)
		{
			if ((tf - t)<h)
			{
				h = tf - t;
			}
			t += h;
			ganzzahl += 1;
			tout[ganzzahl] = t;
			for (int w = 0; w<n; w++)
			{
				for (int x = 0; x<6; x++)
				{
					vektor[w] += c5[x] * f[w][x];
				}
			}
			for (int aa = 0; aa<n; aa++)
			{
				y[aa] = yi[aa] + h*vektor[aa];
				yout[aa][ganzzahl] = y[aa];
			}
		}
		if (delta>4)
		{
			h = h * 4;
		}
		else
		{
			h = h*delta;
		}
		//if (h<hmin)
		//{
		//	cout << "Step size below minimum.";
		//}
		for (int x = 0; x<n; x++)
		{
			vektor[x] = 0.0;
			vektor2[x] = 0.0;
			yi[x] = 0.0;
			y_inner[x] = 0.0;
		}
	}
	return ganzzahl;
}*/

VECTOR3 CALCGAR(MATRIX3 REFSM, MATRIX3 SMNB)
{
	VECTOR3 X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, A_MG;
	double sinOGA, cosOGA, sinMGA, cosMGA, sinIGA, cosIGA, OGA, IGA, MGA;

	X_SM = _V(REFSM.m11, REFSM.m12, REFSM.m13);
	Y_SM = _V(REFSM.m21, REFSM.m22, REFSM.m23);
	Z_SM = _V(REFSM.m31, REFSM.m32, REFSM.m33);
	X_NB = _V(SMNB.m11, SMNB.m12, SMNB.m13);
	Y_NB = _V(SMNB.m21, SMNB.m22, SMNB.m23);
	Z_NB = _V(SMNB.m31, SMNB.m32, SMNB.m33);

	A_MG = unit(crossp(X_NB, Y_SM));
	cosOGA = dotp(A_MG, Z_NB);
	sinOGA = dotp(A_MG, Y_NB);
	OGA = atan3(sinOGA, cosOGA);
	cosMGA = dotp(Y_SM, crossp(A_MG, X_NB));
	sinMGA = dotp(Y_SM, X_NB);
	MGA = atan3(sinMGA, cosMGA);
	cosIGA = dotp(A_MG, Z_SM);
	sinIGA = dotp(A_MG, X_SM);
	IGA = atan3(sinIGA, cosIGA);

	return _V(OGA, IGA, MGA);
}

MATRIX3 CALCSMSC(VECTOR3 GA)
{
	double IGA, OGA, MGA;
	VECTOR3 X_NB, Y_NB, Z_NB;

	OGA = GA.x;
	IGA = GA.y;
	MGA = GA.z;
	X_NB = unit(_V(cos(IGA)*cos(MGA), sin(MGA), -sin(IGA)*cos(MGA)));
	Z_NB = unit(_V(cos(IGA)*sin(OGA)*sin(MGA)+cos(OGA)*sin(IGA), -sin(OGA)*cos(MGA), cos(OGA)*cos(IGA)-sin(OGA)*sin(MGA)*sin(IGA)));
	Y_NB = crossp(Z_NB, X_NB);

	return _M(X_NB.x, X_NB.y, X_NB.z, Y_NB.x, Y_NB.y, Y_NB.z, Z_NB.x, Z_NB.y, Z_NB.z);
}

VECTOR3 CALCGTA(MATRIX3 des)
{
	VECTOR3 X_D, Y_D, Z_D, Z_D_apo;
	double tx, ty, tz, sintx, sinty, sintz, costx, costy, costz;

	X_D = _V(des.m11, des.m12, des.m13);
	Y_D = _V(des.m21, des.m22, des.m23);
	Z_D = _V(des.m31, des.m32, des.m33);
	Z_D_apo = unit(_V(-X_D.z, 0, X_D.x));

	sinty = Z_D_apo.x;
	costy = Z_D_apo.z;
	ty = atan2(sinty, costy);

	sintz = X_D.y;
	costz = Z_D_apo.z*X_D.x-Z_D_apo.x*X_D.z;
	tz = atan2(sintz, costz);

	costx = dotp(Z_D_apo, Z_D);
	sintx = dotp(Z_D_apo, Y_D);
	tx = atan2(sintx, costx);

	return _V(tx, ty, tz);
}

void CALCSXA(MATRIX3 SMNB, VECTOR3 S_SM, double &TA, double &SA)
{
	MATRIX3 SBNB, NBSB;
	VECTOR3 X_SB, Y_SB, Z_SB, S_SB, U_TPA;
	double a, sinSA, cosSA;

	X_SB = _V(1.0, 0.0, 0.0);
	Z_SB = _V(0.0, 0.0, 1.0);
	Y_SB = _V(0.0, 1.0, 0.0);

	a = -0.5676353234;
	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
	NBSB = transpose_matrix(SBNB);

	S_SB = mul(NBSB, mul(SMNB, S_SM));
	U_TPA = unit(crossp(Z_SB, S_SB));
	sinSA = dotp(U_TPA, -X_SB);
	cosSA = dotp(U_TPA, Y_SB);
	SA = atan3(sinSA, cosSA);
	TA = acos(dotp(Z_SB, S_SB));
}

void CALCCOASA(MATRIX3 SMNB, VECTOR3 S_SM, double &SPA, double &SXP) 
{
	MATRIX3 SBNB, NBSB;
	VECTOR3 X_SB, Y_SB, Z_SB, S_SB;
	double a;

	X_SB = _V(1.0, 0.0, 0.0);
	Z_SB = _V(0.0, 0.0, 1.0);
	Y_SB = _V(0.0, 1.0, 0.0);

	a = -PI05;
	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
	NBSB = transpose_matrix(SBNB);

	S_SB = mul(NBSB, mul(SMNB, S_SM));
	
	SXP = atan(S_SB.y / S_SB.z);//asin(S_SB.x);
	SPA = asin(S_SB.x);//atan(-S_SB.y / S_SB.z);
}

void periapo(VECTOR3 R, VECTOR3 V, double mu, double &apo, double &peri)
{
	double a, e, epsilon;
	VECTOR3 Ex,H;

	H = crossp(R, V);

	Ex = crossp(V, H) / mu - R / length(R);
	e = length(Ex);
	epsilon = 0.5*OrbMech::power(length(V), 2.0) - mu / length(R);
	a = -0.5*mu / epsilon;
	peri = (-e + 1.0)*a;
	apo = (e + 1.0)*a;
	if (apo < 0)
	{
		apo = DBL_MAX;
	}
}

MATRIX3 LaunchREFSMMAT(double lat, double lng, double mjd, double A_Z)
{
	VECTOR3 R_P, g_p, REFS0, REFS3, REFS6, E, S, U_Z;
	MATRIX3 Rot1;
	OBJHANDLE hEarth;

	hEarth = oapiGetObjectByName("Earth");
	Rot1 = GetRotationMatrix(hEarth, mjd);
	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)));
	g_p = -unit(R_P);
	U_Z = _V(0.0, 0.0, 1.0);
	REFS6 = unit(g_p);
	E = unit(crossp(REFS6, U_Z));
	S = unit(crossp(E, REFS6));
	REFS0 = E*sin(A_Z) + S*cos(A_Z);
	REFS3 = unit(crossp(REFS6, REFS0));

	REFS0 = rhmul(Rot1, REFS0);
	REFS3 = rhmul(Rot1, REFS3);
	REFS6 = rhmul(Rot1, REFS6);

	return _M(REFS0.x, REFS0.y, REFS0.z, REFS3.x, REFS3.y, REFS3.z, REFS6.x, REFS6.y, REFS6.z);
}

VECTOR3 DOI_calc(VECTOR3 R, VECTOR3 V, double r_LS, double h_p, double mu)
{
	double r, r_p;
	VECTOR3 U_R, U_V, U_Z, V_R;
	r = length(R);
	r_p = h_p + r_LS;
	U_R = unit(R);
	U_V = unit(V);
	U_Z = unit(crossp(U_V, U_R));
	V_R = crossp(U_R, U_Z)*sqrt(2.0*mu*r_p/(r*r+r*r_p));
	return V_R;
}

void LunarLandingPrediction(VECTOR3 R_D, VECTOR3 V_D, double t_D, double t_E, VECTOR3 R_LSA, double h_DP, double theta_F, double t_F, OBJHANDLE plan, double GETbase, double mu, double & t_DOI, double &t_PDI, double &t_L, VECTOR3 &DV_DOI, double &CR)
{
	double er,nmax,t_U,r_DP,r_P,r_A,r_D,v_D,a_D,t_H,tLMJD,alpha;
	VECTOR3 U_N, U_L, U_LS,V_DH,R_PP,V_PP,R_LS;
	MATRIX3 Rot;
	int n;

	er = 1.0;
	nmax = 15;
	n = 0;

	t_U = t_E - t_D;
	r_DP = length(R_LSA) + h_DP;

	while (abs(er) > 0.0005 && nmax >= n)
	{
		t_D += t_U;
		rv_from_r0v0(R_D, V_D, t_U, R_D, V_D, mu);
		r_P = r_DP;
		r_A = length(R_D);
		r_D = length(R_D);
		v_D = length(V_D);
		U_N = unit(crossp(R_D, V_D));
		a_D = mu*r_D / (2.0*mu - r_D*v_D*v_D);
		t_H = PI*sqrt(OrbMech::power(r_A + r_P, 3.0) / (8.0*mu));
		V_DH = DOI_calc(R_D, V_D, length(R_LSA), h_DP, mu);
		rv_from_r0v0(R_D, V_DH, t_H, R_PP, V_PP, mu);
		t_L = t_D + t_H + t_F;
		tLMJD = t_L / 24.0 / 3600.0 + GETbase;
		U_L = unit(R_PP)*cos(theta_F) + unit(crossp(U_N, R_PP))*sin(theta_F);
		Rot = GetRotationMatrix(plan, tLMJD);
		R_LS = rhmul(Rot, R_LSA);
		U_LS = unit(R_LS - U_N*dotp(U_N, R_LS));
		er = length(U_L - U_LS);
		alpha = sign(dotp(U_N, crossp(U_L, U_LS)))*acos(dotp(U_L, U_LS));
		t_U = alpha*sqrt(OrbMech::power(a_D, 3.0) / mu);
		n++;
	}
	t_DOI = t_D;
	t_PDI = t_DOI + t_H;
	DV_DOI = V_DH - V_D;
	CR = -length(R_LS)*sign(dotp(U_N, R_LS))*acos(dotp(unit(R_LS), U_LS));
}

void xaxislambert(VECTOR3 RA1, VECTOR3 VA1, VECTOR3 RP2off, double dt2, int N, bool tgtprograde, double mu, VECTOR3 &VAP2, double &zoff)
{
	VECTOR3 RPP1, RPP2, VAP1, i, j, k, dVLV1, dVLV2;
	double f1, f2, r1, r2, y;
	MATRIX3 Q_Xx;
	int nmax, n;

	j = unit(crossp(VA1, RA1));
	k = unit(-RA1);
	i = crossp(j, k);
	Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

	f2 = 1;
	n = 0;
	nmax = 10;

	RPP1 = RP2off;
	RPP2 = RP2off*(length(RP2off) + 10.0) / length(RP2off);

	VAP1 = OrbMech::elegant_lambert(RA1, VA1, RPP1, dt2, N, tgtprograde, mu);
	VAP2 = OrbMech::elegant_lambert(RA1, VA1, RPP2, dt2, N, tgtprograde, mu);

	r1 = length(RPP1);
	r2 = length(RPP2);

	while (abs(f2)>0.01 && nmax >= n)
	{
		dVLV1 = mul(Q_Xx, VAP1 - VA1);
		dVLV2 = mul(Q_Xx, VAP2 - VA1);

		f1 = dVLV1.z;
		f2 = dVLV2.z;

		y = r2 - f2*(r2 - r1) / (f2 - f1);
		VAP1 = VAP2;
		r1 = r2;
		r2 = y;

		VAP2 = OrbMech::elegant_lambert(RA1, VA1, unit(RP2off)*r2, dt2, N, tgtprograde, mu);

		n++;
	}

	zoff += length(RP2off) - r2;
}

/*void poweredflight(VESSEL* vessel, VECTOR3 R, VECTOR3 V, OBJHANDLE gravref, THRUSTER_HANDLE thruster, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &t_go)
{
	double v_ex, f_T, a_T, tau, L, S, J, mu, dV;
	VECTOR3 R_thrust, V_thrust, R_c1, V_c1, R_c2, V_c2, R_grav, V_grav, U_TD;

	dV = length(V_G);
	U_TD = unit(V_G);

	mu = GGRAV*oapiGetMass(gravref);

	v_ex = vessel->GetThrusterIsp0(thruster);
	f_T = vessel->GetThrusterMax0(thruster);
	a_T = f_T / m;
	tau = v_ex / a_T;

	L = dV;
	t_go = tau*(1.0 - exp(-L / v_ex));
	J = L*tau - v_ex*t_go;
	S = -J + t_go*L;

	R_thrust = U_TD*S;
	V_thrust = U_TD*L;

	R_c1 = R - R_thrust*1.0 / 10.0 - V_thrust*1.0 / 30.0*t_go;
	V_c1 = V + R_thrust*6.0 / 5.0 / t_go - V_thrust*1.0 / 10.0;

	rv_from_r0v0(R_c1, V_c1, t_go, R_c2, V_c2, mu);

	V_grav = V_c2 - V_c1;
	R_grav = R_c2 - R_c1 - V_c1*t_go;

	R_cutoff = R + V*t_go + R_grav + R_thrust;
	V_cutoff = V + V_grav + V_thrust;
}*/

void poweredflight(VECTOR3 R, VECTOR3 V, double mjd0, OBJHANDLE gravref, double f_T, double v_ex, double m, VECTOR3 V_G, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &m_cutoff, double &t_go)
{
	double dt, dt_max, a_T, tau, m0, mnow, dV, dVnow, t_remain, t;
	VECTOR3 U_TD, gp, g, R0, V0, Rnow, Vnow, dvdt;

	dV = length(V_G);
	U_TD = unit(V_G);
	R0 = R;
	V0 = V;
	m0 = m;
	t = 0.0;
	Rnow = R0;
	Vnow = V0;
	mnow = m;
	dVnow = dV;

	dt_max = 0.1;
	dt = 1.0;

	gp = gravityroutine(R0, gravref, mjd0);

	while (dt != 0.0)
	{
		a_T = f_T / mnow;
		tau = v_ex / a_T;
		t_remain = tau*(1.0 - exp(-dVnow / v_ex));
		dt = min(dt_max, t_remain);
		dvdt = U_TD*f_T / mnow*dt;

		Rnow = Rnow + (Vnow + gp*dt*0.5 + dvdt*0.5)*dt;
		g = gravityroutine(Rnow, gravref, mjd0);
		Vnow = Vnow + (g + gp)*dt*0.5 + dvdt;
		gp = g;
		dVnow -= length(dvdt);
		mnow -= f_T / v_ex*dt;
		t += dt;
	}
	R_cutoff = Rnow;
	V_cutoff = Vnow;
	m_cutoff = mnow;
	t_go = t;
	
}

VECTOR3 gravityroutine(VECTOR3 R, OBJHANDLE gravref, double mjd0)
{
	OBJHANDLE hEarth;
	VECTOR3 U_R, U_Z, g;
	double rr, mu;

	hEarth = oapiGetObjectByName("Earth");
	U_R = unit(R);
	MATRIX3 obli_E = OrbMech::GetObliquityMatrix(hEarth, mjd0);
	U_Z = mul(obli_E, _V(0, 1, 0));
	U_Z = _V(U_Z.x, U_Z.z, U_Z.y);

	rr = dotp(R, R);
	mu = GGRAV*oapiGetMass(gravref);

	if (gravref == hEarth)
	{
		double costheta, R_E, J2E;
		VECTOR3 g_b;

		costheta = dotp(U_R, U_Z);
		R_E = oapiGetSize(hEarth);
		J2E = oapiGetPlanetJCoeff(hEarth, 0);
		g_b = -(U_R*(1.0 - 5.0*costheta*costheta) + U_Z*2.0*costheta)*mu / rr*3.0 / 2.0*J2E*power(R_E, 2.0) / rr;
		g = -U_R*mu / rr + g_b;
	}
	else
	{
		g = -U_R*mu / rr;
	}
	return g;
}

void impulsive(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_T, double isp, double m, VECTOR3 DV, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff)
{
	VECTOR3 R_ig, V_ig, V_go, R_ref, V_ref, dV_go, R_d, V_d, R_p, V_p, i_z, i_y;
	double t_slip_old, mu, t_go, v_goz, dr_z, dt_go, m_p;
	int n, nmax;

	nmax = 100;
	t_slip = 0;
	t_slip_old = 1;
	dt_go = 1;
	mu = GGRAV*oapiGetMass(gravref);
	V_go = DV;
	R_ref = R;
	V_ref = V + DV;
	i_y = -unit(crossp(R_ref, V_ref));

	while (abs(t_slip - t_slip_old) > 0.01)
	{
		n = 0;
		//rv_from_r0v0(R, V, t_slip, R_ig, V_ig, mu);
		oneclickcoast(R, V, MJD, t_slip, R_ig, V_ig, gravref, gravref);
		while ((length(dV_go) > 0.01 || n < 2) && n <= nmax)
		{
			poweredflight(R_ig, V_ig, MJD, gravref, f_T, isp, m, V_go, R_p, V_p, m_p, t_go);
			//rv_from_r0v0(R_ref, V_ref, t_go + t_slip, R_d, V_d, mu);
			oneclickcoast(R_ref, V_ref, MJD, t_go + t_slip, R_d, V_d, gravref, gravref);
			i_z = unit(crossp(R_d, i_y));
			dr_z = dotp(i_z, R_d - R_p);
			v_goz = dotp(i_z, V_go);
			dt_go = -2.0 * dr_z / v_goz;
			dV_go = V_d - V_p;
			//dV_go = (V_go_apo - V_go)*0.5;
			V_go = V_go + dV_go;
			n++;
		}
		t_slip_old = t_slip;
		t_slip += dt_go*0.1;
	}
	if (n >= nmax)
	{
		sprintf(oapiDebugString(), "Iteration failed!");
	}
	//Llambda = V_go;

	//double apo, peri;
	//periapo(R_p, V_p, mu, apo, peri);

	VECTOR3 X, Y, Z, dV_LV, DV_P, DV_C, V_G;

	MATRIX3 Q_Xx;
	double theta_T;

	X = unit(crossp(crossp(R_ig, V_ig), R_ig));
	Y = unit(crossp(V_ig, R_ig));
	Z = -unit(R_ig);

	Q_Xx = _M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z);
	dV_LV = mul(Q_Xx, V_go);
	DV_P = X*dV_LV.x + Z*dV_LV.z;
	if (length(DV_P) != 0.0)
	{
		theta_T = -length(crossp(R_ig, V_ig))*length(dV_LV)*m / OrbMech::power(length(R_ig), 2.0) / f_T;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, Y))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + Y*dV_LV.y;
	}
	else
	{
		V_G = X*dV_LV.x + Y*dV_LV.y + Z*dV_LV.z;
	}
	Llambda = V_G;

	R_cutoff = R_p;
	V_cutoff = V_p;
	m_cutoff = m_p;
	MJD_cutoff = MJD + (t_go + t_slip) / 24.0 / 3600.0;
}

double findlatitude(VECTOR3 R, VECTOR3 V, double mjd, OBJHANDLE gravref, double lat, bool up, VECTOR3 &Rlat, VECTOR3 &Vlat)
{
	MATRIX3 Rot;
	VECTOR3 R0, V0, R0_equ, V0_equ, R1, V1, R1_equ, V1_equ, H, u;
	double dt, ddt, mu, Tguess, mjd0, sgn, sign2, inc, cosI, sinBeta, cosBeta, sinBeta2, cosBeta2, l1, l0, lat_now, dl, lat_des;
	int i;

	R0 = R;
	V0 = V;
	mjd0 = mjd;
	lat_des = lat;

	i = 0;
	dt = 0.0;
	ddt = 1.0;
	mu = GGRAV*oapiGetMass(gravref);
	Tguess = PI2 / sqrt(mu)*OrbMech::power(length(R0), 1.5);
	Rot = GetObliquityMatrix(gravref, mjd);
	if (up)
	{
		sgn = 1.0;
	}
	else
	{
		sgn = -1.0;
	}
	R0_equ = rhtmul(Rot, R0);
	V0_equ = rhtmul(Rot, V0);
	H = crossp(R0_equ, V0_equ);
	inc = acos(H.z / length(H));
	if (inc < abs(lat_des))
	{
		lat_des = inc;
	}

	while (abs(ddt) > 0.1)
	{
		oneclickcoast(R0, V0, mjd0, dt, R1, V1, gravref, gravref);
		R1_equ = rhtmul(Rot, R1);
		V1_equ = rhtmul(Rot, V1);

		H = crossp(R1_equ, V1_equ);
		cosI = H.z / length(H);
		if (acos(cosI) < abs(lat_des))
		{
			lat_des = inc;
		}
		sinBeta = cosI / cos(lat_des);
		cosBeta = sgn*sqrt(1.0 - sinBeta*sinBeta);
		l1 = atan2(tan(lat_des), cosBeta);

		u = unit(R1_equ);
		lat_now = atan(u.z / sqrt(u.x*u.x + u.y*u.y));
		if (V1_equ.z > 0.0)
		{
			sign2 = 1.0;
		}
		else
		{
			sign2 = -1.0;
		}
		sinBeta2 = cosI / cos(lat_now);
		cosBeta2 = sign2*sqrt(1.0 - sinBeta2*sinBeta2);
		l0 = atan2(tan(lat_now), cosBeta2);

		dl = l1 - l0;
		ddt = Tguess*dl / PI2;
		if (abs(ddt) > 100.0)
		{
			ddt = sign(ddt)*100.0;
		}
		dt += ddt;
		i++;
	}
	Rlat = R1;
	Vlat = V1;

	return dt;
}

void checkstar(MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &trunnion, double &shaft)
{
	MATRIX3 SMNB, Q1, Q2, Q3;
	double OGA, IGA, MGA, TA, SA;
	VECTOR3 U_LOS, USTAR, S_SM;
	int star;

	OGA = IMU.x;
	IGA = IMU.y;
	MGA = IMU.z;

	Q1 = _MRy(IGA);
	Q2 = _MRz(MGA);
	Q3 = _MRx(OGA);

	SMNB = mul(Q3, mul(Q2, Q1));

	U_LOS = ULOS(REFSMMAT, SMNB, 0.0, 0.0);
	star = OrbMech::FindNearestStar(U_LOS, R_C, R_E, 50.0*RAD);

	if (star == -1)
	{
		staroct = 0;

		trunnion = 0;
		shaft = 0;
	}
	else
	{

		USTAR = navstars[star];

		S_SM = mul(REFSMMAT, USTAR);
		CALCSXA(SMNB, S_SM, TA, SA);

		staroct = decimal_octal(star + 1);

		trunnion = TA;
		shaft = SA;
	}

	//sprintf(oapiDebugString(), "%d, %f, %f", staroct, SA*DEG, TA*DEG);
}

void coascheckstar(MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &spa, double &sxp)
{
	MATRIX3 SMNB, Q1, Q2, Q3;
	double OGA, IGA, MGA;
	VECTOR3 U_LOS, USTAR, S_SM;
	int star;

	OGA = IMU.x;
	IGA = IMU.y;
	MGA = IMU.z;

	Q1 = _MRy(IGA);
	Q2 = _MRz(MGA);
	Q3 = _MRx(OGA);

	SMNB = mul(Q3, mul(Q2, Q1));

	U_LOS = ULOS(REFSMMAT, SMNB, 57.47*RAD, 0.0);
	star = OrbMech::FindNearestStar(U_LOS, R_C, R_E, 10.0*RAD);//31.5*RAD);

	if (star == -1)
	{
		staroct = 0;

		spa = 0;
		sxp = 0;
	}
	else
	{

		USTAR = navstars[star];

		S_SM = mul(REFSMMAT, USTAR);
		CALCCOASA(SMNB, S_SM, spa, sxp);

		staroct = decimal_octal(star + 1);

	}

	//sprintf(oapiDebugString(), "%d, %f, %f", staroct, SA*DEG, TA*DEG);
}

double imulimit(double a)
{
	if (a > 359.5)
	{
		return a - 359.5;
	}
	return a;
}

MATRIX3 tensorp(VECTOR3 u, VECTOR3 v)
{
	return _M(u.x*v.x, u.x*v.y, u.x*v.z, u.y*v.x, u.y*v.y, u.y*v.z, u.z*v.x, u.z*v.y, u.z*v.z);
}

MATRIX3 skew(VECTOR3 u)
{
	return _M(0.0, -u.z, u.y, u.z, 0.0, -u.x, -u.y, u.x, 0.0);
}

VECTOR3 RotateVector(VECTOR3 rotaxis, double angle, VECTOR3 vec)
{
	VECTOR3 k = unit(rotaxis);
	return vec*cos(angle) + crossp(k, vec)*sin(angle) + k*dotp(k,vec)*(1.0 - cos(angle));
}

double round(double number)
{
	return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

double trunc(double d)
{
	return (d > 0) ? floor(d) : ceil(d);
}

double quadratic(double *T, double *DV)
{
	double a, b, x;

	a = -2.0*(-DV[0] * T[1] + DV[0] * T[2] + DV[1] * T[0] - DV[1] * T[2] - DV[2] * T[0] + DV[2] * T[1]) / ((T[1] - T[0])*(T[2] - T[0])*(T[1] - T[2]));
	b = (DV[0] * T[1] * T[1] - DV[0] * T[2] * T[2] - DV[1] * T[0] * T[0] + DV[1] * T[2] * T[2] + DV[2] * T[0] * T[0] - DV[2] * T[1] * T[1]) / ((T[0] - T[1])*(T[0] - T[2])*(T[2] - T[1]));

	x = -b / a;

	return x;
}

}

CoastIntegrator::CoastIntegrator(VECTOR3 R00, VECTOR3 V00, double mjd0, double deltat, OBJHANDLE planet, OBJHANDLE outplanet)
{
	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	this->planet = planet;
	this->outplanet = outplanet;

	K = 0.3;
	dt_lim = 4000;
	R_E = oapiGetSize(planet);
	mu = oapiGetMass(planet)*GGRAV;
	jcount = oapiGetPlanetJCoeffCount(planet);
	JCoeff = new double[jcount];
	for (int i = 0; i < jcount; i++)
	{
		JCoeff[i] = oapiGetPlanetJCoeff(planet, i);
	}

	this->R00 = R00;
	this->V00 = V00;
	this->mjd0 = mjd0;
	R0 = R00;
	V0 = V00;
	t_0 = 0;
	t = 0;
	tau = 0;
	t_F = t_0 + deltat;
	delta = _V(0, 0, 0);// [0 0 0]';
	nu = _V(0, 0, 0);// [0 0 0]';
	R_CON = R0;
	V_CON = V0;
	x = 0;
	if (planet == hEarth)
	{
		r_MP = 7178165.0;
		r_dP = 80467200.0;
		mu_Q = GGRAV*oapiGetMass(hMoon);
		rect1 = 0.75*OrbMech::power(2.0, 22.0);
		rect2 = 0.75*OrbMech::power(2.0, 3.0);
		P = 0;
	}
	else
	{
		r_MP = 2538090.0;
		r_dP = 16093440.0;
		mu_Q = GGRAV*oapiGetMass(hEarth);
		rect1 = 0.75*OrbMech::power(2.0, 18.0);
		rect2 = 0.75*OrbMech::power(2.0, -1.0);
		P = 1;
	}
	hSun = oapiGetObjectByName("Sun");
	mu_S = GGRAV*oapiGetMass(hSun);

	MATRIX3 obli_E = OrbMech::GetObliquityMatrix(hEarth, mjd0);
	U_Z_E = mul(obli_E, _V(0, 1, 0));
	U_Z_E = _V(U_Z_E.x, U_Z_E.z, U_Z_E.y);

	MATRIX3 obli_M = OrbMech::GetObliquityMatrix(hMoon, mjd0);
	U_Z_M = mul(obli_M, _V(0, 1, 0));
	U_Z_M = _V(U_Z_M.x, U_Z_M.z, U_Z_M.y);

	cMoon = oapiGetCelbodyInterface(hMoon);
	cEarth = oapiGetCelbodyInterface(hEarth);
	cSun = oapiGetCelbodyInterface(hSun);

	R_QC = R0;
	r_SPH = 64373760.0;

	B = 1;

	double *EarthPos;
	EarthPos = new double[12];
	VECTOR3 EarthVec, EarthVecVel;

	cEarth->clbkEphemeris(mjd0 + t_F/2.0/24.0/3600.0, EPHEM_TRUEPOS | EPHEM_TRUEVEL, EarthPos);

	EarthVec = OrbMech::Polar2Cartesian(EarthPos[2] * AU, EarthPos[1], EarthPos[0]);
	EarthVecVel = OrbMech::Polar2CartesianVel(EarthPos[2] * AU, EarthPos[1], EarthPos[0], EarthPos[5] * AU, EarthPos[4], EarthPos[3]);
	R_ES0 = -EarthVec;
	V_ES0 = -EarthVecVel;
	W_ES = length(crossp(R_ES0, V_ES0) / OrbMech::power(length(R_ES0), 2.0));
}

bool CoastIntegrator::iteration()
{
	double rr, dt_max, dt, h, x_apo, gamma, s, alpha_N, x_t, Y, r_qc;
	VECTOR3 alpha, R_apo, V_apo, R, a_d, ff;
	VECTOR3 k[3];

	R = R_CON;
	rr = length(R_CON);
	r_qc = length(R_QC);
	if (rr < r_MP)
	{
		M = 0;
	}
	else
	{
		M = 1;
	}
	dt_max = 0.3*min(dt_lim, min(K*OrbMech::power(rr, 1.5) / sqrt(mu), K*OrbMech::power(r_qc, 1.5) / sqrt(mu_Q)));
	Y = OrbMech::sign(t_F - t);
	dt = Y*min(abs(t_F - t), dt_max);

	if (M == 1)
	{
		if (P == 1)
		{
			if (rr > r_SPH)
			{
				double MJD, *MoonPos;
				VECTOR3 R_EM, V_PQ;

				MoonPos = new double[12];

				MJD = mjd0 + t / 86400.0;
				cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);

				if (B == 1)
				{
					R_EM = _V(MoonPos[0], MoonPos[2], MoonPos[1]);
					R_PQ = -R_EM;
				}
				V_PQ = -_V(MoonPos[3], MoonPos[5], MoonPos[4]);
				R_CON = R_CON - R_PQ;
				V_CON = V_CON - V_PQ;
				planet = hEarth;

				R_E = oapiGetSize(planet);
				mu = oapiGetMass(planet)*GGRAV;
				jcount = oapiGetPlanetJCoeffCount(planet);
				JCoeff = new double[jcount];
				for (int i = 0; i < jcount; i++)
				{
					JCoeff[i] = oapiGetPlanetJCoeff(planet, i);
				}

				r_MP = 7178165.0;
				r_dP = 80467200.0;
				mu_Q = GGRAV*oapiGetMass(hMoon);
				rect1 = 0.75*OrbMech::power(2.0, 22.0);
				rect2 = 0.75*OrbMech::power(2.0, 3.0);
				P = 0;
				R0 = R_CON + delta;
				V0 = V_CON + nu;
				R_CON = R0;
				V_CON = V0;
				delta = _V(0, 0, 0);
				nu = _V(0, 0, 0);// [0 0 0]';
				x = 0;
				tau = 0;
			}
		}
		else
		{
			double MJD, *MoonPos;
			VECTOR3 R_EM, V_PQ;

			MoonPos = new double[12];

			MJD = mjd0 + t / 86400.0;
			cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);

			if (B == 1)
			{
				R_EM = _V(MoonPos[0], MoonPos[2], MoonPos[1]);
				R_PQ = R_EM;
				R_QC = R - R_PQ;
			}
			if (length(R_QC) < r_SPH)
			{
				V_PQ = _V(MoonPos[3], MoonPos[5], MoonPos[4]);
				R_CON = R_CON - R_PQ;
				V_CON = V_CON - V_PQ;
				planet = hMoon;

				R_E = oapiGetSize(planet);
				mu = oapiGetMass(planet)*GGRAV;
				jcount = oapiGetPlanetJCoeffCount(planet);
				JCoeff = new double[jcount];
				for (int i = 0; i < jcount; i++)
				{
					JCoeff[i] = oapiGetPlanetJCoeff(planet, i);
				}

				r_MP = 2538090.0;
				r_dP = 16093440.0;
				mu_Q = GGRAV*oapiGetMass(hEarth);
				rect1 = 0.75*OrbMech::power(2.0, 18.0);
				rect2 = 0.75*OrbMech::power(2.0, -1.0);
				P = 1;
				R0 = R_CON + delta;
				V0 = V_CON + nu;
				R_CON = R0;
				V_CON = V0;
				delta = _V(0, 0, 0);
				nu = _V(0, 0, 0);// [0 0 0]';
				x = 0;
				tau = 0;
			}
		}
	}
	if (length(delta) / length(R_CON) > 0.01 || length(delta) > rect1 || length(nu) > rect2)
	{
		R0 = R_CON + delta;
		V0 = V_CON + nu;
		R_CON = R0;
		V_CON = V0;
		delta = _V(0, 0, 0);
		nu = _V(0, 0, 0);// [0 0 0]';
		x = 0;
		tau = 0;
	}
	h = 0;
	alpha = delta;
	R_apo = R_CON;
	V_apo = V_CON;
	x_apo = x;
	for (int j = 0; j < 3; j++)
	{
		R = R_CON + alpha;
		a_d = adfunc(R);
		ff = f(alpha, R, a_d);
		k[j] = ff;
		if (j < 2)
		{
			h = h + 0.5*dt;
			alpha = delta + (nu + ff*h*0.5)*h;
			t = t + 0.5*dt;
			tau = tau + 0.5*dt;
			//tau = t - t_0;
			//R_apo = R_CON*(tau - dt / 2);
			//	V_apo = V_CON*(tau - dt / 2);
			//x_tau = x*(tau - dt / 2);
			s = sqrt(mu) / length(R_apo)*0.5*dt;
			gamma = dotp(R_apo, V_apo) / (length(R_apo)*sqrt(mu)*2.0);
			alpha_N = 2.0 / length(R0) - OrbMech::power(length(V0), 2.0) / mu;
			x_t = x_apo + s*(1.0 - gamma*s*(1.0 - 2.0 * gamma*s) - 1.0 / 6.0 * (1.0 / length(R_apo) - alpha_N)*s*s);
			//[R_CON, V_CON, x] = rv_from_r0v0(R0, V0, tau, mu, x_t);
			OrbMech::rv_from_r0v0(R0, V0, tau, R_CON, V_CON, mu, x_t);
		}
	}
	delta = delta + (nu + (k[0] + k[1] * 2.0)*dt*1.0 / 6.0)*dt;
	nu = nu + (k[0] + k[1] * 4.0 + k[2]) * 1.0 / 6.0 *dt;

	if (abs(t - t_F) < 1e-6)
	{
		R2 = R_CON + delta;
		V2 = V_CON + nu;

		if (outplanet == NULL)
		{
			outplanet = planet;
		}
		else if (planet != outplanet)
		{
			double MJD, *MoonPos;
			VECTOR3 R_EM, V_PQ, V_EM;

			MoonPos = new double[12];

			MJD = mjd0 + t / 86400.0;
			cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);

			R_EM = _V(MoonPos[0], MoonPos[2], MoonPos[1]);
			V_EM = _V(MoonPos[3], MoonPos[5], MoonPos[4]);

			if (planet == hEarth)
			{
				R_PQ = R_EM;
				V_PQ = V_EM;
			}
			else
			{
				R_PQ = -R_EM;
				V_PQ = -V_EM;
			}

			R2 = R2 - R_PQ;
			V2 = V2 - V_PQ;
		}

		return true;
	}
	return false;
}

void CoastIntegrator::SolarEphemeris(double t, VECTOR3 &R_ES, VECTOR3 &V_ES)
{
	R_ES = R_ES0*cos(W_ES*t)+crossp(R_ES0,unit(crossp(R_ES0, V_ES0)))*sin(W_ES*t);
	V_ES = V_ES0;
}

VECTOR3 CoastIntegrator::f(VECTOR3 alpha, VECTOR3 R, VECTOR3 a_d)
{
	VECTOR3 R_CON;
	double q;

	R_CON = R - alpha;
	q = dotp((alpha - R*2.0), alpha) / (OrbMech::power(length(R), 2.0));
	return -(R*fq(q) + alpha)*mu / OrbMech::power(length(R_CON), 3.0) + a_d;
}

double CoastIntegrator::fq(double q)
{
	return q*(3.0 + 3.0 * q + q*q) / (1.0 + OrbMech::power(1 + q, 1.5));
}

VECTOR3 CoastIntegrator::adfunc(VECTOR3 R)
{
	double r, costheta, P2, P3, P4, P5;
	VECTOR3 U_R, U_Z, a_dP, a_d, a_dQ, a_dS;
	a_dP = _V(0, 0, 0);
	r = length(R);
	if (r < r_dP)// && planet == hEarth)
	{
		U_R = unit(R);
		if (planet == hEarth)
		{
			U_Z = U_Z_E;//_V(0, 0, 1.0);// [0 0 1]'; 
		}
		else
		{
			U_Z = U_Z_M;//_V(obli.m12, obli.m22, obli.m32);
		}
		costheta = dotp(U_R, U_Z);
		P2 = 3.0 * costheta;
		P3 = 0.5*(15.0*costheta*costheta - 3.0);
		a_dP += (U_R*P3 - U_Z*P2)*JCoeff[0] * OrbMech::power(R_E / r, 2.0);
		if (jcount > 1)
		{
			P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
			a_dP += (U_R*P4 - U_Z*P3)*JCoeff[1] * OrbMech::power(R_E / r, 3.0);
			if (jcount > 2)
			{
				P5 = 0.25*(9.0*costheta*P4 - 5.0 * P3);
				a_dP += (U_R*P5 - U_Z*P4)*JCoeff[2] * OrbMech::power(R_E / r, 4.0);
			}
		}
		a_dP *= mu / OrbMech::power(r, 2.0);
	}
	if (M == 1)
	{
		double q_Q, q_S, MJD;
		VECTOR3 R_SC, R_PS, R_EM, R_ES, V_ES;
		double *MoonPos;
		MoonPos = new double[12];

		MJD = mjd0 + t / 86400.0;

		cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS, MoonPos);
		SolarEphemeris(t - t_F/2.0, R_ES, V_ES);
		R_EM = _V(MoonPos[0], MoonPos[2], MoonPos[1]);

		if (planet == hEarth)
		{
			R_PQ = R_EM;
			R_PS = R_ES;
		}
		else
		{
			R_PQ = -R_EM;
			R_PS = R_ES - R_EM;
		}
		R_QC = R - R_PQ;
		R_SC = R - R_PS;

		q_Q = dotp(R - R_PQ*2.0, R) / OrbMech::power(length(R_PQ), 2.0);
		q_S = dotp(R - R_PS*2.0, R) / OrbMech::power(length(R_PS), 2.0);
		a_dQ = -(R_PQ*fq(q_Q) + R)*mu_Q / OrbMech::power(length(R_QC), 3.0);
		a_dS = -(R_PS*fq(q_S) + R)*mu_S / OrbMech::power(length(R_SC), 3.0);
		B = 0;
		a_d = a_dP + a_dQ + a_dS;
	}
	else
	{
		a_d = a_dP;
	}
	return a_d;
}

MATRIX3 operator+(MATRIX3 a, MATRIX3 b)
{
	return _M(a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13, a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23, a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33);
}

VECTOR3 rhmul(const MATRIX3 &A, const VECTOR3 &b)	//For the left handed Orbiter matrizes, A is left handed, b is right handed, result is right handed
{
	return _V(
		A.m11*b.x + A.m12*b.z + A.m13*b.y,
		A.m31*b.x + A.m32*b.z + A.m33*b.y,
		A.m21*b.x + A.m22*b.z + A.m23*b.y);
}

VECTOR3 rhtmul(const MATRIX3 &A, const VECTOR3 &b)
{
	return _V(
		A.m11*b.x + A.m21*b.z + A.m31*b.y,
		A.m13*b.x + A.m23*b.z + A.m33*b.y,
		A.m12*b.x + A.m22*b.z + A.m32*b.y);
}