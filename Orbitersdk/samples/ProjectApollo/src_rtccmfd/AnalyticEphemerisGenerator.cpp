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
	if (in.coe_osc.i < 0.0 || in.coe_osc.i > PI)
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

		double L_D, DX_L, X_L, X_L_dot, ddt;
		int LINE, COUNT;
		bool DH;

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

				if (DH)
				{
					//First iteration
					X_L_dot = CurrentBlock.l_dot + CurrentBlock.g_dot;
				}
				else
				{
					//Later iterations
					VECTOR3 R, V;
					OrbMech::GIMKIC(CurrentBlock.coe_osc, OrbMech::mu_Earth, R, V);
					X_L_dot = OrbMech::GetTrueMotion(R, V, OrbMech::mu_Earth) + CurrentBlock.g_dot;
				}
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

		double L_D, DX_L, X_L, X_L_dot, ddt;
		int LINE, COUNT;
		bool DH;

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
				if (DH)
				{
					//First iteration
					X_L_dot = CurrentBlock.l_dot + CurrentBlock.g_dot;
				}
				else
				{
					//Later iterations
					VECTOR3 R, V;
					OrbMech::GIMKIC(CurrentBlock.coe_osc, OrbMech::mu_Moon, R, V);
					X_L_dot = OrbMech::GetTrueMotion(R, V, OrbMech::mu_Moon) + CurrentBlock.g_dot;
				}
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

namespace AnalyticEphemerisGenerator
{
	int coast(RTCC* r, VehicleDataBlock sv0, double dt, VehicleDataBlock& sv1)
	{
		//TBD: Use RTCC subroutine EMMENI for now. In the future a new, slightly simplified integrator should be implemented for AEG-like integration.
		EMMENIInputTable in;
		std::string modulename;
		bool bDrag;

		if (sv0.sv.RBI == 0) modulename = "PMMAEG";
		else modulename = "PMMLAEG";

		//Don't process more than 4 days
		if (abs(dt) > 4.0 * 24.0 * 3600.0)
		{
			//Error: The amount of time to update elements exceeds four days
			r->PMXSPT(modulename, 80);
			return -1;
		}
		//Also limit eccentricity. TBD: And semi-major axis?
		CELEMENTS elem = OrbMech::GIMIKC(sv0.sv.R, sv0.sv.V, sv0.sv.RBI == BODY_EARTH ? OrbMech::mu_Earth : OrbMech::mu_Moon);
		if (elem.e > 0.85)
		{
			//Error: Input elements were rejected because the elements did not fall within the limits
			r->PMXSPT(modulename, 80);
			return -1;
		}

		in.AnchorVector = sv0.sv;
		in.Area = sv0.Area;
		in.Weight = sv0.Weight;
		in.DensityMultiplier = sv0.KFactor;
		in.MaxIntegTime = abs(dt);
		if (dt >= 0.0)
		{
			in.IsForwardIntegration = 1.0;
		}
		else
		{
			in.IsForwardIntegration = -1.0;
		}
		bDrag = sv0.sv.RBI == BODY_EARTH && sv0.KFactor > 0.0;
		if (bDrag)
		{
			//With drag
			in.CutoffIndicator = 3; //Altitude
			in.EarthRelStopParam = 50.0*1852.0; //50 NM limit
			in.StopParamRefFrame = 0; //Only for Earth
		}
		else
		{
			//Without drag
			in.CutoffIndicator = 1;
		}
		//Do the integration
		r->EMMENI(in);

		if (in.TerminationCode == 3)
		{
			//Error: terminated on altitude (reentered)
			r->PMXSPT(modulename, 82);
			return -2;
		}
		//Write output
		sv1 = sv0;
		sv1.sv = in.sv_cutoff;

		return 0;
	}

	double ArgumentOfLatitude(EphemerisData sv0)
	{
		//Argument of latitude from R and V

		VECTOR3 K, h, n;
		double u;

		K = _V(0, 0, 1);
		h = unit(crossp(sv0.R, sv0.V));
		n = crossp(K, h);
		u = acos2(dotp(unit(n), unit(sv0.R)));
		if (sv0.R.z < 0)
		{
			u = PI2 - u;
		}
		return u;
	}

	void SecularRates(RTCC* r, EphemerisData sv0, double& l_dot, double& g_dot, double& h_dot)
	{
		EphemerisData sv_true;
		double mu;
		int err;
		//Convert from inertial to true coordinates
		err = r->ELVCNV(sv0, sv0.RBI == BODY_EARTH ? 1 : 3, sv_true);
		if (err) sv_true = sv0; //TBD: Process error? Will cause failure in other places anyway

		//Calculate secular rates with osculating elements. For g_dot and h_dot the mean orbital elements should be used, but the difference is small
		mu = sv0.RBI == BODY_EARTH ? OrbMech::mu_Earth : OrbMech::mu_Moon;
		//Calculate osculating elements
		CELEMENTS coe = OrbMech::GIMIKC(sv_true.R, sv_true.V, mu);
		//Calculate secular rates
		OrbMech::BrouwerSecularRates(coe, coe, sv0.RBI, l_dot, g_dot, h_dot);
	}

	int TimeOfArrivalRoutine(RTCC* r, VehicleDataBlock sv0, int opt, double param, double DN, VehicleDataBlock& sv1)
	{
		//opt: 0 = time, 1 = mean anomaly, 2 = argument of latitude, 3 = maneuver line

		//Time option
		if (opt == 0)
		{
			//param is GMT
			return coast(r, sv0, param - sv0.sv.GMT, sv1);
		}

		//Other options
		CELEMENTS coe_osc;
		double L_D, DX_L, X_L, X_L_dot, ddt, l_dot, g_dot, h_dot, mu, u;
		int LINE, COUNT, err;
		bool DH;

		if (sv0.sv.RBI == BODY_EARTH)
		{
			mu = OrbMech::mu_Earth;
		}
		else
		{
			mu = OrbMech::mu_Moon;
		}
		if (opt != 3)
		{
			L_D = param;
		}
		else
		{
			L_D = ArgumentOfLatitude(sv0.sv);
		}
		DX_L = 1.0;
		DH = true;
		LINE = 0;
		COUNT = 24;
		sv1 = sv0;

		//Calculate secular rates
		SecularRates(r, sv0.sv, l_dot, g_dot, h_dot);

		do
		{
			//Calculate osculating elements
			coe_osc = OrbMech::GIMIKC(sv1.sv.R, sv1.sv.V, mu);
			u = ArgumentOfLatitude(sv1.sv);

			//Mean anomaly
			if (opt == 1)
			{
				X_L = coe_osc.l;
				X_L_dot = l_dot;
			}
			//Argument of latitude
			else if (opt == 2)
			{
				X_L = u;
				if (DH)
				{
					//First iteration
					X_L_dot = l_dot + g_dot;
				}
				else
				{
					//Later iterations
					X_L_dot = OrbMech::GetTrueMotion(sv1.sv.R, sv1.sv.V, mu) + g_dot;
				}
			}
			//Maneuver line
			else
			{
				X_L = u;
				X_L_dot = l_dot + g_dot;
				LINE = 2;
			}

			if (DH)
			{
				double DN_apo = DN * PI2;
				ddt = DN_apo / l_dot;
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

			//Update to new time
			if (err = coast(r, sv1, ddt, sv1))
			{
				//AEG error
				return err;
			}
			//Decrement iteration counter
			COUNT--;

		} while (abs(DX_L) > 1e-4 && COUNT > 0);

		if (COUNT == 0)
		{
			//Failed to converge
			return -3;
		}
		return 0;
	}

	int TAUA(RTCC* r, VehicleDataBlock sv_A0, VehicleDataBlock sv_P0, VehicleDataBlock& sv_P1, double& DELH, double& TA)
	{
		int err, n, nmax;

		//Update passive vehicle to time of sv_A0
		if (err = coast(r, sv_P0, sv_A0.sv.GMT - sv_P0.sv.GMT, sv_P1)) return err;

		double theta, tol, l_dot, g_dot, h_dot, dt;

		tol = 0.00001;
		DELH = TA = 0.0;
		n = 0;
		nmax = 10;

		//Calculate phase angle
		theta = OrbMech::PHSANG(sv_A0.sv.R, sv_A0.sv.V, sv_P1.sv.R);
		//Secular rates of passive vehicle
		SecularRates(r, sv_P1.sv, l_dot, g_dot, h_dot);

		while (n < nmax && abs(theta) > tol)
		{
			//Take to time
			dt = theta / (l_dot + g_dot);
			if (err = coast(r, sv_P1, dt, sv_P1)) return err;
			//Calculate phase angle
			theta = OrbMech::PHSANG(sv_A0.sv.R, sv_A0.sv.V, sv_P1.sv.R);
			n++;
		}
		if (n >= nmax) return 1;
		//Calculate time lag. Positive if active vehicle is behind.
		TA = sv_A0.sv.GMT - sv_P1.sv.GMT;
		//Calculate DH. Positive if active vehicle is below.
		DELH = length(sv_P1.sv.R) - length(sv_A0.sv.R);
		return 0;
	}

	int QDRTPI(RTCC* r, VehicleDataBlock sv_P0, double DH, double E_L, VehicleDataBlock& sv_P1)
	{
		//QDRTPI determines the passive vehicle state vector radially above the active vehicle at TPI defined by DH and E_L
		sv_P1 = sv_P0;

		int s_F, err;
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
			if (t != 0.0)
			{
				err = coast(r, sv_P0, t, sv_P1);
				if (err) return err;
			}

			e_T = PI05 - E_L - asin(((length(sv_P1.sv.R) - DH) * cos(E_L) / length(sv_P0.sv.R))) - acos2(dotp(unit(sv_P0.sv.R), unit(sv_P1.sv.R))) * OrbMech::sign(dotp(crossp(sv_P1.sv.R, sv_P0.sv.R), crossp(sv_P0.sv.R, sv_P0.sv.V)));

			if (abs(e_T) >= eps1)
			{
				OrbMech::ITER(c, s_F, e_T, p, t, e_To, to);
				if (s_F == 1)
				{
					return 1;
				}
			}

		} while (abs(e_T) >= eps1);
		return 0;
	}

	int PMMDAN(RTCC* rtcc, VehicleDataBlock ELM, int IND, double &T_c, double &T_c_apo)
	{
		//INPUTS:
		//ELM: Input vehicle data block
		//IND: Number of desired predictions (1 or 2)
		//OUTPUTS:
		//ERR: <0 = AEG error return, 0 = no error, 1 = orbit is totally in daylight, 2 = convergence failure
		//T_c and T_c_apo: Times of two next environment changes. A positive time indicate an upcoming environment of daylight and a negative time for darkness.
		VehicleDataBlock sv_temp;
		VECTOR3 R_EM, V_EM, R_ES, R_S, H, N, N_apo;
		double r_S, mu, cos_theta, R_e, r, phi1, phi2, phi3, n, cos_phi1, sin_alpha, h, cos_eta, sin_eta, F, dt, S_T, l_dot, g_dot, h_dot;
		int J, I_c, AEGERR;
		bool daylight;

		J = 0;
		//Get sun vector
		rtcc->PLEFEM(1, ELM.sv.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);

		if (ELM.sv.RBI == BODY_EARTH)
		{
			R_S = R_ES;
			mu = OrbMech::mu_Earth;
			R_e = OrbMech::R_Earth;
		}
		else
		{
			R_S = R_ES - R_EM;
			mu = OrbMech::mu_Moon;
			R_e = rtcc->BZLAND.rad[RTCC_LMPOS_BEST];
		}

		//Initialize
		sv_temp = ELM;
		SecularRates(rtcc, ELM.sv, l_dot, g_dot, h_dot);

		r = length(sv_temp.sv.R);
		r_S = length(R_ES);
		cos_theta = dotp(sv_temp.sv.R, R_S) / r / r_S;
		if (cos_theta >= 0)
		{
			//Vehicle is in daylight
			daylight = true;
		}
		else if (r*sqrt(1.0 - cos_theta * cos_theta) >= R_e)
		{
			//Vehicle is in daylight
			daylight = true;
		}
		else
		{
			//Vehicle is in darkness
			daylight = false;
		}
		I_c = 0;
		T_c = sv_temp.sv.GMT;
	RTCC_PMMDAN_2_2:
		if (I_c > 0)
		{
			r = length(sv_temp.sv.R);
		}
		H = crossp(sv_temp.sv.R, sv_temp.sv.V);
		N = crossp(R_S, H);
		n = length(N);
		if (daylight)
		{
			n = -n;
		}
		N_apo = N / n;
		cos_phi1 = dotp(sv_temp.sv.R, N_apo) / r;
		phi1 = acos(cos_phi1);
		sin_alpha = sqrt(1.0 - pow(R_e / r, 2));
		h = length(H);
		cos_eta = dotp(H, R_S) / h / r_S;
		sin_eta = sqrt(1.0 - cos_eta * cos_eta);
		if (sin_eta <= sin_alpha)
		{
			//Orbit is totally in daylight
			T_c = 0.0;
			T_c_apo = 0.0;
			return 1;
		}
		phi2 = asin(sin_alpha / sin_eta);
		//F is the z-component of RxN_apo. This code assumes the orbit is prograde for Earth, retrograde for Moon. TBD: Should maybe be generalized to not depend on that.
		F = sv_temp.sv.R.x*N_apo.y - sv_temp.sv.R.y*N_apo.x;
		if (sv_temp.sv.RBI == BODY_MOON)
		{
			F = -F;
		}
		//Quadrant test
		if (daylight)
		{
			if (F >= 0)
			{
				phi3 = phi1 + phi2;
			}
			else
			{
				if (cos_phi1 > 0)
				{
					phi3 = phi2 - phi1;
				}
				else
				{
					phi3 = PI2 - phi1 + phi2;
				}
			}
		}
		else
		{
			if (F > 0)
			{
				phi3 = phi1 - phi2;
			}
			else
			{
				phi3 = -phi1 - phi2;
			}
		}
		dt = phi3 / (l_dot + g_dot);
		T_c = T_c + dt;
		if (abs(dt) > 0.00055*3600.0)
		{
			if (I_c >= 4)
			{
				//Convergence failure
				T_c = 0.0;
				T_c_apo = 0.0;
				return 2;
			}
			I_c++;
		RTCC_PMMDAN_2_4:
			//Update to time
			AEGERR = coast(rtcc, sv_temp, dt, sv_temp);
			//Error?
			if (AEGERR)
			{
				T_c = 0.0;
				T_c_apo = 0.0;
				return AEGERR;
			}
			goto RTCC_PMMDAN_2_2; //No
		}
		if (daylight)
		{
			T_c = -T_c;
		}
		if (IND == 1)
		{
			T_c_apo = 0.0;
			return 0;
		}
		if (J <= 0)
		{
			S_T = T_c;
			daylight = !daylight;
			if (daylight == false)
			{
				phi2 = -phi2;
			}
			phi3 = PI + 2.0*phi2;
			dt = phi3 / (l_dot + g_dot);
			T_c = abs(T_c) + dt;
			J = 1;
			I_c = 1;
			goto RTCC_PMMDAN_2_4;
		}
		else
		{
			T_c_apo = T_c;
			T_c = S_T;
		}
		return 0;
	}

	int PMMTLC(RTCC* rtcc, VehicleDataBlock AEGIN, double DESLAM, VehicleDataBlock &AEGOUT)
	{
		//INPUTS:
		//AEGIN: Initial vehicle data block
		//DESLAM: Desired longitude in radians
		//OUTPUTS:
		//AEGOUT: Vehicle data block updated to time of the desired longitude crossing
		//K (Error): -1 = Unrecoverable AEG error, >0 = failure to converge, 0 = no error

		CELEMENTS elem;
		EphemerisData sv_true;
		double i_CB, g_CB, h_CB, u_CB, Z, DELTA, lambda_V, w_CB, dlambda, DELTADOT, dt, u_CB_dot, mu_CB, l_dot, g_dot, h_dot;
		int coord_true, K, AEGERR;

		//Normalize input longitude
		OrbMech::normalizeAngle(DESLAM, true);

		if (AEGIN.sv.RBI == BODY_EARTH)
		{
			w_CB = OrbMech::w_Earth;
			mu_CB = OrbMech::mu_Earth;
			coord_true = 1;
			Z = 1.0;
		}
		else
		{
			w_CB = OrbMech::w_Moon;
			mu_CB = OrbMech::mu_Moon;
			coord_true = 3;
			Z = 0.0;
		}
		K = 1;

		//Move input to output
		AEGOUT = AEGIN;
		do
		{
			//Convert from inertial to true coordinates
			if (rtcc->ELVCNV(AEGOUT.sv, coord_true, sv_true))
			{
				//Conversion failure
				return -1;
			}
			//Calculate elements
			elem = OrbMech::GIMIKC(sv_true.R, sv_true.V, mu_CB);
			//Save elements
			i_CB = elem.i;
			g_CB = elem.g;
			h_CB = elem.h;
			u_CB = ArgumentOfLatitude(sv_true);
			//Calculate secular rates
			SecularRates(rtcc, AEGOUT.sv, l_dot, g_dot, h_dot);

			DELTA = atan2(sin(u_CB)*cos(i_CB), cos(u_CB));
			if (DELTA < 0)
			{
				DELTA += PI2;
			}
			lambda_V = h_CB + DELTA - Z * (w_CB * AEGOUT.sv.GMT);
			lambda_V = fmod(lambda_V, PI2);
			if (lambda_V < 0)
			{
				lambda_V += PI2;
			}
			dlambda = DESLAM - lambda_V;
			if (K <= 1)
			{
				DELTADOT = l_dot + g_dot;
				if (i_CB > PI05)
				{
					DELTADOT = -DELTADOT;
					if (dlambda > 0)
					{
						dlambda = dlambda - PI2;
					}
				}
				else
				{
					if (dlambda < 0)
					{
						dlambda = dlambda + PI2;
					}
				}
			}
			else
			{
				u_CB_dot = sqrt(mu_CB*elem.a*(1.0 - pow(elem.e, 2))) / pow(length(AEGOUT.sv.R), 2) + g_dot;
				DELTADOT = cos(i_CB)*u_CB_dot / (pow(cos(u_CB), 2) + pow(sin(u_CB), 2)*pow(cos(i_CB), 2));
				if (abs(dlambda) > PI)
				{
					if (dlambda > 0)
					{
						dlambda = dlambda - PI2;
					}
					else
					{
						dlambda = dlambda + PI2;
					}
				}
			}
			dt = dlambda / (h_dot + DELTADOT - w_CB);
			if (abs(dt) <= 0.01)
			{
				break;
			}
			if (K > 5)
			{
				//Failed to converge
				return K;
			}
			K++;

			if (AEGERR = coast(rtcc, AEGOUT, dt, AEGOUT))
			{
				//Unrecoverable AEG error
				return AEGERR;
			}
		} while (K < 6);
		//Converged
		return 0;
	}

	//Coelliptic maneuver calculation
	void PCMCEM(VehicleDataBlock sv_A0, VehicleDataBlock sv_PC, double DH, double mu, double& DV_H, double& DV_R)
	{
		//sv_A0 at CDH TIG, sv_PC at position match
		double V_Cb, V_CRb, gamma_C, V_CHb, a_T, a_C, r_T_dot, r_C_dot, V_C_apo, gamma_C_apo, V_CHa;

		V_Cb = length(sv_A0.sv.V);
		V_CRb = dotp(sv_A0.sv.R, sv_A0.sv.V) / length(sv_A0.sv.R);
		gamma_C = asin(V_CRb / V_Cb);
		V_CHb = V_Cb * cos(gamma_C);
		a_T = 1.0 / (2.0 / length(sv_PC.sv.R) - dotp(sv_PC.sv.V, sv_PC.sv.V) / mu);
		a_C = a_T - DH;
		r_T_dot = dotp(sv_PC.sv.R, sv_PC.sv.V) / length(sv_PC.sv.R);
		r_C_dot = r_T_dot * pow(a_T / a_C, 1.5);
		//Velocity after the maneuver
		V_C_apo = sqrt(mu * (2.0 / length(sv_A0.sv.R) - 1.0 / a_C));
		//Flight path angle after maneuver
		gamma_C_apo = asin(r_C_dot / V_C_apo);
		//Horizontal velocity after maneuver
		V_CHa = V_C_apo * cos(gamma_C_apo);
		DV_H = V_CHa - V_CHb;
		DV_R = V_CRb - r_C_dot;
	}
}