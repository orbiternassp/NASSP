#include "EntryCalculations.h"

Entry::Entry(VESSEL *v, OBJHANDLE gravref, double GETbase, double EntryTIG, double EntryAng, double EntryLng, int critical, double entryrange, bool entrynominal)
{
	MA1 = 8e8;
	C0 = 1.81000432e8;
	C1 = 1.5078514;
	C2 = -6.49993054e-9;
	C3 = 9.769389245e-18;
	D2 = -4.8760771e-4;
	D3 = 4.5419476e-8;
	D4 = -1.4317675e-12;
	k1 = 7.0e6;
	k2 = 6.495e6;
	k3 = -0.06105;
	k4 = -0.10453;

	this->vessel = v;
	this->GETbase = GETbase;
	this->gravref = gravref;
	this->EntryAng = EntryAng;
	this->EntryLng = EntryLng;

	this->critical = critical;

	VECTOR3 R0, V0;
	double EntryInterface;

	vessel->GetRelativePos(gravref, R0);
	vessel->GetRelativeVel(gravref, V0);
	mjd = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R0.x, R0.z, R0.y));
	V0B = mul(Rot, _V(V0.x, V0.z, V0.y));

	get = (mjd - GETbase)*24.0*3600.0;

	EntryInterface = 400000.0 * 0.3048;

	hEarth = oapiGetObjectByName("Earth");

	RCON = oapiGetSize(oapiGetObjectByName("Earth")) + EntryInterface;
	RD = RCON;
	mu = GGRAV*oapiGetMass(hEarth);

	EntryTIGcor = EntryTIG;

	Tguess = PI2 / sqrt(mu)*OrbMech::power(length(R0), 1.5);

	tigslip = 100.0;
	ii = 0;
	entryphase = 0;

	bool stop;

	dt0 = EntryTIGcor - get;

	coast = new CoastIntegrator(R0B, V0B, mjd, dt0, gravref, hEarth);
	stop = false;
	while (stop == false)
	{
		stop = coast->iteration();
	}
	R11B = coast->R2;
	V11B = coast->V2;
	delete coast;

	x2 = OrbMech::cot(PI05 - EntryAng);
	if (length(R11B) > k1)
	{
		D1 = 1.6595;
	}
	else
	{
		D1 = 1.69107;
	}

	if (critical == 0)
	{
		EMSAlt = 284643.0*0.3048;
	}
	else
	{
		EMSAlt = 297431.0*0.3048;
	}
	revcor = -5;

	this->entryrange = entryrange;

	if (entryrange == 0)
	{
		rangeiter = 1;
	}
	else
	{
		rangeiter = 2;
	}

	R_E = oapiGetSize(gravref);
	earthorbitangle = tan((-31.7 - 2.15)*RAD);

	this->entrynominal = entrynominal;
}

Entry::Entry(OBJHANDLE gravref)
{
	this->critical = critical;
	this->gravref = gravref;

	double EntryInterface;
	EntryInterface = 400000.0 * 0.3048;

	RCON = oapiGetSize(gravref) + EntryInterface;
	Rot = OrbMech::J2000EclToBRCS(40222.525);
}

void Entry::xdviterator2()
{
	double xdes, xact, x_err,x_apo, x_err_apo, epsilon;
	int i;
	i = 0;
	dx = 1;
	epsilon = pow(2.0, -20.0);

	xdes = earthorbitangle - tan(acos(R_E / length(R1B)));

	while (abs(dx) > epsilon && i < 135)
	{
		p_CON = theta2 / (theta1 - x*x);
		V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
		DV = V2 - V1B;
		dv = length(DV);

		Entry_DV = _V(dotp(DV, U_H), 0.0, -dotp(DV, U_R1));

		xact = Entry_DV.z / Entry_DV.x;
		x_err = xdes - xact;


		if (i == 0)
		{
			dx = -0.01*(xdes - xact);
		}
		else
		{
			dx = x_err*(x_apo - x) / (x_err - x_err_apo);
		}
		x_err_apo = x_err;
		x_apo = x;
		x += dx;
		i++;
	}


}

void Entry::xdviterator()
{
	int i;
	double epsilon;
	i = 0;

	p_CON = theta2 / (theta1 - x*x);
	V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
	DV = V2 - V1B;
	dv = length(DV);

	beta8 = xmax - xmin;
	if (beta8 > dxmax)
	{
		dx = 0.5*beta8*OrbMech::sign(dx);
	}

	epsilon = pow(2.0, -20.0);

	while (abs(dx) > epsilon && i < 135)
	{
		dvapo = dv;
		xapo = x;
		x += dx;
		dvcalc();
		if (dv > dvapo)
		{
			dx = -dx*0.5;
		}
		beta9 = x + 1.1*dx;
		if (beta9 > xmax)
		{
			dx = (xmax - x) / 2.0;
		}
		else if (beta9 < xmin)
		{
			dx = (xmin - x) / 2.0;
		}
		i++;
	}
}

void Entry::dvcalc()
{
	p_CON = theta2 / (theta1 - x*x);
	V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
	DV = V2 - V1B;
	dv = length(DV);
}

void Entry::limitxchange()
{
	double beta7;
	beta7 = abs(x - xapo);
	if (beta7>2.0*dxmax)
	{
		x = xapo;
		if (x > xmax)
		{
			x = xmax;
		}
		else
		{
			if (x < xmin)
			{
				x = xmin;
			}
		}
		p_CON = theta2 / (theta1 - x*x);
		V2 = (U_R1*x + U_H)*theta3*sqrt(p_CON);
		DV = V2 - V1B;
		dv = length(DV);
	}
}

void Entry::reentryconstraints()
{
	if (n1 == 0)
	{
		if (EntryAng == 0)
		{
			if (length(R11B) > k1)
			{
				x2 = k4;
			}
			else
			{
				x2 = k3;
			}
		}
		n1 = 1;
	}
	else
	{
		if (EntryAng == 0)
		{
			double v2;
			v2 = length(VEI);
			x2_apo = D1 + D2*v2 + D3*v2*v2 + D4*v2*v2*v2;
		}
		else
		{
			x2_apo = x2;
		}
	}
}

bool Entry::EntryIter()
{
	double dt1, r1b, MA2, lambda, beta1, beta5, beta10;
	double dt2, R_ERR, dt21apo, beta12, x2PRE, alpha_N, c3, sing, cosg, p_N, beta2, beta3, beta4, RF, phi4;
	double dt21, beta13, beta14, dRCON, S, RPRE_apo, t32, v3, S_FPA, gammaE, phie, te, t_LS, Sphie, Cphie, tLSMJD, l, m, n, phi, lambda2;
	double dlng, beta6, xlim, C_FPA;
	bool stop;
	VECTOR3 U_V1, eta, RPRE, VPRE, N, REI, R3, V3, UR3, U_H3, U_LS, LSEF;
	MATRIX3 R;
	double x2_err,x2_err_apo,Dx2;

	x2_err = 1.0;

	RCON = RD;
	dt1 = EntryTIGcor - get-dt0;

	if (dt1 != 0)
	{
		coast = new CoastIntegrator(R11B, V11B, mjd + dt0/24.0/3600.0, dt1, hEarth, hEarth);
		stop = false;
		while (stop == false)
		{
			stop = coast->iteration();
		}
		R1B = coast->R2;
		V1B = coast->V2;
		delete coast;
	}
	else
	{
		R1B = R11B;
		V1B = V11B;
	}

	

	//x[0] = 0.0;
	//x[1] = 0.0;
	//dv[0] = 100000.0;
	//V2 = entrydviter(R1B, V1B, RCON, x[1], x2, mu);
	//dv[1] = length(V2);

	//h = 0.00001;

	U_R1 = unit(R1B);
	U_V1 = unit(V1B);
	C_FPA = dotp(U_R1, U_V1);
	if (abs(C_FPA) < 0.99966)
	{
		eta = crossp(R1B, V1B);
	}
	else
	{
		eta = _V(0.0, 0.0, 1.0);
	}
	if (eta.z < 0)
	{
		eta = -eta;
	}
	U_H = unit(crossp(eta, R1B));
	r1b = length(R1B);
	MA2 = C0 + C1*r1b + C2*r1b*r1b + C3*r1b*r1b*r1b;
	n1 = 0;
	reentryconstraints();
	f2 = 0;
	while (abs(x2_err)>0.0001)
	{
		lambda = r1b / RCON;
		beta1 = 1.0 + x2*x2;
		beta5 = lambda*beta1;
		theta1 = beta5*lambda - 1.0;
		theta2 = 2.0*r1b*(lambda - 1.0);
		theta3 = sqrt(mu) / r1b;
		beta10 = beta5*(MA1 - r1b) / (MA1 - RCON) - 1.0;
		if (beta10 < 0.0)
		{
			xmin = 0.0;
		}
		else
		{
			xmin = -sqrt(beta10);
		}
		dxmax = -xmin / 16.0;
		beta10 = beta5*(MA2 - r1b) / (MA2 - RCON) - 1.0;
		if (beta10 < 0.0)
		{
			xmax = 0.0;
		}
		else
		{
			xmax = sqrt(beta10);
		}
		if (critical == 0)
		{
			if (C_FPA >= 0)
			{
				x = xmax;
				dx = -dxmax;
			}
			else
			{
				x = xmin;
				dx = dxmax;
			}
		}
		else if (ii == 0)
		{
			x = xmin;
			dx = dxmax;
		}
		if (ii == 0)
		{
			if (critical == 0 || critical == 1)
			{
				if (entrynominal)
				{
					xdviterator();
					xdviterator2();
				}
				else
				{
					xdviterator();
				}
			}
			else
			{
				dvcalc();
			}
		}
		else
		{
			if (critical == 0)
			{
				if (entrynominal)
				{
					xdviterator();
					xdviterator2();
				}
				else
				{
					xdviterator();
				}
			}
			else
			{
				dvcalc();
			}
		}

		Entry_DV = _V(dotp(DV, U_H), 0.0, -dotp(DV, U_R1));

		dt2 = OrbMech::time_radius(R1B, V2, RCON, -1, mu);
		OrbMech::rv_from_r0v0(R1B, V2, dt2, REI, VEI, mu);
		double rtest;
		rtest = length(REI);
		reentryconstraints();
		x2_err = x2_apo - x2;

		if (n1 == 1 || critical == 0)
		{
			Dx2 = x2_err;
		}
		else
		{
			Dx2 = x2_err*(x2_apo - x2) / (x2_err - x2_err_apo);
		}
		x2_err_apo = x2_err;
		x2_apo = x2;
		x2 += Dx2;
		n1++;
	}

	if (entryphase == 0)
	{
		OrbMech::rv_from_r0v0(R1B, V2, dt2, REI, VEI, mu);
		t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu);
		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		augekugel(v3, gammaE, phie, te);

		t2 = EntryTIGcor + dt2;
		t_LS = t2 + t32 + te;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		tLSMJD = GETbase + t_LS / 24.0 / 3600.0;
		U_LS = tmul(Rot, U_LS);
		U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
		R = OrbMech::GetRotationMatrix2(hEarth, tLSMJD);
		LSEF = tmul(R, U_LS);
		l = LSEF.x;
		m = LSEF.z;
		n = LSEF.y;
		phi = asin(n);
		if (m > 0)
		{
			lambda2 = acos(l / cos(phi));
		}
		else
		{
			lambda2 = PI2 - acos(l / cos(phi));
		}
		if (lambda2 > PI){ lambda2 -= PI2; }

		dlng = EntryLng - lambda2;
		if (abs(dlng) > PI)
		{
			dlng = dlng - OrbMech::sign(dlng)*PI2;
		}
		EntryLngcor = lambda2;
		EntryLatcor = phi;

		if (critical == 0)
		{
			tigslip = Tguess*dlng / PI2;
			EntryTIGcor += tigslip;
		}
		else
		{
			if (ii == 0)
			{
				dx = -dlng * RAD;
				xapo = x;
				dlngapo = lambda2;
				x += dx;

			}
			else
			{
				dx = (x - xapo) / (lambda2 - dlngapo)*dlng;
				if (dv > 2804.0)
				{
					dx = 0.5*max(1.0,revcor);
					revcor++;
				}
				else if (abs(dx) > dxmax)
				{
					dx = OrbMech::sign(dx)*dxmax;
				}
				xapo = x;
				dlngapo = lambda2;
				x += dx;

			}
		}
	}
	else
	{
		

		//phi2 = 1.0;
		R_ERR = 1000000.0;

		n1 = 0;
		//RD = RCON;

		while (abs(R_ERR) > 100.0 || n1 == 0)
		{
			dt21apo = 100000000.0;

			coast = new CoastIntegrator(R1B, V2, mjd + (dt0 + dt1) / 24.0 / 3600.0, dt2, hEarth, hEarth);
			stop = false;
			while (stop == false)
			{
				stop = coast->iteration();
			}
			RPRE = coast->R2;
			VPRE = coast->V2;
			delete coast;

			beta12 = 100.0;
			x2PRE = 1000000;
			while (abs(beta12) > 0.000007 && abs(x2 - x2PRE) > 0.00001)
			{
				//gamma2 = asin((dotp(unit(RPRE), unit(VPRE))));
				// cot(PI05 - gamma2);
				c3 = length(RPRE)*pow(length(VPRE), 2.0) / mu;
				alpha_N = 2.0 - c3;
				N = crossp(unit(RPRE), unit(VPRE));
				sing = length(N);
				cosg = dotp(unit(RPRE), unit(VPRE));
				x2PRE = cosg / sing;
				p_N = c3*sing*sing;
				beta2 = p_N*beta1;
				/*beta2 = 1.0 - alpha_N*p_N*beta1;// p_N*beta1;
				if (beta2 > 0)
				{
				beta3 = sqrt(beta2);
				}
				else
				{
				beta3 = 0;
				}
				//beta4 = RD / length(RPRE);// (1 + beta3*phi2) / alpha_N;*/
				beta3 = 1.0 - alpha_N*beta2;
				if (beta3 < 0)
				{
					beta4 = 1.0 / alpha_N;
				}
				else
				{
					beta4 = beta2 / (1.0 - phi2*sqrt(beta3));
				}
				beta12 = beta4 - 1.0;
				if (abs(beta12) > 0.000007)
				{
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
					dt21 = OrbMech::time_radius(RPRE, VPRE*phi4, RF, -phi4, mu);
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
					coast = new CoastIntegrator(RPRE, VPRE, mjd + (dt0 + dt1 + dt2) / 24.0 / 3600.0, dt21, hEarth, hEarth);
					dt2 += dt21;
					stop = false;
					while (stop == false)
					{
						stop = coast->iteration();
					}
					RPRE = coast->R2;
					VPRE = coast->V2;
					delete coast;
				}
			}

			R_ERR = length(RPRE) - RD;
			if (n1 == 0)
			{
				RCON = RD*RD / length(RPRE);
				dRCON = RCON - RD;
			}
			else
			{
				S = dRCON / (RPRE_apo - length(RPRE));
				if (abs(S + 2.0) > 2.0)
				{
					S = -4.0;
				}
				dRCON = S*R_ERR;
				RCON = RCON + dRCON;
			}
			RPRE_apo = length(RPRE);

			n1++;
			f2 = 0;
			lambda = r1b / RCON;
			beta1 = 1.0 + x2*x2;
			beta5 = lambda*beta1;
			theta1 = beta5*lambda - 1.0;
			theta2 = 2.0*length(R1B)*(lambda - 1.0);
			theta3 = sqrt(mu) / length(R1B);
			beta10 = beta5*(MA1 - r1b) / (MA1 - RCON) - 1.0;
			if (beta10 < 0.0)
			{
				xmin = 0.0;
			}
			else
			{
				xmin = -sqrt(beta10);
			}
			dxmax = -xmin / 16.0;
			beta10 = beta5*(MA2 - r1b) / (MA2 - RCON) - 1.0;
			if (beta10 < 0.0)
			{
				xmax = 0.0;
			}
			else
			{
				xmax = sqrt(beta10);
			}
			//x_apo = x;
			beta6 = beta5*(2.0 - lambda) - 1.0;
			if (critical == 0)
			{
				if (beta6 > 0)
				{
					xlim = sqrt(beta6);
					if (phi2 == 1)
					{
						xmax = xlim;
						xmin = -xlim;
						if (x > 0)
						{
							x = xmax;
							dx = -dxmax;
						}
						else
						{
							x = xmin;
							dx = dxmax;
						}
					}
					else
					{
						if (x > 0)
						{
							xmin = xlim;
							x = xmax;
							dx = -dxmax;
						}
						else
						{
							xmax = -xlim;
							x = xmin;
							dx = dxmax;
						}
					}
				}
				else
				{
					if (phi2 == 1)
					{
						phi2 = -1.0;
						n1 = 0;
					}
					if (x > 0)
					{
						x = xmax;
						dx = -dxmax;
					}
					else
					{
						x = xmin;
						dx = dxmax;
					}
				}

				if (entrynominal)
				{
					xdviterator();
					xdviterator2();
				}
				else
				{
					xdviterator();
				}
			}
			else
			{
				dvcalc();
			}
		}
		Entry_DV = _V(dotp(DV, U_H), 0.0, -dotp(DV, U_R1));

		//dt2 = OrbMech::time_radius(R1B, V2, RCON, -1, mu);

		//rv_from_r0v0(R1B, V2, dt2, REI, VEI, mu);
		t32 = OrbMech::time_radius(RPRE, VPRE, length(RPRE) - 30480.0, -1, mu);
		OrbMech::rv_from_r0v0(RPRE, VPRE, t32, R3, V3, mu);
		UR3 = unit(R3);
		v3 = length(V3);
		S_FPA = dotp(UR3, V3) / v3;
		gammaE = asin(S_FPA);
		augekugel(v3, gammaE, phie, te);

		t2 = EntryTIGcor + dt2;
		t_LS = t2 + t32 + te;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		tLSMJD = GETbase + t_LS / 24.0 / 3600.0;
		U_LS = tmul(Rot, U_LS);
		U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
		R = OrbMech::GetRotationMatrix2(hEarth, tLSMJD);
		LSEF = tmul(R, U_LS);
		l = LSEF.x;
		m = LSEF.z;
		n = LSEF.y;
		phi = asin(n);
		if (m > 0)
		{
			lambda2 = acos(l / cos(phi));
		}
		else
		{
			lambda2 = PI2 - acos(l / cos(phi));
		}
		if (lambda2 > PI){ lambda2 -= PI2; }

		dlng = EntryLng - lambda2;
		if (abs(dlng) > PI)
		{
			dlng = dlng-OrbMech::sign(dlng)*PI2;
		}
		EntryLngcor = lambda2;
		EntryLatcor = phi;

		if (critical == 0)
		{
			tigslip = Tguess*dlng / PI2;
			EntryTIGcor += tigslip;
		}
		else
		{
			if (ii == 0)
			{
				dx = -dlng * RAD;
				xapo = x;
				dlngapo = lambda2;
				x += dx;

			}
			else
			{
				dx = (x - xapo) / (lambda2 - dlngapo)*dlng;
				if (dv > 2804.0)
				{
					dx = 0.5*max(1.0, revcor);
					revcor++;
				}
				else if (abs(dx) > dxmax)
				{
					dx = OrbMech::sign(dx)*dxmax;
				}
				xapo = x;
				dlngapo = lambda2;
				x += dx;
				
			}
		}
	}
	ii++;

	if ((abs(dlng) > 0.005*RAD && ii < 100) || entryphase == 0)
	{
		if (abs(tigslip) < 10.0 || (critical > 0 && abs(dlng)<0.1*RAD))
		{
			entryphase = 1;

			if (RCON - p_CON*beta1 >= 0)
			{
				phi2 = 1.0;
			}
			else
			{
				phi2 = -1.0;
			}
		}
		return false;
	}
	else
	{
		VECTOR3 R05G, V05G;
		double dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu);

		EntryRTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		EntryVIO = length(V05G);
		EntryRET = dt2 + t32 + dt22;
		EntryAng = atan(x2);
		return true;
	}
}

void Entry::EntryUpdateCalc()
{
	VECTOR3 R0, V0, R0B, V0B, REI, VEI, R3, V3, UR3, U_H3, U_LS, LSEF;
	double mjd, RCON, mu, dt2, t32, v3, S_FPA, gammaE, phie, te, t2, t_LS, Sphie, Cphie, tLSMJD, l, m, n, phi, lambda2, EntryInterface;
	MATRIX3 Rot, R;
	OBJHANDLE hEarth;
	VECTOR3 R05G, V05G;
	double dt22;

	hEarth = oapiGetObjectByName("Earth");

	EntryInterface = 400000 * 0.3048;
	RCON = oapiGetSize(hEarth) + EntryInterface;
	mu = GGRAV*oapiGetMass(gravref);

	vessel->GetRelativePos(gravref, R0);
	vessel->GetRelativeVel(gravref, V0);
	mjd = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R0.x, R0.z, R0.y));
	V0B = mul(Rot, _V(V0.x, V0.z, V0.y));

	dt2 = OrbMech::time_radius_integ(R0B, V0B, mjd, RCON, -1, gravref, hEarth, REI, VEI);

	//rv_from_r0v0(R0B, V0B, dt2, REI, VEI, mu);

	t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, mu);
	OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu);
	UR3 = unit(R3);
	v3 = length(V3);
	S_FPA = dotp(UR3, V3) / v3;
	gammaE = asin(S_FPA);
	augekugel(v3, gammaE, phie, te);

	for (int iii = 0;iii < rangeiter;iii++)
	{
		t2 = dt2;
		t_LS = t2 + t32 + te;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		tLSMJD = mjd + t_LS / 24.0 / 3600.0;
		U_LS = tmul(Rot, U_LS);
		U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
		R = OrbMech::GetRotationMatrix2(gravref, tLSMJD);
		LSEF = tmul(R, U_LS);
		l = LSEF.x;
		m = LSEF.z;
		n = LSEF.y;
		phi = asin(n);
		if (m > 0)
		{
			lambda2 = acos(l / cos(phi));
		}
		else
		{
			lambda2 = PI2 - acos(l / cos(phi));
		}
		if (lambda2 > PI) { lambda2 -= PI2; }

		EntryLatPred = phi;
		EntryLngPred = lambda2;

		
		dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu);

		EntryRTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		if (entryrange != 0)
		{
			te *= entryrange / EntryRTGO;
			phie = entryrange + 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		}
	}
	EntryVIO = length(V05G);
	EntryRET = dt2 + t32 + dt22;
}

void Entry::Reentry(VECTOR3 REI, VECTOR3 VEI, double mjd0)
{
	double t32,v3,S_FPA, gammaE,phie,te,t_LS,Sphie,Cphie, tLSMJD,l,m,n,phi,lambda2;
	VECTOR3 R3, V3, UR3, U_H3, U_LS, LSEF;
	MATRIX3 R;

	t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, mu);
	OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu);
	UR3 = unit(R3);
	v3 = length(V3);
	S_FPA = dotp(UR3, V3) / v3;
	gammaE = asin(S_FPA);
	augekugel(v3, gammaE, phie, te);

	t_LS = t32 + te;
	Sphie = sin(0.00029088821*phie);
	Cphie = cos(0.00029088821*phie);
	U_H3 = unit(crossp(crossp(R3, V3), R3));
	U_LS = UR3*Cphie + U_H3*Sphie;

	tLSMJD = mjd0 + t_LS / 24.0 / 3600.0;
	U_LS = tmul(Rot, U_LS);
	U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
	R = OrbMech::GetRotationMatrix2(gravref, tLSMJD);
	LSEF = tmul(R, U_LS);
	l = LSEF.x;
	m = LSEF.z;
	n = LSEF.y;
	phi = asin(n);
	if (m > 0)
	{
		lambda2 = acos(l / cos(phi));
	}
	else
	{
		lambda2 = PI2 - acos(l / cos(phi));
	}
	if (lambda2 > PI){ lambda2 -= PI2; }

	EntryLatPred = phi;
	EntryLngPred = lambda2;

	VECTOR3 R05G, V05G;
	double dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
	OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu);

	EntryRTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
	EntryVIO = length(V05G);
	EntryRET = t32 + dt22;
}

void Entry::augekugel(double ve, double gammae, double &phie, double &Te)
{
	double vefps, gammaedeg, K1, K2;

	vefps = ve / 0.3048;
	gammaedeg = gammae * DEG;
	if (vefps <= 21000)
	{
		K1 = 5500.0;
	}
	else
	{
		if (vefps <= 28000.0)
		{
			K1 = 2400.0 + 0.443*(28000.0 - vefps);
		}
		else
		{
			K1 = 2400.0;
		}
	}
	if (vefps <= 24000.0)
	{
		K2 = -3.2 - 0.001222*(24000.0 - vefps);
	}
	else
	{
		if (vefps <= 28400.0)
		{
			K2 = 1.0 - 0.00105*(28000.0 - vefps);
		}
		else
		{
			K2 = 2.4 + 0.000285*(vefps - 32000.0);
		}
	}
	phie = K1 / (abs(gammaedeg) - K2);
	if (vefps < 26000.0)
	{
		Te = 8660.0 * phie / vefps;
	}
	else
	{
		Te = phie / 3.0;
	}
}