/****************************************************************************
This file is part of Project Apollo - NASSP

Analytic Ephemeris Generator

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

#include "AnalyticEphemerisGenerator.h"
#include "rtcc.h"

AEGDataBlock::AEGDataBlock()
{
	InputOutputInd = 11;
	ENTRY = 0;
	TIMA = 0;
	HarmonicsInd = 1;
	ICSUBD = 0.0;
	VehArea = 0.0;
	Item7 = 0.0;
	Item8 = 0.0;
	Item9 = 0.0;
	Item10 = 0.0;

	for (int i = 0; i < 6; i++)
	{
		coe_osc.data[i] = coe_mean.data[i] = 0.0;
	}

	TS = 0.0;
	l_dot = 0.0;
	g_dot = 0.0;
	h_dot = 0.0;
	TE = 0.0;
	f = 0.0;
	U = 0.0;
	R = 0.0;
}

PMMAEG::PMMAEG()
{

}

void PMMAEG::CALL(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out)
{
	AEGDataBlock tempblock;

	header.ErrorInd = 0;

	if (abs(in.TE - in.TS) > 96.0*3600.0)
	{
		goto NewPMMAEG_V846;
	}
	if (in.coe_osc.a<0.4*OrbMech::R_Earth || in.coe_osc.a>9.0*OrbMech::R_Earth)
	{
		goto NewPMMAEG_V846;
	}
	if (in.coe_osc.e<0.0 || in.coe_osc.e>0.85)
	{
		goto NewPMMAEG_V846;
	}
	if (in.coe_osc.i<0.18 || in.coe_osc.i>1.05)
	{
		goto NewPMMAEG_V846;
	}
	if (in.coe_osc.l<0.0 || in.coe_osc.l>PI2)
	{
		goto NewPMMAEG_V846;
	}
	if (in.coe_osc.g<0.0 || in.coe_osc.g>PI2)
	{
		goto NewPMMAEG_V846;
	}
	if (in.coe_osc.h<0.0 || in.coe_osc.h>PI2)
	{
		goto NewPMMAEG_V846;
	}

	if (in.TE == in.TS && in.ENTRY != 0 && in.TIMA == 0)
	{
		//Input time equals output time, we have initialized elements and time option. Nothing to do
		CurrentBlock = in;
		goto NewPMMAEG_V1030;
	}

	if (in.TIMA >= 4)
	{
		//Save a, e, i, u, t, h, r, t_f from previous block for phase lag routine
		tempblock = CurrentBlock;
	}

	//Uninitialized
	if (in.ENTRY == 0)
	{
		in.coe_mean = OrbMech::LyddaneOsculatingToMean(in.coe_osc, BODY_EARTH);

		OrbMech::BrouwerSecularRates(in.coe_osc, in.coe_mean, BODY_EARTH, in.l_dot, in.g_dot, in.h_dot);

		in.f = OrbMech::MeanToTrueAnomaly(in.coe_osc.l, in.coe_osc.e);
		in.U = in.f + in.coe_osc.g;
		if (in.U >= PI2)
		{
			in.U -= PI2;
		}
		in.R = in.coe_osc.a*(1.0 - in.coe_osc.e*in.coe_osc.e) / (1.0 + in.coe_osc.e*cos(in.coe_osc.g)*cos(in.U) + in.coe_osc.e*sin(in.coe_osc.g)*sin(in.U));
		in.ENTRY = 1;
	}

	//Initial values for final state
	CurrentBlock = in;

	double dt, theta_R;
	bool firstpass = true;

	if (in.TIMA == 0 || in.TIMA >= 4)
	{
		if (in.TIMA == 0)
		{
			dt = in.TE - in.TS;
			if (dt == 0.0)
			{
				goto NewPMMAEG_V2000;
			}
		}
		else
		{
			dt = tempblock.TE - in.TS;
		}
	NewPMMAEG_V1000:
		CurrentBlock.coe_mean.l = CurrentBlock.l_dot*dt + in.coe_mean.l;
		CurrentBlock.coe_mean.g = CurrentBlock.g_dot*dt + in.coe_mean.g;
		CurrentBlock.coe_mean.h = CurrentBlock.h_dot*dt + in.coe_mean.h;

		OrbMech::normalizeAngle(CurrentBlock.coe_mean.l);
		OrbMech::normalizeAngle(CurrentBlock.coe_mean.g);
		OrbMech::normalizeAngle(CurrentBlock.coe_mean.h);

		CurrentBlock.TE = CurrentBlock.TS = in.TS + dt;
		CurrentBlock.coe_osc = OrbMech::LyddaneMeanToOsculating(CurrentBlock.coe_mean, BODY_EARTH);
	}
	else
	{
		CurrentBlock.coe_osc = in.coe_osc;

		double L_D, DX_L, DH, X_L, X_L_dot, ddt;
		int LINE, COUNT;

		if (in.TIMA != 3)
		{
			L_D = in.Item8;
		}
		else
		{
			L_D = in.U;
		}
		DX_L = 1.0;
		DH = true;
		dt = 0.0;
		LINE = 0;
		COUNT = 24;

		do
		{
			//Mean anomaly
			if (in.TIMA == 1)
			{
				X_L = CurrentBlock.coe_osc.l;
				X_L_dot = CurrentBlock.l_dot;
			}
			//Argument of latitude
			else if (in.TIMA == 2)
			{
				double u = OrbMech::MeanToTrueAnomaly(CurrentBlock.coe_osc.l, CurrentBlock.coe_osc.e) + CurrentBlock.coe_osc.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = CurrentBlock.l_dot + CurrentBlock.g_dot;
			}
			//Maneuver line
			else
			{
				double u = OrbMech::MeanToTrueAnomaly(CurrentBlock.coe_osc.l, CurrentBlock.coe_osc.e) + CurrentBlock.coe_osc.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = CurrentBlock.l_dot + CurrentBlock.g_dot;
				LINE = 2;
			}

			if (DH)
			{
				double DN_apo = in.Item10 * PI2;
				ddt = DN_apo / CurrentBlock.l_dot;
				DH = false;

				if (LINE != 0)
				{
					L_D = L_D + CurrentBlock.g_dot * ddt + DN_apo;
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
					L_D = L_D + ddt * CurrentBlock.g_dot;
				}
			}

			dt += ddt;
			CurrentBlock.coe_mean.l = CurrentBlock.l_dot*dt + in.coe_mean.l;
			CurrentBlock.coe_mean.g = CurrentBlock.g_dot*dt + in.coe_mean.g;
			CurrentBlock.coe_mean.h = CurrentBlock.h_dot*dt + in.coe_mean.h;

			OrbMech::normalizeAngle(CurrentBlock.coe_mean.l);
			OrbMech::normalizeAngle(CurrentBlock.coe_mean.g);
			OrbMech::normalizeAngle(CurrentBlock.coe_mean.h);

			CurrentBlock.coe_osc = OrbMech::LyddaneMeanToOsculating(CurrentBlock.coe_mean, BODY_EARTH);

			COUNT--;

		} while (abs(DX_L) > 2e-4 && COUNT > 0);

		if (COUNT == 0)
		{
			header.ErrorInd = -3;
		}

		CurrentBlock.TE = CurrentBlock.TS = in.TS + dt;
	}

NewPMMAEG_V2000:
	CurrentBlock.f = OrbMech::MeanToTrueAnomaly(CurrentBlock.coe_osc.l, CurrentBlock.coe_osc.e);
	CurrentBlock.U = CurrentBlock.f + CurrentBlock.coe_osc.g;
	if (CurrentBlock.U >= PI2)
	{
		CurrentBlock.U -= PI2;
	}
	CurrentBlock.R = CurrentBlock.coe_osc.a*(1.0 - CurrentBlock.coe_osc.e*CurrentBlock.coe_osc.e) / (1.0 + CurrentBlock.coe_osc.e*cos(CurrentBlock.coe_osc.g)*cos(CurrentBlock.U) + CurrentBlock.coe_osc.e*sin(CurrentBlock.coe_osc.g)*sin(CurrentBlock.U));

	if (in.TIMA >= 4)
	{
		theta_R = CurrentBlock.U - tempblock.U - 2.0*atan(tan((CurrentBlock.coe_osc.h - tempblock.coe_osc.h) / 2.0)*(sin(0.5*(CurrentBlock.coe_osc.i + tempblock.coe_osc.i - PI)) / sin(0.5*(CurrentBlock.coe_osc.i - tempblock.coe_osc.i + PI))));
		if (theta_R < -PI)
		{
			theta_R += PI2;
		}
		else if (theta_R >= PI)
		{
			theta_R -= PI2;
		}
		if (in.TIMA == 4)
		{
			CurrentBlock.Item10 = theta_R;
		}
		else
		{
			if (firstpass)
			{
				CurrentBlock.Item10 = theta_R;
				firstpass = false;
			}
		}
	}

	if (in.TIMA >= 5)
	{
		CurrentBlock.Item8 = CurrentBlock.R - tempblock.R;
		CurrentBlock.Item9 = CurrentBlock.TE - tempblock.TE;
		dt += -theta_R / (CurrentBlock.l_dot + CurrentBlock.g_dot);
		if (abs(theta_R) > 0.00005)
		{
			goto NewPMMAEG_V1000;
		}
	}

NewPMMAEG_V1030:
	//Move output into area supplied by the calling program (already done)
	out = CurrentBlock;
NewPMMAEG_V305:
	return;
NewPMMAEG_V846:
	header.ErrorInd = -1;
	goto NewPMMAEG_V305;
}

PMMLAEG::PMMLAEG(RTCC *r) : RTCCModule(r)
{

}

void PMMLAEG::CALL(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out)
{
	AEGDataBlock tempblock;
	CELEMENTS coe_osc0, coe_osc1, coe_mean1;
	MATRIX3 Rot;
	VECTOR3 P, W;

	header.ErrorInd = 0;

	if (in.coe_osc.a < 0.2*OrbMech::R_Earth || in.coe_osc.a > 5.0*OrbMech::R_Earth)
	{
		goto NewPMMLAEG_V846;
	}
	if (in.coe_osc.e < 0.0 || in.coe_osc.e > 0.3)
	{
		goto NewPMMLAEG_V846;
	}
	if (in.coe_osc.i < 0 || in.coe_osc.i > PI)
	{
		goto NewPMMLAEG_V846;
	}
	if (in.coe_osc.l < 0.0 || in.coe_osc.l >= PI2)
	{
		goto NewPMMLAEG_V846;
	}
	if (in.coe_osc.g < 0.0 || in.coe_osc.g >= PI2)
	{
		goto NewPMMLAEG_V846;
	}
	if (in.coe_osc.h < 0.0 || in.coe_osc.h >= PI2)
	{
		goto NewPMMLAEG_V846;
	}

	if (in.TE == in.TS && in.ENTRY != 0 && in.TIMA == 0)
	{
		CurrentBlock = in;
		//Input time equals output time, we have initialized elements and time option. Nothing to do
		goto NewPMMLAEG_V1030;
	}

	if (in.TIMA >= 4)
	{
		//Save a, e, i, u, t, h, r, t_f from previous block for phase lag routine
		tempblock = CurrentBlock;
	}

	CurrentBlock = in;

	//Matrix to rotate to selenographic inertial
	if (pRTCC->PLEFEM(5, in.TS / 3600.0, 0, NULL, NULL, NULL, &Rot))
	{
		goto NewPMMLAEG_V846;
	}

	//Uninitialized
	if (in.ENTRY == 0)
	{
		//Selenocentric to selenographic
		coe_osc0 = in.coe_osc;
		pRTCC->PIVECT(in.coe_osc.i, in.coe_osc.g, in.coe_osc.h, P, W);
		P = mul(Rot, P);
		W = mul(Rot, W);
		pRTCC->PIVECT(P, W, coe_osc0.i, coe_osc0.g, coe_osc0.h);

		//Osculating to mean
		in.coe_mean = OrbMech::LyddaneOsculatingToMean(coe_osc0, BODY_MOON);

		OrbMech::BrouwerSecularRates(in.coe_osc, in.coe_mean, BODY_MOON, in.l_dot, in.g_dot, in.h_dot);
		CurrentBlock.l_dot = in.l_dot;
		CurrentBlock.g_dot = in.g_dot;
		CurrentBlock.h_dot = in.h_dot;

		in.f = OrbMech::MeanToTrueAnomaly(in.coe_osc.l, in.coe_osc.e);
		in.U = in.f + in.coe_osc.g;
		if (in.U >= PI2)
		{
			in.U -= PI2;
		}
		in.R = in.coe_osc.a*(1.0 - in.coe_osc.e*in.coe_osc.e) / (1.0 + in.coe_osc.e*cos(in.coe_osc.g)*cos(in.U) + in.coe_osc.e*sin(in.coe_osc.g)*sin(in.U));
		in.ENTRY = 1;
	}
	else
	{
		CurrentBlock.l_dot = in.l_dot;
		CurrentBlock.g_dot = in.g_dot;
		CurrentBlock.h_dot = in.h_dot;
	}

	coe_mean1 = in.coe_mean;

	double dt, theta_R;
	bool firstpass = true;

	if (in.TIMA == 0 || in.TIMA >= 4)
	{
		if (in.TIMA == 0)
		{
			dt = in.TE - in.TS;
		}
		else
		{
			dt = tempblock.TE - in.TS;
		}
	NewPMMLAEG_V1000:
		coe_mean1.l = CurrentBlock.l_dot*dt + in.coe_mean.l;
		coe_mean1.g = CurrentBlock.g_dot*dt + in.coe_mean.g;
		coe_mean1.h = CurrentBlock.h_dot*dt + in.coe_mean.h;

		OrbMech::normalizeAngle(coe_mean1.l);
		OrbMech::normalizeAngle(coe_mean1.g);
		OrbMech::normalizeAngle(coe_mean1.h);

		CurrentBlock.TE = CurrentBlock.TS = in.TS + dt;
		coe_osc1 = OrbMech::LyddaneMeanToOsculating(coe_mean1, BODY_MOON);

		//Selenographic to selenocentric
		pRTCC->PIVECT(coe_osc1.i, coe_osc1.g, coe_osc1.h, P, W);
		P = tmul(Rot, P);
		W = tmul(Rot, W);
		pRTCC->PIVECT(P, W, coe_osc1.i, coe_osc1.g, coe_osc1.h);
	}
	else
	{
		coe_osc1 = in.coe_osc;

		double L_D, DX_L, DH, X_L, X_L_dot, ddt;
		int LINE, COUNT;

		if (in.TIMA != 3)
		{
			L_D = in.Item8;
		}
		else
		{
			L_D = in.U;
		}
		DX_L = 1.0;
		DH = true;
		dt = 0.0;
		LINE = 0;
		COUNT = 24;

		do
		{
			//Mean anomaly
			if (in.TIMA == 1)
			{
				X_L = coe_osc1.l;
				X_L_dot = CurrentBlock.l_dot;
			}
			//Argument of latitude
			else if (in.TIMA == 2)
			{
				double u = OrbMech::MeanToTrueAnomaly(coe_osc1.l, coe_osc1.e) + coe_osc1.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = CurrentBlock.l_dot + CurrentBlock.g_dot;
			}
			//Maneuver line
			else
			{
				double u = OrbMech::MeanToTrueAnomaly(coe_osc1.l, coe_osc1.e) + coe_osc1.g;
				u = fmod(u, PI2);
				if (u < 0)
					u += PI2;

				X_L = u;
				X_L_dot = CurrentBlock.l_dot + CurrentBlock.g_dot;
				LINE = 2;
			}

			if (DH)
			{
				double DN_apo = in.Item10 * PI2;
				ddt = DN_apo / CurrentBlock.l_dot;
				DH = false;

				if (LINE != 0)
				{
					L_D = L_D + CurrentBlock.g_dot * ddt + DN_apo;
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
					L_D = L_D + ddt * CurrentBlock.g_dot;
				}
			}

			dt += ddt;
			coe_mean1.l = CurrentBlock.l_dot*dt + in.coe_mean.l;
			coe_mean1.g = CurrentBlock.g_dot*dt + in.coe_mean.g;
			coe_mean1.h = CurrentBlock.h_dot*dt + in.coe_mean.h;

			OrbMech::normalizeAngle(coe_mean1.l);
			OrbMech::normalizeAngle(coe_mean1.g);
			OrbMech::normalizeAngle(coe_mean1.h);

			coe_osc1 = OrbMech::LyddaneMeanToOsculating(coe_mean1, BODY_MOON);

			//Selenographic to selenocentric
			pRTCC->PIVECT(coe_osc1.i, coe_osc1.g, coe_osc1.h, P, W);
			P = tmul(Rot, P);
			W = tmul(Rot, W);
			pRTCC->PIVECT(P, W, coe_osc1.i, coe_osc1.g, coe_osc1.h);

			COUNT--;

		} while (abs(DX_L) > 2e-4 && COUNT > 0);

		if (COUNT == 0)
		{
			header.ErrorInd = -3;
		}

		CurrentBlock.TE = CurrentBlock.TS = in.TS + dt;
	}

	CurrentBlock.coe_osc = coe_osc1;
	CurrentBlock.f = OrbMech::MeanToTrueAnomaly(CurrentBlock.coe_osc.l, CurrentBlock.coe_osc.e);
	CurrentBlock.U = CurrentBlock.f + CurrentBlock.coe_osc.g;
	if (CurrentBlock.U >= PI2)
	{
		CurrentBlock.U -= PI2;
	}
	CurrentBlock.R = CurrentBlock.coe_osc.a*(1.0 - CurrentBlock.coe_osc.e*CurrentBlock.coe_osc.e) / (1.0 + CurrentBlock.coe_osc.e*cos(CurrentBlock.coe_osc.g)*cos(CurrentBlock.U) + CurrentBlock.coe_osc.e*sin(CurrentBlock.coe_osc.g)*sin(CurrentBlock.U));

	if (in.TIMA >= 4)
	{
		theta_R = CurrentBlock.U - tempblock.U - 2.0*atan(tan((CurrentBlock.coe_osc.h - tempblock.coe_osc.h) / 2.0)*(sin(0.5*(CurrentBlock.coe_osc.i + tempblock.coe_osc.i - PI)) / sin(0.5*(CurrentBlock.coe_osc.i - tempblock.coe_osc.i + PI))));
		if (theta_R < -PI)
		{
			theta_R += PI2;
		}
		else if (theta_R >= PI)
		{
			theta_R -= PI2;
		}
		if (in.TIMA == 4)
		{
			CurrentBlock.Item10 = theta_R;
		}
		else
		{
			if (firstpass)
			{
				CurrentBlock.Item10 = theta_R;
				firstpass = false;
			}
		}
	}

	if (in.TIMA >= 5)
	{
		CurrentBlock.Item8 = CurrentBlock.R - tempblock.R;
		CurrentBlock.Item9 = CurrentBlock.TE - tempblock.TE;
		dt += -theta_R / (CurrentBlock.l_dot + CurrentBlock.g_dot);
		if (abs(theta_R) > 0.00005)
		{
			goto NewPMMLAEG_V1000;
		}
	}

NewPMMLAEG_V1030:
	//Move output into area supplied by the calling program
	out.ENTRY = 0;
	out.Item7 = CurrentBlock.Item7;
	out.Item8 = CurrentBlock.Item8;
	out.Item9 = CurrentBlock.Item9;
	out.Item10 = CurrentBlock.Item10;
	out.coe_osc = CurrentBlock.coe_osc;
	out.f = CurrentBlock.f;
	out.U = CurrentBlock.U;
	out.R = CurrentBlock.R;
	out.TS = CurrentBlock.TS;
	out.TE = CurrentBlock.TE;
NewPMMLAEG_V305:
	return;
NewPMMLAEG_V846:
	header.ErrorInd = -1;
	goto NewPMMLAEG_V305;
}