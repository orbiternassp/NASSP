/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Translunar Injection Processor

Project Apollo is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Project Apollo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Project Apollo; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

See http://nassp.sourceforge.net/license/ for more details.

**************************************************************************/

#include "GeneralizedIterator.h"
#include "rtcc.h"
#include "TLIProcessor.h"

TLIFirstGuess::TLIFirstGuess(RTCC *r) : RTCCModule(r),
WE(0.262517142)
{

}

void TLIFirstGuess::Calculate(const TLIFirstGuessInputs& in)
{
	inp = in;

	CIST(in.BHA + inp.Hour * WE);
	if (CO1) return;

	double TOIDIS = -outp.T1;
	UPDATE(TOIDIS);
	outp.DEL = DEL;
}

void TLIFirstGuess::CIST(double RAGBT)
{
	double ORBPER, B1, RNVBT, TOPCY, C4, W, RQDFT, A2M, PT1, CTIO, T1;
	double AZ2M, B2M, C2, RA2, A2V, A3V, AZ2V, BETA, AFNTMH, CL, Z, ALFA, A4, AZ4, B4, RA4, CARC, WTT, WTTER;
	double DUMCT, CTTPG, RTOPCY, DTOPCY, T4, T7, T8;
	int NUMIT, IEX, IAI0, IOS, IOQ;
	double BADA2M, DUM1, DUM2, PCARC, CCARC, TOLCOR;

	DUM1 = 0.0;

	WV = sqrt(19.9094165 / pow(inp.R1 / 3443.93359, 3));
	ORBPER = PI2 / WV;
	//Angle from ascending node to longitude of launch along on the earth's equator, in radians
	B1 = atan2(sin(inp.C1) * sin(inp.AZ1), cos(inp.AZ1));
	//Argument of the input parking orbit state vector past its ascending node on the earth's equator, in radians
	A1 = atan2(sin(inp.C1), cos(inp.C1) * cos(inp.AZ1));
	//Calculate inclination of the earth parking orbit to the earth's equator, in radians
	FIV = abs(atan(sin(inp.C1) / (cos(inp.C1) * sin(B1))));
	//Ascending node of the VOP for insertion at base time
	RNVBT = RAGBT + inp.FLO1 - B1;
	RNVBT = HELP(RNVBT);

	if (inp.debug)
	{

	}

	TOPCY = ORBPER * (inp.ORBNUM - 1.0) + 40.0;
	CO1 = 0;
	PERCYN(TOPCY);
	if (CO1) return;

	FIVTL = 0;
	C4 = 0;
	R4 = inp.R1 + 12;
	W = ENERGY(inp.IPERT, C4);
	RQDFT = FLYTYM(inp.IPERT);

	A2M = A6 + 1.33 / DEG;
	A2M = HELP(A2M);
	PT1 = 0;
	NUMIT = 0;
	IEX = 0;
	IAI0 = 1;
	IOS = 1;

	do
	{
		NUMIT = NUMIT + 1;
		GEOARG(FIM, A2M, RNM, AZ2M, B2M, C2, RA2);

		if (abs(C2) >= FIV)
		{
			//Inaccessible node logic
			if (IEX == 0)
			{
				IEX = 1;
			}
			C2 = FIV * OrbMech::sign(C2);
			BADA2M = A2M;
			IOQ = 1;
			//fprintf("INACCESSIBLE NODE ON MOP CALLED FOR.\n");
			if (IEX == 3)
			{
				//fprintf("NECESSARY NODE ON MOP IS INACCESSIBLE. NO CONVERGENCE.\n");
				return;
			}
			else if (IEX == 1)
			{
				if (C2 < 0)
				{
					IOQ = 2;
				}

				IEX = 2;
				//fprintf("NODE PLACED AT BEGINNING OF INACESSIBLE REGION ON MOP.\n");
			}
			else
			{
				if (C2 > 0)
				{
					IOQ = 2;
				}
				IEX = 3;
				//fprintf("NODE PLACED AT END OF INACESSIBLE REGION ON MOP.\n");
			}

			GEOLAT(FIM, C2, DUM1, IOQ, A2M, B2M, AZ2M, DUM2);
			GEOARG(FIM, A2M, RNM, AZ2M, B2M, DUM1, RA2);
			double TCOR = A2M - BADA2M;
			TCOR = HELP(TCOR);
			TOPCY = TOPCY + TCOR / WM;
			PERCYN(TOPCY);
			FIVTL = AZ2M - PI05;
			RQDFT = FLYTYM(inp.IPERT);
			W = ENERGY(inp.IPERT, C2);
		}

		GEOLAT(FIV, C2, RA2, inp.IPOA, A2V, A3V, AZ2V, RNV);
		FIVTL = AZ2M - AZ2V;

		SUBB(inp.IPERT, BETA, AFNTMH);
		CL = SUBCL(inp.IPERT);
		TLIMP(W);

		Z = atan(sin(BETA) * cos(FIVTL) / cos(BETA));
		ALFA = pow(cos(BETA) / (cos(Z) * cos(CL)), 2) - 1.0;
		if (ALFA < 0)
		{
			ALFA = 0;
		}
		ALFA = atan(sqrt(ALFA)) + Z;
		A4 = A2V + ALFA;
		A4 = HELP(A4);
		GEOARG(FIV, A4, RNV, AZ4, B4, C4, RA4);

		CARC = A4 - A1 - APS;
		CARC = HELP(CARC);
		if (CARC < 0 && inp.ORBNUM != 0.0)
		{
			CARC = CARC + PI2;
		}
		if (NUMIT == 1)
		{
			PCARC = CARC;
		}

		if (abs(CARC - PCARC) >= PI)
		{
			//Orbit coast time excursion logic
			if (IAI0 == 1)
			{
				IAI0 = 2;
				CCARC = CARC;
				//fprintf("ORBIT COAST ARC EXCURSION. ARC WILL BE CONSTRAINED TO WITHIN PI OF NEXT VALUE.\n");
			}
			else
			{
				double SDAIO = 1.0;
				if (CARC > CCARC)
				{
					SDAIO = -1.0;
				}
				CARC = CARC + SDAIO * PI2;
			}
		}
		PCARC = CARC;
		if (inp.ORBNUM != 0.0)
		{
			CTIO = ORBPER * (CARC / (PI2)+inp.ORBNUM - 1.0);
		}
		else
		{
			CTIO = ORBPER * (CARC / (PI2));
		}
		T1 = RNV - RNVBT;
		T1 = HELP(T1);
		T1 = T1 / WE;
		if (NUMIT == 1)
		{
			PT1 = T1;
		}

		if (abs(T1 - PT1) >= 12.0)
		{
			//Time of orbit state vector excursion logic
			if (IOS == 1)
			{
				IOS = 2;
				TOLCOR = PI2 / WE * OrbMech::sign(T1);
				//fprintf("CHANGE IN INSERTION TIME EXCEEDS 12 HOURS. SIGN WILL BE CONSTRAINED TO THAT OF NEXT VALUE.\n");
			}
			else
			{
				T1 = T1 + TOLCOR;
			}
		}

		PT1 = T1;
		DUMCT = APS / WV;
		CTTPG = CTIO + DUMCT;
		RTOPCY = T1 + CTIO + DUMCT + RQDFT;
		DTOPCY = RTOPCY - TOPCY;

		T8 = T1 + CTIO;
		T4 = T8 + DUMCT;
		T7 = T4 + FTOCO;

		if (abs(DTOPCY) >= inp.TOL && NUMIT < inp.MAXIT)
		{
			TOPCY = RTOPCY;
			PERCYN(TOPCY);
			if (CO1) return;
			W = ENERGY(inp.IPERT, C4);
			RQDFT = FLYTYM(inp.IPERT);
			A2M = A6 - BETA;
			A2M = HELP(A2M);
		}

	} while (abs(DTOPCY) >= inp.TOL && NUMIT < inp.MAXIT);

	double A3, AZ3, B3, C3, RA3, S;

	A3 = A2V + AFNTMH;
	A3 = HELP(A3);
	GEOARG(FIV, A3, RNV, AZ3, B3, C3, RA3);
	S = A4 - A3;
	S = HELP(S);
	MH.x = cos(C3) * cos(RA3);
	MH.y = cos(C3) * sin(RA3);
	MH.z = sin(C3);

	WTT = W * 2.0;
	WTTER = WTT * pow(3600.0 / (6076.11549 * 3443.93359), 2);

	outp.T1 = T1;
	outp.CTIO = CTIO;
	outp.C3 = WTTER;
	outp.S = S;
	outp.TOPCY = TOPCY;
}

void TLIFirstGuess::UPDATE(double TOIDIS)
{
	VECTOR3 VEC1, UMH, HV, POV, DUM;
	double PSI, DOT, HOMSQ, HOM, DISRNV, AZ1, B1, C1, RA1, ARC;

	PSI = WM * TOIDIS * ((WV - WE * cos(FIV)) / (WV - WM * cos(FIVTL)));
	DOT = dotp(MH, HM);
	HOMSQ = dotp(HM, HM);
	HOM = sqrt(HOMSQ);
	VEC1 = crossp(HM, MH);

	UMH = HM * DOT / HOMSQ + VEC1 * sin(PSI) / HOM + crossp(VEC1, HM) * cos(PSI) / HOMSQ;

	DISRNV = RNV + WE * TOIDIS;
	HV = _V(sin(FIV) * sin(DISRNV), -sin(FIV) * cos(DISRNV), cos(FIV));

	DUM = crossp(HV, UMH);
	DEL = ANGLE(HV, UMH, DUM);
	DEL = PI05 - DEL;

	GEOARG(FIV, A1, DISRNV, AZ1, B1, C1, RA1);
	POV = _V(cos(C1) * cos(RA1), cos(C1) * sin(RA1), sin(C1));
	ARC = ANGLE(POV, DUM, HV);
	ARC = ARC - PI05;
	ARC = HELP(ARC);
}

void TLIFirstGuess::GetOutput(TLIFirstGuessOutputs& out)
{
	out = outp;
}

void TLIFirstGuess::TLIMP(double W)
{
	double U, UE, FKMPNM, FPKM, RORB, C3, CDIF, ETA, DV;
	double VPGSQ, VPG, HSQ, H, P, A, E, B, R7, G7D, COEF;

	U = 0.23167004e13;
	//Gravitational constant of the earth, in km^3/sec^2
	UE = 398603.2;
	//Conversion factor km/NM
	FKMPNM = 1.852;
	//Conversion factor, ft/km
	FPKM = 3280.8399;
	//Radius of circular earth parking orbit, in kilometers
	RORB = inp.R1 * FKMPNM;
	//Trajectory energy, in (km/sec)^2
	C3 = 2.0 * W / (FPKM * FPKM);
	//Difference between the energies of the circular parking orbit and the trajectory, in (km/sec)^2
	CDIF = UE / RORB + C3;
	//Empirical equations
	//True anomaly of TLI cutoff, in radians
	ETA = atan((2.1397405 - RORB / 5750.0) * (1.0143460 / inp.TTW - 0.02) / (260.73592 / CDIF + 1.6338479));
	//Angle from beginning of coplanar TLI to perigee, in radians (equals alpha plus sigma)
	APS = atan((2.4185082 - RORB / 4620.0) * (1.0365969 / inp.TTW - 0.051020408) / (254.80898 / CDIF + 2.1846192));
	//Perigee radius
	R4 = inp.R1 + (-0.15664127 * pow(CDIF, 3) + 34.56894 * pow(CDIF, 2) - 253.71417 * CDIF) / (RORB * pow(inp.TTW, 2) * FKMPNM);
	//Characteristic velocity of coplanar TLI maneuver, in ft/sec
	DV = (sqrt(CDIF + UE / RORB) - sqrt(UE / RORB) + (pow(CDIF - 11.61, 2) * (2.7022098 - RORB / 3850.0) * (2.0264543e-6 / inp.TTW + 3.632748e-8)) / inp.TTW) * FPKM;

	VPGSQ = 2.0 * (W + U / R4);
	VPG = sqrt(VPGSQ);
	HSQ = pow(R4, 2) * VPGSQ;
	H = R4 * VPG;
	P = HSQ / U;
	A = -U / (2.0 * W);
	E = 1.0 - R4 / A;
	B = sqrt(abs(A * P));
	R7 = P / (1.0 + E * cos(ETA));
	G7D = atan(E * R7 / P * sin(ETA)) * DEG;
	COEF = 6076.11549 / 3600.0 * A / H;
	//Flight time on trajectory of TLI cutoff past perigee, in hours
	FTOCO = COEF * (2.0 * B * atan(R4 / B * tan(ETA / 2.0)) - E * R7 * sin(ETA));
}

double TLIFirstGuess::SUBCL(int IPERT) const
{
	double SECL, OBCL, CL;

	if (IPERT <= 1)
	{
		SECL = 0;
	}
	else
	{
		SECL = (0.11 - 8e-7 * EMR) * sin(2.0 * SMOPL + 0.611) - 0.01;
	}
	if (IPERT == 0 || IPERT == 2)
	{
		OBCL = 0;
	}
	else
	{
		OBCL = 1.6457056e-2 * sin(2.0 * AM + 0.166);
	}
	CL = 6.3814106 - (1.1030239e-5) * EMR + (5.3567533e-3) * EMRDOT + 6950.0 / (inp.RPC + 652.0) + 0.0114 * abs(inp.YPC) + 0.01892 * inp.YPC * FIVTL - cos(FIVTL) * (0.055 * inp.XPC - 1.35) -
		pow(0.2457 - 4.5e-7 * EMR, 2) * (888.88889 + 1.0 / (5.0625 * inp.XPC / (9.828e5 - 1.8 * EMR) - 0.001125)) + 0.055 * inp.XPC + SECL + OBCL;
	CL = CL / DEG;
	return CL;
}

void TLIFirstGuess::SUBB(int IPERT, double& BETA, double& AFNTMH) const
{
	double XC, YC, PLUS, YT, S, Z, COTH, SITH;

	XC = (1.93 - 0.037 * inp.XPC) / DEG;
	YC = (-OrbMech::sign(FIVTL) * (0.6 - 0.02 * inp.XPC) - 0.035 * inp.YPC) / DEG;
	if (IPERT == 0)
	{
		PLUS = 0;
	}
	else if (IPERT == 1)
	{
		PLUS = 0.0317 * cos(AM - 18.0 / DEG);
	}
	else
	{
		PLUS = (0.0285 + 0.0115 * cos(3.85 * DS)) * cos(AM - (10.0 + 5.0 * cos(3.85 * DS)) / DEG);
	}
	YT = (PLUS - inp.YPC * (0.015165 - 0.000201 * inp.XPC)) / DEG;
	if (abs(YT) < abs(FIVTL))
	{
		S = (-sin(YT - YC) - cos(FIVTL) * sin(YC)) / (sin(FIVTL) * cos(YC));
		Z = -OrbMech::sign(S) * atan(1 / sqrt(1 / (S * S) - 1));
	}
	else
	{
		YT = OrbMech::sign(YT) * abs(FIVTL);
		S = -1.0;
		if (FIVTL * YT < 0)
		{
			S = 1.0;
		}
		Z = -OrbMech::sign(S) * 90.0 / DEG;
		//fprintf("WARNING, TANGENCY SUEFACE PROBLEM. YT REDEFINED AS SIGN(ABS(FIVTL),YT)\n");
	}
	BETA = Z - XC;
	COTH = sin(FIVTL) * cos(Z);
	SITH = sqrt(1.0 - COTH * COTH);
	AFNTMH = Z / cos(FIVTL) + (YC - YT) / (SITH / COTH);
}

void TLIFirstGuess::GEOLAT(double FI, double C, double RA, int I, double& A, double& B, double& AZ, double& RN) const
{
	if (C == FI)
	{
		A = PI05;
		B = PI05;
		AZ = PI05;
	}
	else if (C == -FI)
	{
		A = -PI05;
		B = -PI05;
		AZ = PI05;
	}
	else if (C == 0.0)
	{
		if (I == 1)
		{
			A = 0;
			B = 0;
			AZ = PI05 - FI;
		}
		else
		{
			A = PI;
			B = PI;
			AZ = PI05 + FI;
		}
	}
	else
	{
		double CSCA = sin(FI) / sin(C);
		A = atan(1 / sqrt(CSCA * CSCA - 1.0));
		if (I == 2)
		{
			A = PI - A;
		}
		if (C < 0)
		{
			A = -A;
		}
		B = atan2(sin(A) * cos(FI), cos(A));
		AZ = atan2(cos(FI), sin(FI) * cos(A));
	}

	RN = RA - B;
	RN = HELP(RN);
}

void TLIFirstGuess::GEOARG(double FI, double A, double RN, double &AZ, double &B, double &C, double &RA) const
{
	if (FI == 0.0)
	{
		AZ = PI05;
		B = A;
		C = 0;
	}
	else if (FI == PI)
	{
		AZ = -PI05;
		B = -A;
		C = 0;
	}
	else if (FI == PI05)
	{
		if (abs(A) < PI05)
		{
			AZ = 0;
			B = 0;
			C = A;
		}
		else if (A >= PI05)
		{
			AZ = PI;
			B = PI;
			C = PI - A;
		}
		else
		{
			AZ = PI;
			B = PI;
			C = -A - PI;
		}
	}
	else if (A == PI05)
	{
		AZ = PI05;
		B = PI05;
		C = FI;
	}
	else if (A == -PI05)
	{
		AZ = PI05;
		B = -PI05;
		C = -FI;
	}
	else if (A == 0.0)
	{
		AZ = PI05 - FI;
		B = 0;
		C = 0;
	}
	else if (A == PI)
	{
		AZ = PI05 + FI;
		B = PI;
		C = 0;
	}
	else
	{
		AZ = atan2(cos(FI), sin(FI) * cos(A));
		B = atan2(sin(A) * cos(FI), cos(A));
		C = atan(sin(FI) * sin(B) / cos(FI));
	}
	RA = RN + B;
	RA = HELP(RA);
}

double TLIFirstGuess::ENERGY(int IPERT, double C4) const
{
	double SEW, EOBW, PHI, FI, XGN, YGN, COA, A, COB, B, F, W;

	//SEW = Effect of solar gravitation
	if (IPERT <= 1)
	{
		SEW = 0;
	}
	else
	{
		SEW = 3.9e4 * sin(2.0 * SMOPL - 1.657) - 1.3e4;
	}

	//EOBW = Effect of Earth oblateness
	if (IPERT == 0 || IPERT == 2)
	{
		EOBW = 0;
	}
	else
	{
		EOBW = (5.97 - 2.67 * cos(C4)) * cos(2.55 * C4) * 1e5;
	}
	PHI = (30.0 + inp.XPC * 2.0 / 3.0) / DEG;
	PHI = sin(PHI) / cos(PHI);
	FI = 1.0 / (EMR - 29000.0);
	XGN = -68.0 + EMR / 10000.0 + 0.625 * inp.XPC;
	YGN = -sin(FIVTL) * (9.6 - 0.16 * (XGN + 48.0)) / DEG;
	XGN = XGN / DEG;
	COA = cos(XGN) * cos(YGN);
	A = atan(sqrt(1 / (COA * COA) - 1.0));
	COB = sin(YGN) * sin(inp.YPC / DEG) + cos(YGN) * cos(inp.YPC / DEG) * cos(inp.XPC / DEG - XGN);
	B = atan(sqrt(1 / (COB * COB) - 1));
	F = (B - A) * DEG;
	W = -2714728.4 - 1.4002127e12 * FI + EMRDOT * (3070.6244 + FI * 2.1470226e6 + EMRDOT * 1.1495569) + 750.0 * R4 + (1.0 - cos(FIVTL)) * (2.2 * EMR - 6.8e4) +
		F * (-75150.0 - 0.05 * EMR - 8.75 * EMRDOT) - (4050.0 + 364500.0 / (F - 90.0)) * (2.25 * EMRDOT - 0.01 * EMR + 7070.0) +
		(8.6016e6) * (PHI*PHI) * (1.0 / ((inp.RPC - 1015.0) / (2217.025 * PHI) + 1.0) - 1.0) + SEW + EOBW;
	return W;
}

double TLIFirstGuess::FLYTYM(int IPERT) const
{
	double SGFT, XGN, YGN, COA, A, COB, B, F, RQDFT;

	if (IPERT <= 1)
	{
		SGFT = 0;
	}
	else
	{
		SGFT = 2e-7 * EMR + cos(0.1745 - 2.0 * SMOPL) * (EMR * 9.6e-7 - EMRDOT * 7.5e-5 - 0.01554) - 0.028;
	}
	XGN = -68.0 + EMR / 10000.0 + 0.625 * inp.XPC;
	YGN = -sin(FIVTL) * (9.6 - 0.16 * (XGN + 48)) / DEG;
	XGN = XGN / DEG;
	COA = cos(XGN) * cos(YGN);
	A = atan(sqrt(1 / (COA * COA) - 1));
	COB = sin(YGN) * sin(inp.YPC / DEG) + cos(YGN) * cos(inp.YPC / DEG) * cos(inp.XPC / DEG - XGN);
	B = atan(sqrt(1 / (COB * COB) - 1));
	F = (B - A) * DEG;
	RQDFT = (-23.480035 + EMR * 4.455251e-4 - EMRDOT * (EMR * 1.6723713e-7 - 1.5007662e-2) + (cos(FIVTL) - 1.0) * (EMR * 2.4e-5 - 3.96) -
		pow(0.012 - 3.3e-6 * EMR + 5e-4 * EMRDOT, 2) * (425.53191 + 1.0 / (F * 2.209e-5 / (0.048 - EMR * 1.32e-5 + EMRDOT * 0.002)
			- 0.00235)) + SGFT) * pow(inp.RPC / 1015, 0.17);
	return RQDFT;
}

void TLIFirstGuess::PERCYN(double T)
{
	VECTOR3 RM, VM, RS, SCHM, IV;
	double RAM, RA6, DECM, C6, XHM, YHM, ZHM, B6, AZ6;
	double RAS, AS, SMOPD;

	const double C = 3443.93358;

	bool IERR = pRTCC->PLEFEM(1, T + inp.Hour, 0, &RM, &VM, &RS, NULL);
	if (IERR)
	{
		CO1 = 2;
		return;
	}
	//Convert from m and m/s to Er and Er/hr
	RM /= 6378165.0;
	VM /= 6378165.0 / 3600.0;
	RS /= 6378165.0;

	EMR = length(RM) * C;
	EMRDOT = dotp(RM, VM) / length(RM) * C;

	RAM = atan2(RM.y, RM.x);
	RA6 = RAM + PI;
	RA6 = HELP(RA6);
	DECM = atan2(RM.z, sqrt(RM.x * RM.x + RM.y * RM.y));
	C6 = -DECM;
	HM = crossp(RM, VM);
	XHM = HM.x;
	YHM = HM.y;
	ZHM = HM.z;
	WM = length(HM) / (pow(length(RM), 2));
	FIM = atan(sqrt(XHM * XHM + YHM * YHM) / ZHM);
	RNM = atan2(XHM, -YHM);
	B6 = RA6 - RNM;
	B6 = HELP(B6);
	A6 = atan2(sin(B6), cos(B6) * cos(FIM));
	AM = A6 + PI;
	AM = HELP(AM);
	AZ6 = atan2(abs(sin(B6)), cos(B6) * abs(sin(C6)));
	RAS = atan2(RS.y, RS.x);
	DS = atan2(RS.z, sqrt(RS.x * RS.x + RS.y * RS.y));
	AS = atan(sqrt(pow(length(RS), 2) / (RS.x * RS.x) - 1.0));
	if (RS.x < 0) AS = PI - AS;
	if (RS.z < 0) AS = -AS;
	SCHM = crossp(RS, HM);
	IV = crossp(RM, SCHM);
	SMOPL = atan2(length(IV), dotp(RM, SCHM));
	if (dotp(IV, HM) < 0)
	{
		SMOPL = -SMOPL;
	}
	SMOPL += PI05;
	SMOPL = HELP(SMOPL);
	SMOPD = PI05 - atan2(length(SCHM), dotp(RS, HM));
}

double TLIFirstGuess::HELP(double X) const
{
	while (X > PI) X -= PI2;
	while (X <= -PI)X += PI2;
	return X;
}

double TLIFirstGuess::ANGLE(VECTOR3 VEC1, VECTOR3 VEC2, VECTOR3 VEC3) const
{
	VECTOR3 H;
	double DUM, XR, H4;

	H = crossp(VEC1, VEC2);
	H4 = length(H);
	DUM = dotp(VEC1, VEC2);
	XR = atan2(H4, DUM);
	DUM = dotp(H, VEC3);
	if (DUM < 0)
	{
		XR = -XR;
	}
	return XR;
}

TLIProcessor::TLIProcessor(RTCC *r) : TLTrajectoryComputers(r)
{

}

void TLIProcessor::Init(TLIMEDQuantities med, TLMCCMissionConstants constants, double GMTBase)
{
	TLTrajectoryComputers::Init(constants, GMTBase);

	MEDQuantities = med;
}

void TLIProcessor::Main(TLIOutputData &out)
{
	ErrorIndicator = 0;

	//Get time of mixture ratio shift
	TLITargetingParametersTable *tlitab = NULL;
	int LD;

	LD = pRTCC->GZGENCSN.RefDayOfYear;

	for (int counter = 0; counter < 10; counter++)
	{
		if (LD == pRTCC->PZSTARGP.data[counter].Day)
		{
			tlitab = &pRTCC->PZSTARGP.data[counter];
			break;
		}
	}

	if (tlitab == NULL)
	{
		//Error
		out.ErrorIndicator = 5;
		return;
	}
	T_MRS_SIVB = tlitab->T2[MEDQuantities.Opportunity - 1] / 3600.0;

	switch (MEDQuantities.Mode)
	{
	case 1:
		Option1();
		break;
	case 2:
		Option2_5(true);
		break;
	case 3:
		Option3();
		break;
	case 4:
		Option4();
		break;
	case 5:
		Option2_5(false);
		break;
	default:
		out.ErrorIndicator = 1;
		return;
	}

	out.ErrorIndicator = ErrorIndicator;
	if (out.ErrorIndicator) return;

	//Convert to LVDC parameters

	//Convert to ECT at GRR

	EphemerisData2 sv, sv_ECT;

	sv.R = outarray.sv_tli_cut.R;
	sv.V = outarray.sv_tli_cut.V;
	sv.GMT = pRTCC->SystemParameters.MCGRIC*3600.0;
	pRTCC->ELVCNV(sv, 0, 1, sv_ECT);

	OELEMENTS coe = OrbMech::coe_from_sv(sv_ECT.R, sv_ECT.V, mu_E);
	coe.h = dotp(sv_ECT.V, sv_ECT.V) - 2.0*mu_E / length(sv_ECT.R);

	OELEMENTS coe2 = LVTAR(coe, pRTCC->SystemParameters.MCLGRA, pRTCC->SystemParameters.MCERTS*pRTCC->SystemParameters.MCGRIC);

	out.uplink_data.Inclination = coe2.i;
	out.uplink_data.theta_N = coe2.RA;
	out.uplink_data.e = coe2.e;
	out.uplink_data.C3 = coe2.h;
	out.uplink_data.alpha_D = coe2.w;
	out.uplink_data.f = coe2.TA;
	out.uplink_data.GMT_TIG = outarray.sv_tli_ign.GMT;

	out.dv_TLI = outarray.dv_TLI;
	out.sv_TLI_ign = outarray.sv_tli_ign;
	out.sv_TLI_cut = outarray.sv_tli_cut;
}

void TLIProcessor::Option1()
{
	PMMSPTInput in;

	in.QUEID = 34;
	in.GMT = MEDQuantities.state.GMT;
	in.R = MEDQuantities.state.R;
	in.V = MEDQuantities.state.V;
	in.Table = MEDQuantities.mpt;
	in.InjOpp = MEDQuantities.Opportunity;
	in.mpt = NULL;
	in.CurMan = NULL;

	int err = pRTCC->PMMSPT(in);

	if (err)
	{
		ErrorIndicator = 1;
		return;
	}
	//Data is now in PZTTLIPL

	//Propagate state to TIG
	EMSMISSInputTable emsin;

	emsin.AnchorVector.R = pRTCC->PZTTLIPL.R;
	emsin.AnchorVector.V = pRTCC->PZTTLIPL.V;
	emsin.AnchorVector.GMT = pRTCC->PZTTLIPL.TB6;
	emsin.AnchorVector.RBI = BODY_EARTH;

	emsin.MaxIntegTime = pRTCC->PZTTLIPL.TIG - emsin.AnchorVector.GMT;
	emsin.VehicleCode = MEDQuantities.mpt;
	emsin.useInputWeights = true;
	emsin.WeightsTable = &MEDQuantities.WeightsTable;

	pRTCC->EMSMISS(&emsin);

	if (emsin.NIAuxOutputTable.ErrorCode)
	{
		ErrorIndicator = 1;
		return;
	}

	std::vector<double> var, arr;
	var.resize(20);
	arr.resize(20);

	var[4] = pRTCC->PZTTLIPL.C3 / pow(R_E / 3600.0, 2); //C3 in Er^2/hr^2
	var[5] = 0.0; //dt_EPO

	//Calculate delta
	VECTOR3 N_I;
	N_I = unit(crossp(pRTCC->PZTTLIPL.R, pRTCC->PZTTLIPL.V));
	var[6] = asin(dotp(pRTCC->PZTTLIPL.T, N_I));

	void *constPtr;

	outarray.TLIIndicator = true;
	outarray.EllipticalCaseIndicator = false;
	outarray.TLIOnlyIndicator = true;

	outarray.sv0.R = pRTCC->PZTTLIPL.R;
	outarray.sv0.V = pRTCC->PZTTLIPL.V;
	outarray.sv0.GMT = pRTCC->PZTTLIPL.TIG;
	outarray.sv0.RBI = BODY_EARTH;
	outarray.M_i = MEDQuantities.WeightsTable.ConfigWeight;
	outarray.sigma_TLI = pRTCC->PZTTLIPL.sigma;
	constPtr = &outarray;

	IntegratedTrajectoryComputer(var, constPtr, arr, false);
}

//Hybrid ellipse
void TLIProcessor::Option3()
{
	//Propagate state to TIG
	EMSMISSInputTable in;

	in.AnchorVector = MEDQuantities.state;
	in.MaxIntegTime = MEDQuantities.GMT_TIG - MEDQuantities.state.GMT;
	if (in.MaxIntegTime < 0)
	{
		in.MaxIntegTime = abs(in.MaxIntegTime);
		in.IsForwardIntegration = false;
	}
	in.VehicleCode = RTCC_MPT_CSM;
	in.useInputWeights = true;
	in.WeightsTable = &MEDQuantities.WeightsTable;

	pRTCC->EMSMISS(&in);

	if (in.NIAuxOutputTable.ErrorCode)
	{
		ErrorIndicator = 1;
		return;
	}

	outarray.sv0 = in.NIAuxOutputTable.sv_cutoff;
	outarray.M_i = in.NIAuxOutputTable.CutoffWeight;

	//Calculate initial guess for C3
	double C3_guess, R_I;

	R_I = length(outarray.sv0.R);
	C3_guess = pow(MEDQuantities.dv_available, 2) - OrbMech::mu_Earth / R_I + 2.0*MEDQuantities.dv_available*sqrt(OrbMech::mu_Earth / R_I);

	bool err = HybridMission(C3_guess, MEDQuantities.dv_available);
	if (err)
	{
		ErrorIndicator = 2;
		return;
	}
}

void TLIProcessor::Option4()
{
	//Propagate state to TIG
	EMSMISSInputTable in;

	in.AnchorVector = MEDQuantities.state;
	in.MaxIntegTime = MEDQuantities.GMT_TIG - MEDQuantities.state.GMT;
	if (in.MaxIntegTime < 0)
	{
		in.MaxIntegTime = abs(in.MaxIntegTime);
		in.IsForwardIntegration = false;
	}
	in.VehicleCode = RTCC_MPT_CSM;
	in.useInputWeights = true;
	in.WeightsTable = &MEDQuantities.WeightsTable;

	pRTCC->EMSMISS(&in);

	if (in.NIAuxOutputTable.ErrorCode)
	{
		ErrorIndicator = 1;
		return;
	}

	outarray.sv0 = in.NIAuxOutputTable.sv_cutoff;
	outarray.M_i = in.NIAuxOutputTable.CutoffWeight;

	//Calculate initial guess for C3
	double R_C, a_C, V_C, C3_guess;

	R_C = length(outarray.sv0.R);
	a_C = (R_C + R_E + MEDQuantities.h_ap) / 2.0;
	V_C = sqrt(mu_E*(2.0 / R_C - 1.0 / a_C));
	C3_guess = V_C * V_C - 2.0*mu_E / R_C;

	bool err = ConicTLIIEllipse(C3_guess, MEDQuantities.h_ap);
	if (err)
	{
		ErrorIndicator = 2;
		return;
	}
	err = IntegratedTLIIEllipse(outarray.C3_TLI, MEDQuantities.h_ap);
	if (err)
	{
		ErrorIndicator = 2;
		return;
	}
}

void TLIProcessor::Option2_5(bool freereturn)
{
	//Initial guess
	TLIFirstGuessInputs fgin;
	TLIFirstGuessOutputs fgout;
	TLIFirstGuess cist(pRTCC);

	//Step 1: propagate state to estimated TIG
	EMSMISSInputTable in;

	in.AnchorVector = MEDQuantities.state;
	in.MaxIntegTime = MEDQuantities.GMT_TIG - MEDQuantities.state.GMT;
	if (in.MaxIntegTime < 0)
	{
		in.MaxIntegTime = abs(in.MaxIntegTime);
		in.IsForwardIntegration = false;
	}
	in.VehicleCode = RTCC_MPT_CSM;
	in.useInputWeights = true;
	in.WeightsTable = &MEDQuantities.WeightsTable;

	pRTCC->EMSMISS(&in);

	if (in.NIAuxOutputTable.ErrorCode)
	{
		ErrorIndicator = 1;
		return;
	}

	outarray.sv0 = in.NIAuxOutputTable.sv_cutoff;
	outarray.M_i = in.NIAuxOutputTable.CutoffWeight;
	outarray.rad_lls = OrbMech::R_Moon;

	double rmag, vmag, rtasc, decl, fpav, az;
	OrbMech::rv_from_adbar(outarray.sv0.R, outarray.sv0.V, rmag, vmag, rtasc, decl, fpav, az);

	//Step 2: First guess logic
	fgin.BHA = pRTCC->SystemParameters.MCLAMD;
	fgin.Hour = outarray.sv0.GMT / 3600.0;
	fgin.C1 = decl;
	fgin.FLO1 = OrbMech::LongitudeConversion(rtasc, outarray.sv0.GMT, OrbMech::w_Earth, pRTCC->SystemParameters.MCLAMD, true);
	fgin.AZ1 = az;
	fgin.R1 = rmag / 1852.0;
	fgin.ORBNUM = 0.0;
	fgin.IPOA = MEDQuantities.IPOA;
	fgin.IPERT = 3;
	fgin.XPC = 0.0;
	fgin.YPC = 0.0;
	fgin.RPC = (OrbMech::R_Moon + MEDQuantities.h_PC) / 1852.0;

	cist.Calculate(fgin);
	int err = cist.GetError();
	if (err)
	{
		return;
	}
	cist.GetOutput(fgout);

	//Step 3: Converge trajectory
	double GMT_node, lat_nd, lng_nd;
	if (freereturn)
	{
		GMT_node = outarray.sv0.GMT + fgout.TOPCY*3600.0;
		lat_nd = MEDQuantities.lat_PC;
		lng_nd = PI;
	}
	else
	{
		GMT_node = MEDQuantities.GMT_node;
		lat_nd = MEDQuantities.lat_PC;
		lng_nd = MEDQuantities.lng_node;
	}
	err = IntegratedTLIToNode(fgout.C3, fgout.CTIO, fgout.DEL / 4.0, fgout.S, GMT_node, lat_nd, lng_nd);
	if (err)
	{
		ErrorIndicator = 2;
		return;
	}

	if (freereturn)
	{
		err = IntegratedTLIFlyby(outarray.C3_TLI, outarray.dt_EPO / 3600.0, outarray.delta_TLI, fgout.S, MEDQuantities.lat_PC);
		if (err)
		{
			ErrorIndicator = 2;
			return;
		}
	}
}

bool TLIProcessor::HybridMission(double C3_guess, double dv_TLI)
{
	double C3_guess_ER = C3_guess / pow(R_E / 3600.0, 2);

	void *constPtr;
	outarray.TLIIndicator = true;
	outarray.TLIOnlyIndicator = true;
	//EllipticalCaseIndicator is used to decide between the nominal vs. alternate mission TLI polynomials
	if (C3_guess_ER < -5.0)
	{
		outarray.EllipticalCaseIndicator = true;
	}
	else
	{
		outarray.EllipticalCaseIndicator = false;
	}
	outarray.sigma_TLI = MissionConstants.sigma;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[4] = true;

	block.IndVarGuess[4] = C3_guess_ER;
	block.IndVarGuess[5] = 0.0; //dt_EPO
	block.IndVarGuess[6] = MissionConstants.delta;

	block.IndVarStep[4] = pow(2, -23);
	block.IndVarWeight[4] = 4.0;

	block.DepVarSwitch[9] = true;
	block.DepVarLowerLimit[9] = dv_TLI - 1.0*0.3048;
	block.DepVarUpperLimit[9] = dv_TLI + 1.0*0.3048;
	block.DepVarClass[9] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

bool TLIProcessor::ConicTLIIEllipse(double C3_guess, double h_ap)
{
	void *constPtr;
	outarray.MidcourseCorrectionIndicator = false;
	outarray.TLIIndicator = true;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[10] = true;

	block.IndVarGuess[10] = C3_guess / pow(R_E / 3600.0, 2);
	block.IndVarGuess[11] = MissionConstants.delta;
	block.IndVarGuess[12] = MissionConstants.sigma;

	block.IndVarStep[10] = pow(2, -23);

	block.IndVarWeight[10] = 4.0;

	block.DepVarSwitch[21] = true;

	block.DepVarLowerLimit[21] = (h_ap - 1.0*1852.0) / R_E;

	block.DepVarUpperLimit[21] = (h_ap + 1.0*1852.0) / R_E;

	block.DepVarClass[21] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

bool TLIProcessor::IntegratedTLIIEllipse(double C3_guess_ER, double h_ap)
{
	void *constPtr;
	outarray.TLIIndicator = true;
	outarray.EllipticalCaseIndicator = true;
	outarray.sigma_TLI = MissionConstants.sigma;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[4] = true;

	block.IndVarGuess[4] = C3_guess_ER;
	block.IndVarGuess[5] = 0.0; //dt_EPO
	block.IndVarGuess[6] = MissionConstants.delta;

	block.IndVarStep[4] = pow(2, -23);
	block.IndVarWeight[4] = 4.0;

	block.DepVarSwitch[8] = true;
	block.DepVarLowerLimit[8] = (h_ap - 1.0*1852.0) / R_E;
	block.DepVarUpperLimit[8] = (h_ap + 1.0*1852.0) / R_E;
	block.DepVarClass[8] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

bool TLIProcessor::IntegratedTLIToNode(double C3_guess_ER, double T_c_hrs, double delta, double sigma, double GMT_nd, double lat_nd, double lng_nd)
{
	void *constPtr;
	outarray.TLIIndicator = true;
	outarray.EllipticalCaseIndicator = false;
	outarray.sigma_TLI = sigma;
	outarray.NodeStopIndicator = true;
	outarray.GMT_nd = GMT_nd;

	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[4] = true; //C3
	block.IndVarSwitch[5] = true; //DT EPO
	block.IndVarSwitch[6] = true; //TLI plane change

	block.IndVarGuess[4] = C3_guess_ER;
	block.IndVarGuess[5] = T_c_hrs;
	block.IndVarGuess[6] = delta;

	block.IndVarStep[4] = pow(2, -21);
	block.IndVarStep[5] = pow(2, -22);
	block.IndVarStep[6] = pow(2, -21);

	block.IndVarWeight[4] = 1.0;
	block.IndVarWeight[5] = 1.0;
	block.IndVarWeight[6] = 1.0;

	double R_nd;
	R_nd = OrbMech::R_Moon + MEDQuantities.h_PC;

	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[1] = true;
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[3] = true;
	block.DepVarLowerLimit[0] = (R_nd - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[1] = lat_nd - 0.01*RAD;
	block.DepVarLowerLimit[2] = lng_nd - 0.01*RAD;
	block.DepVarLowerLimit[3] = 90.0*RAD;
	block.DepVarUpperLimit[0] = (R_nd + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[1] = lat_nd + 0.01*RAD;
	block.DepVarUpperLimit[2] = lng_nd + 0.01*RAD;
	block.DepVarUpperLimit[3] = 182.0*RAD;
	block.DepVarWeight[3] = 64.0;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 1;
	block.DepVarClass[2] = 1;
	block.DepVarClass[3] = 2;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

bool TLIProcessor::IntegratedTLIFlyby(double C3_guess_ER, double T_c_hrs, double delta, double sigma, double lat_pc)
{
	void *constPtr;
	outarray.TLIIndicator = true;
	outarray.EllipticalCaseIndicator = false;
	outarray.sigma_TLI = sigma;
	outarray.NodeStopIndicator = false;
	outarray.LunarFlybyIndicator = false;

	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[4] = true; //C3
	block.IndVarSwitch[5] = true; //DT EPO
	block.IndVarSwitch[6] = true; //TLI plane change

	block.IndVarGuess[4] = C3_guess_ER;
	block.IndVarGuess[5] = T_c_hrs;
	block.IndVarGuess[6] = delta;

	block.IndVarStep[4] = pow(2, -21);
	block.IndVarStep[5] = pow(2, -22);
	block.IndVarStep[6] = pow(2, -21);

	block.IndVarWeight[4] = 1.0;
	block.IndVarWeight[5] = 1.0;
	block.IndVarWeight[6] = 1.0;

	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarSwitch[5] = true;
	block.DepVarSwitch[6] = true;
	block.DepVarSwitch[7] = true;
	block.DepVarLowerLimit[3] = 90.0*RAD;
	block.DepVarLowerLimit[4] = (MEDQuantities.h_PC - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[5] = lat_pc - 0.01*RAD;
	block.DepVarLowerLimit[6] = 0.0;
	block.DepVarLowerLimit[7] = (60.85*1852.0) / R_E;
	block.DepVarUpperLimit[3] = 182.0*RAD;
	block.DepVarUpperLimit[4] = (MEDQuantities.h_PC + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[5] = lat_pc + 0.01*RAD;
	block.DepVarUpperLimit[6] = 90.0*RAD;
	block.DepVarUpperLimit[7] = (70.85*1852.0) / R_E;
	block.DepVarWeight[3] = 64.0;
	block.DepVarWeight[6] = 8.0;
	block.DepVarClass[3] = 2;
	block.DepVarClass[4] = 1;
	block.DepVarClass[5] = 1;
	block.DepVarClass[6] = 2;
	block.DepVarClass[7] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

OELEMENTS TLIProcessor::LVTAR(OELEMENTS coe, double lng_PAD, double RAGL) const
{
	//lng_PAD: longitude of the launch pad
	//RAGL: Greenwich hour angle at GRR

	OELEMENTS coe2;
	double theta_e;

	theta_e = RAGL + lng_PAD;
	coe2.RA = coe.RA + PI - theta_e;

	if (coe2.RA >= PI2) coe2.RA -= PI2;
	if (coe2.RA < 0) coe2.RA += PI2;

	coe2.i = coe.i;
	coe2.e = coe.e;
	coe2.h = coe.h; //C3

	coe2.w = PI - coe.w; //alpha
	if (coe2.w < 0) coe2.w += PI2;

	coe2.TA = coe.TA;

	return coe2;
}