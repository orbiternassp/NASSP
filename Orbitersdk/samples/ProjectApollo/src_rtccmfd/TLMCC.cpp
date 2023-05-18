/****************************************************************************
This file is part of Project Apollo - NASSP

Translunar Midcourse Correction Processor

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

#include "OrbMech.h"
#include "GeneralizedIterator.h"
#include "rtcc.h"
#include "TLMCC.h"

TLMCCProcessor::TLMCCProcessor(RTCC *r) : TLTrajectoryComputers(r)
{

}

void TLMCCProcessor::Init(TLMCCDataTable data, TLMCCMEDQuantities med, TLMCCMissionConstants cst, double GMTBase)
{
	TLTrajectoryComputers::Init(cst, GMTBase);

	DataTable = data;
	MEDQuantities = med;

	//Copy constants to array
	outarray.lat_lls = DataTable.lat_lls;
	outarray.lng_lls = DataTable.lng_lls;
	outarray.rad_lls = DataTable.rad_lls;
	outarray.psi_lls = DataTable.psi_lls;
	outarray.Config = med.Config;
	outarray.useSPS = med.useSPS;
	outarray.TA_LOI = med.TA_LOI;
	outarray.LMMass = med.LMMass;
	outarray.site_rotation_LPO2 = med.site_rotation_LPO2;
	outarray.Revs_LPO1 = med.Revs_LPO1;
	outarray.Revs_LPO2 = med.Revs_LPO2;
	outarray.H_A_LPO1 = med.H_A_LPO1;
	outarray.H_P_LPO1 = med.H_P_LPO1;
	outarray.H_A_LPO2 = med.H_A_LPO2;
	outarray.H_P_LPO2 = med.H_P_LPO2;
	outarray.dh_bias = med.dh_bias;

	outarray.sv_lls1.RBI = BODY_MOON;
	outarray.sv_loi.RBI = BODY_MOON;
	outarray.SGSLOI.RBI = BODY_MOON;
	outarray.sv_lls2.RBI = BODY_MOON;
}

void TLMCCProcessor::Main(TLMCCOutputData &out)
{
	double Wdot;

	//Propagate state vector to time of ignition
	int ITS;
	pRTCC->PMMCEN(MEDQuantities.sv0, 0.0, 10.0*24.0*3600.0, 1, MEDQuantities.T_MCC - MEDQuantities.sv0.GMT, 1.0, sv_MCC, ITS);
	sv_MCC_SOI = sv_MCC;

	//SOI check
	VECTOR3 R_EM, V_EM, R_ES, R2;
	double r1, r2, Ratio;
	EPHEM(sv_MCC.GMT, R_EM, V_EM, R_ES);
	if (sv_MCC.RBI == BODY_EARTH)
	{
		R2 = sv_MCC.R - R_EM;
	}
	else
	{
		R2 = sv_MCC.R + R_EM;
	}
	r1 = length(sv_MCC.R);
	r2 = length(R2);
	Ratio = r2 / r1;
	if (sv_MCC.RBI == BODY_EARTH)
	{
		if (Ratio < 0.275)
		{
			pRTCC->ELVCNV(sv_MCC, 2, sv_MCC);
			sv_MCC.RBI = BODY_MOON;
		}
	}
	else
	{
		if (Ratio < 1.0 / 0.275)
		{
			pRTCC->ELVCNV(sv_MCC, 0, sv_MCC);
			sv_MCC.RBI = BODY_EARTH;
		}
	}

	if (sv_MCC.RBI == BODY_EARTH)
	{
		outarray.KREF_MCC = 1;
	}
	else
	{
		outarray.KREF_MCC = 2;
	}
	if (MEDQuantities.Config)
	{
		outarray.M_i = MEDQuantities.CSMMass + MEDQuantities.LMMass;
	}
	else
	{
		outarray.M_i = MEDQuantities.CSMMass;
	}
	if (!MEDQuantities.Config || MEDQuantities.useSPS)
	{
		isp_MCC = isp_SPS;
		Wdot = 29.60667013;
	}
	else
	{
		isp_MCC = isp_DPS;
		Wdot = 14.63626597;
	}

	outtab = DataTable;
	outtab.mode = MEDQuantities.Mode;

	switch (MEDQuantities.Mode)
	{
	case 1:
		Option1();
		break;
	case 2:
		Option2();
		break;
	case 3:
		Option3();
		break;
	case 4:
		Option4();
		break;
	case 5:
		Option5();
		break;
	case 6:
		Option6();
		break;
	case 7:
		Option7();
		break;
	case 8:
		Option8();
		break;
	case 9:
		if (MEDQuantities.INCL_fr == 0.0)
		{
			Option9A();
		}
		else
		{
			Option9B();
		}
		break;
	}

	VECTOR3 DV_LOI;

	if (MEDQuantities.Mode <= 5)
	{
		EphemerisData sv_MCC_apo;
		sv_MCC_apo = sv_MCC_SOI;
		sv_MCC_apo.V += DV_MCC;
		DV_LOI = CalcLOIDV(sv_MCC_apo, outarray.gamma_nd);
	}
	//Calc MPT parameters
	out.R_MCC = sv_MCC_SOI.R;
	out.V_MCC = sv_MCC_SOI.V;
	out.GMT_MCC = sv_MCC_SOI.GMT;
	out.RBI = sv_MCC_SOI.RBI;

	out.V_MCC_apo = sv_MCC_SOI.V + DV_MCC;

	//Calc display quantities
	out.display.Mode = MEDQuantities.Mode;
	if (out.display.Mode == 1)
	{
		out.display.Return = 0;
	}
	else if (out.display.Mode == 4 || out.display.Mode == 5)
	{
		out.display.Return = 1;
	}
	else
	{
		out.display.Return = 2;
	}
	if (out.display.Mode >= 2 && out.display.Mode <= 5)
	{
		if (out.display.Mode == 2 || out.display.Mode == 4)
		{
			out.display.AZ_min = DataTable.psi_lls;
			out.display.AZ_max = DataTable.psi_lls;
		}
		else
		{
			out.display.AZ_min = MEDQuantities.AZ_min;
			out.display.AZ_max = MEDQuantities.AZ_max;
		}
	}
	else
	{
		out.display.AZ_max = 0.0;
		out.display.AZ_min = 0.0;
	}
	out.display.Config = MEDQuantities.Config;
	double dv = length(DV_MCC);
	double T_start = -outarray.M_i / Wdot * (1.0 + (exp(-dv / isp_MCC) - 1.0) / (dv / isp_MCC));
	out.display.GET_MCC = pRTCC->GETfromGMT(sv_MCC_SOI.GMT) + T_start;
	out.display.DV_MCC = DV_MCC;
	out.display.h_PC = outarray.h_pl;
	
	if (out.display.Mode <= 5)
	{
		dv = length(DV_LOI);
		T_start = -outarray.M_mcc / Wdot * (1.0 + (exp(-dv / isp_MCC) - 1.0) / (dv / isp_MCC));
		out.display.GET_LOI = pRTCC->GETfromGMT(outarray.GMT_nd) + T_start;
		out.display.DV_LOI = DV_LOI;
	}
	else
	{
		out.display.GET_LOI = 0.0;
		out.display.DV_LOI = _V(0, 0, 0);
	}
	out.display.AZ_act = outarray.AZ_act;
	out.display.H_bo = 0.0;//???
	if (out.display.Mode >= 2 && out.display.Mode <= 5)
	{
		out.display.delta_lat = 0.0; //TBD
		dv = length(outarray.DV_TEI);
		T_start = -outarray.M_lopc / Wdot * (1.0 + (exp(-dv / isp_MCC) - 1.0) / (dv / isp_MCC));
		out.display.GET_TEI = pRTCC->GETfromGMT(outarray.GMT_tei) + T_start;
		out.display.DV_TEI = outarray.DV_TEI;
		out.display.DV_LOPC = outarray.DV_LOPC;
	}
	else
	{
		out.display.delta_lat = 0.0;
		out.display.HA_LPO = 0.0;
		out.display.HP_LPO = 0.0;
		out.display.DV_LOPC = _V(0, 0, 0);
		out.display.GET_TEI = 0.0;
		out.display.DV_TEI = _V(0, 0, 0);
		out.display.DV_REM = 0.0;
	}
	if (out.display.Mode == 1)
	{
		out.display.GET_LC = 0.0;
		out.display.lat_IP = 0.0;
		out.display.lng_IP = 0.0;
		out.display.v_EI = 0.0;
		out.display.gamma_EI = 0.0;
	}
	else
	{
		if (out.display.Mode >= 6)
		{
			out.display.GET_LC = pRTCC->GETfromGMT(outarray.GMT_ip);
			out.display.lat_IP = outarray.lat_ip;
			out.display.lng_IP = outarray.lng_ip;
			out.display.v_EI = outarray.v_EI;
		}
		else
		{
			out.display.GET_LC = pRTCC->GETfromGMT(outarray.GMT_ip_pr);
			out.display.lat_IP = outarray.lat_ip_pr;
			out.display.lng_IP = outarray.lng_ip_pr;
			out.display.v_EI = outarray.v_EI_pr;
		}
		out.display.gamma_EI = gamma_reentry;
	}
	if (out.display.Mode == 2 || out.display.Mode == 3 || out.display.Mode >= 6)
	{
		out.display.incl_fr = outarray.incl_fr;
	}
	else
	{
		out.display.incl_fr = 0.0;
	}
	if (out.display.Mode >= 2 && out.display.Mode <= 5)
	{
		out.display.incl_pr = outarray.incl_pr;
	}
	else
	{
		out.display.incl_pr = 0.0;
	}
	out.display.GMTV = MEDQuantities.sv0.GMT;
	out.display.GETV = pRTCC->GETfromGMT(MEDQuantities.sv0.GMT);
	out.display.CSMWT = MEDQuantities.CSMMass;
	out.display.LMWT = MEDQuantities.LMMass;

	VECTOR3 X_PHV, Y_PHV, Z_PHV;
	Z_PHV = unit(-sv_MCC_SOI.R);
	Y_PHV = unit(crossp(sv_MCC_SOI.V, sv_MCC_SOI.R));
	X_PHV = crossp(Y_PHV, Z_PHV);
	out.display.YAW_MCC = atan(dotp(Y_PHV, unit(DV_MCC)) / dotp(X_PHV, unit(DV_MCC)));

	out.outtab = outtab;
}

VECTOR3 TLMCCProcessor::CalcLOIDV(EphemerisData sv_MCC_apo, double gamma_nd)
{
	EphemerisData sv_nd;
	VECTOR3 U1, U2, RF, VF;
	double mfm0;
	int ITS;

	pRTCC->PMMCEN(sv_MCC_apo, 0.0, 10.0*24.0*3600.0, 2, sin(gamma_nd), 1.0, sv_nd, ITS);

	VECTOR3 h1 = unit(crossp(sv_nd.R, sv_nd.V));
	double R = 1.0;
	double V = 1.0;
	double gamma = 0.0;
	RVIO(false, U1, U2, R, V, DataTable.lng_lls, DataTable.lat_lls, gamma, outarray.AZ_act);
	LIBRAT(U1, U2, sv_nd.GMT + DataTable.dt_lls, 6);
	double dpsi_loi = acos(dotp(h1, crossp(U1, U2)));
	double dv_loi = MissionConstants.V_pcynlo - length(sv_nd.V);
	outarray.dpsi_loi = dpsi_loi;

	BURN(sv_nd.R, sv_nd.V, dv_loi, -gamma_nd, dpsi_loi, isp_MCC, mfm0, RF, VF);
	return VF - sv_nd.V;
}

void TLMCCProcessor::Option1()
{
	//Empirical first guess
	double V, R, lng, lat, psi, T;
	R = (DataTable.h_pc2 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / R) + (DataTable.lng_pc2 - PI)*13.5*0.3048*3600.0 / R_E;
	lng = DataTable.lng_pc2;
	lat = DataTable.lat_pc2;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc2 / 3600.0;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);
	//Step 2
	ConvergeTLMC(outarray.v_pl, outarray.psi_pl, outarray.lng_pl, lat, R, T, true);
	//Step 3
	double R_nd = DataTable.rad_lls + DataTable.h_nd;
	IntegratedXYZTTrajectory(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, R_nd, DataTable.lat_nd, DataTable.lng_nd, DataTable.GMT_nd);
	VECTOR3 RF, VF;
	double mfm0;
	
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	outarray.AZ_act = DataTable.psi_lls;
}

void TLMCCProcessor::Option2()
{
	//Empirical first guess
	double V, R, lng, lat, psi, T;
	R = (DataTable.h_pc2 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / R);
	lng = PI;
	lat = DataTable.lat_pc2;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc2 / 3600.0;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	ConicFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, DataTable.h_pc2, DataTable.lat_pc2);

	//Step 3
	ConicFreeReturnOptimizedFixedOrbitToLLS(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, DataTable.gamma_loi);
	double h_pl3 = outarray.h_pl;
	double lat_pl3 = outarray.lat_pl;
	double GMT_pl3 = outarray.GMT_pl / 3600.0;
	double gamma_loi3 = outarray.gamma_nd;
	double dt_lls3 = outarray.dt_lls;

	double dt = -OrbMech::period(outarray.sv_lls1.R, outarray.sv_lls1.V, mu_M)*(double)(MEDQuantities.Revs_circ + 1);
	EphemerisData sv_circ;
	INT ITS;
	pRTCC->PMMCEN(outarray.sv_lls1, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_circ, ITS);
	dt = OrbMech::time_radius(sv_circ.R, sv_circ.V, DataTable.rad_lls+MEDQuantities.H_T_circ, 1.0, mu_M);
	pRTCC->PMMCEN(sv_circ, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_circ, ITS);
	sv_circ.V = unit(sv_circ.V)*sqrt(mu_M / (DataTable.rad_lls + MEDQuantities.H_T_circ));

	double DV_PPC;
	EphemerisData sv_lls2 = PPC(sv_circ, DataTable.lat_lls, DataTable.lng_lls, DataTable.psi_lls, MissionConstants.n, 0, DV_PPC);
	outarray.sv_lls2.R = sv_lls2.R;
	outarray.sv_lls2.V = sv_lls2.V;
	outarray.sv_lls2.GMT = sv_lls2.GMT;
	outarray.M_lopc = MCOMP(DV_PPC, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_cir);

	double lat_TEI, lng_TEI, GMT_TEI, dlng, mfm0;
	EphemerisData sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;

	GMT_TEI = outarray.GMT_nd + DataTable.T_lo;
	pRTCC->PMMCEN(outarray.sv_lls2, 0.0, 10.0*24.0*3600.0, 1, GMT_TEI - outarray.sv_lls2.GMT, 1.0, sv_TEI1, ITS);
	sv_TEI2 = sv_TEI1;

	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.GMT, 4);
		OrbMech::latlong_from_r(R_TEI_EMP, lat_TEI, lng_TEI);
		if (lng_TEI < 0)
		{
			lng_TEI += PI2;
		}
		dlng = lng_TEI - PI;
		if (dlng > PI)
		{
			dlng -= PI2;
		}
		else if (dlng < -PI)
		{
			dlng += PI2;
		}
		dt = dlng * 20.0 / RAD;
		pRTCC->PMMCEN(sv_TEI2, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_TEI2, ITS);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp;

	dt_LPO = sv_TEI2.GMT - sv_TEI1.GMT;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.GMT, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI2)
	{
		dpsi_tei -= PI2;
	}

	EphemerisData S1 = sv_MCC;
	EphemerisData S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;

	double DT_te = DataTable.dt_lls - outarray.dt_lls + DataTable.T_te;

	//Step 4
	ConicTransEarthInjection((DataTable.T_lo + dt_LPO) / 3600.0, 2800.0*0.3048*3600.0 / R_E, 0.0, dpsi_tei, DT_te, false);

	//Step 5
	ConicTransEarthInjection(outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, DT_te, true);

	ConvergeTLMC(outarray.v_pl, outarray.psi_pl, outarray.lng_pl, lat_pl3, (h_pl3 + DataTable.rad_lls) / R_E, GMT_pl3, true);

	//Step 6
	IntegratedFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl3, lat_pl3);
	double GMT_pl6 = outarray.GMT_pl / 3600.0;
	//With the trajectory computer, the time used for the state vector at the start of LPO is biased by the difference between the time of perilune passage
	//in step 6 (integrated) and step 3 (conic).
	outarray.dt_bias_conic_prec = GMT_pl6 - GMT_pl3;
	VECTOR3 DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	VECTOR3 DV_temp = VF - sv_MCC.V;

	EphemerisData S_apo;
	VECTOR3 NewGuess, DV7, DV8;
	double r, v1, theta, phi, gamma1, psi1, v2, gamma2, psi2, v_c, dv_char7, dv_char8;

TLMCC_Option_2_E:

	S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;

	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 7
	ConicFullMissionFixedOrbit(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, gamma_loi3, outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, outarray.T_te, outarray.M_tei, true);
	DV7 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV7.x*R_E / 3600.0, DV7.y, DV7.z, isp_MCC, 0.0, v_c, dv_char7, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	outtab.GMT_pc1 = outtab.GMT_pc2 = outarray.GMT_pl;
	outtab.h_pc1 = outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc1 = outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc1 = outtab.lng_pc2 = outarray.lng_pl;
	outtab.gamma_loi = outarray.gamma_nd;
	outtab.dpsi_loi = outarray.dpsi_loi;
	outtab.dt_lls = outarray.dt_lls;
	outtab.T_lo = outarray.T_lo;
	outtab.psi_lls = outarray.AZ_act;
	outtab.dpsi_tei = outarray.dpsi_tei;
	outtab.dv_tei = outarray.dv_tei*R_E / 3600.0;
	outtab.T_te = outarray.T_te;

	//Step 8
	IntegratedFreeReturnFlyby(sv_MCC, NewGuess.x, NewGuess.y, NewGuess.z, outarray.h_pl, outarray.lat_pl);
	DV8 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV8.x*R_E / 3600.0, DV8.y, DV8.z, isp_MCC, 0.0, v_c, dv_char8, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	if (abs(dv_char8 - dv_char7) > 1.0*0.3048 && ((dv_char8 / dv_char7) > 1.03 || (dv_char8 / dv_char7) < 1.0 / 1.03))
	{
		goto TLMCC_Option_2_E;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV8.x*R_E / 3600.0, DV8.y, DV8.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	EphemerisData sv_MCC_apo = sv_MCC;
	sv_MCC_apo.V = VF;

	//Step 9
	EphemerisData sv_nd;
	pRTCC->PMMCEN(sv_MCC_apo, 0.0, 10.0*24.0*3600.0, 2, sin(outarray.gamma_nd), 1.0, sv_nd, ITS);

	LIBRAT(sv_nd.R, sv_nd.GMT, 4);
	OrbMech::latlong_from_r(sv_nd.R, outtab.lat_nd, outtab.lng_nd);
	if (outtab.lng_nd < 0)
	{
		outtab.lng_nd += PI2;
	}
	outtab.h_nd = length(sv_nd.R) - DataTable.rad_lls;
	outtab.GMT_nd = sv_nd.GMT;
	outarray.GMT_nd = sv_nd.GMT;

	outarray.AZ_act = DataTable.psi_lls;
}

void TLMCCProcessor::Option3()
{
	//Empirical first guess
	double V, R, lng, lat, psi, T;
	R = (DataTable.h_pc2 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / R);
	lng = PI;
	lat = DataTable.lat_pc2;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc2 / 3600.0;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	ConicFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, DataTable.h_pc2, DataTable.lat_pc2);

	//Step 3
	ConicFreeReturnOptimizedFreeOrbitToLOPC(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, DataTable.gamma_loi, DataTable.dpsi_loi, DataTable.dt_lls / 3600.0, MEDQuantities.AZ_min, MEDQuantities.AZ_max);
	double h_pl3 = outarray.h_pl;
	double lat_pl3 = outarray.lat_pl;
	double GMT_pl3 = outarray.GMT_pl / 3600.0;
	double gamma_loi3 = outarray.gamma_nd;
	double dpsi_loi3 = outarray.dpsi_loi;
	double dt_lls3 = outarray.dt_lls;

	double lat_TEI, lng_TEI, GMT_TEI, dlng, dt;
	EphemerisData sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;
	INT ITS;

	GMT_TEI = outarray.GMT_nd + DataTable.T_lo;
	pRTCC->PMMCEN(outarray.sv_lls2, 0.0, 10.0*24.0*3600.0, 1, GMT_TEI - outarray.sv_lls2.GMT, 1.0, sv_TEI1, ITS);
	sv_TEI2 = sv_TEI1; 
	
	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.GMT, 4);
		OrbMech::latlong_from_r(R_TEI_EMP, lat_TEI, lng_TEI);
		if (lng_TEI < 0)
		{
			lng_TEI += PI2;
		}
		dlng = lng_TEI - PI;
		if (dlng > PI)
		{
			dlng -= PI2;
		}
		else if (dlng < -PI)
		{
			dlng += PI2;
		}
		dt = dlng * 20.0 / RAD;
		pRTCC->PMMCEN(sv_TEI2, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_TEI2, ITS);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp, mfm0;

	dt_LPO = sv_TEI2.GMT - sv_TEI1.GMT;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.GMT, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI2)
	{
		dpsi_tei -= PI2;
	}

	EphemerisData S1 = sv_MCC;
	EphemerisData S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;

	double DT_te = DataTable.T_lo - outarray.T_lo + DataTable.T_te;

	//Step 4
	ConicTransEarthInjection((DataTable.T_lo + dt_LPO) / 3600.0, 2800.0*0.3048*3600.0 / R_E, 0.0, dpsi_tei, DT_te, false);

	//Step 5
	ConicTransEarthInjection(outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, DT_te, true);

	ConvergeTLMC(outarray.v_pl*3600.0 / R_E, outarray.psi_pl, outarray.lng_pl, lat_pl3, (h_pl3 + DataTable.rad_lls) / R_E, GMT_pl3, true);

	//Step 6
	IntegratedFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl3, lat_pl3);
	double GMT_pl6 = outarray.GMT_pl / 3600.0;
	//With the trajectory computer, the time used for the state vector at the start of LPO is biased by the difference between the time of perilune passage
	//in step 6 (integrated) and step 3 (conic).
	outarray.dt_bias_conic_prec = GMT_pl6 - GMT_pl3;
	VECTOR3 DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	VECTOR3 DV_temp = VF - sv_MCC.V;

	EphemerisData S_apo;
	VECTOR3 NewGuess, DV7, DV8;
	double r, v1, theta, phi, gamma1, psi1, v2, gamma2, psi2, v_c, dv_char7, dv_char8;

TLMCC_Option_3_E:

	S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;

	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 7
	ConicFullMissionFreeOrbit(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, 0.0, gamma_loi3, dpsi_loi3, dt_lls3 / 3600.0, outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, outarray.T_te, MEDQuantities.AZ_min, MEDQuantities.AZ_max, outarray.M_tei, true);
	DV7 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV7.x*R_E / 3600.0, DV7.y, DV7.z, isp_MCC, 0.0, v_c, dv_char7, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	outtab.GMT_pc1 = outtab.GMT_pc2 = outarray.GMT_pl;
	outtab.h_pc1 = outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc1 = outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc1 = outtab.lng_pc2 = outarray.lng_pl;
	outtab.gamma_loi = outarray.gamma_nd;
	outtab.dpsi_loi = outarray.dpsi_loi;
	outtab.dt_lls = outarray.dt_lls;
	outtab.T_lo = outarray.T_lo;
	outtab.psi_lls = outarray.AZ_act;
	outtab.dpsi_tei = outarray.dpsi_tei;
	outtab.dv_tei = outarray.dv_tei*R_E / 3600.0;
	outtab.T_te = outarray.T_te;

	//Step 8
	IntegratedFreeReturnFlyby(sv_MCC, NewGuess.x, NewGuess.y, NewGuess.z, outarray.h_pl, outarray.lat_pl);
	DV8 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV8.x*R_E / 3600.0, DV8.y, DV8.z, isp_MCC, 0.0, v_c, dv_char8, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	if (abs(dv_char8 - dv_char7) > 1.0*0.3048 && ((dv_char8 / dv_char7) > 1.03 || (dv_char8 / dv_char7) < 1.0 / 1.03))
	{
		goto TLMCC_Option_3_E;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV8.x*R_E / 3600.0, DV8.y, DV8.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	EphemerisData sv_MCC_apo = sv_MCC;
	sv_MCC_apo.V = VF;
	//Step 9
	EphemerisData sv_nd;

	pRTCC->PMMCEN(sv_MCC_apo, 0.0, 10.0*24.0*3600.0, 2, sin(outarray.gamma_nd), 1.0, sv_nd, ITS);

	LIBRAT(sv_nd.R, sv_nd.GMT, 4);
	OrbMech::latlong_from_r(sv_nd.R, outtab.lat_nd, outtab.lng_nd);
	if (outtab.lng_nd < 0)
	{
		outtab.lng_nd += PI2;
	}
	outtab.h_nd = length(sv_nd.R) - DataTable.rad_lls;
	outtab.GMT_nd = sv_nd.GMT;
	outarray.GMT_nd = sv_nd.GMT;
}

void TLMCCProcessor::Option4()
{
	bool recycle = false;
	//Empirical first guess
	double V, R, lng, lat, psi, T, ddt;
	if (outarray.KREF_MCC == 2)
	{
		ddt = 0.0;
	}
	else
	{
		VECTOR3 R_EM, V_EM, R_ES;
		double ainv;
		EBETA(sv_MCC.R, sv_MCC.V, mu_E, ainv);
		double GMT_PC = DataTable.GMT_pc1;
		double GMT_TLI = pRTCC->GMTfromGET(DataTable.GET_TLI + 328.0);
		if (sv_MCC.GMT - GMT_TLI > 20.0*3600.0)
		{
			ddt = 0.0;
		}
		else
		{
			EPHEM(GMT_PC, R_EM, V_EM, R_ES);
			ddt = 0.0;//DDELTATIME(1.0 / ainv / R_E, (sv_MCC.MJD - MJD_TLI)*24.0, length(R_EM) / R_E, PI - acos(dotp(unit(R_EM), unit(V_EM))), (MJD_PC - MJD_TLI)*24.0);
		}
	}
	R = (DataTable.h_pc2 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / R) - 0.0022*ddt + (DataTable.lng_pc2 - PI)*13.5*0.3048*3600.0 / R_E;
	lng = DataTable.lng_pc2 - 0.025*ddt;
	lat = DataTable.lat_pc2;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc2 / 3600.0 + ddt;

	double T_min, T_max, T_min_sea, T_max_sea, T_min_dps, T_max_dps, dt_min, dt_max;
	T_min_sea = MEDQuantities.T_min_sea*3600.0;
	T_max_sea = MEDQuantities.T_max_sea*3600.0;
	T_min_dps = MissionConstants.T_t1_min_dps;
	T_max_dps = MissionConstants.T_t1_max_dps;
	if (T_min_dps < T_min_sea)
	{
		T_min = T_min_sea;
	}
	else
	{
		T_min = T_min_dps;
	}

	if (T_max_dps < T_max_sea)
	{
		T_max = T_max_dps;
	}
	else
	{
		T_max = T_max_sea;
	}
	if (T < T_min / 3600.0)
	{
		T = T_min / 3600.0;
	}
	else if (T > T_max / 3600.0)
	{
		T = T_max / 3600.0;
	}
	dt_min = T_min - MEDQuantities.T_MCC;
	dt_max = T_max - MEDQuantities.T_MCC;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	ConicNonfreeReturnOptimizedFixedOrbitToLLS(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, DataTable.gamma_loi, dt_min, dt_max);
	double h_nd2 = outarray.h_nd;
	double lat_nd2 = outarray.lat_nd;
	double lng_nd2 = outarray.lng_nd;
	double v_nd2 = outarray.v_pl*3600.0 / R_E;
	double psi_nd2 = outarray.psi_pl;
	double GMT_nd2 = (outarray.GMT_nd) / 3600.0;
	double dt_lls2 = outarray.dt_lls;

	double dt = -OrbMech::period(outarray.sv_lls1.R, outarray.sv_lls1.V, mu_M)*(double)(MEDQuantities.Revs_circ + 1);
	EphemerisData sv_circ;
	int ITS;

	pRTCC->PMMCEN(outarray.sv_lls1, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_circ, ITS);
	dt = OrbMech::time_radius(sv_circ.R, sv_circ.V, DataTable.rad_lls + MEDQuantities.H_T_circ, 1.0, mu_M);
	pRTCC->PMMCEN(sv_circ, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_circ, ITS);
	sv_circ.V = unit(sv_circ.V)*sqrt(mu_M / (DataTable.rad_lls + MEDQuantities.H_T_circ));

	double DV_PPC;
	EphemerisData sv_lls2 = PPC(sv_circ, DataTable.lat_lls, DataTable.lng_lls, DataTable.psi_lls, MissionConstants.n, 0, DV_PPC);
	outarray.DV_LOPC = _V(DV_PPC, 0, 0);
	outarray.sv_lls2.R = sv_lls2.R;
	outarray.sv_lls2.V = sv_lls2.V;
	outarray.sv_lls2.GMT = sv_lls2.GMT;
	outarray.M_lopc = MCOMP(DV_PPC, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_cir) - MEDQuantities.LMMass;

	double lat_TEI, lng_TEI, GMT_TEI, dlng, mfm0;
	EphemerisData sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;

	GMT_TEI = outarray.GMT_nd + DataTable.T_lo;
	pRTCC->PMMCEN(outarray.sv_lls2, 0.0, 10.0*24.0*3600.0, 1, GMT_TEI - outarray.sv_lls2.GMT, 1.0, sv_TEI1, ITS);
	sv_TEI2 = sv_TEI1;

	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.GMT, 4);
		OrbMech::latlong_from_r(R_TEI_EMP, lat_TEI, lng_TEI);
		if (lng_TEI < 0)
		{
			lng_TEI += PI2;
		}
		dlng = lng_TEI - PI;
		if (dlng > PI)
		{
			dlng -= PI2;
		}
		else if (dlng < -PI)
		{
			dlng += PI2;
		}
		dt = dlng * 20.0 / RAD;
		pRTCC->PMMCEN(sv_TEI2, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_TEI2, ITS);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp;

	dt_LPO = sv_TEI2.GMT - sv_TEI1.GMT;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.GMT, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI)
	{
		dpsi_tei -= PI2;
	}

	EphemerisData S_apo, S3I;
	EphemerisData S1 = sv_MCC;
	EphemerisData S2C = sv_MCC;
	EphemerisData S3C = outarray.sv_loi;
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;

	double DT_te = DataTable.dt_lls - outarray.dt_lls + DataTable.T_te;

	//Step 2A
	ConicTransEarthInjection((DataTable.T_lo + dt_LPO) / 3600.0, 2800.0*0.3048*3600.0 / R_E, 0.0, dpsi_tei, DT_te, false);

	//Step 2B
	ConicTransEarthInjection(outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, DT_te, true);

	//Step 3
	ConvergeTLMC(v_nd2, psi_nd2, lng_nd2, lat_nd2, (DataTable.rad_lls + h_nd2) / R_E, GMT_nd2, true);

	//Step 4
	IntegratedXYZTTrajectory(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, DataTable.rad_lls + h_nd2, lat_nd2, lng_nd2, GMT_nd2*3600.0);

	VECTOR3 DV4, DV_temp, NewGuess, DV5, DV6;
	double r, v1, theta, phi, gamma1, psi1, v2, gamma2, psi2, GMT_nd, v_c, dv_char5, dv_char6;
TLMCC_Option_4_D:
	DV4 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, DV4.x*R_E / 3600.0, DV4.y, DV4.z, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;
	S3I = outarray.sv_loi;
	S_apo = S2C;
	S_apo.V = S2C.V - DV_temp;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);
	outarray.LOIOffset = S3I.V - S3C.V;

	//Step 5
	ConicFullMissionFixedOrbit(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, DataTable.gamma_loi, outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, outarray.T_te, outarray.M_tei, false, dt_min, dt_max);
	DV5 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	S2C = sv_MCC;
	S3C = outarray.sv_loi;
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV5.x*R_E / 3600.0, DV5.y, DV5.z, isp_MCC, 0.0, v_c, dv_char5, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 6
	GMT_nd = outarray.GMT_nd;
	IntegratedXYZTTrajectory(sv_MCC, NewGuess.x, NewGuess.y, NewGuess.z, outarray.h_nd + DataTable.rad_lls, outarray.lat_nd, outarray.lng_nd, GMT_nd);
	DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, 0.0, v_c, dv_char6, mfm0, RF, VF);

	//TBD
	if (recycle == false && (abs(dv_char6 - dv_char5) > 1.0*0.3048))
	{
		recycle = true;
		goto TLMCC_Option_4_D;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	VECTOR3 R_pl, V_pl;
	double beta, ainv, GMT_pl;
	beta = EBETA(outarray.sv_loi.R, outarray.sv_loi.V, mu_M, ainv);
	XBETA(outarray.sv_loi.R, outarray.sv_loi.V, outarray.sv_loi.GMT, beta, 2, R_pl, V_pl, GMT_pl);
	outarray.AZ_act = DataTable.psi_lls;
	outtab.GMT_pc1 = outtab.GMT_pc2 = outarray.GMT_pl;
	outtab.GMT_nd = GMT_nd;
	outtab.h_nd = outarray.h_nd;
	outtab.lat_nd = outarray.lat_nd;
	outtab.lng_nd = outarray.lng_nd;
	outtab.h_pc1 = outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc1 = outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc1 = outtab.lng_pc2 = outarray.lng_pl;
	outtab.gamma_loi = outarray.gamma_nd;
	outtab.dpsi_loi = outarray.dpsi_loi;
	outtab.dt_lls = outarray.dt_lls;
	outtab.T_lo = outarray.T_lo;
	outtab.psi_lls = outarray.AZ_act;
	outtab.dpsi_tei = outarray.dpsi_tei;
	outtab.dv_tei = outarray.dv_tei*R_E / 3600.0;
	outtab.T_te = outarray.T_te;	
}

void TLMCCProcessor::Option5()
{
	bool recycle = false;
	//Empirical first guess
	double V, R, lng, lat, psi, T, ddt, h_pl;
	if (outarray.KREF_MCC == 2)
	{
		ddt = 0.0;
	}
	else
	{
		VECTOR3 R_EM, V_EM, R_ES;
		double ainv;
		EBETA(sv_MCC.R, sv_MCC.V, mu_E, ainv);
		double GMT_PC = DataTable.GMT_pc1;
		double GMT_TLI = pRTCC->GMTfromGET(DataTable.GET_TLI + 328.0);
		if (sv_MCC.GMT - GMT_TLI > 20.0*3600.0)
		{
			ddt = 0.0;
		}
		else
		{
			EPHEM(GMT_PC, R_EM, V_EM, R_ES);
			ddt = DDELTATIME(1.0 / ainv / R_E, (sv_MCC.GMT - GMT_TLI) / 3600.0, length(R_EM) / R_E, PI - acos(dotp(unit(R_EM), unit(V_EM))), (GMT_PC - GMT_TLI) / 3600.0);
		}
	}

	//MED override
	if (MEDQuantities.H_pl_mode5 < 0)
	{
		h_pl = DataTable.h_pc2;
	}
	else
	{
		h_pl = MEDQuantities.H_pl_mode5;
	}

	R = (h_pl + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / R) - 0.0022*ddt + (DataTable.lng_pc2 - PI)*13.5*0.3048*3600.0 / R_E;
	lng = DataTable.lng_pc2 - 0.025*ddt;
	lat = DataTable.lat_pc2;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc2 / 3600.0 + ddt;

	double T_min, T_max, T_min_sea, T_max_sea, T_min_dps, T_max_dps, dt_min, dt_max;
	T_min_sea = MEDQuantities.T_min_sea*3600.0;
	T_max_sea = MEDQuantities.T_max_sea*3600.0;
	T_min_dps = MissionConstants.T_t1_min_dps;
	T_max_dps = MissionConstants.T_t1_max_dps;
	if (T_min_dps < T_min_sea)
	{
		T_min = T_min_sea;
	}
	else
	{
		T_min = T_min_dps;
	}

	if (T_max_dps < T_max_sea)
	{
		T_max = T_max_dps;
	}
	else
	{
		T_max = T_max_sea;
	}
	if (T < T_min / 3600.0)
	{
		T = T_min / 3600.0;
	}
	else if (T > T_max / 3600.0)
	{
		T = T_max / 3600.0;
	}
	dt_min = T_min - MEDQuantities.T_MCC;
	dt_max = T_max - MEDQuantities.T_MCC;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	ConicNonfreeReturnOptimizedFreeOrbitToLOPC(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, dt_min, dt_max, h_pl);
	double h_nd2 = outarray.h_nd;
	double lat_nd2 = outarray.lat_nd;
	double lng_nd2 = outarray.lng_nd;
	double GMT_nd2 = outarray.GMT_nd / 3600.0;
	double gamma_loi2 = outarray.gamma_nd;
	double dpsi_loi2 = outarray.dpsi_loi;
	double dt_lls2 = outarray.dt_lls;
	double psi_nd2 = outarray.psi_pl;
	double v_nd2 = outarray.v_pl*3600.0 / R_E;

	double lat_TEI, lng_TEI, GMT_TEI, dlng, dt;
	EphemerisData sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;
	INT ITS;

	GMT_TEI = outarray.GMT_nd + DataTable.T_lo;
	pRTCC->PMMCEN(outarray.sv_lls2, 0.0, 10.0*24.0*3600.0, 1, GMT_TEI - outarray.sv_lls2.GMT, 1.0, sv_TEI1, ITS);
	sv_TEI2 = sv_TEI1;

	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.GMT, 4);
		OrbMech::latlong_from_r(R_TEI_EMP, lat_TEI, lng_TEI);
		if (lng_TEI < 0)
		{
			lng_TEI += PI2;
		}
		dlng = lng_TEI - PI;
		if (dlng > PI)
		{
			dlng -= PI2;
		}
		else if (dlng < -PI)
		{
			dlng += PI2;
		}
		dt = dlng * 20.0 / RAD;
		pRTCC->PMMCEN(sv_TEI2, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_TEI2, ITS);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp, mfm0;

	dt_LPO = sv_TEI2.GMT - sv_TEI1.GMT;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.GMT, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI2)
	{
		dpsi_tei -= PI2;
	}

	EphemerisData S_apo, S3I;
	EphemerisData S1 = sv_MCC;
	EphemerisData S2C = sv_MCC;
	EphemerisData S3C = outarray.sv_loi;
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;

	double DT_te = DataTable.T_lo - outarray.T_lo + DataTable.T_te;

	//Step 2A
	ConicTransEarthInjection((DataTable.T_lo + dt_LPO) / 3600.0, 2800.0*0.3048*3600.0 / R_E, 0.0, dpsi_tei, DT_te, false);

	//Step 2B
	ConicTransEarthInjection(outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, DT_te, true);

	//Step 3
	double R_nd2 = DataTable.rad_lls + h_nd2;
	ConvergeTLMC(v_nd2, psi_nd2, lng_nd2, lat_nd2, R_nd2 / R_E, GMT_nd2, true);

	//Step 4
	IntegratedXYZTTrajectory(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, R_nd2, lat_nd2, lng_nd2, GMT_nd2*3600.0);
	
	VECTOR3 DV4, DV_temp, NewGuess, DV5, DV6;
	double r, v1, theta, phi, gamma1, psi1, v2, gamma2, psi2, GMT_nd, v_c, dv_char5, dv_char6;
TLMCC_Option_5_D:
	DV4 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, DV4.x*R_E / 3600.0, DV4.y, DV4.z, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;
	S3I = outarray.sv_loi;
	S_apo = S2C;
	S_apo.V = S2C.V - DV_temp;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);
	outarray.LOIOffset = S3I.V - S3C.V;

	//Step 5
	ConicFullMissionFreeOrbit(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, h_pl, DataTable.gamma_loi, dpsi_loi2, dt_lls2 / 3600.0, outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, outarray.T_te, MEDQuantities.AZ_min, MEDQuantities.AZ_max, outarray.M_tei, false, dt_min, dt_max);
	DV5 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	S2C = sv_MCC;
	S3C = outarray.sv_loi;
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV5.x*R_E / 3600.0, DV5.y, DV5.z, isp_MCC, 0.0, v_c, dv_char5, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 6
	GMT_nd = outarray.GMT_nd;
	IntegratedXYZTTrajectory(sv_MCC, NewGuess.x, NewGuess.y, NewGuess.z, outarray.h_nd + DataTable.rad_lls, outarray.lat_nd, outarray.lng_nd, GMT_nd);
	DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, 0.0, v_c, dv_char6, mfm0, RF, VF);

	//TBD
	if (recycle == false && (abs(dv_char6 - dv_char5) > 1.0*0.3048))
	{
		recycle = true;
		goto TLMCC_Option_5_D;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	VECTOR3 R_pl, V_pl;
	double beta, ainv, GMT_pl;
	beta = EBETA(outarray.sv_loi.R, outarray.sv_loi.V, mu_M, ainv);
	XBETA(outarray.sv_loi.R, outarray.sv_loi.V, outarray.sv_loi.GMT, beta, 2, R_pl, V_pl, GMT_pl);
	outtab.GMT_pc1 = outtab.GMT_pc2 = GMT_pl;
	outtab.GMT_nd = GMT_nd;
	outtab.h_nd = outarray.h_nd;
	outtab.lat_nd = outarray.lat_nd;
	outtab.lng_nd = outarray.lng_nd;
	outtab.h_pc1 = outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc1 = outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc1 = outtab.lng_pc2 = outarray.lng_pl;
	outtab.gamma_loi = outarray.gamma_nd;
	outtab.dpsi_loi = outarray.dpsi_loi;
	outtab.dt_lls = outarray.dt_lls;
	outtab.T_lo = outarray.T_lo;
	outtab.psi_lls = outarray.AZ_act;
	outtab.dpsi_tei = outarray.dpsi_tei;
	outtab.dv_tei = outarray.dv_tei*R_E / 3600.0;
	outtab.T_te = outarray.T_te;
}

void TLMCCProcessor::Option6()
{
	double V, R, lng, lat, psi, T;

	//Empirical first guess
	R = (DataTable.h_pc1 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / (R + 20.0*1852.0 / R_E));
	lng = PI;
	lat = DataTable.lat_pc1;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc1 / 3600.0;
	
	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);
	//Step 2
	ConvergeTLMC(outarray.v_pl, outarray.psi_pl, outarray.lng_pl, lat, R, T, true);
	//Step4
	IntegratedFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, DataTable.h_pc1, DataTable.lat_pc1);
	VECTOR3 RF, VF;
	double mfm0;

	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	outarray.AZ_act = 0.0;
}

void TLMCCProcessor::Option7()
{
	double V, R, lng, lat, psi, T;

	//Empirical first guess
	R = (MEDQuantities.H_pl + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / (R + 20.0*1852.0 / R_E));
	lng = PI;
	lat = DataTable.lat_pc1;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc1 / 3600.0;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);
	//Step 2
	ConvergeTLMC(outarray.v_pl, outarray.psi_pl, outarray.lng_pl, lat, R, T, true);
	//Step 3
	IntegratedFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, MEDQuantities.H_pl, DataTable.lat_pc1);
	VECTOR3 RF, VF;
	double mfm0;

	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	outarray.AZ_act = 0.0;
}

void TLMCCProcessor::Option8()
{
	double V, R, lng, lat, psi, T, h_pl;

	//Empirical first guess
	R = (DataTable.h_pc1 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / (R + 20.0*1852.0 / R_E));
	lng = PI;
	lat = DataTable.lat_pc1;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc1 / 3600.0;

	double dt = (DataTable.GMT_pc1 - sv_MCC.GMT) / 3600.0;
	if (dt < 15.0)
	{
		double R0 = (DataTable.rad_lls + 60.0*1852.0) / R_E;
		R = R0 + dt * (R - R0) / 15.0;
	}

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	if (MEDQuantities.H_pl > 0)
	{
		h_pl = MEDQuantities.H_pl;
	}
	else
	{
		h_pl = R * R_E - DataTable.rad_lls;
	}
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split, GMT_pc;
	GMT_pc = DataTable.GMT_pc1;
	EPHEM(GMT_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, GMTBASE + GMT_pc / 24.0 / 3600.0), R_EM);
	incl_pg = abs(asin(R_EM.z / length(R_EM))) + 2.0*RAD;
	ConicFreeReturnInclinationFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, incl_pg);
	lat_split = outarray.lat_pl;
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_split += MEDQuantities.lat_bias;
	}
	else
	{
		lat_split -= MEDQuantities.lat_bias;
	}

	//Step 3
	ConicFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, lat_split);

	VECTOR3 DV3 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	ConvergeTLMC(V, psi, lng, lat_split, R, T, true);
	//Step 4
	IntegratedFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, lat_split);
	VECTOR3 RF, VF, DV_temp;
	double mfm0;
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	//Step 5
	ConicFreeReturnInclinationFlyby(sv_MCC, DV3.x, DV3.y, DV3.z, h_pl, outarray.incl_fr);
	VECTOR3 DV5 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);

	EphemerisData S1 = sv_MCC;
	EphemerisData S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, DV5.x*R_E / 3600.0, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;
	EphemerisData S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;
	double theta, phi, r, v1, v2, gamma1, gamma2, psi1, psi2;
	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	VECTOR3 NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	double lat_pl_max, lat_pl_min;
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_pl_max = lat_split + 20.0*RAD;
		lat_pl_min = lat_split - 0.5*RAD;
	}
	else
	{
		lat_pl_max = lat_split + 0.5*RAD;
		lat_pl_min = lat_split - 20.0*RAD;
	}

	//Step 6
	ConicFreeReturnInclinationFlyby(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, h_pl, abs(MEDQuantities.INCL_fr), lat_pl_min, lat_pl_max);
	VECTOR3 DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(S_apo.R, S_apo.V, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - S_apo.V;
	VF = sv_MCC.V + DV_temp;
	RVIO(true, sv_MCC.R, sv_MCC.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, RF, VF, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 7
	IntegratedFreeReturnInclinationFlyby(sv_MCC, NewGuess.x, NewGuess.y, NewGuess.z, h_pl, abs(MEDQuantities.INCL_fr));
	VECTOR3 DV7 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, DV7.x*R_E / 3600.0, DV7.y, DV7.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;

	outarray.AZ_act = 0.0;
}

void TLMCCProcessor::Option9A()
{
	bool recycle = false;

	double V, R, lng, lat, psi, T, h_pl;
	bool err;

	//Empirical first guess
	R = (DataTable.h_pc1 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / (R + 20.0*1852.0 / R_E));
	lng = PI;
	lat = DataTable.lat_pc1;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc1 / 3600.0;

	double dt = (DataTable.GMT_pc1 - sv_MCC.GMT) / 3600.0;
	if (dt < 15.0)
	{
		double R0 = (DataTable.rad_lls + 60.0*1852.0) / R_E;
		R = R0 + dt * (R - R0) / 15.0;
	}

	//Step 1
	err = ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split, GMT_pc;

	if (MEDQuantities.H_pl > 0)
	{
		h_pl = MEDQuantities.H_pl;
	}
	else
	{
		h_pl = R * R_E - DataTable.rad_lls;
	}

	GMT_pc = DataTable.GMT_pc1;
	EPHEM(GMT_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, GMTBASE + GMT_pc / 24.0 / 3600.0), R_EM);
	incl_pg = abs(asin(R_EM.z / length(R_EM))) + 2.0*RAD;
	ConicFreeReturnInclinationFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, incl_pg);
	lat_split = outarray.lat_pl;
	lat_split += MEDQuantities.lat_bias;

	//Step 3
	ConicFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, lat_split);

	VECTOR3 DV3 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	ConvergeTLMC(V, psi, lng, lat_split, R, T, true);

	//Step 4
	IntegratedFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, lat_split);
	double inc_fr_conv = outarray.incl_fr;
	VECTOR3 RF, VF, DV_temp;
	double mfm0;
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	//Step 5
	ConicFreeReturnInclinationFlyby(sv_MCC, DV3.x, DV3.y, DV3.z, h_pl, outarray.incl_fr);
	VECTOR3 DV5 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	EphemerisData S1 = sv_MCC;
	EphemerisData S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, DV5.x*R_E / 3600.0, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;
	EphemerisData S_apo;
	VECTOR3 NewGuess, DV6, DV7, DV8, DV9;
	double h_conv, theta, phi, r, v1, v2, gamma1, gamma2, psi1, psi2, inc_opt, v_c, dv_char8, dv_char9;

	h_conv = DataTable.h_pc1;

TLMCC_Option_9A_C:
	
	S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;
	
	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	if (recycle == false)
	{
		//Step 6
		ConicFreeReturnInclinationFlyby(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, h_conv, inc_fr_conv);
		DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
		//Step 7
		ConicFreeReturnOptimizedInclinationFlyby(S_apo, DV6.x, DV6.y, DV6.z, inc_fr_conv - 0.01*RAD, inc_fr_conv + 0.01*RAD, 1);
		DV7 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
		//BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV7.x, DV7.y, DV7.z, isp_MCC, 0.0, v_c, dv_char7, mfm0, RF, VF);
		NewGuess = DV7;
	}
	//Step 8
	ConicFreeReturnOptimizedInclinationFlyby(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, 0.0*RAD, 88.0*RAD, 2);
	DV8 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	inc_opt = outarray.incl_fr;
	h_conv = outarray.h_pl;
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV8.x*R_E / 3600.0, DV8.y, DV8.z, isp_MCC, 0.0, v_c, dv_char8, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 9
	IntegratedFreeReturnInclinationFlyby(sv_MCC, NewGuess.x, NewGuess.y, NewGuess.z, h_conv, inc_opt);
	DV9 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV9.x*R_E / 3600.0, DV9.y, DV9.z, isp_MCC, 0.0, v_c, dv_char9, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	if (abs(dv_char9 - dv_char8) > 1.0*0.3048 && ((dv_char9 / dv_char8) > 1.03 || (dv_char9 / dv_char8) < 1.0 / 1.03))
	{
		recycle = true;
		goto TLMCC_Option_9A_C;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV9.x*R_E / 3600.0, DV9.y, DV9.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

void TLMCCProcessor::Option9B()
{
	double V, R, lng, lat, psi, T, h_pl;
	bool recycle = false;

	//Empirical first guess
	R = (DataTable.h_pc1 + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / (R + 20.0*1852.0 / R_E));
	lng = PI;
	lat = DataTable.lat_pc1;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc1 / 3600.0;

	double dt = (DataTable.GMT_pc1 - sv_MCC.GMT) / 3600.0;
	if (dt < 15.0)
	{
		double R0 = (DataTable.rad_lls + 60.0*1852.0) / R_E;
		R = R0 + dt * (R - R0) / 15.0;
	}

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split, GMT_pc;

	if (MEDQuantities.H_pl > 0)
	{
		h_pl = MEDQuantities.H_pl;
	}
	else
	{
		h_pl = R * R_E - DataTable.rad_lls;
	}

	GMT_pc = DataTable.GMT_pc1;
	EPHEM(GMT_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, GMTBASE + GMT_pc / 24.0 / 3600.0), R_EM);
	incl_pg = abs(asin(R_EM.z / length(R_EM))) + 2.0*RAD;
	ConicFreeReturnInclinationFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, incl_pg);
	lat_split = outarray.lat_pl;
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_split += MEDQuantities.lat_bias;
	}
	else
	{
		lat_split -= MEDQuantities.lat_bias;
	}

	//Step 3
	ConicFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, lat_split);

	VECTOR3 DV3 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	ConvergeTLMC(V, psi, lng, lat_split, R, T, true);
	//Step 4
	IntegratedFreeReturnFlyby(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, h_pl, lat_split);
	VECTOR3 RF, VF, DV_temp;
	double mfm0;
	BURN(sv_MCC.R, sv_MCC.V, outarray.dv_mcc*R_E / 3600.0, outarray.dgamma_mcc, outarray.dpsi_mcc, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	//Step 5
	ConicFreeReturnInclinationFlyby(sv_MCC, DV3.x, DV3.y, DV3.z, h_pl, outarray.incl_fr);
	VECTOR3 DV5 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);

	EphemerisData S1 = sv_MCC;
	EphemerisData S2C = sv_MCC;
	EphemerisData S_apo;
	VECTOR3 NewGuess, DV6, DV7, DV8;
	double theta, phi, r, v1, v2, gamma1, gamma2, psi1, psi2, lat_pl_max, lat_pl_min, v_c, dv_char7, h_conv, inc_conv, dv_char8;

	BURN(sv_MCC.R, sv_MCC.V, DV5.x*R_E / 3600.0, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;

TLMCC_Option_9B_C:

	S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;

	
	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);
 
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_pl_max = lat_split + 20.0*RAD;
		lat_pl_min = lat_split - 0.5*RAD;
	}
	else
	{
		lat_pl_max = lat_split + 0.5*RAD;
		lat_pl_min = lat_split - 20.0*RAD;
	}

	if (recycle == false)
	{
		//Step 6
		ConicFreeReturnInclinationFlyby(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, MEDQuantities.H_pl_min + 20.0*1852.0, abs(MEDQuantities.INCL_fr), lat_pl_min, lat_pl_max);
		DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	}
	else
	{
		DV6 = NewGuess;
	}

	//Step 7
	ConicFreeReturnOptimizedInclinationFlyby(S_apo, DV6.x, DV6.y, DV6.z, abs(MEDQuantities.INCL_fr) - 0.01*RAD, abs(MEDQuantities.INCL_fr) + 0.01*RAD, 1);
	DV7 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	inc_conv = outarray.incl_fr;
	h_conv = outarray.h_pl;
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV7.x*R_E / 3600.0, DV7.y, DV7.z, isp_MCC, 0.0, v_c, dv_char7, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 8
	IntegratedFreeReturnInclinationFlyby(sv_MCC, NewGuess.x, NewGuess.y, NewGuess.z, h_conv, inc_conv);
	DV8 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV8.x*R_E / 3600.0, DV8.y, DV8.z, isp_MCC, 0.0, v_c, dv_char8, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	if (abs(dv_char8 - dv_char7) > 1.0*0.3048 && ((dv_char8 / dv_char7) > 1.03 || (dv_char8 / dv_char7) < 1.0 / 1.03))
	{
		recycle = true;
		goto TLMCC_Option_9B_C;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV8.x*R_E / 3600.0, DV8.y, DV8.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

bool TLMCCProcessor::ConvergeTLMC(double V, double azi, double lng, double lat, double r, double GMT_pl, bool integrating)
{
	void *constPtr;
	outarray.TLMCIntegrating = integrating;
	outarray.sv0 = sv_MCC;
	constPtr = &outarray;

	bool ConvergeTLMCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConvergeTLMCPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = V;
	block.IndVarGuess[1] = azi;
	block.IndVarGuess[2] = lng;
	block.IndVarGuess[3] = lat;
	block.IndVarGuess[4] = r;
	block.IndVarGuess[5] = 0.0;
	block.IndVarGuess[6] = GMT_pl;
	block.IndVarStep[0] = pow(2, -19);
	block.IndVarStep[1] = pow(2, -19);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;

	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[1] = true;
	block.DepVarSwitch[2] = true;
	if (outarray.KREF_MCC == 1)
	{
		block.DepVarSwitch[3] = true;
	}
	block.DepVarLowerLimit[0] = (sv_MCC.R.x - 0.0657*1852.0) / R_E;
	block.DepVarLowerLimit[1] = (sv_MCC.R.y - 0.0657*1852.0) / R_E;
	block.DepVarLowerLimit[2] = (sv_MCC.R.z - 0.0657*1852.0) / R_E;
	block.DepVarLowerLimit[3] = 0.0;
	block.DepVarUpperLimit[0] = (sv_MCC.R.x + 0.0657*1852.0) / R_E;
	block.DepVarUpperLimit[1] = (sv_MCC.R.y + 0.0657*1852.0) / R_E;
	block.DepVarUpperLimit[2] = (sv_MCC.R.z + 0.0657*1852.0) / R_E;
	block.DepVarUpperLimit[3] = 92.0*RAD;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 1;
	block.DepVarClass[2] = 1;
	block.DepVarClass[3] = 2;
	block.DepVarWeight[3] = 64.0;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::IntegratedXYZTTrajectory(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double R_nd, double lat_nd, double lng_nd, double GMT_node)
{
	void *constPtr;

	outarray.sv0 = sv0;
	outarray.NodeStopIndicator = true;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;
	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarGuess[3] = (GMT_node - MEDQuantities.T_MCC) / 3600.0;
	block.IndVarStep[0] = pow(2, -23);
	block.IndVarStep[1] = pow(2, -19);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
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
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicFreeReturnInclinationFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_pg, double lat_pl_min, double lat_pl_max)
{
	void *constPtr;

	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = true;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;
	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarStep[0] = pow(2, -23);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -21);
	block.IndVarWeight[0] = 1.0;
	block.IndVarWeight[1] = 1.0;
	block.IndVarWeight[2] = 1.0;
	block.DepVarSwitch[0] = true;
	if (lat_pl_min != 0.0)
	{
		block.DepVarSwitch[1] = true;
	}
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarLowerLimit[0] = (H_pl - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[1] = lat_pl_min;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[3] = (64.0965*1852.0) / R_E;
	block.DepVarLowerLimit[4] = inc_pg - 0.01*RAD;
	block.DepVarUpperLimit[0] = (H_pl + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[1] = lat_pl_max;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = (67.5665*1852.0) / R_E;
	block.DepVarUpperLimit[4] = inc_pg + 0.01*RAD;
	block.DepVarWeight[1] = 32.0;
	block.DepVarWeight[2] = 1.0;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 2;
	block.DepVarClass[2] = 2;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicFreeReturnOptimizedInclinationFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double inc_pg_min, double inc_pg_max, int inc_class)
{
	void *constPtr;

	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = true;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;
	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarStep[0] = pow(2, -23);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -21);
	block.IndVarWeight[0] = 1.0;
	block.IndVarWeight[1] = 1.0;
	block.IndVarWeight[2] = 1.0;
	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarSwitch[15] = true;
	block.DepVarLowerLimit[0] = MEDQuantities.H_pl_min / R_E;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[3] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[4] = inc_pg_min;
	block.DepVarLowerLimit[15] = outarray.M_i + 5000.0*0.453;
	block.DepVarUpperLimit[0] = MEDQuantities.H_pl_max / R_E;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[4] = inc_pg_max;
	block.DepVarUpperLimit[15] = outarray.M_i + 5000.0*0.453;
	block.DepVarWeight[0] = 8.0;
	block.DepVarWeight[2] = 1.0;
	block.DepVarWeight[4] = 8.0;
	block.DepVarWeight[15] = 1.0;
	block.DepVarClass[0] = 2;
	block.DepVarClass[2] = 2;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = inc_class;
	block.DepVarClass[15] = 3;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::IntegratedFreeReturnFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl)
{
	void *constPtr;
	outarray.NodeStopIndicator = false;
	outarray.LunarFlybyIndicator = true;
	outarray.sv0 = sv0;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;
	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -19);
	block.IndVarStep[2] = pow(2, -19);
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarSwitch[5] = true;
	block.DepVarSwitch[6] = true;
	block.DepVarSwitch[7] = true;
	block.DepVarLowerLimit[3] = 90.0*RAD;
	block.DepVarLowerLimit[4] = (H_pl - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[5] = lat_pl - 0.01*RAD;
	block.DepVarLowerLimit[6] = 0.0;
	block.DepVarLowerLimit[7] = (64.0965*1852.0) / R_E;
	block.DepVarUpperLimit[3] = 182.0*RAD;
	block.DepVarUpperLimit[4] = (H_pl + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[5] = lat_pl + 0.01*RAD;
	block.DepVarUpperLimit[6] = 90.0*RAD;
	block.DepVarUpperLimit[7] = (67.5665*1852.0) / R_E;
	block.DepVarWeight[3] = 64.0;
	block.DepVarWeight[6] = 8.0;
	block.DepVarClass[3] = 2;
	block.DepVarClass[4] = 1;
	block.DepVarClass[5] = 1;
	block.DepVarClass[6] = 2;
	block.DepVarClass[7] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicFreeReturnFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = true;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;
	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarStep[0] = pow(2, -23);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -21);
	block.IndVarWeight[0] = 1.0;
	block.IndVarWeight[1] = 1.0;
	block.IndVarWeight[2] = 1.0;
	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[1] = true;
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarLowerLimit[0] = (H_pl - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[1] = lat_pl - 0.01*RAD;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[3] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[4] = 0.0;
	block.DepVarUpperLimit[0] = (H_pl + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[1] = lat_pl + 0.01*RAD;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[4] = 90.0*RAD;
	block.DepVarWeight[2] = 64.0;
	block.DepVarWeight[4] = 8.0;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 1;
	block.DepVarClass[2] = 2;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = 2;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::IntegratedFreeReturnInclinationFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_fr)
{
	void *constPtr;
	outarray.NodeStopIndicator = false;
	outarray.LunarFlybyIndicator = true;
	outarray.sv0 = sv0;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &IntegratedTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;
	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarStep[0] = pow(2, -23);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -21);
	block.IndVarWeight[0] = 1.0;
	block.IndVarWeight[1] = 1.0;
	block.IndVarWeight[2] = 1.0;
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarSwitch[6] = true;
	block.DepVarSwitch[7] = true;
	block.DepVarLowerLimit[3] = 90.0*RAD;
	block.DepVarLowerLimit[4] = (H_pl - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[6] = inc_fr - 0.01*RAD;
	block.DepVarLowerLimit[7] = (64.0965*1852.0) / R_E;
	block.DepVarUpperLimit[3] = 182.0*RAD;
	block.DepVarUpperLimit[4] = (H_pl + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[6] = inc_fr + 0.01*RAD;
	block.DepVarUpperLimit[7] = (67.5665*1852.0) / R_E;
	block.DepVarWeight[3] = 1.0;
	block.DepVarClass[3] = 2;
	block.DepVarClass[4] = 1;
	block.DepVarClass[6] = 1;
	block.DepVarClass[7] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicFreeReturnOptimizedFixedOrbitToLLS(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = false;
	outarray.FixedOrbitIndicator = true;
	outarray.FirstSelect = true;
	outarray.FirstOptimize = true;
	outarray.LLSStopIndicator = true;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarSwitch[3] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarGuess[3] = gamma_loi;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarStep[3] = pow(2, -19);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.IndVarWeight[3] = 8.0;
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarSwitch[5] = true;
	block.DepVarSwitch[6] = true;
	block.DepVarSwitch[17] = true;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[3] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[4] = 0.0;
	block.DepVarLowerLimit[5] = -0.01*RAD;
	block.DepVarLowerLimit[6] = -0.1*1852.0 / R_E;
	block.DepVarLowerLimit[17] = 100000.0*0.453;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[4] = 75.0*RAD;
	block.DepVarUpperLimit[5] = 0.01*RAD;
	block.DepVarUpperLimit[6] = 0.1*1852.0 / R_E;
	block.DepVarUpperLimit[17] = 100000.0*0.453;
	block.DepVarWeight[2] = 64.0;
	block.DepVarWeight[4] = 8.0;
	block.DepVarWeight[17] = 1.0;
	block.DepVarClass[2] = 2;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = 2;
	block.DepVarClass[5] = 1;
	block.DepVarClass[6] = 1;
	block.DepVarClass[17] = 3;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicNonfreeReturnOptimizedFixedOrbitToLLS(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_min, double T_max)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = false;
	outarray.FixedOrbitIndicator = true;
	outarray.FirstSelect = true;
	outarray.FirstOptimize = true;
	outarray.LLSStopIndicator = true;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarGuess[3] = gamma_loi;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[5] = true;
	block.DepVarSwitch[6] = true;
	block.DepVarSwitch[17] = true;
	block.DepVarSwitch[20] = true;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[5] = -0.01*RAD;
	block.DepVarLowerLimit[6] = -0.1*1852.0 / R_E;
	block.DepVarLowerLimit[17] = 100000.0*0.453;
	block.DepVarLowerLimit[20] = T_min / 3600.0 - 2.0;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[5] = 0.01*RAD;
	block.DepVarUpperLimit[6] = 0.1*1852.0 / R_E;
	block.DepVarUpperLimit[17] = 100000.0*0.453;
	block.DepVarUpperLimit[20] = T_max / 3600.0 + 2.0;
	block.DepVarWeight[2] = 64.0;
	block.DepVarWeight[17] = 1.0;
	block.DepVarWeight[20] = 0.125;
	block.DepVarClass[2] = 2;
	block.DepVarClass[5] = 1;
	block.DepVarClass[6] = 1;
	block.DepVarClass[17] = 3;
	block.DepVarClass[20] = 2;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicFreeReturnOptimizedFreeOrbitToLOPC(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double dpsi_loi, double DT_lls, double AZ_min, double AZ_max)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = false;
	outarray.FixedOrbitIndicator = false;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	if (abs(AZ_max - AZ_min) < 0.02*RAD)
	{
		AZ_max += 0.01*RAD;
		AZ_min -= 0.01*RAD;
	}

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarSwitch[3] = true;
	block.IndVarSwitch[4] = true;
	block.IndVarSwitch[5] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarGuess[3] = gamma_loi;
	block.IndVarGuess[4] = dpsi_loi;
	block.IndVarGuess[5] = DT_lls;
	block.IndVarGuess[6] = DataTable.T_lo / 3600.0;
	block.IndVarGuess[7] = DataTable.dv_tei*3600.0 / R_E;
	block.IndVarGuess[8] = 0.0;
	block.IndVarGuess[9] = DataTable.dpsi_tei;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarStep[3] = pow(2, -19);
	block.IndVarStep[4] = pow(2, -19);
	block.IndVarStep[5] = pow(2, -18);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.IndVarWeight[3] = 8.0;
	block.IndVarWeight[4] = 1.0;
	block.IndVarWeight[5] = 1e-3;
	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	block.DepVarSwitch[7] = true;
	block.DepVarSwitch[8] = true;
	block.DepVarSwitch[9] = true;
	block.DepVarSwitch[10] = true;
	block.DepVarSwitch[18] = true;
	block.DepVarLowerLimit[0] = 40.0*1852.0 / R_E;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[3] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[4] = 0.0;
	block.DepVarLowerLimit[7] = (MissionConstants.H_LPO - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[8] = DataTable.lat_lls - 0.01*RAD;
	block.DepVarLowerLimit[9] = DataTable.lng_lls - 0.01*RAD;
	block.DepVarLowerLimit[10] = AZ_min;
	block.DepVarLowerLimit[18] = 70000.0*0.453;
	block.DepVarUpperLimit[0] = 100.0*1852.0 / R_E;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[4] = 75.0*RAD;
	block.DepVarUpperLimit[7] = (MissionConstants.H_LPO + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[8] = DataTable.lat_lls + 0.01*RAD;
	block.DepVarUpperLimit[9] = DataTable.lng_lls + 0.01*RAD;
	block.DepVarUpperLimit[10] = AZ_max;
	block.DepVarUpperLimit[18] = 70000.0*0.453;
	block.DepVarWeight[0] = 8.0;
	block.DepVarWeight[2] = 64.0;
	block.DepVarWeight[4] = 8.0;
	block.DepVarWeight[18] = 1.0;
	block.DepVarClass[0] = 2;
	block.DepVarClass[2] = 2;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = 2;
	block.DepVarClass[7] = 1;
	block.DepVarClass[8] = 1;
	block.DepVarClass[9] = 1;
	block.DepVarClass[10] = 1;
	block.DepVarClass[18] = 3;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicNonfreeReturnOptimizedFreeOrbitToLOPC(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double T_min, double T_max, double h_pl)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = false;
	outarray.FixedOrbitIndicator = false;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;
	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarSwitch[4] = true;
	block.IndVarSwitch[5] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarGuess[3] = DataTable.gamma_loi;
	block.IndVarGuess[4] = DataTable.dpsi_loi;
	block.IndVarGuess[5] = DataTable.dt_lls / 3600.0;
	block.IndVarGuess[6] = DataTable.T_lo / 3600.0;
	block.IndVarGuess[7] = DataTable.dv_tei*3600.0 / R_E;
	block.IndVarGuess[8] = 0.0;
	block.IndVarGuess[9] = DataTable.dpsi_tei;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarStep[4] = pow(2, -19);
	block.IndVarStep[5] = pow(2, -18);
	block.IndVarWeight[0] = 8.0;
	block.IndVarWeight[1] = 8.0;
	block.IndVarWeight[2] = 8.0;
	block.IndVarWeight[4] = 1.0;
	block.IndVarWeight[5] = 1e-3;
	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[2] = true;
	//RTCC Requirements document say to use this, but I think that is outdated
	//block.DepVarSwitch[7] = true;
	block.DepVarSwitch[8] = true;
	block.DepVarSwitch[9] = true;
	block.DepVarSwitch[10] = true;
	block.DepVarSwitch[18] = true;
	block.DepVarSwitch[20] = true;
	block.DepVarLowerLimit[0] = (h_pl - 0.1*1852.0) / R_E;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[7] = (MissionConstants.H_LPO - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[8] = DataTable.lat_lls - 0.01*RAD;
	block.DepVarLowerLimit[9] = DataTable.lng_lls - 0.01*RAD;
	block.DepVarLowerLimit[10] = DataTable.psi_lls - 0.01*RAD;
	block.DepVarLowerLimit[18] = outarray.M_i + 6000.0*0.453;
	block.DepVarLowerLimit[20] = T_min / 3600.0 - 2.0;
	block.DepVarUpperLimit[0] = (h_pl + 0.1*1852.0) / R_E;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[7] = (MissionConstants.H_LPO + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[8] = DataTable.lat_lls + 0.01*RAD;
	block.DepVarUpperLimit[9] = DataTable.lng_lls + 0.01*RAD;
	block.DepVarUpperLimit[10] = DataTable.psi_lls + 0.01*RAD;
	block.DepVarUpperLimit[18] = outarray.M_i + 6000.0*0.453;
	block.DepVarUpperLimit[20] = T_max / 3600.0 + 2.0;
	block.DepVarWeight[2] = 64.0;
	block.DepVarWeight[18] = 1.0;
	block.DepVarWeight[20] = 0.125;
	block.DepVarClass[0] = 1;
	block.DepVarClass[2] = 2;
	block.DepVarClass[7] = 1;
	block.DepVarClass[8] = 1;
	block.DepVarClass[9] = 1;
	block.DepVarClass[10] = 1;
	block.DepVarClass[18] = 3;
	block.DepVarClass[20] = 2;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicTransEarthInjection(double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, bool lngiter)
{
	void *constPtr;
	outarray.MidcourseCorrectionIndicator = false;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[6] = true;
	block.IndVarSwitch[7] = true;
	block.IndVarSwitch[8] = true;
	block.IndVarSwitch[9] = true;
	block.IndVarGuess[6] = T_lo;
	block.IndVarGuess[7] = dv_tei;
	block.IndVarGuess[8] = dgamma_tei;
	block.IndVarGuess[9] = dpsi_tei;
	block.IndVarStep[6] = pow(2, -19);
	block.IndVarStep[7] = pow(2, -19);
	block.IndVarStep[8] = pow(2, -19);
	block.IndVarStep[9] = pow(2, -19);
	block.IndVarWeight[6] = 1e-6;
	block.IndVarWeight[7] = 1.0;
	block.IndVarWeight[8] = 1.0;
	block.IndVarWeight[9] = 1.0;
	block.DepVarSwitch[11] = true;
	block.DepVarSwitch[12] = true;
	block.DepVarSwitch[13] = true;
	if (lngiter)
	{
		block.DepVarSwitch[14] = true;
		block.DepVarSwitch[19] = true;
		block.DepVarLowerLimit[13] = T_te / 3600.0 - 8.0;
		block.DepVarUpperLimit[13] = T_te / 3600.0 + 8.0;
		block.DepVarClass[13] = 2;
	}
	else
	{
		block.DepVarLowerLimit[13] = T_te / 3600.0 - 0.1;
		block.DepVarUpperLimit[13] = T_te / 3600.0 + 0.1;
		block.DepVarClass[13] = 1;
	}
	block.DepVarLowerLimit[11] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[12] = 0.0;
	block.DepVarLowerLimit[14] = -0.2*RAD;
	block.DepVarLowerLimit[19] = outarray.M_tei + 6000.0*0.453;
	block.DepVarUpperLimit[11] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[12] = 75.0*RAD;
	block.DepVarUpperLimit[14] = 0.2*RAD;
	block.DepVarUpperLimit[19] = outarray.M_tei + 6000.0*0.453;
	block.DepVarWeight[12] = 8.0;
	block.DepVarWeight[13] = 1.0;
	block.DepVarWeight[19] = 1.0;
	block.DepVarClass[11] = 1;
	block.DepVarClass[12] = 2;
	block.DepVarClass[14] = 1;
	block.DepVarClass[19] = 3;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicFullMissionFreeOrbit(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double h_pl, double gamma_loi, double dpsi_loi, double dt_lls, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double AZ_min, double AZ_max, double mass, bool freereturn, double T_min, double T_max)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	if (freereturn)
	{
		outarray.FreeReturnIndicator = true;
	}
	else
	{
		outarray.FreeReturnIndicator = false;
	}
	outarray.FreeReturnOnlyIndicator = false;
	outarray.FixedOrbitIndicator = false;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	if (abs(AZ_max - AZ_min) < 0.02*RAD)
	{
		AZ_max += 0.01*RAD;
		AZ_min -= 0.01*RAD;
		block.DepVarClass[10] = 1;
	}
	else
	{
		block.DepVarClass[10] = 2;
	}

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarSwitch[3] = true;
	block.IndVarSwitch[4] = true;
	block.IndVarSwitch[5] = true;
	block.IndVarSwitch[6] = true;
	block.IndVarSwitch[7] = true;
	block.IndVarSwitch[8] = true;
	block.IndVarSwitch[9] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarGuess[3] = gamma_loi;
	block.IndVarGuess[4] = dpsi_loi;
	block.IndVarGuess[5] = dt_lls;
	block.IndVarGuess[6] = T_lo;
	block.IndVarGuess[7] = dv_tei;
	block.IndVarGuess[8] = dgamma_tei;
	block.IndVarGuess[9] = dpsi_tei;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarStep[3] = pow(2, -19);
	block.IndVarStep[4] = pow(2, -19);
	block.IndVarStep[5] = pow(2, -18);
	block.IndVarStep[6] = pow(2, -21);
	block.IndVarStep[7] = pow(2, -21);
	block.IndVarStep[8] = pow(2, -21);
	block.IndVarStep[9] = pow(2, -21);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.IndVarWeight[3] = 8.0;
	block.IndVarWeight[4] = 1.0;
	block.IndVarWeight[5] = 1e-3;
	block.IndVarWeight[6] = 1e-6;
	block.IndVarWeight[7] = 1.0;
	block.IndVarWeight[8] = 8.0;
	block.IndVarWeight[9] = 8.0;
	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[2] = true;
	if (freereturn)
	{
		block.DepVarSwitch[3] = true;
		block.DepVarSwitch[4] = true;
		block.DepVarSwitch[7] = true;
	}
	else
	{
		//RTCC Requirements document says to use this, but I think that is outdated
		//block.DepVarSwitch[7] = true;
		block.DepVarSwitch[20] = true;
	}
	block.DepVarSwitch[8] = true;
	block.DepVarSwitch[9] = true;
	block.DepVarSwitch[10] = true;
	block.DepVarSwitch[11] = true;
	block.DepVarSwitch[12] = true;
	block.DepVarSwitch[13] = true;
	block.DepVarSwitch[14] = true;
	block.DepVarSwitch[19] = true;
	if (freereturn)
	{
		block.DepVarLowerLimit[0] = 40.0*1852.0 / R_E;
	}
	else
	{
		block.DepVarLowerLimit[0] = (h_pl - 0.1*1852.0) / R_E;
		block.DepVarLowerLimit[20] = T_min / 3600.0;
	}
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[3] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[4] = 0.0;
	block.DepVarLowerLimit[7] = (MissionConstants.H_LPO - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[8] = DataTable.lat_lls - 0.01*RAD;
	block.DepVarLowerLimit[9] = DataTable.lng_lls - 0.01*RAD;
	block.DepVarLowerLimit[10] = AZ_min;
	block.DepVarLowerLimit[11] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[12] = 0.0;
	block.DepVarLowerLimit[13] = T_te / 3600.0 - 8.0;
	block.DepVarLowerLimit[14] = -0.2*RAD;
	block.DepVarLowerLimit[19] = outarray.M_tei + 6000.0*0.453;
	if (freereturn)
	{
		block.DepVarUpperLimit[0] = 100.0*1852.0 / R_E;
	}
	else
	{
		block.DepVarUpperLimit[0] = (h_pl + 0.1*1852.0) / R_E;
		block.DepVarUpperLimit[20] = T_max / 3600.0;
	}
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[4] = 75.0*RAD;
	block.DepVarUpperLimit[7] = (MissionConstants.H_LPO + 0.5*1852.0) / R_E;
	block.DepVarUpperLimit[8] = DataTable.lat_lls + 0.01*RAD;
	block.DepVarUpperLimit[9] = DataTable.lng_lls + 0.01*RAD;
	block.DepVarUpperLimit[10] = AZ_max;
	block.DepVarUpperLimit[11] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[12] = 75.0*RAD;
	block.DepVarUpperLimit[13] = T_te / 3600.0 + 8.0;
	block.DepVarUpperLimit[14] = 0.2*RAD;
	block.DepVarUpperLimit[19] = outarray.M_tei + 6000.0*0.453;
	block.DepVarWeight[0] = 8.0;
	block.DepVarWeight[2] = 64.0;
	block.DepVarWeight[4] = 1.0;
	block.DepVarWeight[10] = 1.0;
	block.DepVarWeight[12] = 1.0;
	block.DepVarWeight[13] = 0.125;
	block.DepVarWeight[19] = 0.25; //If this is 1.0 the mass optimization overpowers other constraints. Why do we have to do this?
	block.DepVarWeight[20] = 0.125;
	if (freereturn)
	{
		block.DepVarClass[0] = 2;
	}
	else
	{
		block.DepVarClass[0] = 1;
	}
	block.DepVarClass[2] = 2;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = 2;
	block.DepVarClass[7] = 1;
	block.DepVarClass[8] = 1;
	block.DepVarClass[9] = 1;
	block.DepVarClass[11] = 1;
	block.DepVarClass[12] = 2;
	block.DepVarClass[14] = 1;
	block.DepVarClass[19] = 3;
	block.DepVarClass[20] = 2;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::ConicFullMissionFixedOrbit(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double mass, bool freereturn, double T_min, double T_max)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	if (freereturn)
	{
		outarray.FreeReturnIndicator = true;
	}
	else
	{
		outarray.FreeReturnIndicator = false;
	}
	outarray.FreeReturnOnlyIndicator = false;
	outarray.FixedOrbitIndicator = true;
	outarray.FirstSelect = true;
	outarray.FirstOptimize = true;
	outarray.LLSStopIndicator = false;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &ConicMissionComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarSwitch[3] = true;
	block.IndVarSwitch[6] = true;
	block.IndVarSwitch[7] = true;
	block.IndVarSwitch[8] = true;
	block.IndVarSwitch[9] = true;
	block.IndVarGuess[0] = dv_guess;
	block.IndVarGuess[1] = dgamma_guess;
	block.IndVarGuess[2] = dpsi_guess;
	block.IndVarGuess[3] = gamma_loi;
	block.IndVarGuess[6] = T_lo;
	block.IndVarGuess[7] = dv_tei;
	block.IndVarGuess[8] = dgamma_tei;
	block.IndVarGuess[9] = dpsi_tei;
	block.IndVarStep[0] = pow(2, -21);
	block.IndVarStep[1] = pow(2, -21);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarStep[3] = pow(2, -19);
	//Change to -19 (instead of -21) to permit faster convergence of time at node for non-free return
	block.IndVarStep[6] = pow(2, -19);
	block.IndVarStep[7] = pow(2, -21);
	block.IndVarStep[8] = pow(2, -21);
	block.IndVarStep[9] = pow(2, -21);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.IndVarWeight[3] = 8.0;
	block.IndVarWeight[6] = 1e-6;
	block.IndVarWeight[7] = 1.0;
	block.IndVarWeight[8] = 8.0;
	block.IndVarWeight[9] = 8.0;
	block.DepVarSwitch[2] = true;
	if (freereturn)
	{
		block.DepVarSwitch[3] = true;
		block.DepVarSwitch[4] = true;
		block.DepVarSwitch[13] = true;
	}
	else
	{
		block.DepVarSwitch[20] = true;
	}
	block.DepVarSwitch[5] = true;
	block.DepVarSwitch[6] = true;
	block.DepVarSwitch[11] = true;
	block.DepVarSwitch[12] = true;
	block.DepVarSwitch[14] = true;
	block.DepVarSwitch[19] = true;
	if (freereturn)
	{
		
	}
	else
	{
		block.DepVarLowerLimit[20] = T_min / 3600.0;
	}
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[3] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[4] = 0.0;
	block.DepVarLowerLimit[5] = -0.01*RAD;
	block.DepVarLowerLimit[6] = -0.1*1852.0 / R_E;
	block.DepVarLowerLimit[11] = 64.0965*1852.0 / R_E;
	block.DepVarLowerLimit[12] = 0.0;
	block.DepVarLowerLimit[13] = T_te / 3600.0 - 8.0;
	block.DepVarLowerLimit[14] = -0.2*RAD;
	block.DepVarLowerLimit[19] = outarray.M_tei + 6000.0*0.453;
	if (freereturn)
	{

	}
	else
	{
		block.DepVarUpperLimit[20] = T_max / 3600.0;
	}
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[4] = 75.0*RAD;
	block.DepVarUpperLimit[5] = 0.01*RAD;
	block.DepVarUpperLimit[6] = 0.1*1852.0 / R_E;
	block.DepVarUpperLimit[11] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[12] = 75.0*RAD;
	block.DepVarUpperLimit[13] = T_te / 3600.0 + 8.0;
	block.DepVarUpperLimit[14] = 0.2*RAD;
	block.DepVarUpperLimit[19] = outarray.M_tei + 6000.0*0.453;
	block.DepVarWeight[2] = 64.0;
	block.DepVarWeight[4] = 1.0;
	block.DepVarWeight[12] = 1.0;
	block.DepVarWeight[13] = 0.125;
	block.DepVarWeight[19] = 1.0;
	block.DepVarWeight[20] = 0.125;
	block.DepVarClass[2] = 2;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = 2;
	block.DepVarClass[5] = 1;
	block.DepVarClass[6] = 1;
	block.DepVarClass[11] = 1;
	block.DepVarClass[12] = 2;
	block.DepVarClass[13] = 3;
	block.DepVarClass[14] = 1;
	block.DepVarClass[19] = 3;
	block.DepVarClass[20] = 2;

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

double TLMCCProcessor::DDELTATIME(double a, double dt_apo, double xm, double betam, double dt)
{
	double ddt;

	if (a > 0.0 && a < 14.0)
	{
		a = 14.0;
	}
	else if (a > 91.0 || a < 0.0)
	{
		a = 91.0;
	}

	static const double b[] = { 0.48237399e1, -0.33327854e3, 0.98116717e4, -0.15785115e3, 0.42236397e-1, 0.50951511, 0.35644845, -0.15132136e-1, -0.77055739e2,
		0.32689925e1, 0.31964177e4, -0.16267514e1, -0.99746361e6, 0.16614352e-2, 0.10673486e4, -0.12140743e-2, 0.43980221e-2, -0.46467769e2, 0.44041338e-1, -0.23454849,
		0.55472146e7, -0.42499530e-4, 0.22349293e-3, -0.55596397e5, 0.28837746e-1, -0.91659504e-3, -0.53786773, 0.37944308e-2, 0.85832895e2,-0.69204019e-3 };

	ddt = b[0] + b[1] / a + b[2] / (a*a) + b[3] * dt_apo / a + b[4] * dt_apo*dt_apo + b[5] * dt_apo + b[6] * xm + b[7] * xm*dt_apo + b[8] * xm / a + b[9] * xm*dt_apo / a + b[10] * dt_apo / (a*a) + b[11] * dt_apo*dt_apo / a +
		b[12] / pow(a, 3) + b[13] * pow(dt_apo, 3) + b[14] * xm / (a*a) + b[15] * xm*dt_apo*dt_apo + b[16] * dt_apo*dt_apo*xm / a + b[17] * xm*dt_apo / (a*a) + b[18] * dt_apo*dt_apo*xm / (a*a) +
		b[19] * xm*cos(betam) + b[20] / pow(a, 4) + b[21] * pow(dt, 3) + b[22] * pow(xm, 3) + b[23] * dt_apo / pow(a, 4) + b[24] * dt + b[25] * dt*dt + b[26] * dt*xm / a + b[27] * dt*xm + b[28] * dt / a +
		b[29] * dt*dt*xm*xm / (a*a);

	return ddt - dt;
}

EphemerisData TLMCCProcessor::PPC(EphemerisData SIN, double lat1, double lng1, double azi1, int RT1, int INTL, double &DVS)
{
	if (INTL != 0)
	{
		//goto TLMCC_PPC_E;
	}

	EphemerisData S, SG, SLLG, SLLS, SMB, SMA;
	VECTOR3 H, H_apo, ND1, HSMA, TSMB, PSMB;
	double dt, GMT, PP, DV, r, v, theta, phi, gamma, psi, DL, D1, T1, T2, sin_az, DT, D, DAZ;
	int i, K, ITS;
	int IMAX = 1;

	S = SIN;
	GMT = S.GMT;
	PP = PI2 * length(S.R) / sqrt(mu_M / length(S.R));
	i = 1;
	DV = 0.0;
TLMCC_PPC_J:
	dt = PP * (double)(RT1 + 1);
	pRTCC->PMMCEN(S, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, SG, ITS);
	LIBRAT(SG.R, SG.V, SG.GMT, 5);
	r = length(S.R);
	v = length(S.V);
	theta = lng1;
	phi = lat1;
	gamma = 0.0;
	psi = azi1;
	RVIO(false, SLLG.R, SLLG.V, r, v, theta, phi, gamma, psi);
	DL = atan2(length(crossp(SG.R, SLLG.R)), dotp(SG.R, SLLG.R));
	SLLG.GMT = S.GMT + (PP*(DL / PI2 + 1.0 + (double)(RT1)));
TLMCC_PPC_C:
	SLLS = SLLG;
	LIBRAT(SLLS.R, SLLS.V, SLLS.GMT, 6);
	H = unit(crossp(S.R, S.V));
	H_apo = unit(crossp(SLLS.R, SLLS.V));
	ND1 = unit(crossp(H, H_apo));
	if (crossp(SLLS.R, ND1).z <= 0)
	{
		ND1 = -ND1;
	}
	K = 0;
	D1 = acos(dotp(ND1, S.R) / length(S.R));
	if (crossp(S.R, ND1).z < 0)
	{
		D1 = PI2 - D1;
	}
	SMB.GMT = S.GMT + PP*(1.0-D1/PI2+(double)(RT1));
	if (SLLG.GMT < SMB.GMT)
	{
		SLLG.GMT += PP;
		goto TLMCC_PPC_C;
	}
TLMCC_PPC_D:
	if (K <= 5)
	{
		T1 = S.GMT + PP * (double)(RT1);
		T2 = S.GMT + PP * (1.0 + (double)(RT1));
		if (SMB.GMT < T1)
		{
			SMB.GMT += PP;
		}
		else if (SMB.GMT > T2)
		{
			SMB.GMT -= PP;
		}
		T1 = SMB.GMT;
		T2 = SMB.GMT + PP;
		if (SLLG.GMT < T1)
		{
			SLLG.GMT += PP;
		}
		else if (SLLG.GMT > T2)
		{
			SLLG.GMT -= PP;
		}
	}
	SLLS.GMT = SLLG.GMT;
	SMA.GMT = SMB.GMT;
	CTBODY(S.R, S.V, S.GMT, SMB.GMT, 2, mu_M, SMB.R, SMB.V);
	CTBODY(SLLS.R, SLLS.V, SLLS.GMT, SMA.GMT, 2, mu_M, SMA.R, SMA.V);
	H = unit(crossp(SMA.R, SMA.V));
	H_apo = unit(crossp(SMB.R, SMB.V));
	ND1 = unit(crossp(H, H_apo));
	sin_az = length(crossp(H, H_apo));
	if (K <= 5)
	{
		if (crossp(SLLS.R, ND1).z <= 0)
		{
			ND1 = -ND1;
		}
	}
	if (K > 5 && sin_az < 0.0015)
	{
		goto TLMCC_PPC_H;
	}
	DT = abs(atan(length(crossp(unit(ND1), unit(SMB.R)))))*PP / PI2;
	DT *= OrbMech::sign(crossp(ND1, SMB.R).z);
	D = abs(atan(length(crossp(unit(SMA.R), unit(ND1)))))*PP / PI2;
	D *= OrbMech::sign(crossp(SMA.R, ND1).z);
	goto TLMCC_PPC_G;
TLMCC_PPC_H:
	HSMA = crossp(SMA.V, SMA.R);
	TSMB = crossp(HSMA, SMB.R);
	PSMB = unit(crossp(TSMB, HSMA));
	D = abs(asin(length(crossp(SMA.R, PSMB) / length(SMA.R)))*PP / PI2);
	D *= OrbMech::sign(crossp(SMA.R, PSMB).z);
	DT = 0.0;
TLMCC_PPC_G:
	if (abs(D) + abs(DT) < 0.036 || K >= 10)
	{
		DAZ = atan(length(crossp(H_apo, H)) / dotp(H_apo, H));
		DV = DV + 2.0*length(S.V)*sin(abs(DAZ) / 2.0);
		i++;
		if (i <= IMAX)
		{
			S = SLLS;
			SG = SLLG;
			goto TLMCC_PPC_J;
		}
		else
		{
			goto TLMCC_PPC_I;
		}
	}
	else
	{
		SLLG.GMT += DT + D;
		SMB.GMT += DT;
		SLLS = SLLG;
		LIBRAT(SLLS.R, SLLS.V, SLLS.GMT, 6);
		K++;
		goto TLMCC_PPC_D;
	}
TLMCC_PPC_I:
	DVS = DV;
	return SLLS;
}