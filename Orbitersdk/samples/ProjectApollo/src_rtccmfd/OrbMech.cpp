#include "OrbMech.h"
#include <limits>
#include <vector>

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

	double TrueToMeanAnomaly(double ta, double eccdp)
	{
		double ma = 0.0;
		double E = TrueToEccentricAnomaly(ta, eccdp);
		ma = E - eccdp * sin(E);
		if (ma < 0.0)
			ma = ma + PI2;
		return ma;
	}

	double TrueToEccentricAnomaly(double ta, double ecc)
	{
		double ea;
		double cosTa = cos(ta);
		double eccCosTa = ecc * cosTa;
		double sinEa = (sqrt(1.0 - ecc * ecc)*sin(ta)) / (1.0 + eccCosTa);
		double cosEa = (ecc + cosTa) / (1.0 + eccCosTa);
		ea = atan2(sinEa, cosEa);

		if (ea < 0.0)
			ea = ea + PI2;

		return ea;
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

	double round_to(double value, double precision)
	{
		return round(value / precision) * precision;
	}

	void SStoHHMMSS(double time, int &hours, int &minutes, double &seconds, double precision)
	{
		time = round_to(time, precision);

		double mins;
		hours = (int)trunc(time / 3600.0);
		mins = fmod(time / 60.0, 60.0);
		minutes = (int)trunc(mins);
		seconds = (mins - minutes) * 60.0;
	}

	// Format time to HHH:MM:SS.
	void format_time(char *buf, double time)
	{
		buf[0] = 0; // Clobber
		if (time < 0) { return; } // don't do that

		int hours, minutes;
		double seconds;

		SStoHHMMSS(time, hours, minutes, seconds);

		sprintf(buf, "%03d:%02d:%02.0lf", hours, minutes, seconds);
	}

	// Format precise time.
	void format_time_prec(char *buf, double time)
	{
		buf[0] = 0; // Clobber
		if (time < 0) { return; } // don't do that

		int hours, minutes;
		double seconds;

		SStoHHMMSS(time, hours, minutes, seconds, 0.01);

		sprintf(buf, "HRS XXX%03d\nMIN XXXX%02d\nSEC XX%05.2f", hours, minutes, seconds);
	}

	void adbar_from_rv(double rmag, double vmag, double rtasc, double decl, double fpav, double az, VECTOR3 &R, VECTOR3 &V)
	{
		R = _V(cos(decl)*cos(rtasc), cos(decl)*sin(rtasc), sin(decl))*rmag;
		V.x = vmag*(cos(rtasc)*(-cos(az)*sin(fpav)*sin(decl) + cos(fpav)*cos(decl)) - sin(az)*sin(fpav)*sin(rtasc));
		V.y = vmag*(sin(rtasc)*(-cos(az)*sin(fpav)*sin(decl) + cos(fpav)*cos(decl)) + sin(az)*sin(fpav)*cos(rtasc));
		V.z = vmag*(cos(az)*cos(decl)*sin(fpav) + cos(fpav)*sin(decl));
	}

	void rv_from_adbar(VECTOR3 R, VECTOR3 V, double &rmag, double &vmag, double &rtasc, double &decl, double &fpav, double &az)
	{
		VECTOR3 u;

		u = unit(R);
		rmag = length(R);
		vmag = length(V);
		fpav = acos2(dotp(unit(R), unit(V)));
		decl = atan2(u.z, sqrt(u.x*u.x + u.y*u.y));
		rtasc = atan2(u.y, u.x);
		az = atan2(R.x*V.y - R.y*V.x, V.z*rmag - R.z*dotp(R, V) / rmag);
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

int MarscherEquationInversion(double sin_theta, double cos_theta, double cot_gamma, double r1, double alpha_N, double p_N, double &x, double &xi, double &c1, double &c2)
{
	// Marscher equation inversion
	// INPUTS:
	// sin_theta: sine of true anomaly difference
	// cos_theta: cosine of true anomaly difference
	// cot_gamma: cotangent of flight path angle (measured from local vertical)
	// r1: Radius magnitude at initial position
	// alpha_N: Ratio of magnitude of initial position vector to semi-major axis
	// p_N: Ratio of semi-latus rectum to initial position vector magnitude
	// OUTPUTS:
	// x: Universal anomaly
	// xi: Product of alpha_N and x squared
	// c1 and c2: Intermediate variables

	double W[4], a;
	int n;
	bool W1MAX; //W[0] is near infinite

	const double C_A_MAX = 79.0;
	const double C_TOL = 1e-7;

	//Divisor safety
	W[0] = 1.0 - cos_theta;

	W1MAX = false;
	if (abs(W[0]) < 1e-10)
	{
		W1MAX = true;
		if (sin_theta >= 0.0)
		{
			W[0] = 1e10;
		}
		else
		{
			W[0] = -1e10;
		}
	}
	else
	{
		W[0] = sqrt(p_N) * (sin_theta / W[0] - cot_gamma);
	}

	// Error checking for physically impossible solution
	if (alpha_N < 0.0)
	{
		if (W[0] <= 0.0)
		{
			return 1;
		}
		if (!W1MAX && W[0] * W[0] + alpha_N <= 0.0)
		{
			return 2;
		}
	}

	if (abs(W[0]) <= 1.0)
	{
		for (n = 0; n < 3; n++)
		{
			W[n + 1] = sqrt(W[n] * W[n] + alpha_N) + abs(W[n]);
		}
		a = 1.0 / W[3];
	}
	else
	{
		double V[4], W1inv;

		W1inv = abs(sin_theta / (sqrt(p_N) * (1.0 + cos_theta - sin_theta * cot_gamma)));
		V[0] = 1.0;

		for (n = 0; n < 3; n++)
		{
			V[n + 1] = sqrt(V[n] * V[n] + alpha_N * W1inv * W1inv) + V[n];
		}
		a = W1inv / V[3];
	}

	double C_A, C_B, C_C, C_D, X_N;

	C_A = C_B = X_N = 1.0;
	C_C = alpha_N * a * a;
	do
	{
		C_A += 2.0;
		C_B = -C_B * C_C;
		C_D = C_B / C_A;
		X_N = X_N + C_D;
		if (C_A >= C_A_MAX)
		{
			//Series nonconvergent
			return 3;
		}
	} while (abs(C_D) >= C_TOL);

	X_N = 16.0 * a * X_N;
	if (W[0] <= 0.0)
	{
		X_N = PI2 / sqrt(alpha_N) - X_N;
	}
	xi = alpha_N * X_N * X_N;
	x = sqrt(r1) * X_N;
	c1 = sqrt(r1 * p_N) * cot_gamma;
	c2 = 1.0 - alpha_N;

	return 0;
}

int time_theta(VECTOR3 R1, VECTOR3 V1, double dtheta, double mu, double &dt)
{
	//Wrapper function in case the output state vector isn't needed
	VECTOR3 R2, V2;

	return time_theta(R1, V1, dtheta, mu, R2, V2, dt);
}

int time_theta(VECTOR3 R1, VECTOR3 V1, double dtheta, double mu, VECTOR3 &R2, VECTOR3 &V2, double &dt)
{
	// INPUTS:
	// R1: Input position vector
	// V1: Input velocity vector
	// dtheta: Change in true anomaly
	// mu: specific gravitation parameter
	// OUTPUTS:
	// return: 0 = no errors, 1 = multiple orbits requested for hyperbolic orbit, 2 = orbit too nearly rectilinear, 3 and 4 = no physically realizable solution (hyperbolic)
	// R2: Output position vector
	// V2: Output velocity vector
	// dt: time equivalent to input angle dtheta

	// Handle zero case first
	if (dtheta == 0.0)
	{
		dt = 0.0;
		R2 = R1;
		V2 = V1;
		return 0;
	}

	// Internally this function will only handle positive dtheta, so set a flag if the angle is negative
	bool negative;

	// Handle negative angle
	if (dtheta < 0)
	{
		dtheta = -dtheta;
		V1 = -V1;
		negative = true;
	}
	else
	{
		negative = false;
	}

	int n;

	// Number of orbits
	n = (int)(dtheta / PI2);
	dtheta = fmod(dtheta, PI2);

	double r1, v1, alpha;

	// Initial position magnitude
	r1 = length(R1);
	// Initial velocity magnitude
	v1 = length(V1);
	// Reciprocal of semi-major axis
	alpha = 2.0 / r1 - v1*v1 / mu;

	// Error check: Cannot request multiple orbits for hyperbolic orbit
	if (n > 0 && alpha <= 0.0)
	{
		return 1;
	}

	//Geometric parameters
	VECTOR3 I_R_1, I_V_1;
	double sin_gamma, cos_gamma, cot_gamma, C3, alpha_N, p_N, x, xi, c1, c2, S, C, r2;
	int err;

	//Unit position vector
	I_R_1 = R1 / r1;
	//Unit velocity vector
	I_V_1 = V1 / v1;

	//Sine of flight path angle (measured from local vertical)
	sin_gamma = length(crossp(I_R_1, I_V_1));

	//Rectilinear orbit check
	if (abs(sin_gamma) < 1e-12)
	{
		return 2;
	}

	//Cosine of flight path angle (measured from local vertical)
	cos_gamma = dotp(I_R_1, I_V_1);
	//Cotangent of flight path angle (measured from local vertical)
	cot_gamma = cos_gamma / sin_gamma;
	//Energy
	C3 = r1*v1*v1 / mu;
	//Ratio of magnitude of initial position vector to semi-major axis
	alpha_N = 2.0 - C3;
	//Ratio of semi-latus rectum to initial position vector magnitude
	p_N = C3 * pow(sin_gamma, 2);

	//Marscher equation inversion
	err = MarscherEquationInversion(sin(dtheta), cos(dtheta), cot_gamma, r1, alpha_N, p_N, x, xi, c1, c2);
	if (err)
	{
		//Error return. Plus 2 so that time_theta returns a unique error code for each error type
		return err + 2;
	}

	//Stumpff functions. TBD: These Stumpff functions can run into numerical trouble very close to parabolic orbits
	S = stumpS(xi);
	C = stumpC(xi);
	dt = (c1*x*x*C + x * (c2*x*x*S + r1)) / sqrt(mu);

	//State vector
	R2 = R1 * (1.0 - x*x / r1 * C) + V1 * (dt - x*x*x / sqrt(mu)*S);
	r2 = length(R2);
	V2 = R1 * (sqrt(mu) / (r1*r2)*x*(xi*S - 1.0)) + V1 * (1.0 - x * x / r2 * C);

	//Multiple orbits
	if (n > 0)
	{
		double P;

		//Calculate orbital period
		P = PI2 / (pow(alpha, 1.5) * sqrt(mu));
		dt = dt + P * (double)n;
	}

	//If input angle was negative, reverse the output
	if (negative)
	{
		dt = -dt;
		V2 = -V2;
	}

	return 0;
}

void ra_and_dec_from_r(VECTOR3 R, double &ra, double &dec)
{
	double r, l, m, n;

	r = length(R);
	l = R.x / r;
	m = R.y / r;
	n = R.z / r;

	dec = asin(n);

	if (m > 0)
	{
		ra = acos(l / cos(dec));
	}
	else
	{
		ra = PI2 - acos(l / cos(dec));
	}
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
	double error2, ratio, C, S, F, dFdx, x, Z, ddFddx, delta_n;
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
		Z = a * x*x;
		C = stumpC(Z);
		S = stumpS(Z);
		F = ro*vro / sqrt(mu)*x*x*C + (1.0 - a*ro)*OrbMech::power(x, 3.0)*S + ro*x - sqrt(mu)*dt;
		dFdx = ro*vro / sqrt(mu)*x*(1.0 - a*x*x*S) + (1.0 - a*ro)*x*x*C + ro;
		ddFddx = (1.0 - ro * a)*(1.0 - S * Z)*x + ro * vro / sqrt(mu)*(1.0 - C * Z);
		//ratio = F / dFdx;
		delta_n = 2.0 * sqrt(abs(4.0 * pow(dFdx, 2) - 5.0 * F*ddFddx));
		ratio = 5.0 * F / (dFdx + sign(dFdx)*delta_n);
		x = x - ratio;
	}
	return x;
}

double kepler_U_equation(double x, double ro, double vro, double a, double mu)
{
	return (ro*vro / sqrt(mu)*x*x*stumpC(a*x*x) + (1.0 - a * ro)*x*x*x*stumpS(a*x*x) + ro * x) / sqrt(mu);
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

double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle)
{
	if (firstAngle > PI2)
	{
		firstAngle -= PI2;
	}
	else if (firstAngle < 0)
	{
		firstAngle += PI2;
	}

	if (secondAngle > PI2)
	{
		secondAngle -= PI2;
	}
	else if (secondAngle < 0)
	{
		secondAngle += PI2;
	}

	double difference = secondAngle - firstAngle;
	while (difference < -PI) difference += PI2;
	while (difference > PI) difference -= PI2;
	return difference;
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

double kepler_E(double e, double M, double error2)
{
	double ratio, E;
	//{
	//	This function uses Newton's method to solve Kepler's
	//		equation E - e*sin(E) = M for the eccentric anomaly,
	//		given the eccentricity and the mean anomaly.
	//		E - eccentric anomaly(radians)
	//		e - eccentricity, passed from the calling program
	//		M - mean anomaly(radians), passed from the calling program
	//}
	// ----------------------------------------------

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

void rv_from_r0v0_obla(VECTOR3 R1, VECTOR3 V1, double MJD, double dt, VECTOR3 &R2, VECTOR3 &V2)
{
	//Only for Earth!

	OELEMENTS coe, coe2;
	double h, e, Omega_0, i, omega_0, theta0, a, T, n, E_0, t_0, t_f, n_p, t_n, M_n, E_n, theta_n, Omega_dot, omega_dot, Omega_n, omega_n;

	coe = coe_from_sv(R1, V1, mu_Earth);
	h = coe.h;
	e = coe.e;
	Omega_0 = coe.RA;
	i = coe.i;
	omega_0 = coe.w;
	theta0 = coe.TA;

	a = h*h / mu_Earth * 1.0 / (1.0 - e*e);
	T = 2.0 * PI / sqrt(mu_Earth)*OrbMech::power(a, 3.0 / 2.0);
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

	Omega_dot = -(3.0 / 2.0 * sqrt(mu_Earth)*J2_Earth * OrbMech::power(R_Earth, 2.0) / (OrbMech::power(1.0 - OrbMech::power(e, 2.0), 2.0) * OrbMech::power(a, 7.0 / 2.0)))*cos(i);
	omega_dot = -(3.0 / 2.0 * sqrt(mu_Earth)*J2_Earth * OrbMech::power(R_Earth, 2.0) / (OrbMech::power(1.0 - OrbMech::power(e, 2.0), 2.0) * OrbMech::power(a, 7.0 / 2.0)))*(5.0 / 2.0 * sin(i)*sin(i) - 2.0);

	Omega_n = Omega_0 + Omega_dot*dt;
	omega_n = omega_0 + omega_dot*dt;

	coe2.h = h;
	coe2.e = e;
	coe2.RA = Omega_n;
	coe2.i = i;
	coe2.w = omega_n;
	coe2.TA = theta_n;

	sv_from_coe(coe2, mu_Earth, R2, V2);
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
	rp_factor = h*h / (mu*(1.0 + e*cos(TA)));
	rp.x = rp_factor*cos(TA);
	rp.y = rp_factor*sin(TA);
	rp.z = 0;
	vp_factor = mu / h;
	vp.x = -vp_factor*sin(TA);
	vp.y = vp_factor*(e + cos(TA));
	vp.z = 0;
	R3_W = _M(cos(RA), sin(RA), 0.0, -sin(RA), cos(RA), 0.0, 0.0, 0.0, 1.0);
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

	E = (R*(OrbMech::power(v, 2) - mu / r) - V*r*vr) * 1.0 / mu;
	e = length(E);

	if (n != 0)
	{
		if (e > eps)
		{
			w = acos(dotp(N, E) / n / e);
			if (E.z < 0)
			{
				w = PI2 - w;
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
		TA = acos2(dotp(unit(E), unit(R)));
		if (vr < 0.0)
		{
			TA = PI2 - TA;
		}
	}
	else
	{
		cp = crossp(N, R);
		if (cp.z >= 0)
		{
			TA = acos2(dotp(unit(N), unit(R)));
		}
		else
		{
			TA = PI2 - acos2(dotp(unit(N), unit(R)));
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

void PACSS13_from_coe(OELEMENTS coe, double lat, double A_Z, double mu, VECTOR3 &R_S, VECTOR3 &V_S)
{
	MATRIX3 MSG, MEG;
	VECTOR3 R_G, R_E, V_G, V_E;
	double C3, inc, theta_N, e, f, a, h, alpha_D;
	OELEMENTS coe2;

	C3 = coe.h;
	inc = coe.i;
	theta_N = coe.RA;
	e = coe.e;
	alpha_D = coe.w;
	f = coe.TA;
	a = -mu / C3;
	h = sqrt(mu*a*(1.0 - e * e));
	//coe2 = [h e pi - theta_N - lng inc pi - alpha_D f];
	coe2.e = e;
	coe2.h = h;
	coe2.i = inc;
	coe2.RA = theta_N + PI;
	coe2.TA = f;
	coe2.w = PI - alpha_D;

	sv_from_coe(coe2, mu, R_E, V_E);

	MSG = MSGMatrix(lat, A_Z);
	MEG = MEGMatrix(0.0);

	R_G = mul(MEG, R_E);
	V_G = mul(MEG, V_E);
	R_S = tmul(MSG, R_G);
	V_S = tmul(MSG, V_G);
}

MATRIX3 MSGMatrix(double phi_L, double A_Z)
{
	//Transformation matrix from S-system to G-system

	return _M(cos(phi_L), sin(phi_L)*sin(A_Z), -sin(phi_L)*cos(A_Z), -sin(phi_L), cos(phi_L)*sin(A_Z), -cos(phi_L)*cos(A_Z), 0, cos(A_Z), sin(A_Z));
}

MATRIX3 MEGMatrix(double theta_E)
{
	//Transformation matrix from E-system to G-system

	return _M(cos(theta_E), sin(theta_E), 0, 0, 0, -1, -sin(theta_E), cos(theta_E), 0);
}

VECTOR3 elegant_lambert(VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double dt, int N, bool prog, double mu)
{
	double tol, ratio, r1, r2, c, s, theta, lambda, T, l, m, x, h1, h2, B, y, z, x_new, A;
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
	y = OrbMech::power((double)(N)*m*PI / 4.0, 1.0 / 3.0);
	x = 0.5*((m / y / y - (1.0 + l)) - sqrt(power(m / y / y - (1 + l), 2.0) - 4.0*l));

	ratio = 1;
	n = 0;
	while (abs(ratio) > tol && nMax >= n)
	{
		n = n + 1;
		h1 = (l + x)*(1.0 + 2.0 * x + l) / (2.0 * (l - OrbMech::power(x, 2.0)));
		h2 = m*sqrt(x) / (2.0 * (l - OrbMech::power(x, 2)))*((l - OrbMech::power(x, 2.0))*((double)(N)*PI05 + atan(sqrt(x))) / sqrt(x) - (l + x));
		B = 27.0 * h2 / (4.0 * OrbMech::power(sqrt(x)*(1 + h1), 3));
		if (B < -1.0)
		{
			B = -1.0;
		}
		if (B >= 0)
		{
			z = 2.0 * cosh(1.0 / 3.0 * acosh(sqrt(B + 1.0)));
		}
		else
		{
			z = 2.0 * cos(1.0 / 3.0 * acos(sqrt(B + 1.0)));
		}
		if (B >= 0)
		{
			A = OrbMech::power((sqrt(B) + sqrt(B + 1.0)), 1.0 / 3.0);
			y = 2.0 / 3.0 * sqrt(x)*(1.0 + h1)*(sqrt(B + 1.0) / (A + 1.0 / A) + 1.0);
		}
		else
		{
			y = 2.0 / 3.0 * (1.0 + h1)*(B / z + 1.0);
		}
		//y = 2.0 / 3.0 * (1.0 + h1)*(sqrt(B + 1.0) / z + 1.0);
		x_new = 0.5*((m / y / y - (1.0 + l)) - sqrt(OrbMech::power(m / y / y - (1.0 + l), 2) - 4.0 * l));
		if (x_new < 0)
		{
			x_new = 0.5*((m / y / y - (1.0 + l)) + sqrt(OrbMech::power(m / y / y - (1.0 + l), 2) - 4.0 * l));
		}

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

bool oneclickcoast(int Epoch, VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, VECTOR3 &R1, VECTOR3 &V1, int gravref, int &gravout)
{
	bool stop, soichange;
	CoastIntegrator* coast;
	coast = new CoastIntegrator(Epoch, R0, V0, mjd0, dt, gravref, gravout);
	stop = false;
	while (stop == false)
	{
		stop = coast->iteration();
	}
	R1 = coast->R2;
	V1 = coast->V2;
	gravout = coast->outplanet;
	soichange = coast->soichange;
	delete coast;
	return soichange;
}

bool oneclickcoast(int Epoch, VECTOR3 R0, VECTOR3 V0, double mjd0, double dt, VECTOR3 &R1, VECTOR3 &V1, OBJHANDLE gravref, OBJHANDLE &gravout)
{
	//Temporary
	int gr, go;
	if (gravref == oapiGetObjectByName("Earth"))
	{
		gr = BODY_EARTH;
	}
	else
	{
		gr = BODY_MOON;
	}
	if (gravout == NULL)
	{
		go = -1;
	}
	else if (gravout == oapiGetObjectByName("Earth"))
	{
		go = BODY_EARTH;
	}
	else
	{
		go = BODY_MOON;
	}

	bool soichange = oneclickcoast(Epoch, R0, V0, mjd0, dt, R1, V1, gr, go);
	if (go == BODY_EARTH)
	{
		gravout = oapiGetObjectByName("Earth");
	}
	else
	{
		gravout = oapiGetObjectByName("Moon");
	}
	
	return soichange;
}

SV coast(int Epoch, SV sv0, double dt)
{
	if (dt == 0.0)
	{
		return sv0;
	}

	SV sv1;
	OBJHANDLE gravout = NULL;

	OrbMech::oneclickcoast(Epoch, sv0.R, sv0.V, sv0.MJD, dt, sv1.R, sv1.V, sv0.gravref, gravout);
	sv1.gravref = gravout;
	sv1.mass = sv0.mass;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
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

VECTOR3 Vinti(int Epoch, VECTOR3 R1, VECTOR3 V1, VECTOR3 R2, double mjd0, double dt, int N, bool prog, int gravref, int gravin, int gravout, VECTOR3 V_guess, double tol)
{
	double h, rho, error3, mu, max_dr;
	int nMax, nMax2, n;
	VECTOR3 Vt1, V1_star, dr2, R2_star, V2_star, R1_ref, V1_ref, R2_ref;
	VECTOR3 v_l[3][4];
	VECTOR3 R2l[3][4];
	VECTOR3 V2l[3][4];
	VECTOR3 T[3];
	MATRIX3 T2;

	h = 10e-3;
	rho = 0.5;
	error3 = 100.0;
	dr2 = _V(1.0, 1.0, 1.0);
	n = 0;
	nMax = 100;
	nMax2 = 10;

	if (gravref == BODY_EARTH)
	{
		mu = mu_Earth;
	}
	else
	{
		mu = mu_Moon;
	}

	double hvec[4] = { h / 2, -h / 2, rho*h / 2, -rho*h / 2 };

	if (length(V_guess) == 0.0)
	{
		if (gravref != gravin)
		{
			oneclickcoast(Epoch, R1, V1, mjd0, 0.0, R1_ref, V1_ref, gravin, gravref);
			R2_ref = R2;
		}
		else if (gravref != gravout)
		{
			VECTOR3 V2_ref;
			R1_ref = R1;
			V1_ref = V1;
			oneclickcoast(Epoch, R2, V1, mjd0 + dt / 24.0 / 3600.0, 0.0, R2_ref, V2_ref, gravout, gravref);
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
			oneclickcoast(Epoch, R1_ref, Vt1, mjd0, 0.0, R1_unused, Vt1, gravref, gravin);
		}
		V1_star = Vt1*sign(dt);
	}
	else
	{
		V1_star = V_guess;
	}

	if (gravref == BODY_EARTH && gravin == gravout && dt>0)
	{
		rv_from_r0v0_obla(R1, V1_star, mjd0, dt, R2_star, V2_star);
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
					rv_from_r0v0_obla(R1, v_l[i][j], mjd0, dt, R2l[i][j], V2l[i][j]);
				}
			}
			for (int i = 0; i < 3; i++)
			{
				T[i] = (R2l[i][2] - R2l[i][3] - (R2l[i][0] - R2l[i][1])*OrbMech::power(rho, 3.0)) * 1.0 / (rho*h*(1.0 - OrbMech::power(rho, 2.0)));
			}
			T2 = _M(T[0].x, T[1].x, T[2].x, T[0].y, T[1].y, T[2].y, T[0].z, T[1].z, T[2].z);
			V1_star = V1_star + mul(inverse(T2), dr2);
			rv_from_r0v0_obla(R1, V1_star, mjd0, dt, R2_star, V2_star);
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

	oneclickcoast(Epoch, R1, V1_star, mjd0, dt, R2_star, V2_star, gravin, gravout);
	dr2 = R2 - R2_star;
	max_dr = 0.5*length(R2_star);
	if (length(dr2) > max_dr)
	{
		dr2 = unit(dr2)*max_dr;
	}

	while (length(dr2) > tol && nMax >= n)
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
				oneclickcoast(Epoch, R1, v_l[i][j], mjd0, dt, R2l[i][j], V2l[i][j], gravin, gravout);
			}
		}
		for (int i = 0; i < 3; i++)
		{
			T[i] = (R2l[i][2] - R2l[i][3] - (R2l[i][0] - R2l[i][1])*OrbMech::power(rho, 3.0)) * 1.0 / (rho*h*(1.0 - OrbMech::power(rho, 2.0)));
		}
		T2 = _M(T[0].x, T[1].x, T[2].x, T[0].y, T[1].y, T[2].y, T[0].z, T[1].z, T[2].z);
		V1_star = V1_star + mul(inverse(T2), dr2);
		oneclickcoast(Epoch, R1, V1_star, mjd0, dt, R2_star, V2_star, gravin, gravout);
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

double NSRsecant(int Epoch, VECTOR3 RA, VECTOR3 VA, VECTOR3 RP, VECTOR3 VP, double mjd0, double x, double DH, OBJHANDLE gravref)
{
	double theta, SW, dh_CDH, mu;
	VECTOR3 RA2, VA2, RP2, VP2, u, RA2_alt, VA2_alt, RPC, VPC;

	mu = GGRAV*oapiGetMass(gravref);

	oneclickcoast(Epoch, RA, VA, mjd0, x, RA2, VA2, gravref, gravref);
	oneclickcoast(Epoch, RP, VP, mjd0, x, RP2, VP2, gravref, gravref);

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

MATRIX3 GetRotationMatrix(int plan, double t)
{
	double T_p, L_0, e_rel, phi_0, T_s, L_rel, phi, CL, SL, CP, SP;
	MATRIX3 R_ref, Rot3, Rot4, R_rel, R_rot;

	if (plan == BODY_EARTH)
	{
		T_p = T_p_Earth;		//Precession Period
		L_0 = L_0_Earth;		//LAN in the "beginning"
		e_rel = e_rel_Earth;	//Obliquity / axial tilt of the earth in radians
		phi_0 = phi_0_Earth;	//Sidereal Rotational Offset
		T_s = T_s_Earth;		//Sidereal Rotational Period
		R_ref = R_ref_Earth;
		Rot4 = R_obl_Earth;
	}
	else
	{
		T_p = T_p_Moon;			//Precession Period
		L_0 = L_0_Moon;			//LAN in the "beginning"
		e_rel = e_rel_Moon;		//Obliquity / axial tilt of the earth in radians
		phi_0 = phi_0_Moon;		//Sidereal Rotational Offset
		T_s = T_s_Moon;			//Sidereal Rotational Period
		R_ref = R_ref_Moon;
		Rot4 = R_obl_Moon;
	}

	L_rel = L_0 + PI2*(t - LAN_MJD) / T_p;
	CL = cos(L_rel);
	SL = sin(L_rel);
	Rot3 = _M(CL, 0, -SL, 0, 1, 0, SL, 0, CL);
	R_rel = mul(Rot3, Rot4);
	phi = phi_0 + PI2*(t - LAN_MJD) / T_s + (L_0 - L_rel)*cos(e_rel);
	CP = cos(phi);
	SP = sin(phi);
	R_rot = _M(CP, 0, -SP, 0, 1, 0, SP, 0, CP);
	return mul(R_ref, mul(R_rel, R_rot));
}

MATRIX3 GetObliquityMatrix(int plan, double t)
{
	MATRIX3 Rot, Rot5, Rot6;
	VECTOR3 s;
	double e_ecl, L_ecl;

	Rot = GetRotationMatrix(plan, t);

	s = mul(Rot, _V(0.0, 1.0, 0.0));
	e_ecl = acos(s.y);
	L_ecl = atan(-s.x / s.z);
	Rot5 = _M(cos(L_ecl), 0.0, -sin(L_ecl), 0.0, 1.0, 0.0, sin(L_ecl), 0.0, cos(L_ecl));
	Rot6 = _M(1.0, 0.0, 0.0, 0.0, cos(e_ecl), -sin(e_ecl), 0.0, sin(e_ecl), cos(e_ecl));
	return mul(Rot5, Rot6);
}

double GetPlanetCurrentRotation(int plan, double t)
{
	double T_p, L_0, e_rel, phi_0, T_s, L_rel, phi, e_ecl, L_ecl, phi_off,r;
	MATRIX3 R_ref, Rot3, Rot4, Rot5, Rot6, R_rel, R_rot, Rot, R_ecl, R_off;
	VECTOR3 s;

	if (plan == BODY_EARTH)
	{
		T_p = T_p_Earth;		//Precession Period
		L_0 = L_0_Earth;		//LAN in the "beginning"
		e_rel = e_rel_Earth;	//Obliquity / axial tilt of the earth in radians
		phi_0 = phi_0_Earth;	//Sidereal Rotational Offset
		T_s = T_s_Earth;		//Sidereal Rotational Period
		R_ref = R_ref_Earth;
		Rot4 = R_obl_Earth;
	}
	else
	{
		T_p = T_p_Moon;			//Precession Period
		L_0 = L_0_Moon;			//LAN in the "beginning"
		e_rel = e_rel_Moon;		//Obliquity / axial tilt of the earth in radians
		phi_0 = phi_0_Moon;		//Sidereal Rotational Offset
		T_s = T_s_Moon;			//Sidereal Rotational Period
		R_ref = R_ref_Moon;
		Rot4 = R_obl_Moon;
	}

	L_rel = L_0 + PI2*(t - LAN_MJD) / T_p;
	Rot3 = _M(cos(L_rel), 0.0, -sin(L_rel), 0.0, 1.0, 0.0, sin(L_rel), 0.0, cos(L_rel));
	R_rel = mul(Rot3, Rot4);
	phi = phi_0 + PI2*(t - LAN_MJD) / T_s + (L_0 - L_rel)*cos(e_rel);
	R_rot = _M(cos(phi), 0.0, -sin(phi), 0.0, 1.0, 0.0, sin(phi), 0.0, cos(phi));
	Rot = mul(R_ref,mul(R_rel, R_rot));
	s = mul(Rot, _V(0.0, 1.0, 0.0));
	e_ecl = acos(s.y);
	L_ecl = atan(-s.x / s.z);
	Rot5 = _M(cos(L_ecl), 0.0, -sin(L_ecl), 0.0, 1.0, 0.0, sin(L_ecl), 0.0, cos(L_ecl));
	Rot6 = _M(1.0, 0.0, 0.0, 0.0, cos(e_ecl), -sin(e_ecl), 0.0, sin(e_ecl), cos(e_ecl));
	R_ecl = mul(Rot5, Rot6);
	R_off = mul(tmat(R_ecl), mul(R_ref, R_rel));
	phi_off = atan(-R_off.m13 / R_off.m11);
	r = phi + phi_off;
	return r;
}

MATRIX3 Orbiter2PACSS13(double mjd, double lat, double lng, double azi)
{
	MATRIX3 Rot1, Rot2, Rot3, Rot4;
	VECTOR3 R_P, UX10, UY10, UZ10;

	Rot1 = GetRotationMatrix(BODY_EARTH, mjd);
	Rot2 = _M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)));
	UX10 = R_P;
	UY10 = unit(crossp(_V(0.0, 0.0, 1.0), UX10));
	UZ10 = crossp(UX10, UY10);

	Rot3 = _M(UX10.x, UX10.y, UX10.z, UY10.x, UY10.y, UY10.z, UZ10.x, UZ10.y, UZ10.z);
	Rot4 = _M(1.0, 0.0, 0.0, 0.0, cos(-azi), -sin(-azi), 0.0, sin(-azi), cos(-azi));

	return mul(tmat(Rot4), mul(Rot3, mul(Rot2, tmat(Rot1))));
}

double findelev(int Epoch, VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double mjd0, double E, OBJHANDLE gravref)
{
	double w_A, w_P, r_A, v_A, r_P, v_P, alpha, t, dt, E_err, E_A, mu;
	VECTOR3 u, R_A, V_A, R_P, V_P, U_L, U_P;

	mu = GGRAV * oapiGetMass(gravref);
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
		oneclickcoast(Epoch, R_A, V_A, mjd0 + t / 24.0 / 3600.0, dt, R_A, V_A, gravref, gravref);
		oneclickcoast(Epoch, R_P, V_P, mjd0 + t / 24.0 / 3600.0, dt, R_P, V_P, gravref, gravref);
		t += dt;
		r_A = length(R_A);
		v_A = length(V_A);
		r_P = length(R_P);
		v_P = length(V_P);
		U_L = unit(R_P - R_A);
		U_P = unit(U_L - R_A*dotp(U_L, R_A) / r_A / r_A);
		E_A = acos(dotp(U_L, U_P*sign(dotp(U_P, crossp(u, R_A)))));
		if (dotp(U_L, R_A) < 0)
		{
			E_A = PI2 - E_A;
		}
		E_err = E - E_A;
	}
	return t;
}

double findelev_conic(VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_P0, VECTOR3 V_P0, double E, double mu)
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
		rv_from_r0v0(R_A, V_A, dt, R_A, V_A, mu);
		rv_from_r0v0(R_P, V_P, dt, R_P, V_P, mu);
		t += dt;
		r_A = length(R_A);
		v_A = length(V_A);
		r_P = length(R_P);
		v_P = length(V_P);
		U_L = unit(R_P - R_A);
		U_P = unit(U_L - R_A * dotp(U_L, R_A) / r_A / r_A);
		E_A = acos(dotp(U_L, U_P*sign(dotp(U_P, crossp(u, R_A)))));
		if (dotp(U_L, R_A) < 0)
		{
			E_A = PI2 - E_A;
		}
		E_err = E - E_A;
	}
	return t;
}

double findelev_gs(int Epoch, MATRIX3 Rot_J_B, VECTOR3 R_A0, VECTOR3 V_A0, VECTOR3 R_gs, double mjd0, double E, OBJHANDLE gravref, double &range)
{
	double w_A, w_P, r_A, v_A, r_P, alpha, t, dt, E_err, E_A, dE, dE_0, dt_0, dt_max, t_S, theta_0;
	VECTOR3 R_A, V_A, R_P, U_L, U_P, U_N, U_LL, R_proj;
	MATRIX3 Rot2;
	int i, body;

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

	if (gravref == oapiGetObjectByName("Moon"))
	{
		w_A = -w_Moon;
		body = BODY_MOON;
	}
	else
	{
		w_A = w_Earth;
		body = BODY_EARTH;
	}
	r_P = length(R_gs);

	Rot2 = OrbMech::GetRotationMatrix(body, mjd0);
	R_P = mul(Rot_J_B, rhmul(Rot2, R_gs));

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

	while ((abs(E_err) > 0.01*RAD || abs(dt)>1.0) && i < 50)
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
		oneclickcoast(Epoch, R_A, V_A, mjd0 + t_S / 24.0 / 3600.0, t - t_S, R_A, V_A, gravref, gravref);
		Rot2 = OrbMech::GetRotationMatrix(body, mjd0 + t / 24.0 / 3600.0);
		R_P = mul(Rot_J_B, rhmul(Rot2, R_gs));

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

double timetoperi_integ(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, OBJHANDLE ref_peri)
{
	VECTOR3 R2, V2;
	return timetoperi_integ(Epoch, R, V, MJD, gravref, ref_peri, R2, V2);
}

double timetoperi_integ(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, OBJHANDLE ref_peri, VECTOR3 &R2, VECTOR3 &V2)
{
	VECTOR3 R0, V0, R1, V1;
	double mu, dt, dt_total;
	int n, nmax;

	mu = GGRAV*oapiGetMass(ref_peri);
	dt_total = 0.0;
	n = 0;
	nmax = 10;

	if (gravref != ref_peri)
	{
		oneclickcoast(Epoch, R, V, MJD, 0.0, R0, V0, gravref, ref_peri);
	}
	else
	{
		R0 = R;
		V0 = V;
	}

	dt = timetoperi(R0, V0, mu);
	oneclickcoast(Epoch, R, V, MJD, dt, R1, V1, gravref, ref_peri);
	dt_total += dt;

	do
	{
		dt = timetoperi(R1, V1, mu);
		oneclickcoast(Epoch, R1, V1, MJD + dt_total / 24.0 / 3600.0, dt, R1, V1, ref_peri, ref_peri);
		dt_total += dt;
		n++;
	} while (abs(dt) > 0.01 && nmax >= n);

	R2 = R1;
	V2 = V1;

	return dt_total;
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
		E_0 = 2.0 * atan(sqrt((1.0 - coe.e) / (1.0 + coe.e))*tan(coe.TA / 2.0));
		chi = -sqrt(a)*E_0;
	}

	alpha = 1.0 / a;

	r0 = length(R);
	vr0 = dotp(R, V) / r0;
	return kepler_U_equation(chi, r0, vr0, alpha, mu);
}

double timetoapo(VECTOR3 R, VECTOR3 V, double mu, int s)
{
	//s = 1: ensure the next apoapsis is returned

	OELEMENTS coe;
	double a, chi, alpha, r0, vr0, dt;

	coe = coe_from_sv(R, V, mu);
	//[h e RA incl w TA a]

	a = coe.h*coe.h / mu / (1.0 - coe.e*coe.e);

	double E_0;
	E_0 = 2 * atan(sqrt((1.0 - coe.e) / (1.0 + coe.e))*tan(coe.TA / 2.0));
	chi = sqrt(a)*(PI - E_0);

	alpha = 1.0 / a;

	r0 = length(R);
	vr0 = dotp(R, V) / r0;
	dt = kepler_U_equation(chi, r0, vr0, alpha, mu);

	if (s == 0 || dt >= 0)
	{
		return dt;
	}

	double T_P = period(R, V, mu);

	return dt + T_P;
}

double time_radius(VECTOR3 R, VECTOR3 V, double r, double s, double mu)
{
	//Conventions:
	//Elliptic: dt is always positive
	//Parabolic and Hyperbolic: dt can be positive or negative

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
			sinhF = s*sqrt(coshF*coshF - 1.0);
		}
		F = atanh(sinhF / coshF);

		coshF0 = (r0 / -a + 1.0) / e;
		
		if (coshF0*coshF0 < 1.0)
		{
			sinhF0 = 0.0;
			coshF0 = 1.0;
		}
		else
		{
			if (vr0 >= 0)
			{
				sinhF0 = sqrt(coshF0*coshF0 - 1.0);
			}
			else
			{
				sinhF0 = -sqrt(coshF0*coshF0 - 1.0);
			}
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
		double cosE, sinE, E, dE;
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

		dE = fmod(E - E0, PI2);
		if (dE < 0)
		{
			dE += PI2;
		}

		x = sqrt(a)*(dE);
	}

	dt = kepler_U_equation(x, r0, vr0, alpha, mu);
	return dt;
}

void ReturnPerigee(int Epoch, VECTOR3 R, VECTOR3 V, double mjd0, OBJHANDLE hMoon, OBJHANDLE hEarth, double phi, double &MJD_peri, VECTOR3 &R_peri, VECTOR3 &V_peri)
{
	//INPUT:
	//R and V in Moon relative coordinates, e>1
	//phi: 1 = return perigee, -1 = Earth departure perigee


	VECTOR3 R_patch, V_patch;
	double r_SPH, dt, MJD_patch, dt2;

	r_SPH = 64373760.0;

	//Assumption: reentry will never happen before 24 hours after leaving the lunar SOI
	dt = time_radius(R, V*phi, r_SPH, 1.0, mu_Moon) + 24.0*3600.0;
	oneclickcoast(Epoch, R, V, mjd0, dt*phi, R_patch, V_patch, hMoon, hEarth);
	MJD_patch = mjd0 + dt*phi / 24.0 / 3600.0;

	dt2 = timetoperi_integ(Epoch, R_patch, V_patch, MJD_patch, hEarth, hEarth, R_peri, V_peri);

	MJD_peri = MJD_patch + dt2 / 24.0 / 3600.0;
}

double time_radius_integ(int Epoch, VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout)
{
	VECTOR3 RPRE, VPRE;
	return time_radius_integ(Epoch, R, V, mjd0, r, s, gravref, gravout, RPRE, VPRE);
}

double time_radius_integ(int Epoch, VECTOR3 R, VECTOR3 V, double mjd0, double r, double s, OBJHANDLE gravref, OBJHANDLE gravout, VECTOR3 &RPRE, VECTOR3 &VPRE)
{
	double dt1, sing, cosg, x2PRE, dt21, beta12, beta4, RF, phi4, dt21apo, beta13, dt2, beta14, mu;
	VECTOR3 N, R0out, V0out;
	int n, nmax;

	mu = GGRAV*oapiGetMass(gravout);
	beta12 = 1.0;
	dt21apo = 100000000.0;
	dt2 = 0.0;
	dt21 = 1.0;
	n = 0;
	nmax = 15;

	oneclickcoast(Epoch, R, V, mjd0, 0.0, R0out, V0out, gravref, gravout);
	dt1 = time_radius(R0out, V0out, r, s, mu);

	oneclickcoast(Epoch, R, V, mjd0, dt1, RPRE, VPRE, gravref, gravout);

	while (abs(beta12) > 0.000007 && abs(dt21)>0.01 && nmax >= n)
	{
		N = crossp(unit(RPRE), unit(VPRE));
		sing = length(N);
		cosg = dotp(unit(RPRE), unit(VPRE));
		x2PRE = cosg / sing;
		beta4 = r / length(RPRE);
		beta12 = beta4 - 1.0;
		RF = beta4*length(RPRE);
		if (beta12*s < 0)
		{
			phi4 = -1.0;
		}
		else if (x2PRE*s < 0)
		{
			phi4 = -1.0;
		}
		else
		{
			phi4 = 1.0;
		}
		dt21 = time_radius(RPRE, VPRE*phi4, RF, phi4*s, mu);
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
			oneclickcoast(Epoch, RPRE, VPRE, mjd0 + (dt1 + dt2) / 24.0 / 3600.0, dt21, RPRE, VPRE, gravout, gravout);
			dt2 += dt21;
		}
		n++;
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
	double R_E, beta1, beta2, a, b, c, d, e, p, q, D0, D1, S, DD, SS[2], sinx[2], pp, alpha, cond, aa, mu;
	double x[4];
	double cosv[2], sinv[2];
	int j, l;

	mu = GGRAV*oapiGetMass(planet);
	R_E = oapiGetSize(planet);

	coe = coe_from_sv(R, V, mu);

	P = _V(cos(coe.w)*cos(coe.RA) - sin(coe.w)*sin(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.RA) + sin(coe.w)*cos(coe.RA)*cos(coe.i), sin(coe.w)*sin(coe.i));
	P = unit(P);
	Q = _V(-sin(coe.w)*cos(coe.RA) - cos(coe.w)*sin(coe.RA)*cos(coe.i), -sin(coe.w)*sin(coe.RA) + cos(coe.w)*cos(coe.RA)*cos(coe.i), cos(coe.w)*sin(coe.i));
	Q = unit(Q);

	beta1 = dotp(unit(sun), P);

	aa = coe.h*coe.h / (mu*(1.0 - coe.e*coe.e));
	p = aa * (1.0 - coe.e*coe.e);

	//Is shadow function vanishing for elliptical orbit?
	/*if (coe.e < 1 && beta1*beta1 > 1.0 - pow(R_E / (aa*(1.0 - coe.e)), 2) && beta1*beta1 < 1.0 - pow(R_E / (aa*(1.0 + coe.e)), 2))
	{
		v1 = 0;
		return;
	}
	//Is shadow function vanishing for hyperbolic orbit?
	else if (coe.e > 1.0 && beta1*beta1 < 1.0 - pow(R_E / (aa*(1.0 - coe.e)), 2))
	{
		v1 = 0;
		return;
	}*/

	beta2 = dotp(unit(sun), Q);
	p = coe.h*coe.h / mu;
	/*A = coe.e*coe.e*R_E*R_E + p*p*beta1*beta1 - p*p*beta2*beta2;
	B = 2.0 * coe.e*R_E;
	C = 2.0*beta1*beta2*p*p;
	D = R_E*R_E + p*p*beta2*beta2 - p*p;*/

	alpha = R_E / p;

	a = pow(alpha,4)*pow(coe.e, 4) - 2.0*pow(alpha, 2)*(beta2*beta2 - beta1*beta1)*coe.e*coe.e + pow(beta1*beta1 + beta2*beta2, 2);
	b = 4.0*pow(alpha, 4)*pow(coe.e, 3) - 4.0*pow(alpha, 2)*(beta2*beta2 - beta1*beta1)*coe.e;
	c = 6.0*pow(alpha, 4)*coe.e*coe.e - 2.0*pow(alpha, 2)*(beta2*beta2 - beta1*beta1) - 2.0*pow(alpha, 2)*(1.0 - beta2*beta2)*coe.e*coe.e + 2.0*(beta2*beta2 - beta1*beta1)*(1.0 - beta2*beta2) - 4.0*beta1*beta1*beta2*beta2;
	d = 4.0*pow(alpha, 4)*coe.e - 4.0*pow(alpha, 2)*(1.0 - beta2*beta2)*coe.e;
	e = pow(alpha, 4) - 2.0*pow(alpha, 2)*(1.0 - beta2*beta2) + pow(1.0 - beta2*beta2,2);

	pp = (8.0*a*c - 3.0 * b*b) / (8.0*a*a);
	q = (b*b*b - 4.0*a*b*c + 8.0*a*a*d) / (8.0 * a*a*a);
	D0 = c*c - 3.0*b*d + 12.0 * a*e;
	D1 = 2.0*c*c*c - 9.0*b*c*d + 27.0*b*b*e + 27.0*a*d*d - 72.0*a*c*e;
	DD = -(D1*D1 - 4.0*D0*D0*D0) / 27.0;

	if (DD > 0)
	{
		double phi;

		phi = acos(D1 / (2.0*sqrt(D0*D0*D0)));
		S = 0.5*sqrt(-2.0 / 3.0*pp + 2.0 / 3.0 / a*sqrt(D0)*cos(phi / 3.0));
	}
	else
	{
		double QQ;

		QQ = OrbMech::power((D1 + sqrt(D1*D1 - 4.0*D0*D0*D0)) / 2.0, 1.0 / 3.0);
		S = 0.5*sqrt(-2.0 / 3.0*pp + 1.0 / (3.0*a)*(QQ + D0 / QQ));
	}
	x[0] = -b / (4.0*a) - S + 0.5*sqrt(-4.0*S*S - 2.0 * pp + q / S);
	x[1] = -b / (4.0*a) - S - 0.5*sqrt(-4.0*S*S - 2.0 * pp + q / S);
	x[2] = -b / (4.0*a) + S + 0.5*sqrt(-4.0*S*S - 2.0 * pp - q / S);
	x[3] = -b / (4.0*a) + S - 0.5*sqrt(-4.0*S*S - 2.0 * pp - q / S);

	j = 0;

	//Select the two physicals solutions from the (up to) four real roots of the quartic
	for (int i = 0; i < 4; i++)
	{
		sinx[0] = sqrt(1.0 - x[i] * x[i]);
		sinx[1] = -sinx[0];

		for (int k = 0;k < 2;k++)
		{
			SS[k] = R_E * R_E*pow(1.0 + coe.e*x[i], 2) + p * p*pow(beta1*x[i] + beta2 * sinx[k], 2) - p * p;
		}
		if (abs(SS[0]) < abs(SS[1]))
		{
			l = 0;
		}
		else
		{
			l = 1;
		}
		cond = beta1 * x[i] + beta2 * sinx[l];
		if (cond < 0)
		{
			cosv[j] = x[i];
			sinv[j] = sinx[l];
			j++;
		}

		/*SS = R_E*R_E*pow(1.0 + coe.e*x[i], 2) + p*p*pow(beta1*x[i] + beta2*sinx, 2) - p*p;
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
		}*/
	}

	//If it didn't find 2 physical solutions, abort
	if (j != 2)
	{
		v1 = 0.0;
		return;
	}

	//Choose entry vs. exit
	double dSS0 = 2.0*p*p*(beta2*cosv[0] - beta1 * sinv[0])*(beta1*cosv[0] + beta2 * sinv[0]) - 2.0*R_E*R_E*coe.e*sinv[0] * (coe.e*cosv[0] + 1.0);

	if (rise)
	{
		if (dSS0 < 0)
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
		if (dSS0 > 0)
		{
			v1 = atan2(sinv[0], cosv[0]);
		}
		else
		{
			v1 = atan2(sinv[1], cosv[1]);
		}
	}
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

VECTOR3 AdjustApoapsis(VECTOR3 R, VECTOR3 V, double mu, double r_apo_des)
{
	MATRIX3 Q_Xx;
	VECTOR3 V_apo;
	double p_H, c_I, dv, r_apo, r_peri, e_H, eps, e_Ho, dvo;
	int s_F;

	if (length(R) > r_apo_des)
	{
		return _V(0.0, 0.0, 0.0);
	}

	p_H = c_I = dv = 0.0;
	s_F = 0;
	eps = 0.1;

	Q_Xx = LVLH_Matrix(R, V);

	do
	{
		V_apo = V + tmul(Q_Xx, _V(dv, 0.0, 0.0));
		periapo(R, V_apo, mu, r_apo, r_peri);
		e_H = r_apo - r_apo_des;

		if (p_H == 0 || abs(e_H) >= eps)
		{
			OrbMech::ITER(c_I, s_F, e_H, p_H, dv, e_Ho, dvo);
			if (s_F == 1)
			{
				return _V(0.0, 0.0, 0.0);
			}
		}
	} while (abs(e_H) >= eps);

	return V_apo - V;
}

VECTOR3 AdjustPeriapsis(VECTOR3 R, VECTOR3 V, double mu, double r_peri_des)
{
	MATRIX3 Q_Xx;
	VECTOR3 V_apo;
	double p_H, c_I, dv, r_apo, r_peri, e_H, eps, e_Ho, dvo;
	int s_F;

	if (length(R) < r_peri_des)
	{
		return _V(0.0, 0.0, 0.0);
	}

	p_H = c_I = dv = 0.0;
	s_F = 0;
	eps = 0.1;

	Q_Xx = LVLH_Matrix(R, V);

	do
	{
		V_apo = V + tmul(Q_Xx, _V(dv, 0.0, 0.0));
		periapo(R, V_apo, mu, r_apo, r_peri);
		e_H = r_peri - r_peri_des;

		if (p_H == 0 || abs(e_H) >= eps)
		{
			OrbMech::ITER(c_I, s_F, e_H, p_H, dv, e_Ho, dvo);
			if (s_F == 1)
			{
				return _V(0.0, 0.0, 0.0);
			}
		}
	} while (abs(e_H) >= eps);

	return V_apo - V;
}

VECTOR3 CircularOrbitDV(VECTOR3 R, VECTOR3 V, double mu)
{
	VECTOR3 U_H, U_hor, V_apo;
	double v_circ;

	U_H = unit(crossp(R, V));
	U_hor = unit(crossp(U_H, unit(R)));
	v_circ = sqrt(mu/length(R));
	V_apo = U_hor*v_circ;

	return V_apo - V;
}

VECTOR3 RotateVelocityVector(VECTOR3 R, VECTOR3 V, double ang)
{
	VECTOR3 V_apo;

	V_apo = RotateVector(unit(R), ang, unit(V));
	V_apo = unit(V_apo)*length(V);

	return V_apo - V;
}

double P29TimeOfLongitude(MATRIX3 Rot_J_B, VECTOR3 R0, VECTOR3 V0, double MJD, OBJHANDLE gravref, double phi_d)
{
	MATRIX3 Rot2;
	VECTOR3 mu_N, mu_S, mu_Z, U_Z, mu_E, mu_C, R, V, mu_D, mu_E_apo;
	double mu, F, dphi, t, phi, lambda, eps_phi, absphidminphi, phidminphi, phi_0, theta_P, theta, t_F;
	int n, s_G, body, err;
	OBJHANDLE hEarth;

	n = 0;
	eps_phi = 0.0001*RAD;
	hEarth = oapiGetObjectByName("Earth");
	absphidminphi = 1.0;

	if (gravref == hEarth)
	{
		body = BODY_EARTH;
		mu = mu_Earth;
		F = 16.0 / 15.0;
	}
	else
	{
		body = BODY_MOON;
		mu = mu_Moon;
		F = 327.8 / 328.8;
	}

	U_Z = _V(0.0, 0.0, 1.0);
	Rot2 = OrbMech::GetRotationMatrix(body, MJD);
	mu_Z = mul(Rot_J_B, rhmul(Rot2, U_Z));

	mu_N = unit(crossp(R0, V0));
	mu_S = unit(crossp(mu_N, R0));
	mu_N = mu_N*sign(dotp(mu_N, mu_Z));
	mu_E = unit(crossp(mu_Z, R0));
	mu_C = unit(crossp(mu_Z, mu_E));

	R = R0;
	V = V0;
	t = MJD;
	dphi = 0.0;

	while (absphidminphi > eps_phi && absphidminphi < PI2 - eps_phi)
	{
		latlong_from_BRCS(Rot_J_B, R, t, gravref, lambda, phi);

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

		err = time_theta(R0, V0, theta, mu, t_F);
		if (err)
		{
			return t;
		}

		t = MJD + t_F / 24.0 / 3600.0;
		rv_from_r0v0(R0, V0, (t - MJD)*24.0*3600.0, R, V, mu);
		n++;
	}

	return t;
}

void latlong_from_BRCS(MATRIX3 Rot_J_B, VECTOR3 R, double MJD, int RefBody, double &lat, double &lng)
{
	MATRIX3 Rot = OrbMech::GetRotationMatrix(RefBody, MJD);
	VECTOR3 R_equ = rhtmul(Rot, tmul(Rot_J_B, R));
	latlong_from_r(R_equ, lat, lng);
}

void latlong_from_BRCS(MATRIX3 Rot_J_B, VECTOR3 R, double MJD, OBJHANDLE gravref, double &lat, double &lng)
{
	int body;

	if (gravref == oapiGetObjectByName("Earth"))
	{
		body = BODY_EARTH;
	}
	else
	{
		body = BODY_MOON;
	}

	latlong_from_BRCS(Rot_J_B, R, MJD, body, lat, lng);
}

void latlong_from_r(VECTOR3 R, double &lat, double &lng)
{
	VECTOR3 u;

	u = unit(R);
	lat = atan2(u.z, sqrt(u.x*u.x + u.y*u.y));
	lng = atan2(u.y, u.x);
}

VECTOR3 r_from_latlong(double lat, double lng)
{
	return unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)));
}

VECTOR3 r_from_latlong(double lat, double lng, double r)
{
	return r_from_latlong(lat, lng)*r;
}

bool groundstation(MATRIX3 Rot_J_B, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, double lat, double lng, bool rise, double &dt)
{
	double v1, dt_old,h, e, theta0,a,T,n, E_0, t_0, E_1, t_f, R_E, dt_max, rev, T_p, mu;
	VECTOR3 R_GS, gndst;
	MATRIX3 Rot;
	OELEMENTS coe;
	bool los;
	int nn, nmax, body;

	nn = 0;
	dt = 0;
	dt_max = 100.0;
	nmax = 100;
	dt_old = 1;
	rev = 0.0;
	T_p = oapiGetPlanetPeriod(planet);

	if (planet == oapiGetObjectByName("Earth"))
	{
		body = BODY_EARTH;
		mu = mu_Earth;
		R_E = R_Earth;
	}
	else
	{
		body = BODY_MOON;
		mu = mu_Moon;
		R_E = R_Moon;
	}

	while (abs(dt_old - dt) > 0.5 && nn <= nmax)
	{

		Rot = GetRotationMatrix(body, MJD+dt/24.0/3600.0);
		R_GS = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*R_E;
		gndst = mul(Rot_J_B, rhmul(Rot, R_GS));
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
		if (dt > 0 && (dt_old - dt) > 0.5*T)
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

int findNextAOS(MATRIX3 Rot_J_B, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet)
{
	double lat, lng, dt, dtmin;
	int gsmin;
	bool los;

	dtmin = 1000000;

	for (int i = 0; i < NUMBEROFGROUNDSTATIONS; i++)
	{
		lat = groundstations[i][0];
		lng = groundstations[i][1];
		los = groundstation(Rot_J_B, R, V, MJD, planet, lat, lng, 1, dt);
		if (los && dt < dtmin)
		{
			gsmin = i;
			dtmin = dt;
		}
	}
	return gsmin;
}

double sunrise(MATRIX3 Rot_J_B, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE planet, OBJHANDLE planet2, bool rise, bool midnight, bool future)
{
	//midnight = 0-> rise=0:sunset, rise=1:sunrise
	//midnight = 1-> rise=0:midday, rise=1:midnight
	double PlanPos[12];
	VECTOR3 PlanVec, R_EM, R_SE;
	OBJHANDLE hEarth, hMoon, hSun;
	double mu, v1;
	unsigned char options;

	mu = GGRAV*oapiGetMass(planet);

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	hSun = oapiGetObjectByName("Sun");

	CELBODY *cPlan = oapiGetCelbodyInterface(planet);

	OELEMENTS coe;
	double h, e, theta0, a, dt, dt_old;
	int i, imax, err;

	dt = 0;
	dt_old = 1;
	i = 0;
	imax = 20;

	while (abs(dt_old - dt) > 0.5 && i < imax)
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
			}
			options = cEarth->clbkEphemeris(MJD + dt / 24.0 / 3600.0, EPHEM_TRUEPOS, PlanPos);
			if (options & EPHEM_POLAR)
			{
				R_SE = Polar2Cartesian(PlanPos[2] * AU, PlanPos[1], PlanPos[0]);
			}
			else
			{
				R_SE = _V(PlanPos[0], PlanPos[2], PlanPos[1]);
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
			}
		}

		//Convert from ecliptic to Besselian
		PlanVec = mul(Rot_J_B, PlanVec);

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

		if (e > 1.0)
		{
			VECTOR3 R1, V1;
			double ddt;

			rv_from_r0v0(R, V, dt, R1, V1, mu);

			coe = coe_from_sv(R1, V1, mu);
			h = coe.h;
			e = coe.e;
			theta0 = coe.TA;

			dt_old = dt;
			err = time_theta(R1, V1, calculateDifferenceBetweenAngles(theta0, v1), mu, ddt);
			//Error return with current best estimate
			if (err)
			{
				return dt;
			}
			dt += ddt;
		}
		else
		{
			double T;

			a = h * h / mu * 1.0 / (1.0 - e * e);
			T = PI2 / sqrt(mu)*OrbMech::power(a, 3.0 / 2.0);

			dt_old = dt;
			err = time_theta(R, V, calculateDifferenceBetweenAngles(theta0, v1), mu, dt);
			//Error return with current best estimate
			if (err)
			{
				return dt;
			}
			if (dt < 0 && future)
			{
				dt += T;
			}
		}
		i++;
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
	S_SM = mul(tmat(SMNB), U_NB);
	U_LOS = mul(tmat(REFSMMAT), S_SM);
	return U_LOS;
}

VECTOR3 AOTNavigationBase(double AZ, double EL)
{
	return _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
}

VECTOR3 AOTULOS(MATRIX3 REFSMMAT, MATRIX3 SMNB, double AZ, double EL)
{
	VECTOR3 U_OAN, S_SM, U_LOS;

	U_OAN = AOTNavigationBase(AZ, EL);
	S_SM = mul(tmat(SMNB), U_OAN);
	U_LOS = mul(tmat(REFSMMAT), S_SM);
	return U_LOS;
}

int FindNearestStar(const VECTOR3 *navstars, VECTOR3 U_LOS, VECTOR3 R_C, double R_E, double ang_max)
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
		if (dotpr > last && isnotocculted(ustar, R_C, R_E) && dotpr > cos(ang_max))
		{
			star = i;
			last = dotpr;
		}
	}
	return star;
}

VECTOR3 backupgdcalignment(const VECTOR3 *navstars, MATRIX3 REFS, VECTOR3 R_C, double R_E, int &set)
{
	int starset[3][2];
	double a, SA, TA1, dTA, TA2;
	VECTOR3 s_SMA, s_SMB, s_NBA, s_NBB, imuang;
	MATRIX3 SBNB,SMNB;

	a = -0.5676353234;
	TA1 = 32.5*RAD; //50� mark is at 7.5� trunnion plus 25� from center 
	SA = PI;

	//Star 1: 50� mark. Star 2: R line
	starset[0][0] = 34;
	starset[0][1] = 29;

	starset[1][0] = 2;
	starset[1][1] = 4;

	starset[2][0] = 20;
	starset[2][1] = 27;

	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));

	for (set = 0; set < 3; set++)
	{
		//Get star unit vectors
		s_SMA = navstars[starset[set][0]];
		s_SMB = navstars[starset[set][1]];

		//Calculate angle between the two stars
		dTA = acos(dotp(s_SMA, s_SMB));

		//Calculate trunnion angle pointing to second star
		TA2 = TA1 - dTA;

		//Calculate star unit vectors in navigation base coordinates
		s_NBA = mul(SBNB, _V(sin(TA1)*cos(SA), sin(TA1)*sin(SA), cos(TA1)));
		s_NBB = mul(SBNB, _V(sin(TA2)*cos(SA), sin(TA2)*sin(SA), cos(TA2)));

		//Calculate stable member to navigation base matrix from sighting data
		SMNB = AXISGEN(s_NBA, s_NBB, s_SMA, s_SMB);

		//Calculate IMU/GDC angles from REFSMMAT and SMNB matrix
		imuang = CALCGAR(REFS, SMNB);

		//The first check is to prevent yaw angle from getting too large. 0.74 is roughly 1-cos(75�)
		if (cos(imuang.z*2.0) + 0.74>0 && isnotocculted(s_SMA, R_C, R_E) && isnotocculted(s_SMB, R_C, R_E))
		{
			return imuang;
		}
	}
	return _V(0, 0, 0);
}

MATRIX3 AGSStarAlignment(const VECTOR3 *navstars, VECTOR3 Att1, VECTOR3 Att2, int star1, int star2, int axis, int detent, double AOTCounter)
{
	//Matrix that converts from stable member to navigation base coordinates
	MATRIX3 SMNB1, SMNB2;
	//Unit star vectors in reference coordinates
	VECTOR3 s_REFA, s_REFB;
	//Unit star vectors in stable member coordinates
	VECTOR3 s_SMA, s_SMB;
	VECTOR3 u_OAN, u_XPN, u_YPN, u_XPN_apo, u_YPN_apo, u_XP, u_YP, u_YPN_aapo, s_NB;
	double SEP, EL, AZ, RN, YROT;

	//Get star unit vectors in BRCS coordinates
	s_REFA = navstars[star1 - 1];
	s_REFB = navstars[star2 - 1];

	//SM to NB matrix
	SMNB1 = CALCSMSC(Att1);
	SMNB2 = CALCSMSC(Att2);

	//Calculate angle between the two stars
	SEP = acos(dotp(s_REFA, s_REFB));

	EL = 45.0*RAD;
	AZ = (-60.0 + 60.0*(double)(detent - 1))*RAD;

	//Calculations for star 1
	u_OAN = _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
	u_YPN_apo = _V(0.0, cos(AZ), -sin(AZ));
	u_XPN_apo = crossp(u_YPN_apo, u_OAN);
	RN = 0.0 - AZ;
	u_XPN = u_XPN_apo * cos(RN) + u_YPN_apo * sin(RN);
	u_YPN = -u_XPN_apo * sin(RN) + u_YPN_apo * cos(RN);

	u_XP = tmul(SMNB1, u_XPN);
	u_YP = tmul(SMNB1, u_YPN);
	s_SMA = unit(crossp(u_XP, u_YP));

	//Calculations for star 2
	//axis: 0 = +X, 1 = -X, 2 = +Y, 3 = -Y
	switch (axis)
	{
	case 1:
		YROT = AOTCounter + 90.0*RAD;
		break;
	case 2:
		YROT = AOTCounter;
		break;
	case 3:
		YROT = AOTCounter + 180.0*RAD;
		break;
	default:
		YROT = AOTCounter + 270.0*RAD;
		break;
	}

	u_YPN_aapo = -u_XPN * sin(YROT) + u_YPN * cos(YROT);
	s_NB = u_OAN * cos(SEP) + crossp(u_YPN_aapo, u_OAN)*sin(SEP);
	s_SMB = tmul(SMNB2, s_NB);

	//REFSMMAT
	return AXISGEN(s_SMA, s_SMB, s_REFA, s_REFB);
}

bool isnotocculted(VECTOR3 S_SM, VECTOR3 R_C, double R_E, double dist)
{
	//Inputs:
	//S_SM: star unit vector in reference coordinates
	//R_C: Position vector of the spacecraft in reference coordinates
	//R_E: Radius of the primary body
	//dist: radius of occultation cone

	//Output: true = not occulted, false = star is occulted

	double c,dote;

	c = cos(dist + asin(R_E / length(R_C)));

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

MATRIX3 tmat(MATRIX3 a)
{
	MATRIX3 b;

	b = _M(a.m11, a.m21, a.m31, a.m12, a.m22, a.m32, a.m13, a.m23, a.m33);
	return b;
}

double LinearInterpolation(double x0, double y0, double x1, double y1, double x)
{
	return y0 + (x - x0)*(y1 - y0) / (x1 - x0);
}

void CubicInterpolation(double *x, double *y, double *a)
{
	double **V = NULL;
	double **A = NULL;
	int *P = NULL;
	std::vector<double> sol;
	double Tol;
	V = new double*[4];
	A = new double*[4];
	P = new int[5];
	sol.assign(4, 0);
	for (int i = 0;i < 4;i++)
	{
		V[i] = new double[4];
		A[i] = new double[4];
		for (int j = 0;j < 4;j++)
		{
			V[i][j] = 0.0;
			A[i][j] = 0.0;
		}
	}

	Tol = 0.0000000001;
	VandermondeMatrix(x, 3, V);

	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			A[i][j] = V[i][j];
		}
	}

	LUPDecompose(A, 4, Tol, P);
	LUPSolve(A, P, y, 4, sol);

	for (int i = 0;i < 4;i++)
	{
		a[i] = sol[i];
	}

	delete[] A;
	delete[] V;
	delete[] P;
}

//x = N+1 data points
//N = order of polynomial
//V = (N+1)*(N+1) Vandermonde matrix
void VandermondeMatrix(double *x, int N, double **V)
{
	for (int i = 0;i < N + 1;i++)
	{
		for (int j = 0;j < N + 1;j++)
		{
			V[i][j] = pow(x[i], N - j);
		}
	}
}

int LUPDecompose(double **A, int N, double Tol, int *P)
{

	int i, j, k, imax;
	double maxA, *ptr, absA;

	for (i = 0; i <= N; i++)
		P[i] = i; //Unit permutation matrix, P[N] initialized with N

	for (i = 0; i < N; i++) {
		maxA = 0.0;
		imax = i;

		for (k = i; k < N; k++)
			if ((absA = fabs(A[k][i])) > maxA) {
				maxA = absA;
				imax = k;
			}

		if (maxA < Tol)
		{
			return 0; //failure, matrix is degenerate
		}

		if (imax != i) {
			//pivoting P
			j = P[i];
			P[i] = P[imax];
			P[imax] = j;

			//pivoting rows of A
			ptr = A[i];
			A[i] = A[imax];
			A[imax] = ptr;

			//counting pivots starting from N (for determinant)
			P[N]++;
		}

		for (j = i + 1; j < N; j++) {
			A[j][i] /= A[i][i];

			for (k = i + 1; k < N; k++)
				A[j][k] -= A[j][i] * A[i][k];
		}
	}

	return 1;  //decomposition done 
}

void LUPSolve(double **A, int *P, double *b, int N, std::vector<double> &x) {

	for (int i = 0; i < N; i++) {
		x[i] = b[P[i]];

		for (int k = 0; k < i; k++)
			x[i] -= A[i][k] * x[k];
	}

	for (int i = N - 1; i >= 0; i--) {
		for (int k = i + 1; k < N; k++)
			x[i] -= A[i][k] * x[k];

		x[i] = x[i] / A[i][i];
	}
}

void LUPInvert(double **A, int *P, int N, double **IA) {

	for (int j = 0; j < N; j++) {
		for (int i = 0; i < N; i++) {
			if (P[i] == j)
				IA[i][j] = 1.0;
			else
				IA[i][j] = 0.0;

			for (int k = 0; k < i; k++)
				IA[i][j] -= A[i][k] * IA[k][j];
		}

		for (int i = N - 1; i >= 0; i--) {
			for (int k = i + 1; k < N; k++)
				IA[i][j] -= A[i][k] * IA[k][j];

			IA[i][j] = IA[i][j] / A[i][i];
		}
	}
}

void LinearLeastSquares(std::vector<double> &x, std::vector<double> &y, double &b1, double &b2)
{
	int N = x.size();

	double *xx = NULL;
	double *yy = NULL;
	xx = new double[N];
	yy = new double[N];

	for (int i = 0;i < N;i++)
	{
		xx[i] = x[i];
		yy[i] = y[i];
	}

	b1 = (double(N)*SumProd(xx, yy, N) - Sum(xx, N)*Sum(yy, N)) / ((double)N*SumQuad(xx, N) - QuadSum(xx, N));
	b2 = 1.0 / ((double)N)*(Sum(yy, N) - b1 * Sum(xx, N));

	delete[] xx;
	delete[] yy;
}

double Sum(double *x, int N)
{
	double a = 0.0;

	for (int i = 0;i < N;i++)
	{
		a += x[i];
	}
	return a;
}

double SumProd(double *x, double *y, int N)
{
	double a = 0.0;

	for (int i = 0;i < N;i++)
	{
		a += x[i]*y[i];
	}
	return a;
}

double SumQuad(double *x, int N)
{
	double a = 0.0;

	for (int i = 0;i < N;i++)
	{
		a += x[i] * x[i];
	}
	return a;
}

double QuadSum(double *x, int N)
{
	double a = Sum(x, N);
	return a * a;
}

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

void AGCSignedValue(int &val)
{
	if (val > 037777)
		val = -(077777 - val);
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

int DoubleToDEDA(double x, double q)
{
	int c = 0, out = 0, f = 1;

	x = x * (268435456.0 / pow(2.0, fabs(q)));

	c = 0x3FFF & ((int)fabs(x));

	if (x<0.0) c = 0x7FFF & (~c) + 1; // Polarity change

	while (c != 0) {
		out += (c & 7) * f;
		f *= 10;	c = c >> 3;
	}
	if (x < 0.0) out = -out;
	return out;
}

double DecToDouble(int dec1, int dec2)
{
	if (dec1 > 037777)
		dec1 = -(077777 - dec1);
	if (dec2 > 037777)
		dec2 = -(077777 - dec2);

	double val = OrbMech::power(2.0, -14.0)*dec1 + OrbMech::power(2.0, -28.0)*dec2;

	return val;
}

double OctToDouble(int oct1, int oct2)
{
	unsigned long long bin1, bin2;
	double dec1, dec2,dubb;
	bin1 = octal_binary(oct1);
	bin2 = octal_binary(oct2);
	dec1 = (double)BinToDec(bin1);
	dec2 = (double)BinToDec(bin2);
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
		decimal += (n % 10)*(int)pow(8, i);
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

int SingleToBuffer(double x, int SF, bool TwosComplement)
{
	double x2;
	int c;

	x2 = fabs(x) * pow(2, -SF + 14);

	c = (unsigned)round(x2);

	if (TwosComplement == false && c > 037777)
	{
		c = 037777;
	}

	if (x < 0.0)
	{
		// Polarity change
		c = 0x7FFF & (~c);
		if (TwosComplement)
		{
			c++;
		}
	}

	return c;
}

void DoubleToBuffer(double x, int SF, int &c1, int &c2)
{
	double x2;

	x2 = fabs(x) * pow(2, -SF + 14);

	c1 = (unsigned)x2;
	x2 = x2 - (double)c1;
	x2 *= pow(2, 14);
	c2 = (unsigned)round(x2);
	if (c2 > 037777)
	{
		c2 = 037777;
	}

	if (x < 0.0) c1 = 0x7FFF & (~c1); // Polarity change
	if (x < 0.0) c2 = 0x7FFF & (~c2); // Polarity change
}

void TripleToBuffer(double x, int SF, int &c1, int &c2, int &c3)
{
	double x2;

	x2 = fabs(x) * pow(2, -SF + 14);

	c1 = (unsigned)x2;
	x2 = x2 - (double)c1;
	x2 *= pow(2, 14);
	c2 = (unsigned)x2;
	x2 = x2 - (double)c2;
	x2 *= pow(2, 14);
	c3 = (unsigned)round(x2);
	if (c3 > 037777)
	{
		c3 = 037777;
	}

	if (x < 0.0) c1 = 0x7FFF & (~c1); // Polarity change
	if (x < 0.0) c2 = 0x7FFF & (~c2); // Polarity change
	if (x < 0.0) c3 = 0x7FFF & (~c3); // Polarity change
}

double cot(double a)
{
	return cos(a) / sin(a);
}

double sec(double a)
{
	return 1.0 / cos(a);
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

double TJUDAT(int Y, int M, int D)
{
	int Y_apo = Y - 1900;
	int TMM[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

	int Z = Y_apo / 4;
	if (Y_apo % 4 == 0)
	{
		Z = Z - 1;
		for (int i = 2;i < 12;i++)
		{
			TMM[i] += 1;
		}
	}
	return 2415020.5 + (double)(365 * Y_apo + Z + TMM[M - 1] + D - 1);
}

MATRIX3 J2000EclToBRCSMJD(double mjd)
{
	double t1 = (mjd - 51544.5) / 36525.0;
	double t2 = t1 * t1;
	double t3 = t2 * t1;

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

double MJDOfNBYEpoch(int epoch)
{
	//Calculate MJD of Besselian epoch
	double C, DE, MJD, JD, T;
	int E, XN;
	const double A = 0.0929;
	const double B = 8640184.542;
	const double W1 = 1.720217954160054e-2;

	E = epoch;
	XN = (E - 1901) / 4;
	C = -86400.0*(double)(E - 1900) - 74.164;
	T = 2.0 * C / (-B - sqrt(B*B - 4.0 * A*C));
	DE = 36525.0*T - 365.0*(double)(E - 1900) + 0.5 - (double)XN;

	JD = TJUDAT(epoch, 1, 0);
	MJD = JD - 2400000.5 + DE;
	return MJD;
}

MATRIX3 J2000EclToBRCS(int epoch)
{
	//Calculate the rotation matrix between J2000 and mean Besselian of epoch coordinate systems 
	double MJD = MJDOfNBYEpoch(epoch);
	return J2000EclToBRCSMJD(MJD);
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
	if (a < 0.0)
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

MATRIX3 SBNBMatrix()
{
	double a = -0.5676353234;
	return _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
}

MATRIX3 NBSBMatrix()
{
	return tmat(SBNBMatrix());
}

VECTOR3 SXTNB(double TA, double SA)
{
	//To obtain a unit vector which specifies the direction of the line-of-sight of the sextant in navigation base coordinates
	return mul(SBNBMatrix(), _V(sin(TA)*cos(SA), sin(TA)*sin(SA), cos(TA)));
}

VECTOR3 CALCGAR(MATRIX3 REFSM, MATRIX3 SMNB)
{
	//Input: REFSMMAT and stable member orientation
	//Output: Gimbal Angles
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
	//Input: Gimbal angles
	//Output: Stable member coordinates
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
	//Input: Desired stable member axes referred to present stable member orientation
	//Output: Gyro torquing angles
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
	//Input: Stable member/navigation base matrix, unit star vector
	//Output: Trunnion and shaft angles
	MATRIX3 SBNB, NBSB;
	VECTOR3 X_SB, Y_SB, Z_SB, S_SB, U_TPA;
	double a, sinSA, cosSA;

	X_SB = _V(1.0, 0.0, 0.0);
	Z_SB = _V(0.0, 0.0, 1.0);
	Y_SB = _V(0.0, 1.0, 0.0);

	a = -0.5676353234;
	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
	NBSB = tmat(SBNB);

	S_SB = mul(NBSB, mul(SMNB, S_SM));
	U_TPA = unit(crossp(Z_SB, S_SB));
	sinSA = dotp(U_TPA, -X_SB);
	cosSA = dotp(U_TPA, Y_SB);
	SA = atan3(sinSA, cosSA);
	TA = acos(dotp(Z_SB, S_SB));
}

void CALCCOASA(MATRIX3 SMNB, VECTOR3 S_SM, double &SPA, double &SXP) 
{
	//Input: Stable member/navigation base matrix, unit star vector
	//Output: Star pitch angle and star X position
	MATRIX3 SBNB, NBSB;
	VECTOR3 X_SB, Y_SB, Z_SB, S_SB;
	double a;

	X_SB = _V(1.0, 0.0, 0.0);
	Z_SB = _V(0.0, 0.0, 1.0);
	Y_SB = _V(0.0, 1.0, 0.0);

	a = -PI05;
	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));
	NBSB = tmat(SBNB);

	S_SB = mul(NBSB, mul(SMNB, S_SM));
	
	SXP = atan(S_SB.y / S_SB.z);//asin(S_SB.x);
	SPA = asin(S_SB.x);//atan(-S_SB.y / S_SB.z);
}

MATRIX3 AXISGEN(VECTOR3 s_NBA, VECTOR3 s_NBB, VECTOR3 s_SMA, VECTOR3 s_SMB)
{
	VECTOR3 X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, X, Y, Z;

	X_NB = s_NBA;
	Y_NB = unit(crossp(s_NBA, s_NBB));
	Z_NB = crossp(X_NB, Y_NB);

	X_SM = s_SMA;
	Y_SM = unit(crossp(s_SMA, s_SMB));
	Z_SM = crossp(X_SM, Y_SM);

	X = X_SM*X_NB.x + Y_SM*Y_NB.x + Z_SM*Z_NB.x;
	Y = X_SM*X_NB.y + Y_SM*Y_NB.y + Z_SM*Z_NB.y;
	Z = X_SM*X_NB.z + Y_SM*Y_NB.z + Z_SM*Z_NB.z;

	return _M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z);
}

MATRIX3 ROTCOMP(VECTOR3 U_R, double A)
{
	MATRIX3 I, R;
	
	I = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	R = I * cos(A) + tensorp(U_R, U_R)*(1.0 - cos(A)) + skew(U_R)*sin(A);
	return R;
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

void REVUP(VECTOR3 R, VECTOR3 V, double n, double mu, VECTOR3 &R1, VECTOR3 &V1, double &t)
{
	double a;

	a = 1.0 / (2.0/length(R) - dotp(V, V) / mu);
	t = n * PI2*sqrt(power(a, 3.0) / mu);
	rv_from_r0v0(R, V, t, R1, V1, mu);
}

void RADUP(VECTOR3 R_W, VECTOR3 V_W, VECTOR3 R_C, double mu, VECTOR3 &R_W1, VECTOR3 &V_W1)
{
	double theta, dt;

	theta = sign(dotp(crossp(R_W, R_C), crossp(R_W, V_W)))*acos(dotp(R_W / length(R_W), R_C / length(R_C)));
	time_theta(R_W, V_W, theta, mu, dt);
	rv_from_r0v0(R_W, V_W, dt, R_W1, V_W1, mu);
}

void COE(VECTOR3 R, VECTOR3 V, double h, double mu, VECTOR3 &R_C, VECTOR3 &V_C)
{
	double a, a_D, v_V;

	a = 1.0 / (2.0 / length(R) - dotp(V, V) / mu);
	a_D = a - h;
	v_V = dotp(V, R)*power(a / a_D, 1.5) / length(R);
	R_C = R - unit(R)*h;
	V_C = unit(crossp(crossp(R, V), R))*sqrt(mu*(2.0 / length(R_C) - 1.0 / a_D) - v_V*v_V) + R_C*v_V/length(R_C);
}

void ITER(double &c, int &s, double e, double &p, double &x, double &eo, double &xo, double dx0)
{
	double dx;

	if (c == 0)
	{
		dx = dx0;
		c = c + 1.0;eo = e;xo = x;x = x - dx;
	}
	else if (c == 0.5)
	{
		dx = e / p;
		c = c + 1.0;eo = e;xo = x;x = x - dx;
	}
	else
	{
		if (e - eo == 0)
		{
			dx = 3.0*dx0;
			c = c + 1.0;eo = e;xo = x;x = x - dx;
		}
		else
		{
			p = (e - eo) / (x - xo);
			if (c > 15)
			{
				s = 1;
			}
			else
			{
				dx = e / p;
				c = c + 1.0;eo = e;xo = x;x = x - dx;
			}
		}
	}
}

bool QDRTPI(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double mu, double dh, double E_L, int s, VECTOR3 &R_J, VECTOR3 &V_J)
{
	int s_F;
	double c, t, e_T, e_To, to, eps1, p;

	eps1 = 0.00001;
	p = 1.0;

	c = t = 0.0;
	s_F = 0;
	
	if (E_L > PI)
	{
		E_L = E_L - PI;
	}

	do
	{
		if (s == 1)
		{
			oneclickcoast(Epoch, R, V, MJD, t, R_J, V_J, gravref, gravref);
		}
		else
		{
			rv_from_r0v0(R, V, t, R_J, V_J, mu);
		}

		e_T = PI05 - E_L - asin(((length(R_J) - dh)*cos(E_L) / length(R))) - acos2(dotp(unit(R), unit(R_J)))*sign(dotp(crossp(R_J, R), crossp(R, V)));

		if (abs(e_T) >= eps1)
		{
			ITER(c, s_F, e_T, p, t, e_To, to);
			if (s_F == 1)
			{
				return false;
			}
		}

	} while (abs(e_T) >= eps1);
	return true;
}

bool CSIToDH(VECTOR3 R_A1, VECTOR3 V_A1, VECTOR3 R_P2, VECTOR3 V_P2, double DH, double mu, double &dv)
{
	int s_F;
	double c_I, tt, e_H, dvo, eps2, p_H, e_Ho;
	VECTOR3 u, R_A2, V_A2, V_A1F, R_PH2, V_PH2;

	p_H = c_I = 0.0;
	s_F = 0;
	eps2 = 1.0;

	u = unit(crossp(R_P2, V_P2));
	R_A1 = unit(R_A1 - u * dotp(R_A1, u))*length(R_A1);
	V_A1 = unit(V_A1 - u * dotp(V_A1, u))*length(V_A1);

	do
	{
		V_A1F = V_A1 + unit(crossp(u, R_A1))*dv;
		OrbMech::REVUP(R_A1, V_A1F, 0.5, mu, R_A2, V_A2, tt);
		//t_H2 = t_H1 + tt;
		OrbMech::RADUP(R_P2, V_P2, R_A2, mu, R_PH2, V_PH2);
		e_H = length(R_PH2) - length(R_A2) - DH;

		if (abs(e_H) >= eps2)
		{
			ITER(c_I, s_F, e_H, p_H, dv, e_Ho, dvo);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(e_H) >= eps2);

	return true;
}

VECTOR3 CoellipticDV(VECTOR3 R_A2, VECTOR3 R_PC, VECTOR3 V_PC, double mu)
{
	VECTOR3 u;
	double dH, v_PV, epsilon, a_P, a_A, v_AV, v_AH;

	u = unit(crossp(R_PC, V_PC));
	dH = length(R_PC) - length(R_A2);
	v_PV = dotp(V_PC, R_A2 / length(R_A2));
	epsilon = (length(V_PC)*length(V_PC)) / 2.0 - mu / length(R_PC);
	a_P = -mu / (2.0 * epsilon);
	a_A = a_P - dH;
	v_AV = v_PV * power((a_P / a_A), 1.5);
	v_AH = sqrt(mu*(2.0 / length(R_A2) - 1.0 / a_A) - (v_AV*v_AV));
	return unit(crossp(u, R_A2))*v_AH + unit(R_A2)*v_AV;
}

VECTOR3 ApplyHorizontalDV(VECTOR3 R, VECTOR3 V, double dv)
{
	return unit(crossp(unit(crossp(R, V)), R))*dv;
}

MATRIX3 LVLH_Matrix(VECTOR3 R, VECTOR3 V)
{
	VECTOR3 i, j, k;
	j = unit(crossp(V, R));
	k = unit(-R);
	i = crossp(j, k);
	return _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z); //rotation matrix to LVLH
}

MATRIX3 GetVesselToLocalRotMatrix(VESSEL *v)
{
	MATRIX3 Rot_VG, Rot_LG;

	//Vessel to global
	v->GetRotationMatrix(Rot_VG);
	//Local to global
	oapiGetRotationMatrix(v->GetGravityRef(), &Rot_LG);
	//Vessel to local
	return MatrixRH_LH(mul(tmat(Rot_LG), Rot_VG));
}

void xaxislambert(VECTOR3 RA1, VECTOR3 VA1, VECTOR3 RP2off, double dt2, int N, bool tgtprograde, double mu, VECTOR3 &VAP2, double &zoff)
{
	VECTOR3 RPP1, RPP2, VAP1, dVLV1, dVLV2;
	double f1, f2, r1, r2, y;
	MATRIX3 Q_Xx;
	int nmax, n;

	Q_Xx = LVLH_Matrix(RA1, VA1);

	f2 = 1;
	n = 0;
	nmax = 10;

	RPP1 = RP2off;
	RPP2 = RP2off*(length(RP2off) + 10.0) / length(RP2off);

	VAP1 = elegant_lambert(RA1, VA1, RPP1, dt2, N, tgtprograde, mu);
	VAP2 = elegant_lambert(RA1, VA1, RPP2, dt2, N, tgtprograde, mu);

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

		VAP2 = elegant_lambert(RA1, VA1, unit(RP2off)*r2, dt2, N, tgtprograde, mu);

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
	VECTOR3 U_R, g;
	double rr;

	U_R = unit(R);
	rr = dotp(R, R);

	if (gravref == oapiGetObjectByName("Earth"))
	{
		VECTOR3 g_b, U_Z;
		double costheta;

		U_Z = _V(0, 0, 1);

		costheta = dotp(U_R, U_Z);
		g_b = -(U_R*(1.0 - 5.0*costheta*costheta) + U_Z*2.0*costheta)*mu_Earth / rr*3.0 / 2.0*J2_Earth*power(R_Earth, 2.0) / rr;
		g = -U_R*mu_Earth / rr + g_b;
	}
	else
	{
		g = -U_R*mu_Moon / rr;
	}
	return g;
}

void impulsive(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_av, double isp, double m, VECTOR3 DV, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff)
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
		oneclickcoast(Epoch, R, V, MJD, t_slip, R_ig, V_ig, gravref, gravref);
		while ((length(dV_go) > 0.01 || n < 2) && n <= nmax)
		{
			poweredflight(R_ig, V_ig, MJD, gravref, f_av, isp, m, V_go, R_p, V_p, m_p, t_go);
			//rv_from_r0v0(R_ref, V_ref, t_go + t_slip, R_d, V_d, mu);
			oneclickcoast(Epoch, R_ref, V_ref, MJD, t_go + t_slip, R_d, V_d, gravref, gravref);
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
	
	Llambda = V_go;
	R_cutoff = R_p;
	V_cutoff = V_p;
	m_cutoff = m_p;
	MJD_cutoff = MJD + (t_go + t_slip) / 24.0 / 3600.0;
}

void impulsive(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_av, double isp, double m, VECTOR3 R_ref, VECTOR3 V_ref, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff)
{
	VECTOR3 R_ig, V_ig, V_go, dV_go, R_d, V_d, R_p, V_p, i_z, i_y;
	double t_slip_old, mu, t_go, v_goz, dr_z, dt_go, m_p;
	int n, nmax;

	nmax = 100;
	t_slip = 0;
	t_slip_old = 1;
	dt_go = 1;
	mu = GGRAV*oapiGetMass(gravref);
	V_go = V_ref - V;
	i_y = -unit(crossp(R_ref, V_ref));

	while (abs(t_slip - t_slip_old) > 0.01)
	{
		n = 0;
		//rv_from_r0v0(R, V, t_slip, R_ig, V_ig, mu);
		oneclickcoast(Epoch, R, V, MJD, t_slip, R_ig, V_ig, gravref, gravref);
		while ((length(dV_go) > 0.01 || n < 2) && n <= nmax)
		{
			poweredflight(R_ig, V_ig, MJD, gravref, f_av, isp, m, V_go, R_p, V_p, m_p, t_go);
			//rv_from_r0v0(R_ref, V_ref, t_go + t_slip, R_d, V_d, mu);
			oneclickcoast(Epoch, R_ref, V_ref, MJD, t_go + t_slip, R_d, V_d, gravref, gravref);
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

	Llambda = V_go;
	R_cutoff = R_p;
	V_cutoff = V_p;
	m_cutoff = m_p;
	MJD_cutoff = MJD + (t_go + t_slip) / 24.0 / 3600.0;
}

void impulsive2(int Epoch, VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double f_T, double f_av, double isp, double m, VECTOR3 R_ref, VECTOR3 V_ref, VECTOR3 &Llambda, double &t_slip, VECTOR3 &R_cutoff, VECTOR3 &V_cutoff, double &MJD_cutoff, double &m_cutoff)
{
	VECTOR3 R_ig, V_ig, V_go, dV_go, R_d, V_d, R_p, V_p, i_z, i_y;
	double t_slip_old, mu, t_go, v_goz, dr_z, dt_go, m_p;
	int n, nmax;

	nmax = 100;
	t_slip = 0;
	t_slip_old = 1;
	dt_go = 1;
	mu = GGRAV * oapiGetMass(gravref);
	V_go = V_ref - V;
	i_y = -unit(crossp(R_ref, V_ref));

	while (abs(t_slip - t_slip_old) > 0.01)
	{
		n = 0;
		//rv_from_r0v0(R, V, t_slip, R_ig, V_ig, mu);
		oneclickcoast(Epoch, R, V, MJD, t_slip, R_ig, V_ig, gravref, gravref);
		while ((length(dV_go) > 0.01 || n < 2) && n <= nmax)
		{
			poweredflight(R_ig, V_ig, MJD, gravref, f_av, isp, m, V_go, R_p, V_p, m_p, t_go);
			//rv_from_r0v0(R_ref, V_ref, t_go + t_slip, R_d, V_d, mu);
			oneclickcoast(Epoch, R_ref, V_ref, MJD, t_go + t_slip, R_d, V_d, gravref, gravref);
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
		t_slip += dt_go * 0.1;
	}
	if (n >= nmax)
	{
		sprintf(oapiDebugString(), "Iteration failed!");
	}
	Llambda = V_go;
	R_cutoff = R_p;
	V_cutoff = V_p;
	m_cutoff = m_p;
	MJD_cutoff = MJD + (t_go + t_slip) / 24.0 / 3600.0;
}

double GETfromMJD(double MJD, double GETBase)
{
	return (MJD - GETBase)*24.0*3600.0;
}

double MJDfromGET(double GET, double GETBase)
{
	return GETBase + GET / 24.0 / 3600.0;
}

void format_time_HHMMSS(char *buf, double time) {
	buf[0] = 0; // Clobber
	int hours, minutes, seconds;
	bool neg = false;
	if (time < 0)
	{
		time = abs(time);
		neg = true;
	}
	hours = (int)(time / 3600);
	minutes = (int)((time / 60) - (hours * 60));
	seconds = (int)((time - (hours * 3600)) - (minutes * 60));
	if (neg)
	{
		sprintf(buf, "-%03d:%02d:%02d", hours, minutes, seconds);
	}
	else
	{
		sprintf(buf, "%03d:%02d:%02d", hours, minutes, seconds);
	}
}

void format_time_MMSS(char *buf, double time) {
	buf[0] = 0; // Clobber
	int minutes, seconds;
	if (time < 0) { return; } // don't do that
	minutes = (int)(time / 60);
	seconds = (int)(time - (minutes * 60));
	sprintf(buf, "%d:%02d", minutes, seconds);
}

void checkstar(const VECTOR3 *navstars, MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &trunnion, double &shaft)
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
	star = OrbMech::FindNearestStar(navstars, U_LOS, R_C, R_E, 50.0*RAD);

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

void coascheckstar(const VECTOR3 *navstars, MATRIX3 REFSMMAT, VECTOR3 IMU, VECTOR3 R_C, double R_E, int &staroct, double &spa, double &sxp)
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
	star = FindNearestStar(navstars, U_LOS, R_C, R_E, 10.0*RAD);//31.5*RAD);

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

bool AOTcheckstar(VECTOR3 navstar, MATRIX3 REFSMMAT, VECTOR3 IMU, double AZ, double EL)
{
	//Returns true if a star is visible in the AOT

	MATRIX3 SMNB, Q1, Q2, Q3;
	double OGA, IGA, MGA;
	VECTOR3 U_LOS;

	OGA = IMU.x;
	IGA = IMU.y;
	MGA = IMU.z;

	Q1 = _MRy(IGA);
	Q2 = _MRz(MGA);
	Q3 = _MRx(OGA);

	SMNB = mul(Q3, mul(Q2, Q1));

	U_LOS = AOTULOS(REFSMMAT, SMNB, AZ, EL);

	//Is visible?
	if (acos(dotp(U_LOS, navstar)) < 30.0*RAD) return true;
	return false;
}

void AOTStarAcquisition(VECTOR3 navstar, MATRIX3 REFSMMAT, VECTOR3 IMU, double AZ, double EL, double &YROT, double &SROT)
{
	//Calculates reticle and spirale angles for a given star

	MATRIX3 SMNB;
	VECTOR3 SN; //Star in navigation base coordinates
	VECTOR3 u_OAN, UNITX, TS2, TS4;
	double theta, C1, C2;

	SMNB = CALCSMSC(IMU);
	SN = mul(SMNB, mul(REFSMMAT, navstar));

	u_OAN = _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
	C1 = dotp(u_OAN, SN);

	UNITX = _V(1, 0, 0);
	TS2 = unit(crossp(u_OAN, UNITX));
	TS4 = unit(crossp(u_OAN, SN));
	theta = acos(dotp(TS4, TS2));
	C2 = dotp(TS4, unit(crossp(u_OAN, TS2)));
	if (C2 < 0.0)
	{
		theta = PI2 - theta;
	}
	YROT = PI2 + theta + AZ;
	if (YROT >= PI2)
	{
		YROT -= PI2;
	}
	SROT = YROT + 12.0*acos(C1);
	if (SROT >= PI2)
	{
		SROT -= PI2;
	}
}

bool LMCOASCheckStar(VECTOR3 SI, MATRIX3 RMAT, VECTOR3 IMU, int Axis, double &EL, double &SPX)
{
	//SI - star position vector (ECI)
	//RMAT - REFSMMAT
	//IMU - Gimbal angles
	//Axis: 1 = PX, 2 = PZ

	MATRIX3 GA;
	VECTOR3 SN; //Star position vector - Navbase
	double EPS, GAM, ALP, SCV, R;

	GA = CALCSMSC(IMU);
	SN = mul(GA, mul(RMAT, SI));

	if (Axis == 1)
	{
		//Limited to -35� to -5�
		if (SN.z < 0)
		{
			if (acos(SN.x) > 5.0*RAD) return false;
		}
		else
		{
			if (acos(SN.x) > 35.0*RAD) return false;
		}

		double ARG1, ARG2, ARG3;

		EPS = acos(SN.x);
		GAM = acos(SN.z);
		ALP = atan(SN.y / SN.x);
		ARG1 = sin(GAM)*sin(ALP) / sin(EPS);
		SCV = PI05 - abs(asin(ARG1));
		ARG2 = sin(SCV)*sin(EPS);
		R = SN.z*abs(asin(ARG2) / SN.z);
		ARG3 = cos(EPS) / cos(R);
		SPX = SN.y*abs(acos(ARG3) / SN.y);
		if (SN.y < 0)
		{
			SPX = -abs(SPX);
		}
		else
		{
			SPX = abs(SPX);
		}
		EL = R;
		if (SN.z < 0)
		{
			EL = abs(EL);
		}
		else
		{
			EL = -abs(EL);
		}
	}
	else
	{
		//Check if star is within 5 degrees of Y-Z plane
		if (abs(SN.x) > cos(85.0*RAD)) return false;
		if (SN.y < 0)
		{
			if (acos(SN.z) >= 70.0*RAD) return false;
		}
		else
		{
			if (acos(SN.z) >= 10.0*RAD) return false;
		}

		double HYP;

		EPS = acos(SN.z);
		GAM = acos(SN.x);
		HYP = sqrt(SN.x*SN.x + SN.y*SN.y);
		ALP = atan(HYP / SN.z);
		SCV = PI05 - abs(asin(sin(GAM)*sin(ALP) / sin(EPS)));
		R = SN.x*abs(asin(sin(SCV)*sin(EPS)) / SN.x);
		if (SN.x < 0)
		{
			SPX = -abs(R);
		}
		else
		{
			SPX = abs(R);
		}
		EL = SN.y*abs(acos(cos(EPS) / cos(SPX)) / SN.y);
		if (SN.y < 0.0)
		{
			EL = abs(EL);
		}
		else
		{
			EL = -abs(EL);
		}
		EL -= 30.0*RAD;
	}
	return true;
}

VECTOR3 imulimit(VECTOR3 a)
{
	return _V(imulimit(a.x), imulimit(a.y), imulimit(a.z));
}

double imulimit(double a)
{
	if (a < 0)
	{
		a += 360.0;
	}
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

void normalizeAngle(double & a, bool positive)
{
	a = fmod(a, PI2);
	if (a < 0)
		a += PI2;
	if (!positive)
	{
		if (a > PI)
		{
			a -= PI2;
		}
	}
}

double quadratic(double *T, double *DV)
{
	double a, b, x;

	a = -2.0*(-DV[0] * T[1] + DV[0] * T[2] + DV[1] * T[0] - DV[1] * T[2] - DV[2] * T[0] + DV[2] * T[1]) / ((T[1] - T[0])*(T[2] - T[0])*(T[1] - T[2]));
	b = (DV[0] * T[1] * T[1] - DV[0] * T[2] * T[2] - DV[1] * T[0] * T[0] + DV[1] * T[2] * T[2] + DV[2] * T[0] * T[0] - DV[2] * T[1] * T[1]) / ((T[0] - T[1])*(T[0] - T[2])*(T[2] - T[1]));

	x = -b / a;

	return x;
}

VECTOR3 LMDockedFineAlignment(VECTOR3 lmang, VECTOR3 csmang, bool samerefs)
{
	MATRIX3 LM_REFS, CSM_REFS;

	LM_REFS = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	if (samerefs)
	{
		//REFSMMAT in CMC and LGC identical
		CSM_REFS = LM_REFS;
	}
	else
	{
		//LVLH REFSMMAT in both CMC and LM
		CSM_REFS = _M(LM_REFS.m31, LM_REFS.m32, LM_REFS.m33, LM_REFS.m21, LM_REFS.m22, LM_REFS.m23, -LM_REFS.m11, -LM_REFS.m12, -LM_REFS.m13);
	}

	return finealignLMtoCSM(lmang, csmang, CSM_REFS, LM_REFS);
}

VECTOR3 finealignLMtoCSM(VECTOR3 lmn20, VECTOR3 csmn20, MATRIX3 LM_REFSMMAT, MATRIX3 CSM_REFSMMAT)
{
	MATRIX3 lmmat, csmmat, RX, RY, summat, expmat;
	double DockingAngle;

	DockingAngle = 0.0;

	lmmat = OrbMech::CALCSMSC(lmn20);
	csmmat = OrbMech::CALCSMSC(csmn20);
	RX = OrbMech::_MRx(60.0*RAD - DockingAngle);
	RY = OrbMech::_MRy(180.0*RAD);
	summat = mul(RY, mul(RX, csmmat));
	expmat = mul(summat, tmat(mul(CSM_REFSMMAT, tmat(LM_REFSMMAT))));

	return OrbMech::CALCGTA(mul(OrbMech::tmat(expmat), lmmat));
}

VECTOR3 GimbalAngleConversion(MATRIX3 REFSMMAT1, VECTOR3 GimbalAngles, MATRIX3 REFSMMAT2, bool isCSM)
{
	//INPUT:
	//REFSMMAT1: Current REFSMMAT
	//GimbalAngles: IMU angles with REFSMMAT2
	//REFSMMAT2: Preferred REFSMMAT
	//isCSM: CSM or LM

	//OUTPUT:
	//IMU or FDAI angles

	//Compute rotation matrix for inertial 
	MATRIX3 M_SM_NB, M_BRCS_NB;
	VECTOR3 GimbalAngles2;

	M_SM_NB = CALCSMSC(GimbalAngles);
	M_BRCS_NB = mul(M_SM_NB, REFSMMAT2);
	GimbalAngles2 = CALCGAR(REFSMMAT1, M_BRCS_NB);

	if (isCSM == false)
	{
		//TBD
	}
	return GimbalAngles2;
}

VECTOR3 LMDockedCoarseAlignment(VECTOR3 csmang, bool samerefs)
{
	MATRIX3 LM_REFS, CSM_REFS;

	LM_REFS = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	if (samerefs)
	{
		//REFSMMAT in CMC and LGC identical
		CSM_REFS = LM_REFS;
	}
	else
	{
		//LVLH REFSMMAT in both CMC and LM
		CSM_REFS = _M(LM_REFS.m31, LM_REFS.m32, LM_REFS.m33, LM_REFS.m21, LM_REFS.m22, LM_REFS.m23, -LM_REFS.m11, -LM_REFS.m12, -LM_REFS.m13);
	}

	return LMIMU_from_CSMIMU(CSM_REFS, LM_REFS, csmang);
}

VECTOR3 LMIMU_from_CSMIMU(MATRIX3 CSM_REFSMMAT, MATRIX3 LM_REFSMMAT, VECTOR3 csmang)
{
	MATRIX3 csmmat, M_CSM_LM, lmmat, lmmat2;
	double DockingAngle;

	DockingAngle = 0.0;

	csmmat = CALCSMSC(csmang);
	M_CSM_LM = CSMBodyToLMBody(DockingAngle);
	lmmat = mul(M_CSM_LM, csmmat);
	lmmat2 = mul(lmmat, mul(CSM_REFSMMAT, tmat(LM_REFSMMAT)));

	return OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), lmmat2);
}

MATRIX3 CSMBodyToLMBody(double da)
{
	MATRIX3 RX, RY;

	RX = OrbMech::_MRx(60.0*RAD - da);
	RY = OrbMech::_MRy(180.0*RAD);

	return mul(RY, RX);
}

double QuadraticIterator(int &c, int &s, double &varguess, double *var, double *obj, double obj0, double initstep, double maxstep)
{
	double dvar;

	if (c < 2)
	{
		var[c + 1] = varguess;//(TIGMJD - SVMJD)*24.0*3600.0;
		obj[c + 1] = obj0;//length(DVX);

		dvar = initstep;
	}
	else
	{
		obj[0] = obj[1];
		obj[1] = obj[2];
		obj[2] = obj0;//length(DVX);
		var[0] = var[1];
		var[1] = var[2];
		var[2] = varguess;//(TIGMJD - SVMJD)*24.0*3600.0;

		dvar = OrbMech::quadratic(var, obj) - varguess;// +(SVMJD - TIGMJD)*24.0*3600.0;

		//if the calculated change (dvar) is greater than maxstep, limit change to maxstep
		if (abs(dvar)>maxstep)
		{
			dvar = OrbMech::sign(dvar)*maxstep;

			//If it tries to limit dvar twice in a row, we could get a bad result. So use maxstep/2 instead.
			if (varguess + dvar - var[1] == 0)
			{
				dvar = OrbMech::sign(dvar)*maxstep / 2.0;
			}
		}
	}

	varguess += dvar;

	c++;

	if (c > 100)
	{
		s = 1;
	}


	return dvar;
}

bool SolveSystem(int n, double *A, double *b, double *x, double *det)
{
	int e = 0, *p = new int[n];
	for (int i = 0;i<n;i++) p[i] = i;
	for (int k = 0;k<n;k++) {
		int r = 0; double d = 0.0;
		for (int s = k;s<n;s++) if (fabs(A[s*n + k])>d) { d = fabs(A[s*n + k]); r = s; }
		if (d == 0.0) { delete[]p; return false; }
		if (r != k) { // Do Swaps
			for (int i = 0;i<n;i++) { double x = A[k*n + i]; A[k*n + i] = A[r*n + i]; A[r*n + i] = x; }
			int x = p[k]; p[k] = p[r]; p[r] = x; e++;
		}
		for (int i = k + 1;i<n;i++) { A[i*n + k] /= A[k*n + k]; for (int j = k + 1;j<n;j++) A[i*n + j] -= (A[i*n + k] * A[k*n + j]); }
	}
	for (int i = 0;i<n;i++) { x[i] = b[p[i]];	for (int j = 0;j<i;j++) x[i] -= A[i*n + j] * x[j]; }
	for (int i = n - 1;i >= 0;i--) { for (int j = i + 1;j<n;j++) x[i] -= A[i*n + j] * x[j]; x[i] /= A[i*n + i]; }
	if (det) { *det = 1.0; for (int i = 0;i<n;i++) *det *= A[i*n + i]; if (e & 1) *det *= -1.0; }
	delete[]p;
	return true;
}


bool SolveSeries(double *x, double *y, int ndata, double *out, int m)
{

	double *v = new double[m];
	double *q = new double[m];
	double *M = new double[m*m];

	memset(M, 0, m*m * sizeof(double));
	memset(q, 0, m * sizeof(double));

	for (int i = 0;i<ndata;i++) {
		v[0] = 1.0;
		for (int f = 1;f<m;f++) v[f] = v[f - 1] * x[i];
		for (int f = 0;f<m;f++) for (int g = 0;g<m;g++) M[f*m + g] += (v[f] * v[g]);
		for (int f = 0;f<m;f++) q[f] += (v[f] * y[i]);
	}

	bool bRet = SolveSystem(m, M, q, out, NULL);
	delete[]v; delete[]q; delete[]M;
	return bRet;
}

double GetSemiMajorAxis(VECTOR3 R, VECTOR3 V, double mu)
{
	double eps = length(V)*length(V) / 2.0 - mu / length(R);
	return -mu / (2.0*eps);
}

double GetMeanMotion(VECTOR3 R, VECTOR3 V, double mu)
{
	return sqrt(mu / pow(GetSemiMajorAxis(R, V, mu), 3));
}

double CMCEMSRangeToGo(MATRIX3 Rot_J_B, VECTOR3 R05G, double MJD05G, double lat, double lng)
{
	//INPUT:
	// R05G: position vector at 0.05G in BRCS coordinates
	// MJD05G: MJD at 0.05G
	// lat: splashdown latitude
	// lng: splashdown longitude

	MATRIX3 Rot2;
	VECTOR3 R_P, R_LS, URT0, UUZ, RTE, UTR, urh, URT;
	double WIE, KTETA, theta_rad, WT;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)));
	Rot2 = GetRotationMatrix(BODY_EARTH, MJD05G);
	R_LS = mul(Rot_J_B, rhmul(Rot2, R_P));
	URT0 = R_LS;
	WIE = 72.9211505e-6;
	KTETA = 1000.0;
	UUZ = _V(0, 0, 1);
	RTE = crossp(UUZ, URT0);
	UTR = crossp(RTE, UUZ);
	urh = unit(R05G);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
	theta_rad = acos(dotp(URT0, urh));
	for (int i = 0;i < 3;i++)
	{
		WT = WIE * (KTETA*theta_rad);
		URT = URT0 + UTR * (cos(WT) - 1.0) + RTE * sin(WT);
		theta_rad = acos(dotp(URT, urh));
	}
	return theta_rad * 3437.7468;
}

void EMXINGElev(VECTOR3 R, VECTOR3 R_S, VECTOR3 &N, VECTOR3 &rho, double &sinang)
{
	VECTOR3 rho_apo;

	N = unit(R_S);
	rho = R - R_S;
	rho_apo = unit(rho);
	sinang = dotp(rho_apo, N);
}

double EMXINGElevSlope(VECTOR3 R, VECTOR3 V, VECTOR3 R_S, int body)
{
	VECTOR3 V_S, N, rho, rho_apo, W_E, rho_dot, N_dot;
	double w_E;

	if (body == BODY_EARTH)
	{
		w_E = w_Earth;
	}
	else
	{
		w_E = w_Moon;
	}

	N = unit(R_S);
	rho = R - R_S;
	rho_apo = unit(rho);
	
	W_E = _V(0, 0, 1)*w_E;
	V_S = crossp(W_E, R_S);
	rho_dot = (V - V_S) / length(rho);

	N_dot = V_S / length(R_S);

	return (dotp(rho_dot, N) + dotp(rho_apo, N_dot))*length(rho);
}

CELEMENTS KeplerToEquinoctial(CELEMENTS kep)
{
	CELEMENTS aeq;

	aeq.a = kep.a;
	aeq.e = kep.e*sin(kep.g + kep.h);
	aeq.i = kep.e*cos(kep.g + kep.h);
	aeq.h = sin(kep.i / 2.0)*sin(kep.h);
	aeq.g = sin(kep.i / 2.0)*cos(kep.h);
	aeq.l = kep.h + kep.g + kep.l;
	if (aeq.l >= PI2)
	{
		aeq.l -= PI2;
	}

	return aeq;
}

CELEMENTS EquinoctialToKepler(CELEMENTS aeq)
{
	CELEMENTS kep;

	kep.a = aeq.a;
	kep.e = sqrt(aeq.e*aeq.e + aeq.i*aeq.i);
	if ((aeq.h*aeq.h + aeq.g*aeq.g) <= 1.0)
		kep.i = acos2(1.0 - 2.0*(aeq.h*aeq.h + aeq.g*aeq.g));
	if ((aeq.h*aeq.h + aeq.g*aeq.g) > 1.0)
		kep.i = acos2(1.0 - 2.0*1.0);

	kep.h = atan2(aeq.h, aeq.g);
	while (kep.h < 0)
	{
		kep.h += PI2;
	}
	kep.g = atan2(aeq.e, aeq.i) - kep.h;
	while (kep.g < 0)
	{
		kep.g += PI2;
	}
	kep.l = aeq.l - atan2(aeq.e, aeq.i);
	while (kep.l >= PI2)
	{
		kep.l -= PI2;
	}
	while (kep.l < 0)
	{
		kep.l += PI2;
	}

	return kep;
}

CELEMENTS LyddaneOsculatingToMean(CELEMENTS arr_osc, int body)
{
	CELEMENTS arr_mean, arr_osc2, arr_mean2;
	CELEMENTS aeq, aeq2, aeq_mean, aeq_mean2;
	int j;
	bool stop, pseudostate = false;

	if (arr_osc.i > 175.0*RAD)
	{
		arr_osc.i = PI - arr_osc.i;
		arr_osc.h = -arr_osc.h + PI2;
		pseudostate = true;
	}

	aeq = KeplerToEquinoctial(arr_osc);
	aeq_mean = aeq;
	stop = true;
	j = 0;
	do
	{
		arr_mean = EquinoctialToKepler(aeq_mean);
		arr_osc2 = LyddaneMeanToOsculating(arr_mean, body);
		aeq2 = KeplerToEquinoctial(arr_osc2);
		aeq_mean2 = aeq_mean - (aeq2 - aeq);
		arr_mean2 = EquinoctialToKepler(aeq_mean2);

		//Map to 0 to 2PI
		while (arr_mean2.l < 0)
		{
			arr_mean2.l += PI2;
		}
		while (arr_mean2.l >= PI2)
		{
			arr_mean2.l -= PI2;
		}
		while (arr_mean2.g < 0)
		{
			arr_mean2.g += PI2;
		}
		while (arr_mean2.g >= PI2)
		{
			arr_mean2.g -= PI2;
		}
		if (arr_mean2.e < 1e-6)
		{
			arr_mean2.e = 1e-6;
		}

		if (abs(arr_mean2.a - arr_mean.a) > 0.1)
		{
			stop = 0;
		}
		else if (abs(arr_mean2.e - arr_mean.e) > 0.0001)
		{
			stop = 0;
		}
		else if (abs(arr_mean2.i - arr_mean.i) > 0.0001)
		{
			stop = 0;
		}
		else if (abs(arr_mean2.l - arr_mean.l) > 0.0001)
		{
			stop = 0;
		}
		else if (abs(arr_mean2.g - arr_mean.g) > 0.0001)
		{
			stop = 0;
		}
		else if (abs(arr_mean2.h - arr_mean.h) > 0.0001)
		{
			stop = 0;
		}
		aeq_mean = aeq_mean2;
		if (stop == 1)
		{
			break;
		}
		stop = 1;		
		j = j + 1;
	} while (j < 25);

	if (pseudostate != 0)
	{
		arr_mean2.i = PI - arr_mean2.i;
		arr_mean2.h = -arr_mean2.h + PI2;
	}

	return arr_mean2;
}

CELEMENTS LyddaneMeanToOsculating(CELEMENTS arr, int body)
{
	CELEMENTS out;
	double ae, am, Em, fm, J2, J3, J4, R_e, cn, cn2, theta, theta2, theta4, eccdp2, sinI, sinI2, cosI2, adr, adr2, adr3, a;
	double sinta, costa, costa2, sn2gta, cs2gta, sinGD, cosGD, sin2gd, cs2gd, sn3fgd, snf2gd, csf2gd, cs3fgd;
	double k2, k3, k4, gamma2, gamma3, gamma4, gamma2_apo, gamma3_apo, gamma4_apo, g3dg2, g4dg2;
	double A1_apo, A1, A2_apo, A2, A6, A7, A10, A11, A12, A13, A14, A15, A16, A17, A18, A20, A21, A25, A26;
	double B1, B2, B4, B5, B7, B8, B10, B11, B13, B14;
	double delta1e, de, edl, di, sin_im2_dh, lagaha, lgh, sinMADP, cosMADP, sinraandp, cosraandp;
	bool pseudostate = false;

	if (body == BODY_EARTH)
	{
		J2 = J2_Earth;
		J3 = J3_Earth;
		J4 = J4_Earth;
		R_e = R_Earth;
	}
	else
	{
		J2 = J2_Moon;
		J3 = J3_Moon;
		J4 = 0;
		R_e = R_Moon;
	}

	if (arr.i > 175.0*RAD)
	{
		arr.i = PI - arr.i;
		arr.h = -arr.h;
		pseudostate = true;
	}

	ae = 1.0;
	am = arr.a / R_e;
	eccdp2 = arr.e*arr.e;
	cn2 = 1.0 - eccdp2;
	cn = sqrt(cn2);
	theta = cos(arr.i);
	theta2 = theta * theta;
	theta4 = theta2 * theta2;
	k2 = J2 * pow(ae, 2) / 2.0;
	k3 = -J3 * pow(ae, 3);
	k4 = -3.0 * J4*pow(ae,4) / 8.0;
	gamma2 = k2 / pow(am, 2);
	gamma3 = k3 / pow(am, 3);
	gamma4 = k4 / pow(am, 4);
	gamma2_apo = gamma2 / pow(cn, 4);
	gamma3_apo = gamma3 / pow(cn, 6);
	gamma4_apo = gamma4 / pow(cn, 8);

	g3dg2 = gamma3_apo / gamma2_apo;
	g4dg2 = gamma4_apo / gamma2_apo;

	Em = kepler_E(arr.e, arr.l, 1e-12);
	fm = atan2(sqrt(1.0 - eccdp2)*sin(Em), cos(Em) - arr.e);
	if (fm < 0)
	{
		fm = fm + PI2;
	}
	adr = 1.0 / (1.0 - arr.e*cos(Em));
	adr2 = adr * adr;
	adr3 = adr2 * adr;

	sinI = sin(arr.i);
	sinI2 = sin(arr.i / 2.0);
	cosI2 = cos(arr.i / 2.0);
	sinta = sin(fm);
	costa = cos(fm);
	costa2 = costa * costa;
	sn2gta = sin(2.0*arr.g + 2.0*fm);
	cs2gta = cos(2.0*arr.g + 2.0*fm);
	snf2gd = sin(2.0 * arr.g + fm);
	csf2gd = cos(2.0 * arr.g + fm);
	sinGD = sin(arr.g);
	cosGD = cos(arr.g);
	sin2gd = sin(2.0 * arr.g);
	cs2gd = cos(2.0 * arr.g);
	sn3fgd = sin(3.0*fm + 2.0*arr.g);
	cs3fgd = cos(3.0*fm + 2.0*arr.g);
	sinMADP = sin(arr.l);
	cosMADP = cos(arr.l);
	sinraandp = sin(arr.h);
	cosraandp = cos(arr.h);

	A1_apo = 1.0 / (1.0 - 5.0 * theta2);
	A1 = 1.0 / 8.0 * gamma2_apo*cn2 * (1.0 - 11.0 * theta2 - 40.0 * theta4 * A1_apo);
	A2_apo = 3.0 * theta2 + 8.0 * theta4 * A1_apo;
	A2 = 5.0 / 12.0 * g4dg2 * cn2 * (1.0 - A2_apo);
	A6 = 1.0 / 4.0 * g3dg2;
	A7 = A6 * cn2 * sinI;
	A10 = 2.0 + eccdp2;
	A11 = 3.0 * eccdp2 + 2.0;
	A12 = A11 * theta2;
	A13 = (5.0 * eccdp2 + 2.0)*theta4 * A1_apo;
	A14 = eccdp2 * theta4*theta2 * pow(A1_apo, 2);
	A15 = theta2 * A1_apo;
	A16 = pow(A15, 2);
	A17 = arr.e * sinI;
	A18 = A17 / (1.0 + cn);
	A20 = arr.e * theta;
	A21 = arr.e * A20;
	A25 = 16.0 * A15 + 40.0 * A16 + 3.0;
	A26 = 1.0 / 8.0 * A21*(11.0 + 200.0 * A16 + 80.0 * A15);

	B1 = cn * (A1 - A2) - (1.0 / 16.0 * (A10 - 400.0 * A14 - 40.0 * A13 - 11.0 * A12) + 1.0 / 8.0 * A21*(11.0 + 200.0 * A16 + 80.0 * A15))*gamma2_apo
		+ 5.0 / 24.0 * (-80.0 * A14 - 8.0 * A13 - 3.0 * A12 + 2.0 * A25*A21 + A10)*g4dg2;
	B2 = A6 * A18*(2.0 + cn - eccdp2) + A20 * tan(arr.i / 2.0)*(A6);
	B4 = cn * arr.e*(A1 - A2);
	B5 = cn * A7;
	B7 = cn2 * A17*A1_apo*(1.0 / 8.0 * gamma2_apo*(1.0 - 15.0 * theta2) - 5.0 / 12.0 * g4dg2 * (1.0 - 7.0 * theta2));
	B8 = cn2 * A6;
	B10 = sinI*(5.0 / 12.0 * g4dg2 * A21*A25 - A26 * gamma2_apo);
	B11 = A21 * A6;
	B13 = arr.e * (A1 - A2);
	B14 = A7;

	a = am * (1.0 + gamma2 * ((3.0 * theta2 - 1.0)*arr.e / (cn2*cn2*cn2)*(arr.e*cn + arr.e / (1.0 + cn) + costa *(3.0 + 3.0 * arr.e*costa
		+ eccdp2 * costa2)) + 3.0 * (1.0 - theta2)*adr3 * cs2gta));

	delta1e = B13 * cs2gd + B14 * sinGD;
	de = delta1e - cn2 / 2.0 * (gamma2_apo*(1.0 - theta2)*(3.0 * csf2gd + cs3fgd)
		- 3.0 * gamma2 * 1.0 / (cn2*cn2*cn2)*(1.0 - theta2)*cs2gta*(3.0 * arr.e*costa2
			+ 3.0 * costa + eccdp2 * costa*costa2 + arr.e)
		- gamma2 * 1.0 / (cn2*cn2*cn2)*(3.0 * theta2 - 1.0)*(arr.e*cn + arr.e / (1.0 + cn) + 3.0 * arr.e*costa2 + 3.0 * costa + eccdp2 * costa * costa2));
	edl = B4 * sin2gd - B5 * cosGD
		- 1.0 / 4.0 * cn*cn2 * gamma2_apo*(2.0 * (3.0 * theta2 - 1.0)*(cn2 * adr2 + adr + 1.0)*sinta
			+ 3.0 * (1.0 - theta2)*((-cn2 * adr2 - adr + 1.0)*snf2gd
				+ (cn2 * adr2 + adr + 1.0 / 3.0)*sn3fgd));
	out.e = sqrt(pow(arr.e + de, 2) + pow(edl, 2));

	di = 1.0 / 2.0 * theta*gamma2_apo*sinI*(arr.e*cs3fgd + 3.0 * (arr.e*csf2gd + cs2gta))
		- A20 / cn2*(B7*cs2gd + B8 * sinGD);
	sin_im2_dh = 1.0 / (2.0 * cosI2)*(B10*sin2gd + B11 * cosGD
		- 1.0 / 2.0 * gamma2_apo*theta*sinI*(6.0 * (arr.e*sinta - arr.l + fm)
			- 3.0 * (sn2gta + arr.e * snf2gd) - arr.e * sn3fgd));
	out.i = 2.0 * asin(sqrt(pow(sin_im2_dh, 2) + pow(1.0 / 2.0 * di*cosI2 + sinI2, 2)));

	if (out.e <= 1.0e-11)
	{
		out.l = 0;
	}
	else
	{
		out.l = atan2(edl*cosMADP + (arr.e + de)*sinMADP, (arr.e + de)*cosMADP - edl * sinMADP);
		if (out.l < 0)
		{
			out.l = out.l + PI2;
		}
	}

	if (out.i == 0.0)
	{
		out.h = 0;
	}
	else
	{
		out.h = atan2(sin_im2_dh*cosraandp + sinraandp *(1.0 / 2.0 * di*cosI2 + sinI2), cosraandp*(1.0 / 2.0 * di*cosI2 + sinI2) - sin_im2_dh * sinraandp);
		if (out.h < 0)
		{
			out.h = out.h + PI2;
		}
	}

	lagaha = arr.l + arr.g + arr.h + B1 * sin2gd + B2 * cosGD;
	lgh = lagaha + (1.0 / 4.0 * (cn2 / (cn + 1.0))*arr.e*gamma2_apo*(3.0 * (1.0 - theta2)*(sn3fgd
		*(1.0 / 3.0 + adr2 * cn2 + adr) + snf2gd *(1.0 - adr2 * cn2 - adr))
		+ 2.0 * sinta*(3.0 * theta2 - 1.0)*(1.0 + adr2 * cn2 + adr)))
		+ 3.0 / 2.0 * gamma2_apo*((5.0 * theta2 - 2.0 * theta - 1.0)*(arr.e*sinta + fm - arr.l)) + (3.0 + 2.0 * theta - 5.0 * theta2)
		*(1.0 / 4.0 * gamma2_apo*(arr.e*sn3fgd + 3.0 * (sn2gta + arr.e * snf2gd)));

	out.g = lgh - out.l - out.h;
	while (out.g >= PI2)
	{
		out.g -= PI2;
	}
	while (out.g < 0)
	{
		out.g += PI2;
	}

	if (pseudostate)
	{
		out.i = PI - out.i;
		out.h = -out.h;

		if (out.h < 0)
		{
			out.h = out.h + PI2;
		}
	}

	out.a = a * R_e;
	return out;

	//Lyddane-Cohen improvement on the SMA
	/*double a1 = a;
	double f = OrbMech::MeanToTrueAnomaly(out.l, out.e);
	double ar = (1.0 + out.e*cos(f)) / (1.0 - out.e*out.e);
	double psi = (-1.0 + 3.0*pow(cos(out.i), 2))*(pow(ar, 3) - pow(sqrt(1.0 - out.e*out.e), 3)) + 3.0*(1.0 - pow(cos(out.i), 2))*pow(ar, 3)*cos(2.0*out.g + 2.0*f);
	double da = 1.0 / pow(a1, 3)*(k2*psi*(a1*a1 - k2 * psi + 3.0 / 2.0*(k2 / (cn*cn2))*(1.0 - 3.0*theta2))
		- k2 * k2 / (16.0*pow(cn, 7))*(15.0*cn2*(1.0 - 18.0 / 5.0*theta2 + theta4)
			+ 12.0*cn*(1.0 - 6.0*theta2 + 9.0*theta4) - 15.0*(1.0 - 2.0*theta2 - 7.0*theta4)
			+ 6.0*eccdp2*(1.0 - 16.0*theta2 + 15.0*theta4)*cos(2.0*arr.g))
		+ 9.0*k2*k2 / (2.0*pow(cn, 7))*(1.0 - 6.0*theta2 + 5.0*theta4)*(cos(2.0*arr.g + 2.0*fm)
			+ arr.e*cos(2.0*arr.g + fm) + 1.0 / 3.0*arr.e*cos(2.0*arr.g + 3.0*fm)));
	
	out.a = (am + da)*R_Earth;
	double a1r = a1 * R_Earth;
	return out;*/
}

void BrouwerSecularRates(CELEMENTS coe_osc, CELEMENTS coe_mean, int body, double &l_dot, double &g_dot, double &h_dot)
{
	double mu, n0, eccdp2, cn, cn2, theta, theta2, theta3, theta4, k2, k4, gm2, gm4, gmp2, gmp4, J2, J3, J4, R_e;
	double esing, ecosg, L, u, f, sin_lat, R, ainv;

	if (body == BODY_EARTH)
	{
		mu = mu_Earth;
		J2 = J2_Earth;
		J3 = J3_Earth;
		J4 = J4_Earth;
		R_e = R_Earth;
	}
	else
	{
		mu = mu_Moon;
		J2 = J2_Moon;
		J3 = J3_Moon;
		J4 = 0;
		R_e = R_Moon;
	}

	n0 = sqrt(mu / pow(coe_mean.a, 3));
	eccdp2 = coe_mean.e * coe_mean.e;
	cn2 = 1.0 - eccdp2;
	cn = sqrt(cn2);
	theta = cos(coe_mean.i);
	theta2 = theta * theta;
	theta3 = theta2 * theta;
	theta4 = theta2 * theta2;
	k2 = J2 * pow(R_e, 2) / 2.0;
	k4 = -3.0 * J4*pow(R_e, 4) / 8.0;
	gm2 = k2 / pow(coe_mean.a, 2);
	gm4 = k4 / pow(coe_mean.a, 4);
	gmp2 = gm2 / pow(cn2, 2);
	gmp4 = gm4 / pow(cn2, 4);

	esing = coe_osc.e*sin(coe_osc.g);
	ecosg = coe_osc.e*cos(coe_osc.g);
	L = coe_osc.l + coe_osc.g;
	u = L + (2.0*ecosg*sin(L) - 2.0*esing*cos(L))*(1.0 + 5.0 / 4.0*ecosg*cos(L) + esing * sin(L));
	f = u - coe_osc.g;
	sin_lat = sin(u)*sin(coe_osc.i);
	R = coe_osc.a*(1.0 - coe_osc.e*coe_osc.e) / (1.0 + coe_osc.e*cos(f));
	ainv = 1.0 / coe_osc.a + J2 * pow(R_e, 2) / pow(R, 3)*(1.0 - 3.0*pow(sin_lat, 2)) + J3 * pow(R_e, 3) / pow(R, 4)*(3.0*sin_lat - 5.0*pow(sin_lat, 3)) -
		J4 * pow(R_e, 4) / (4.0*pow(R, 5))*(3.0 - 30.0*pow(sin_lat, 2) + 35.0*pow(sin_lat, 4));
	l_dot = sqrt(mu*pow(ainv, 3));

	//l_dot = n0 + n0 * cn*(gmp2*(3.0 / 2.0*(3.0*theta2 - 1.0) + 3.0 / 32.0*gmp2*(25.0*cn2 + 16.0*cn - 15.0 + (30.0 - 96.0*cn - 90.0*cn2)*theta2
	//	+ (105.0 + 144.0*cn + 25.0*cn2)*theta4)) + 15.0 / 16.0*gmp4*eccdp2*(3.0 - 30.0*theta2 + 35.0*theta4));
	g_dot = n0 * (gmp2*(3.0 / 2.0*(5.0*theta2 - 1.0) + 3.0 / 32.0*gmp2*(25.0*cn2 + 24.0*cn - 35.0
		+ (90.0 - 192.0*cn - 126.0*cn2)*theta2 + (385.0 + 360.0*cn + 45.0*cn2)*theta4))
		+ 5.0 / 16.0*gmp4*(21.0 - 9.0*cn2 + (126.0*cn2 - 270.0)*theta2 + (385.0 - 189.0*cn2)*theta4));
	h_dot = n0 * (gmp2*(3.0 / 8.0*gmp2*((9.0*cn2 + 12.0*cn - 5.0)*theta - (35.0 + 36.0*cn + 5.0*cn2)*theta3) - 3.0*theta)
		+ 5.0 / 4.0*gmp4*theta*(5.0 - 3.0*cn2)*(3.0 - 7.0*theta2));
}

SV PMMAEGS(int Epoch, SV sv0, int opt, double param, bool &error, double DN)
{
	if (sv0.gravref == oapiGetObjectByName("Earth"))
	{
		return PMMAEG(Epoch, sv0, opt, param, error, DN);
	}
	else
	{
		return PMMLAEG(Epoch, sv0, opt, param, error, DN);
	}
}

SV PMMAEG(int Epoch, SV sv0, int opt, double param, bool &error, double DN)
{
	error = false;

	//Update to the given time
	if (opt == 0)
	{
		double MJD1, dt;

		MJD1 = param;
		dt = (MJD1 - sv0.MJD)*24.0*3600.0;
		return coast(Epoch, sv0, dt);
	}
	else
	{
		SV sv1;
		CELEMENTS osc0, osc1;
		double DX_L, X_L, X_L_dot, dt, ddt, L_D, ll_dot, n0, g_dot, J20;
		int LINE, COUNT;
		bool DH;

		J20 = 1082.6269e-6;

		sv1 = sv0;
		osc0 = OrbMech::GIMIKC(sv0.R, sv0.V, mu_Earth);

		if (osc0.e > 0.85)
		{
			error = true;
			return sv0;
		}

		n0 = sqrt(mu_Earth / (osc0.a*osc0.a*osc0.a));
		ll_dot = n0;
		g_dot = n0 * ((3.0 / 4.0)*(J20*R_Earth*R_Earth*(5.0*cos(osc0.i)*cos(osc0.i) - 1.0)) / (osc0.a*osc0.a*pow(1.0 - osc0.e*osc0.e, 2.0)));

		osc1 = osc0;
		if (opt != 3)
		{
			L_D = param;
		}
		else
		{
			double u = OrbMech::MeanToTrueAnomaly(osc1.l, osc1.e) + osc1.g;
			u = fmod(u, PI2);
			if (u < 0)
				u += PI2;
			L_D = u;
		}
		DX_L = 1.0;
		DH = true;
		dt = 0.0;
		LINE = 0;
		COUNT = 24;

		do
		{
			//Mean anomaly
			if (opt == 1)
			{
				X_L = osc1.l;
				X_L_dot = ll_dot;
			}
			//Argument of latitude
			else if (opt == 2)
			{
				double u = OrbMech::MeanToTrueAnomaly(osc1.l, osc1.e) + osc1.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = ll_dot + g_dot;
			}
			//Maneuver line
			else
			{
				double u = OrbMech::MeanToTrueAnomaly(osc1.l, osc1.e) + osc1.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = ll_dot + g_dot;
				LINE = 2;
			}

			if (DH)
			{
				double DN_apo = DN * PI2;
				ddt = DN_apo / ll_dot;
				DH = false;

				if (LINE != 0)
				{
					L_D = L_D + g_dot * ddt + DN_apo;
					while (L_D < 0) L_D += PI2;
					while (L_D >= PI2) L_D -= PI2;
				}
				else
				{
					ddt += (L_D - X_L) / X_L_dot;
				}
			}
			else
			{
				DX_L = L_D - X_L;
				if (abs(DX_L) - PI >= 0)
				{
					if (DX_L > 0)
					{
						DX_L -= PI2;
					}
					else
					{
						DX_L += PI2;
					}
				}
				ddt = DX_L / X_L_dot;
				if (LINE != 0)
				{
					L_D = L_D + ddt * g_dot;
				}
			}


			dt += ddt;
			sv1 = coast(Epoch, sv1, ddt);
			osc1 = OrbMech::GIMIKC(sv1.R, sv1.V, mu_Earth);

			COUNT--;

		} while (abs(DX_L) > 2e-4 && COUNT > 0);

		if (COUNT == 0)
		{
			error = true;
		}

		return sv1;
	}

	return sv0;
}

SV PMMLAEG(int Epoch, SV sv0, int opt, double param, bool &error, double DN)
{
	error = false;

	//Update to the given time
	if (opt == 0)
	{
		double MJD1, dt;

		MJD1 = param;
		dt = (MJD1 - sv0.MJD)*24.0*3600.0;
		return coast(Epoch, sv0, dt);
	}
	//Update to the given mean anomaly (opt=1), argument of latitude (opt=2) or maneuver counter line (opt=3)
	else
	{
		SV sv1;
		CELEMENTS osc0, osc1;
		double DX_L, X_L, X_L_dot, dt, ddt, L_D, ll_dot, n0, g_dot;
		int LINE, COUNT;
		bool DH;

		sv1 = sv0;
		osc0 = OrbMech::GIMIKC(sv0.R, sv0.V, mu_Moon);

		if (osc0.e > 0.3)
		{
			error = true;
			return sv0;
		}

		n0 = sqrt(mu_Moon / (osc0.a*osc0.a*osc0.a));
		ll_dot = n0;
		g_dot = 0.0; // n0 * ((3.0 / 4.0)*(J20*R_Moon*R_Moon*(5.0*cos(osc0.i)*cos(osc0.i) - 1.0)) / (osc0.a*osc0.a*pow(1.0 - osc0.e*osc0.e, 2.0)));

		osc1 = osc0;
		if (opt != 3)
		{
			L_D = param;
		}
		else
		{
			double u = OrbMech::MeanToTrueAnomaly(osc1.l, osc1.e) + osc1.g;
			u = fmod(u, PI2);
			if (u < 0)
				u += PI2;
			L_D = u;
		}
		DX_L = 1.0;
		DH = true;
		dt = 0.0;
		LINE = 0;
		COUNT = 24;

		do
		{
			//Mean anomaly
			if (opt == 1)
			{
				X_L = osc1.l;
				X_L_dot = ll_dot;
			}
			//Argument of latitude
			else if (opt == 2)
			{
				double u = OrbMech::MeanToTrueAnomaly(osc1.l, osc1.e) + osc1.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = ll_dot + g_dot;
			}
			//Maneuver line
			else
			{
				double u = OrbMech::MeanToTrueAnomaly(osc1.l, osc1.e) + osc1.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = ll_dot + g_dot;
				LINE = 2;
			}

			if (DH)
			{
				double DN_apo = DN * PI2;
				ddt = DN_apo / ll_dot;
				DH = false;

				if (LINE != 0)
				{
					L_D = L_D + g_dot * ddt + DN_apo;
					while (L_D < 0) L_D += PI2;
					while (L_D >= PI2) L_D -= PI2;
				}
				else
				{
					ddt += (L_D - X_L) / X_L_dot;
				}
			}
			else
			{
				DX_L = L_D - X_L;
				if (abs(DX_L) - PI >= 0)
				{
					if (DX_L > 0)
					{
						DX_L -= PI2;
					}
					else
					{
						DX_L += PI2;
					}
				}
				ddt = DX_L / X_L_dot;
				if (LINE != 0)
				{
					L_D = L_D + ddt * g_dot;
				}
			}


			dt += ddt;
			sv1 = coast(Epoch, sv1, ddt);
			osc1 = OrbMech::GIMIKC(sv1.R, sv1.V, mu_Moon);

			COUNT--;

		} while (abs(DX_L) > 2e-4 && COUNT > 0);

		if (COUNT == 0)
		{
			error = true;
		}

		return sv1;
	}

	return sv0;
}

CELEMENTS GIMIKC(VECTOR3 R, VECTOR3 V, double mu)
{
	CELEMENTS elem;
	VECTOR3 HH, NN, EE;
	double r, RV, VV, E;

	r = length(R);
	VV = dotp(V, V);
	RV = dotp(R, V);

	HH = crossp(R, V);
	NN = _V(-HH.y, HH.x, 0);
	EE = (R*(VV - mu / r) - V * RV)*1.0 / mu;

	E = VV / 2.0 - mu / r;
	elem.a = -mu / (2.0*E);
	elem.e = length(EE);
	elem.i = acos2(HH.z / length(HH));
	elem.h = acos2(NN.x / length(NN));
	if (NN.y < 0)
	{
		elem.h = PI2 - elem.h;
	}
	elem.g = acos2(dotp(unit(NN),unit(EE)));
	if (EE.z < 0)
	{
		elem.g = PI2 - elem.g;
	}

	if (elem.e < 1.0)
	{
		double TA = acos2(dotp(unit(EE),unit(R)));
		if (RV < 0)
		{
			TA = PI2 - TA;
		}
		elem.l = TrueToMeanAnomaly(TA, elem.e);
	}
	else
	{
		elem.l = 0.0;
	}

	return elem;
}

void GIMKIC(CELEMENTS elem, double mu, VECTOR3 &R, VECTOR3 &V)
{
	double EA, EA_apo, RX, RY, VX, VY, X_apo, Y_apo;
	int K = 20;
	EA_apo = elem.l;

	do
	{
		EA = EA_apo;
		EA_apo = EA + (elem.l - EA + elem.e*sin(EA)) / (1.0 - elem.e*cos(EA));
		if (EA == EA_apo) break;
		K--;
	} while (K > 0);
	RX = elem.a*(cos(EA) - elem.e);
	RY = elem.a*sqrt(1.0 - elem.e*elem.e)*sin(EA);
	VX = -sin(EA) / ((1.0 - elem.e*cos(EA))*(sqrt(elem.a) / sqrt(mu)));
	VY = sqrt(1.0 - elem.e*elem.e)*cos(EA) / ((1.0 - elem.e*cos(EA))*(sqrt(elem.a) / sqrt(mu)));
	X_apo = RX * cos(elem.g) - RY * sin(elem.g);
	Y_apo = RX * sin(elem.g) + RY * cos(elem.g);
	R = _V(X_apo*cos(elem.h) - Y_apo * cos(elem.i)*sin(elem.h), X_apo*sin(elem.h) + Y_apo * cos(elem.i)*cos(elem.h), Y_apo*sin(elem.i));
	X_apo = VX * cos(elem.g) - VY * sin(elem.g);
	Y_apo = VX * sin(elem.g) + VY * cos(elem.g);
	V = _V(X_apo*cos(elem.h) - Y_apo * cos(elem.i)*sin(elem.h), X_apo*sin(elem.h) + Y_apo * cos(elem.i)*cos(elem.h), Y_apo*sin(elem.i));
}

double MeanToTrueAnomaly(double meanAnom, double eccdp, double error2)
{
	double ta;

	double E = kepler_E(eccdp, meanAnom, error2);
	if (E < 0.0)
		E = E + PI2;
	double c = abs(E - PI);
	if (c >= 1.0e-8)
	{
		ta = 2.0 * atan(sqrt((1.0 + eccdp) / (1.0 - eccdp))*tan(E / 2.0));
	}
	else
	{
		ta = E;
	}

	if (ta < 0)
		ta += PI2;

	return ta;
}

SV PositionMatch(int Epoch, SV sv_A, SV sv_P, double mu)
{
	SV sv_A1, sv_P1;
	VECTOR3 u, R_A1, U_L;
	double phase, n, dt, ddt;
	bool error;

	dt = 0.0;

	u = unit(crossp(sv_P.R, sv_P.V));
	U_L = unit(crossp(u, sv_P.R));
	sv_A1 = PMMAEGS(Epoch, sv_A, 0, sv_P.MJD, error);

	do
	{
		R_A1 = unit(sv_A1.R - u * dotp(sv_A1.R, u))*length(sv_A1.R);
		phase = acos(dotp(unit(R_A1), unit(sv_P.R)));
		if (dotp(U_L, R_A1) > 0)
		{
			phase = -phase;
		}
		n = OrbMech::GetMeanMotion(sv_A1.R, sv_A1.V, mu);
		ddt = phase / n;
		sv_A1 = coast(Epoch, sv_A1, ddt);
		dt += ddt;
	} while (abs(ddt) > 0.01);

	return sv_A1;
}

double THETR(double u1, double u2, double i1, double i2, double h1, double h2)
{
	return u1 - u2 - 2.0*atan(tan((h1 - h2) / 2.0)*(sin(0.5*(i1 + i2 - PI)) / sin(0.5*(i1 - i2 + PI))));
}

double PHSANG(VECTOR3 R, VECTOR3 V, VECTOR3 R_D)
{
	VECTOR3 H1, H2, R_PROJ;
	double theta;

	H1 = crossp(R, V);
	H2 = crossp(R_D, R);
	R_PROJ = PROJCT(R, V, R_D);
	theta = acos2(dotp(unit(R), unit(R_PROJ)));
	return theta * sign(dotp(H1, H2));
}

VECTOR3 PROJCT(VECTOR3 U1, VECTOR3 U2, VECTOR3 X)
{
	VECTOR3 N = unit(crossp(U1, U2));
	VECTOR3 X1 = X - N * dotp(X, N);
	return unit(X1)*length(X);
}

void DROOTS(double A, double B, double C, double D, double E, int N, double *x, int &M, int &I)
{
	double eps, a, b, delta;

	M = 0;
	I = 0;
	x[0] = 0.0;
	x[1] = 0.0;
	x[2] = 0.0;
	x[3] = 0.0;
	eps = -pow(10, -6);
	if (N == 3)
	{
		goto OrbMech_DROOTS_B;
	}
	else if (N == 4)
	{
		goto OrbMech_DROOTS_C;
	}

OrbMech_DROOTS_A:
	if (C != 0.0)
	{
		a = D / C;
		b = E / C;
		delta = a * a - 4.0*b;
		if (delta < 0)
		{
			I = 2;
		}
		else
		{
			x[0] = (-a - sqrt(delta)) / 2.0;
			x[1] = (-a + sqrt(delta)) / 2.0;
			M = 2;
		}
	}
	else if (D != 0.0)
	{
		M = 1;
		x[0] = -E / D;
	}
	return;

OrbMech_DROOTS_B:
	if (B == 0.0)
	{
		goto OrbMech_DROOTS_A;
	}
	
	double p, q, r;

	p = C / B;
	q = D / B;
	r = E / B;
	a = 1.0 / 3.0*(3.0*q - p * p);
	b = 1.0 / 27.0*(2.0*p*p*p - 9.0*p*q + 27.0*r);
	delta = pow(a, 3) / 27.0 + b * b / 4.0;

	if (delta == 0.0)
	{
		M = 3;
		x[0] = 2.0*pow(-b / 2.0, 1.0 / 3.0) - p / 3.0;
		x[1] = pow(b / 2.0, 1.0 / 3.0) - p / 3.0;
		x[2] = x[1];
	}
	else if (delta > 0.0)
	{
		I = 2;
		M = 1;
		x[0] = pow(-b / 2.0 + sqrt(delta), 1.0 / 3.0) + pow(-b / 2.0 - sqrt(delta), 1.0 / 3.0);
	}
	else
	{
		double phi = atan(sqrt(1.0 - b * b / 4.0*27.0 / (-pow(a, 3))) / (-b / 2.0 / sqrt(-pow(a, 3) / 27.0)));
		M = 3;
		x[0] = 2.0*sqrt(-a / 3.0)*cos(phi / 3.0) - p / 3.0;
		x[1] = 2.0*sqrt(-a / 3.0)*cos(phi / 3.0 + PI2 / 3.0) - p / 3.0;
		x[2] = 2.0*sqrt(-a / 3.0)*cos(phi / 3.0 + PI2 * 2.0 / 3.0) - p / 3.0;
	}
	return;

OrbMech_DROOTS_C:
	if (A == 0.0)
	{
		goto OrbMech_DROOTS_B;
	}

	double Ba, Ca, Da, Ea, h, z;

	Ba = B / A;
	Ca = C / A;
	Da = D / A;
	Ea = E / A;
	h = -Ba / 4.0;
	p = 6.0*h*h + 3.0*h*Ba + Ca;
	q = 4.0*pow(h, 3) + 3.0*h*h*Ba + 2.0*h*Ca + Da;
	r = pow(h, 4) + Ba * pow(h, 3) + Ca * pow(h, 2) + Da * h + Ea;

	if (q == 0.0)
	{
		goto OrbMech_DROOTS_G;
	}
	a = (p*p - 4.0*r) - 4.0 / 3.0*p*p;
	b = 1.0 / 27.0*(16 * pow(p, 3) - 18.0*p*(p*p - 4.0*r) - 27.0*q*q);
	delta = pow(a, 3) / 27.0 + b * b / 4.0;

	if (delta == 0.0)
	{
		double z1, z2;
		z1 = 2.0*power(-b/2.0, 1.0 / 3.0);
		z2 = power(b / 2.0, 1.0 / 3.0);
		z = max(z1, z2);
	}
	else if (delta > 0.0)
	{
		z = power(-b / 2.0 + sqrt(delta), 1.0 / 3.0) + power(-b / 2.0 - sqrt(delta), 1.0 / 3.0);
	}
	else
	{
		double z1, z2, z3;
		double phi = atan(sqrt(1.0 - b * b / 4.0*27.0 / (-pow(a, 3))) / (-b / 2.0 / sqrt(-pow(a, 3) / 27.0)));
		M = 3;
		z1 = 2.0*sqrt(-a / 3.0)*cos(phi / 3.0) - p / 3.0;
		z2 = 2.0*sqrt(-a / 3.0)*cos(phi / 3.0 + PI2 / 3.0) - p / 3.0;
		z3 = 2.0*sqrt(-a / 3.0)*cos(phi / 3.0 + PI2 * 2.0 / 3.0) - p / 3.0;
		z = max(z1, max(z2, z3));
	}

	double ra, xi, beta, delta1, delta2;

	ra = z - 2.0*p / 3.0;
	xi = 0.5*(p + ra - q / sqrt(ra));
	beta = 0.5*(p + ra + q / sqrt(ra));
	delta1 = ra - 4.0*xi;
	delta2 = ra - 4.0*beta;
	if (delta1 < eps)
	{
		I = 2;
	}
	else
	{
		x[0] = ((-sqrt(ra) - sqrt(abs(delta1))) / 2.0) + h;
		x[1] = ((-sqrt(ra) + sqrt(abs(delta1))) / 2.0) + h;
		M = 2;
	}
	if (delta2 < eps)
	{
		I = I + 2;
		return;
	}
	x[2] = ((sqrt(ra) - sqrt(abs(delta2))) / 2.0) + h;
	x[3] = ((sqrt(ra) + sqrt(abs(delta2))) / 2.0) + h;
	M = M + 2;

	if (M > 2)
	{

	}
	else
	{
		x[0] = x[2];
		x[1] = x[3];
		x[2] = 0.0;
		x[3] = 0.0;
	}

	return;

OrbMech_DROOTS_G:
	delta = p * p - 4.0*r;
	if (delta < eps)
	{
		I = 4;
		return;
	}

	double y1, y2;

	y1 = (-p + sqrt(abs(delta)) / 2.0);
	y2 = (-p - sqrt(abs(delta)) / 2.0);

	if (y1 < eps)
	{
		I = 2;
	}
	else
	{
		x[0] = sqrt(abs(y1)) + h;
		x[1] = -sqrt(abs(y1)) + h;
		M = 2;
	}

	if (y2 < eps)
	{
		I = I + 2;
		return;
	}

	x[2] = sqrt(abs(y2)) + h;
	x[3] = -sqrt(abs(y2)) + h;

	if (M <= 0)
	{
		x[0] = x[2];
		x[1] = x[3];
		x[2] = 0.0;
		x[3] = 0.0;
	}
	M = M + 2;
	return;
}

}

const double CoastIntegrator::r_SPH = 64373760.0;

CoastIntegrator::CoastIntegrator(int Epoch, VECTOR3 R00, VECTOR3 V00, double mjd0, double deltat, int planet, int outplanet)
{
	this->outplanet = outplanet;

	K = 0.3;
	dt_lim = 4000;

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
	if (planet == BODY_EARTH)
	{
		r_MP = 7178165.0;
		r_dP = 80467200.0;
		mu = OrbMech::mu_Earth;
		mu_Q = OrbMech::mu_Moon;
		R_E = OrbMech::R_Earth;
		rect1 = 0.75*OrbMech::power(2.0, 22.0);
		rect2 = 0.75*OrbMech::power(2.0, 3.0);
		P = BODY_EARTH;
	}
	else
	{
		r_MP = 2538090.0;
		r_dP = 16093440.0;
		mu = OrbMech::mu_Moon;
		mu_Q = OrbMech::mu_Earth;
		R_E = OrbMech::R_Moon;
		rect1 = 0.75*OrbMech::power(2.0, 18.0);
		rect2 = 0.75*OrbMech::power(2.0, -1.0);
		P = BODY_MOON;
	}

	Rot = OrbMech::J2000EclToBRCS(Epoch);
	MATRIX3 Rot_M = OrbMech::GetRotationMatrix(BODY_MOON, mjd0);
	U_Z_M = rhmul(Rot_M, _V(0, 0, 1));

	U_Z_E = _V(0, 0, 1);
	U_Z_M = mul(Rot, U_Z_M);

	cMoon = oapiGetCelbodyInterface(oapiGetObjectByName("Moon"));
	cEarth = oapiGetCelbodyInterface(oapiGetObjectByName("Earth"));

	R_QC = R0;

	B = 1;
	soichange = false;
	SunEphemerisInit = false;
}

CoastIntegrator::~CoastIntegrator()
{
}

double CoastIntegrator::GetTime()
{
	return t;
}

VECTOR3 CoastIntegrator::GetPosition()
{
	return R_CON + delta;
}

VECTOR3 CoastIntegrator::GetVelocity()
{
	return V_CON + nu;
}

double CoastIntegrator::GetMJD()
{
	return mjd0 + t / 24.0 / 3600.0;
}

int CoastIntegrator::GetGravRef()
{
	return P;
}

bool CoastIntegrator::iteration(bool allow_stop)
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
	dt_max = 0.3*min(dt_lim, min(K*OrbMech::power(rr, 1.5) / sqrt(mu), (M == 0 ? 10e10 : K * OrbMech::power(r_qc, 1.5) / sqrt(mu_Q))));
	Y = OrbMech::sign(t_F - t);
	dt = Y*min(abs(t_F - t), dt_max);

	if (M == 1)
	{
		if (P == BODY_MOON)
		{
			if (rr > r_SPH)
			{
				double MJD, MoonPos[12];
				VECTOR3 R_EM, V_PQ;

				MJD = mjd0 + t / 86400.0;
				cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);

				if (B == 1)
				{
					R_EM = mul(Rot, _V(MoonPos[0], MoonPos[2], MoonPos[1]));
					R_PQ = -R_EM;
				}
				V_PQ = mul(Rot, -_V(MoonPos[3], MoonPos[5], MoonPos[4]));
				R_CON = R_CON - R_PQ;
				V_CON = V_CON - V_PQ;

				R_E = OrbMech::R_Earth;
				mu = OrbMech::mu_Earth;

				r_MP = 7178165.0;
				r_dP = 80467200.0;
				mu_Q = OrbMech::mu_Moon;
				rect1 = 0.75*OrbMech::power(2.0, 22.0);
				rect2 = 0.75*OrbMech::power(2.0, 3.0);
				P = BODY_EARTH;
				R0 = R_CON + delta;
				V0 = V_CON + nu;
				R_CON = R0;
				V_CON = V0;
				delta = _V(0, 0, 0);
				nu = _V(0, 0, 0);// [0 0 0]';
				x = 0;
				tau = 0;
				soichange = true;
			}
		}
		else
		{
			double MJD, MoonPos[12];
			VECTOR3 R_EM, V_PQ;

			MJD = mjd0 + t / 86400.0;
			cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);

			if (B == 1)
			{
				R_EM = mul(Rot, _V(MoonPos[0], MoonPos[2], MoonPos[1]));
				R_PQ = R_EM;
				R_QC = R - R_PQ;
			}
			if (length(R_QC) < r_SPH)
			{
				V_PQ = mul(Rot, _V(MoonPos[3], MoonPos[5], MoonPos[4]));
				R_CON = R_CON - R_PQ;
				V_CON = V_CON - V_PQ;

				R_E = OrbMech::R_Moon;
				mu = OrbMech::mu_Moon;

				r_MP = 2538090.0;
				r_dP = 16093440.0;
				mu_Q = OrbMech::mu_Earth;
				rect1 = 0.75*OrbMech::power(2.0, 18.0);
				rect2 = 0.75*OrbMech::power(2.0, -1.0);
				P = BODY_MOON;
				R0 = R_CON + delta;
				V0 = V_CON + nu;
				R_CON = R0;
				V_CON = V0;
				delta = _V(0, 0, 0);
				nu = _V(0, 0, 0);
				x = 0;
				tau = 0;
				soichange = true;
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
		nu = _V(0, 0, 0);
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
			s = sqrt(mu) / length(R_apo)*0.5*dt;
			gamma = dotp(R_apo, V_apo) / (length(R_apo)*sqrt(mu)*2.0);
			alpha_N = 2.0 / length(R0) - OrbMech::power(length(V0), 2.0) / mu;
			x_t = x_apo + s*(1.0 - gamma*s*(1.0 - 2.0 * gamma*s) - 1.0 / 6.0 * (1.0 / length(R_apo) - alpha_N)*s*s);
			OrbMech::rv_from_r0v0(R0, V0, tau, R_CON, V_CON, mu, x_t);
		}
	}
	delta = delta + (nu + (k[0] + k[1] * 2.0)*dt*1.0 / 6.0)*dt;
	nu = nu + (k[0] + k[1] * 4.0 + k[2]) * 1.0 / 6.0 *dt;

	if (allow_stop && abs(t - t_F) < 1e-6)
	{
		R2 = R_CON + delta;
		V2 = V_CON + nu;

		if (outplanet == -1)
		{
			outplanet = P;
		}
		else if (P != outplanet)
		{
			double MJD, MoonPos[12];
			VECTOR3 R_EM, V_PQ, V_EM;

			MJD = mjd0 + t / 86400.0;
			cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);

			R_EM = mul(Rot, _V(MoonPos[0], MoonPos[2], MoonPos[1]));
			V_EM = mul(Rot, _V(MoonPos[3], MoonPos[5], MoonPos[4]));

			if (P == BODY_EARTH)
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
	if (SunEphemerisInit == false)
	{
		double EarthPos[12];
		VECTOR3 EarthVec, EarthVecVel;

		cEarth->clbkEphemeris(mjd0 + t_F / 2.0 / 24.0 / 3600.0, EPHEM_TRUEPOS | EPHEM_TRUEVEL, EarthPos);

		EarthVec = OrbMech::Polar2Cartesian(EarthPos[2] * AU, EarthPos[1], EarthPos[0]);
		EarthVecVel = OrbMech::Polar2CartesianVel(EarthPos[2] * AU, EarthPos[1], EarthPos[0], EarthPos[5] * AU, EarthPos[4], EarthPos[3]);
		R_ES0 = -mul(Rot, EarthVec);
		V_ES0 = -mul(Rot, EarthVecVel);
		W_ES = length(crossp(R_ES0, V_ES0) / OrbMech::power(length(R_ES0), 2.0));
		SunEphemerisInit = true;
	}

	R_ES = R_ES0*cos(W_ES*t)+crossp(R_ES0,unit(crossp(V_ES0, R_ES0)))*sin(W_ES*t);
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
	if (r > R_E && r < r_dP)
	{
		U_R = unit(R);
		if (P == BODY_EARTH)
		{
			U_Z = U_Z_E;
		}
		else
		{
			U_Z = U_Z_M;
		}
		costheta = dotp(U_R, U_Z);
		P2 = 3.0 * costheta;
		P3 = 0.5*(15.0*costheta*costheta - 3.0);

		if (P == BODY_EARTH)
		{
			a_dP += (U_R*P3 - U_Z * P2)*OrbMech::J2_Earth * OrbMech::power(R_E / r, 2.0);
			P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
			a_dP += (U_R*P4 - U_Z * P3)*OrbMech::J3_Earth * OrbMech::power(R_E / r, 3.0);
			P5 = 0.25*(9.0*costheta*P4 - 5.0 * P3);
			a_dP += (U_R*P5 - U_Z * P4)*OrbMech::J4_Earth * OrbMech::power(R_E / r, 4.0);
		}
		else
		{
			a_dP += (U_R*P3 - U_Z * P2)*OrbMech::J2_Moon * OrbMech::power(R_E / r, 2.0);
			P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
			a_dP += (U_R*P4 - U_Z * P3)*OrbMech::J3_Moon * OrbMech::power(R_E / r, 3.0);
		}
		
		a_dP *= mu / OrbMech::power(r, 2.0);
	}
	if (M == 1)
	{
		double q_Q, q_S, MJD;
		VECTOR3 R_SC, R_PS, R_EM, R_ES, V_ES;
		double MoonPos[12];

		MJD = mjd0 + t / 86400.0;

		cMoon->clbkEphemeris(MJD, EPHEM_TRUEPOS, MoonPos);
		SolarEphemeris(t - t_F/2.0, R_ES, V_ES);
		R_EM = mul(Rot, _V(MoonPos[0], MoonPos[2], MoonPos[1]));

		if (P == BODY_EARTH)
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
		a_dS = -(R_PS*fq(q_S) + R)*OrbMech::mu_Sun / OrbMech::power(length(R_SC), 3.0);
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

MATRIX3 MatrixRH_LH(MATRIX3 A)
{
	return _M(A.m11, A.m13, A.m12, A.m31, A.m33, A.m32, A.m21, A.m23, A.m22);
}

double acos2(double _X)
{
	return acos(min(1.0, max(-1.0, _X)));
}

double asin2(double _X)
{
	return asin(min(1.0, max(-1.0, _X)));
}

double factorial(unsigned n)
{
	double fact = 1.0;
	for (unsigned i = 1;i <= n;i++)
	{
		fact = fact * i;
	}
	return fact;
}