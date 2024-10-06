/****************************************************************************
This file is part of Project Apollo - NASSP

Ascent Rendezvous Monitor Displays

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

#include "AscentRendezvousMonitor.h"
#include "rtcc.h"

AscentRendezvousMonitor::AscentRendezvousMonitor(RTCC *r) : RTCCModule(r)
{
	
}

int AscentRendezvousMonitor::Calc(const ARMInputs &in, ARMDisplay &out)
{
	int err;

	//Null display values
	for (int i = 0; i < 3; i++)
	{
		out.tab[i].HP = 0.0;
		out.tab[i].GETI = 0.0;
		out.tab[i].DV_Tweak = 0.0;
		out.tab[i].GET_CSI = 0.0;
		out.tab[i].DV_CSI = 0.0;
		out.tab[i].GET_CDH = 0.0;
		out.tab[i].DV_CDH = 0.0;
		out.tab[i].GET_Kick = 0.0;
		out.tab[i].DV_Kick = 0.0;
		out.tab[i].TweakRequired = true;
	}

	//RCS insertion maneuver
	err = CalcTweak(in, out);

	//Apogee kick
	ApoluneKick(in, out);

	return err;
}

int AscentRendezvousMonitor::CalcTweak(const ARMInputs &in, ARMDisplay &out)
{
	RTCC::GLUNIVInput gin;
	RTCC::GLUNIVOutput gout;
	VECTOR3 R_A_Tweak, V_A_Tweak, R_P_TPI, V_P_TPI, am, R_CC, V_CC, V_CCF, R_P_J, V_P_J, R_CFD, R_A_CSI, V_A_CSI, V_CSIF, R_A_CDH, V_A_CDH, R_A_CDHF, V_A_CDHF, R_P_CDH, V_P_CDH, R_CF, V_CF;
	double t_Tweak, t_CSI, t_CDH, dv_tweak, dv_CSI, dv_tweako, dv_CSIo, e_H1, p_H1, p_C, eps1, eps2, c_I, c_F, e_H1o, e_P, e_Po, r_apo, r_peri, dv_CDH;
	int s_F;
	bool stopcond;

	gin.SQRMU = sqrt(OrbMech::mu_Moon);

	//Input checks. If CDH indicator is negative it must be odd
	if (in.CDHIndicator == 0) return 2;
	if (in.CDHIndicator < 0)
	{
		if (-in.CDHIndicator % 2 == 0) return 2;
	}

	//Take CSM state vector to TPI time
	gin.T0 = in.sv_CSM.GMT;
	gin.R0 = in.sv_CSM.R;
	gin.V0 = in.sv_CSM.V;
	gin.Ind = 0;
	gin.PARM = in.t_TPI;

	pRTCC->GLUNIV(gin, gout);

	//GLUNIV error
	if (gout.ERR) return 1;

	//Save passive (CSM) vehicle state vector at time of TPI
	R_P_TPI = gout.R1;
	V_P_TPI = gout.V1;

	for (int i = 0; i < 3; i++)
	{
		//Is vector valid?
		if (in.sv_LM[i].RBI != BODY_MOON) continue;

		//Get LM state vector
		R_A_Tweak = in.sv_LM[i].R;
		V_A_Tweak = in.sv_LM[i].V;
		t_Tweak = in.sv_LM[i].GMT;

		//TBD: Get LM to tweak TIG?

		//Project LM state vector into plane of CSM
		am = unit(crossp(R_P_TPI, V_P_TPI));
		R_CC = unit(R_A_Tweak - am * dotp(R_A_Tweak, am))*length(R_A_Tweak);
		V_CC = unit(V_A_Tweak - am * dotp(V_A_Tweak, am))*length(V_A_Tweak);

		//Obtain TPI target position with QRDTPI
		OrbMech::QDRTPI(0, R_P_TPI, V_P_TPI, 0.0, BODY_MOON, OrbMech::mu_Moon, in.DH, in.E, 0, R_P_J, V_P_J);
		R_CFD = R_P_J - unit(R_P_J)*in.DH;

		//Initial guesses for maneuver
		dv_tweak = 0.0;
		dv_CSI = 0.0;

		//Initialize tweak iteration variables
		c_F = p_C = 0.0;
		s_F = 0;

		//Iteration tolerances
		eps1 = 2.0;
		eps2 = 0.001*RAD;

		do
		{
			//Apply tweak DV
			V_CCF = V_CC + unit(crossp(am, R_CC))*dv_tweak;
			//Take state to CSI
			gin.T0 = t_Tweak;
			gin.R0 = R_CC;
			gin.V0 = V_CCF;
			gin.Ind = 0;

			if (in.CSIFlag == 0.0)
			{
				//Apogee
				double dt = OrbMech::timetoapo(R_CC, V_CCF, OrbMech::mu_Moon, 1);
				gin.PARM = gin.T0 + dt;
			}
			else
			{
				//Time
				gin.PARM = in.t_Ins + in.CSIFlag*60.0;
			}

			pRTCC->GLUNIV(gin, gout);

			//GLUNIV error
			if (gout.ERR)
			{
				return 1;
			}

			//Save CSI state vector
			R_A_CSI = gout.R1;
			V_A_CSI = gout.V1;
			t_CSI = gout.T1;

			//Initialize CSI iteration variables
			c_I = p_H1 = 0.0;

			do
			{
				//Apply CSI DV
				V_CSIF = V_A_CSI + unit(crossp(am, R_A_CSI))*dv_CSI;

				//Take state vector to CDH

				if (in.CDHIndicator > 0)
				{
					//Apsis crossing
					gin.T0 = t_CSI;
					gin.R0 = R_A_CSI;
					gin.V0 = V_CSIF;
					gin.Ind = 4;
					gin.PARM = 0.0;
					gin.PARM3 = 1.0;

					pRTCC->GLUNIV(gin, gout);

					//GLUNIV error
					if (gout.ERR)
					{
						return 1;
					}

					R_A_CDH = gout.R1;
					V_A_CDH = gout.V1;
					t_CDH = gout.T1;
				}
				else
				{
					//N/2 revs

					double dt;
					OrbMech::REVUP(R_A_CSI, V_CSIF, 0.5*(double)(-in.CDHIndicator), OrbMech::mu_Moon, R_A_CDH, V_A_CDH, dt);
					t_CDH = t_CSI + dt;
				}

				//Get position directly above CDH
				OrbMech::RADUP(R_P_TPI, V_P_TPI, R_A_CDH, OrbMech::mu_Moon, R_P_CDH, V_P_CDH);
				//Calculate height error
				e_H1 = length(R_P_CDH) - length(R_A_CDH) - in.DH;

				stopcond = (p_H1 != 0.0 && abs(e_H1) < eps1);

				if (stopcond == false)
				{
					OrbMech::ITER(c_I, s_F, e_H1, p_H1, dv_CSI, e_H1o, dv_CSIo);
					if (s_F == 1)
					{
						return 3;
					}
				}
			} while (stopcond == false);

			//Apply CDH DV
			OrbMech::COE(R_P_CDH, V_P_CDH, in.DH, OrbMech::mu_Moon, R_A_CDHF, V_A_CDHF);
			//DV of CDH
			dv_CDH = length(V_A_CDHF - V_A_CDH);

			//Take state to TPI time
			gin.T0 = t_CDH;
			gin.R0 = R_A_CDHF;
			gin.V0 = V_A_CDHF;
			gin.Ind = 0;
			gin.PARM = in.t_TPI;

			pRTCC->GLUNIV(gin, gout);

			//GLUNIV error
			if (gout.ERR)
			{
				return 1;
			}

			R_CF = gout.R1;
			V_CF = gout.V1;

			e_P = OrbMech::sign(dotp(crossp(R_CF, R_CFD), am))*acos(dotp(unit(R_CFD), unit(R_CF)));

			stopcond = (p_C != 0.0 && abs(e_P) < eps2);

			if (stopcond == false)
			{
				OrbMech::ITER(c_F, s_F, e_P, p_C, dv_tweak, e_Po, dv_tweako);
				if (s_F == 1)
				{
					return 3;
				}
			}
		} while (stopcond == false);

		//Display data
		out.tab[i].GETI = pRTCC->GETfromGMT(t_Tweak);
		out.tab[i].DV_Tweak = dv_tweak;
		out.tab[i].GET_CSI = pRTCC->GETfromGMT(t_CSI);
		out.tab[i].DV_CSI = dv_CSI;
		out.tab[i].GET_CDH = pRTCC->GETfromGMT(t_CDH);
		out.tab[i].DV_CDH = dv_CDH;

		//Calculate perilune after tweak
		OrbMech::periapo(R_CC, V_CCF, OrbMech::mu_Moon, r_apo, r_peri);
		out.tab[i].HP = r_peri - pRTCC->BZLAND.rad[0];

		//Go/No Go calculation
		if (abs(dv_tweak) > 30.0*0.3048)
		{
			out.tab[i].TweakRequired = true;
		}
		else
		{
			out.tab[i].TweakRequired = false;
		}
	}

	return 0;
}

void AscentRendezvousMonitor::ApoluneKick(const ARMInputs &in, ARMDisplay &out)
{
	VECTOR3 R0, V0;
	double T0, r_min, r_apo, r_peri, a, v1, v2, dt;

	r_min = pRTCC->BZLAND.rad[0] + in.h_min;

	for (int i = 0; i < 3; i++)
	{
		//Is vector valid?
		if (in.sv_LM[i].RBI != BODY_MOON) continue;

		//Get LM state vector
		R0 = in.sv_LM[i].R;
		V0 = in.sv_LM[i].V;
		T0 = in.sv_LM[i].GMT;

		//Check on perilune
		OrbMech::periapo(R0, V0, OrbMech::mu_Moon, r_apo, r_peri);

		if (r_peri >= r_min) continue;

		//Time to apoapsis
		dt = OrbMech::timetoapo(R0, V0, OrbMech::mu_Moon, 1);

		//Velocity at apoapsis on current trajectory
		a = (r_apo + r_peri) / 2.0;
		v1 = sqrt(OrbMech::mu_Moon*(2.0 / r_apo - 1.0 / a));

		//Velocity at apoapsis after kick maneuver
		a = (r_apo + r_min) / 2.0;
		v2 = sqrt(OrbMech::mu_Moon*(2.0 / r_apo - 1.0 / a));
		
		out.tab[i].GET_Kick = pRTCC->GETfromGMT(T0 + dt);
		out.tab[i].DV_Kick = v2 - v1;
	}
}

ShortAscentRendezvousMonitor::ShortAscentRendezvousMonitor(RTCC *r) : RTCCModule(r)
{

}

int ShortAscentRendezvousMonitor::Calc(const ShortARMInputs &in, ShortARMDisplay &out)
{
	TwoImpulseOpt opt;
	TwoImpulseResuls res;
	EphemerisData sv_tweak_after;
	double TWEAK, TPI, r_apo, r_peri;

	//Zero output table
	for (int i = 0; i < 3; i++)
	{
		out.tab[i].HP = 0.0;
		out.tab[i].GETI = 0.0;
		out.tab[i].DV_B = _V(0, 0, 0);
		out.tab[i].Att = _V(0, 0, 0);
		out.tab[i].GETTPI = 0.0;
		out.tab[i].DV_TPI = 0.0;
		out.tab[i].Y_H = 0.0;
		out.tab[i].P_H = 0.0;
		out.tab[i].DoBailout = true;
	}

	opt.mode = 5;

	for (int i = 0; i < 3; i++)
	{
		//Is vector valid?
		if (in.sv_LM[i].RBI != BODY_MOON) continue;

		//Tweak TIG
		if (in.ITWEAK)
		{
			TWEAK = in.t_tweak;
		}
		else
		{
			TWEAK = in.t_Ins + in.DT;
		}
		//TPI TIG
		if (in.ITPI)
		{
			TPI = in.t_TPI;
		}
		else
		{
			TPI = TWEAK + in.DTPI;
		}

		//Compute two-impulse solution to TPI offsets
		opt.T1 = TWEAK;
		opt.T2 = TPI;
		opt.sv_A = in.sv_LM[i];
		opt.sv_P = in.sv_CSM;
		opt.DH = in.DH;
		opt.PhaseAngle = in.DTHETA;

		pRTCC->PMSTICN(opt, res);

		if (res.SolutionFound == false) return 1;

		out.tab[i].GETI = pRTCC->GETfromGMT(TWEAK);

		//Apply DV
		sv_tweak_after = res.sv_tig;
		sv_tweak_after.V += res.dV;

		//Calculate perilune after tweak
		OrbMech::periapo(sv_tweak_after.R, sv_tweak_after.V, OrbMech::mu_Moon, r_apo, r_peri);
		out.tab[i].HP = r_peri - pRTCC->BZLAND.rad[0];

		//Display coordinate system
		if (in.IREF)
		{
			//Rotate DV from LVLH to body using AXHOR and computed angle between LVLH and horizon

			MATRIX3 Mat;
			double rho;

			rho = in.Axhor - acos(pRTCC->BZLAND.rad[0] / length(res.sv_tig.R));
			Mat = OrbMech::_MRy(rho);
			out.tab[i].DV_B = mul(Mat, res.dV_LVLH);
		}
		else
		{
			MATRIX3 Mat;
			VECTOR3 DV_P;

			//MCI to platform
			DV_P = mul(in.REFSMMAT, res.dV);
			//Platform to body
			Mat = OrbMech::CALCSMSC(in.IMUAngles);
			out.tab[i].DV_B = mul(Mat, DV_P);
			out.tab[i].Att = in.IMUAngles;
		}

		//Compute two-impulse solution for TPI
		opt.T1 = TPI;
		opt.T2 = -1.0;
		opt.sv_A = in.sv_LM[i];
		opt.sv_P = in.sv_CSM;
		opt.DH = 0.0;
		opt.PhaseAngle = 0.0;
		opt.WT = in.WT;
		pRTCC->PMSTICN(opt, res);

		if (res.SolutionFound == false) return 1;

		out.tab[i].GETTPI = res.T1;
		out.tab[i].DV_TPI = length(res.dV);
		out.tab[i].Y_H = atan2(res.dV_LVLH.y, res.dV_LVLH.x);
		out.tab[i].P_H = -res.dV_LVLH.z / sqrt(res.dV_LVLH.x*res.dV_LVLH.x + res.dV_LVLH.y*res.dV_LVLH.y);

		//Mission rule: bailout required if tweak DV is greater than 60 ft/s or if HP after the tweak would be below 5 NM altitude
		if (length(out.tab[i].DV_B) > 60.0*0.3048 || out.tab[i].HP < 5.0*1852.0)
		{
			out.tab[i].DoBailout = true;
		}
		else
		{
			out.tab[i].DoBailout = false;
		}
	}

	return 0;
}