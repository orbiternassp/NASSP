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

TLMCCProcessor::TLMCCProcessor(RTCC *r) : RTCCModule(r)
{
	R_E = OrbMech::R_Earth;
	R_M = OrbMech::R_Moon;
	mu_E = OrbMech::mu_Earth;
	mu_M = OrbMech::mu_Moon;
	gamma_reentry = -6.52*RAD;
	Reentry_range = 1380.0;
	Reentry_dt = 500.0;
	isp_SPS = 3080.0;
	isp_DPS = 3107.0;
}

void TLMCCProcessor::Init(TLMCCDataTable data, TLMCCMEDQuantities med, TLMCCMissionConstants cst)
{
	DataTable = data;
	MEDQuantities = med;
	Constants = cst;

	outarray.sv_lls1.RBI = BODY_MOON;
	outarray.sv_loi.RBI = BODY_MOON;
	outarray.SGSLOI.RBI = BODY_MOON;
	outarray.sv_lls2.RBI = BODY_MOON;
}

void TLMCCProcessor::Main(TLMCCOutputData &out)
{
	//Propagate state vector to time of ignition
	int ITS;
	pRTCC->PMMCEN(MEDQuantities.sv0, 0.0, 10.0*24.0*3600.0, 1, MEDQuantities.T_MCC - MEDQuantities.sv0.GMT, 1.0, sv_MCC, ITS);

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
			pRTCC->ELVCNV(sv_MCC, 0, 2, sv_MCC);
		}
	}
	else
	{
		if (Ratio < 1.0 / 0.275)
		{
			pRTCC->ELVCNV(sv_MCC, 2, 0, sv_MCC);
		}
	}

	if (sv_MCC.RBI == BODY_EARTH)
	{
		KREF_MCC = 1;
	}
	else
	{
		KREF_MCC = 2;
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
		sv_MCC_apo = sv_MCC;
		sv_MCC_apo.V += DV_MCC;
		DV_LOI = CalcLOIDV(sv_MCC_apo, outarray.gamma_nd);
	}
	//Calc MPT parameters
	out.R_MCC = sv_MCC.R;
	out.V_MCC = sv_MCC.V;
	out.GMT_MCC = sv_MCC.GMT;
	out.RBI = sv_MCC.RBI;

	out.V_MCC_apo = sv_MCC.V + DV_MCC;

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
	out.display.GET_MCC = pRTCC->GETfromGMT(sv_MCC.GMT) + T_start;
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
	Z_PHV = unit(-sv_MCC.R);
	Y_PHV = unit(crossp(sv_MCC.V, sv_MCC.R));
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
	double dv_loi = Constants.V_pcynlo - length(sv_nd.V);
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
	EphemerisData sv_lls2 = PPC(sv_circ, DataTable.lat_lls, DataTable.lng_lls, DataTable.psi_lls, Constants.n, 0, DV_PPC);
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
	if (KREF_MCC == 2)
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
	T_min_dps = Constants.T_t1_min_dps;
	T_max_dps = Constants.T_t1_max_dps;
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
	EphemerisData sv_lls2 = PPC(sv_circ, DataTable.lat_lls, DataTable.lng_lls, DataTable.psi_lls, Constants.n, 0, DV_PPC);
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
	if (KREF_MCC == 2)
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
	T_min_dps = Constants.T_t1_min_dps;
	T_max_dps = Constants.T_t1_max_dps;
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
	//Step4
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
	R = (MEDQuantities.H_pl + DataTable.rad_lls) / R_E;
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

	h_pl = R* R_E - DataTable.rad_lls;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split, GMT_pc;
	GMT_pc = DataTable.GMT_pc1;
	EPHEM(GMT_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, MEDQuantities.GMTBase + GMT_pc / 24.0 / 3600.0), R_EM);
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
	R = (MEDQuantities.H_pl + DataTable.rad_lls) / R_E;
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

	h_pl = R * R_E - DataTable.rad_lls;

	//Step 1
	err = ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split, GMT_pc;
	GMT_pc = DataTable.GMT_pc1;
	EPHEM(GMT_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, MEDQuantities.GMTBase + GMT_pc / 24.0 / 3600.0), R_EM);
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
	R = (MEDQuantities.H_pl + DataTable.rad_lls) / R_E;
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

	h_pl = R * R_E - DataTable.rad_lls;

	//Step 1
	ConvergeTLMC(V, psi, lng, lat, R, T, false);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split, GMT_pc;
	GMT_pc = DataTable.GMT_pc1;
	EPHEM(GMT_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, MEDQuantities.GMTBase + GMT_pc / 24.0 / 3600.0), R_EM);
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
	if (KREF_MCC == 1)
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
	block.DepVarLowerLimit[7] = (Constants.H_LPO - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[8] = DataTable.lat_lls - 0.01*RAD;
	block.DepVarLowerLimit[9] = DataTable.lng_lls - 0.01*RAD;
	block.DepVarLowerLimit[10] = AZ_min;
	block.DepVarLowerLimit[18] = 70000.0*0.453;
	block.DepVarUpperLimit[0] = 100.0*1852.0 / R_E;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[3] = 67.5665*1852.0 / R_E;
	block.DepVarUpperLimit[4] = 75.0*RAD;
	block.DepVarUpperLimit[7] = (Constants.H_LPO + 0.5*1852.0) / R_E;
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
	block.DepVarLowerLimit[7] = (Constants.H_LPO - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[8] = DataTable.lat_lls - 0.01*RAD;
	block.DepVarLowerLimit[9] = DataTable.lng_lls - 0.01*RAD;
	block.DepVarLowerLimit[10] = DataTable.psi_lls - 0.01*RAD;
	block.DepVarLowerLimit[18] = outarray.M_i + 6000.0*0.453;
	block.DepVarLowerLimit[20] = T_min / 3600.0 - 2.0;
	block.DepVarUpperLimit[0] = (h_pl + 0.1*1852.0) / R_E;
	block.DepVarUpperLimit[2] = 182.0*RAD;
	block.DepVarUpperLimit[7] = (Constants.H_LPO + 0.5*1852.0) / R_E;
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
	block.DepVarLowerLimit[7] = (Constants.H_LPO - 0.5*1852.0) / R_E;
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
	block.DepVarUpperLimit[7] = (Constants.H_LPO + 0.5*1852.0) / R_E;
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
	block.DepVarWeight[19] = 1.0;
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

bool ConvergeTLMCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((TLMCCProcessor*)data)->FirstGuessTrajectoryComputer(var, varPtr, arr, mode);
}

bool TLMCCProcessor::FirstGuessTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	TLMCCGeneralizedIteratorArray *vars;
	vars = static_cast<TLMCCGeneralizedIteratorArray*>(varPtr);
	EphemerisData sv_pl, sv_mcc_temp;
	double v_pl, psi_pl, lng_pl, lat_pl, rad_pl, gamma_pl;
	bool err;

	//Independent variables:
	//0 = Perilune velocity in Er/hr
	//1 = Perilune azimuth in rad
	//2 = Perilune longitude in rad
	//3 = Perilune latitude in rad
	//4 = Perilune radius in Er
	//5 = Perilune flight-path angle in rad
	//Dependent variables:
	//0-2: Midcourse position in Er
	//3: Midcourse flight-path angle in rad

	//Store in array
	vars->v_pl = var[0];
	vars->psi_pl = var[1];
	vars->lng_pl = var[2];
	vars->lat_pl = var[3];
	vars->h_pl = var[4] * R_E - DataTable.rad_lls;
	vars->gamma_pl = var[5];

	v_pl = var[0] * R_E / 3600.0;
	psi_pl = var[1];
	lng_pl = var[2];
	lat_pl = var[3];
	rad_pl = var[4] * R_E;
	gamma_pl = var[5];
	sv_pl.GMT = var[6] * 3600.0;
	vars->GMT_pl = sv_pl.GMT;

	//Build state vector at perilune
	RVIO(false, sv_pl.R, sv_pl.V, rad_pl, v_pl, lng_pl, lat_pl, gamma_pl, psi_pl);
	LIBRAT(sv_pl.R, sv_pl.V, sv_pl.GMT, 3);
	sv_pl.RBI = BODY_MOON;

	if (vars->TLMCIntegrating)
	{
		//Integrate backwards to time of MCC
		int ITS;
		pRTCC->PMMCEN(sv_pl, 0.0, 10.0*24.0*3600.0, 1, vars->sv0.GMT - sv_pl.GMT, -1.0, sv_mcc_temp, ITS);
	}
	else
	{
		if (KREF_MCC == 1)
		{
			//We need to patch
			VECTOR3 R_patch, V_patch;
			R_patch = sv_pl.R;
			V_patch = sv_pl.V;
			if (PATCH(R_patch, V_patch, sv_pl.GMT, -1, 2))
			{
				return true;
			}
			//Patch point found. Now to back to time of MCC
			err = CTBODY(R_patch, V_patch, sv_pl.GMT, vars->sv0.GMT, 1, mu_E, sv_mcc_temp.R, sv_mcc_temp.V);
			if (err) return true;
		}
		else
		{
			//Earth centered, two-body backwards propagation to time of MCC
			err = CTBODY(sv_pl.R, sv_pl.V, sv_pl.GMT, vars->sv0.GMT, 2, mu_M, sv_mcc_temp.R, sv_mcc_temp.V);
			if (err) return true;
		}
	}

	arr[0] = sv_mcc_temp.R.x / R_E;
	arr[1] = sv_mcc_temp.R.y / R_E;
	arr[2] = sv_mcc_temp.R.z / R_E;
	vars->V_MCC = sv_mcc_temp.V;

	double r1, r2, v1, v2, theta, phi, gamma1, gamma2, psi1, psi2;

	RVIO(true, vars->sv0.R, vars->sv0.V, r1, v1, theta, phi, gamma1, psi1);
	RVIO(true, sv_mcc_temp.R, sv_mcc_temp.V, r2, v2, theta, phi, gamma2, psi2);
	vars->dv_mcc = (v2 - v1)*3600.0 / R_E;
	vars->dgamma_mcc = gamma2 - gamma1;
	vars->dpsi_mcc = psi2 - psi1;

	if (arr.size() > 3)
	{
		arr[3] = gamma2;
	}

	return false;
}

bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((TLMCCProcessor*)data)->IntegratedTrajectoryComputer(var, varPtr, arr, mode);
}

bool TLMCCProcessor::IntegratedTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	//Independent Variables:
	//0: Delta velocity in Er/hr.
	//1: Delta flight path angle in rad
	//2: Delta azimuth in rad
	//3: Delta time to node in hr.
	//Dependent variables:
	//0: Radius of node in Er
	//1: Latitude of node in rad
	//2: Longitude of node in rad
	//3: Inclination at perilune in rad
	//4: Height at perilune in Er
	//5: Latitude at perilune in rad
	//6: Inclination of return in rad
	//7: Height of return in Er

	TLMCCGeneralizedIteratorArray *vars;
	vars = static_cast<TLMCCGeneralizedIteratorArray*>(varPtr);
	EphemerisData sv0, sv0_apo, sv_nd;
	VECTOR3 H_pg;
	double dv_mcc, dgamma_mcc, dpsi_mcc, mfm0, R_nd, lat_nd, lng_nd, inc_pg, dt_node;
	INT ITS;

	//Store in array
	vars->dv_mcc = var[0];
	vars->dgamma_mcc = var[1];
	vars->dpsi_mcc = var[2];

	dv_mcc = var[0] * R_E / 3600.0;
	dgamma_mcc = var[1];
	dpsi_mcc = var[2];
	dt_node = var[3] * 3600.0;
	sv0 = sv0_apo = vars->sv0;

	BURN(sv0.R, sv0.V, dv_mcc, dgamma_mcc, dpsi_mcc, isp_SPS, mfm0, sv0_apo.R, sv0_apo.V);
	outarray.M_mcc = outarray.M_i*mfm0;
	if (vars->NodeStopIndicator)
	{
		VECTOR3 R_node_emp, V_node_emp, R_pl, V_pl;
		double V_nd, gamma_nd, psi_nd, beta, ainv, GMT_pl, r_pl, v_pl, lng_pl, lat_pl, gamma_pl, psi_pl;

		pRTCC->PMMCEN(sv0_apo, 0.0, 10.0*24.0*3600.0, 1, dt_node, 1.0, sv_nd, ITS);
		vars->sv_loi = sv_nd;
		R_node_emp = sv_nd.R;
		V_node_emp = sv_nd.V;
		LIBRAT(R_node_emp, V_node_emp, sv_nd.GMT, 4);
		RVIO(true, R_node_emp, V_node_emp, R_nd, V_nd, lng_nd, lat_nd, gamma_nd, psi_nd);
		if (lng_nd < 0) lng_nd += PI2;
		H_pg = crossp(R_node_emp, V_node_emp);
		inc_pg = acos(H_pg.z / length(H_pg));

		beta = EBETA(sv_nd.R, sv_nd.V, mu_M, ainv);
		XBETA(sv_nd.R, sv_nd.V, sv_nd.GMT, beta, 2, R_pl, V_pl, GMT_pl);
		LIBRAT(R_pl, V_pl, GMT_pl, 4);
		RVIO(true, R_pl, V_pl, r_pl, v_pl, lng_pl, lat_pl, gamma_pl, psi_pl);

		arr[0] = R_nd / R_E;
		arr[1] = lat_nd;
		arr[2] = lng_nd;
		arr[3] = inc_pg;
		vars->gamma_nd = gamma_nd;
		vars->GMT_nd = sv_nd.GMT;
		vars->lat_pl = lat_pl;
		vars->lng_pl = lng_pl;
		vars->h_pl = r_pl - DataTable.rad_lls;
		vars->GMT_pl = GMT_pl;

		return false;
	}
	else
	{
		EphemerisData sv_pl, sv_r;
		MATRIX3 Rot;
		VECTOR3 R_temp, V_temp, HH_pl, H_equ;
		int ITS;

		//Propagate state vector to perilune
		pRTCC->PMMCEN(sv0_apo, 0.0, 100.0*3600.0, 2, 0.0, 1.0, sv_pl, ITS);
	
		R_temp = sv_pl.R;
		V_temp = sv_pl.V;
		vars->GMT_pl = sv_pl.GMT;
		LIBRAT(R_temp, V_temp, vars->GMT_pl, 4);
		vars->h_pl = length(R_temp) - DataTable.rad_lls;
		OrbMech::latlong_from_r(R_temp, vars->lat_pl, vars->lng_pl);
		if (vars->lng_pl < 0)
		{
			vars->lng_pl += PI2;
		}
		HH_pl = crossp(R_temp, V_temp);
		vars->incl_pl = acos(HH_pl.z / length(HH_pl));

		//Propagate state vector to reentry
		pRTCC->PMMCEN(sv_pl, 0.0, 100.0*3600.0, 2, sin(gamma_reentry), 1.0, sv_r, ITS);

		vars->h_fr = length(sv_r.R) - R_E;
		Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, MEDQuantities.GMTBase + sv_r.GMT / 24.0 / 3600.0);
		R_temp = rhtmul(Rot, sv_r.R);
		V_temp = rhtmul(Rot, sv_r.V);
		H_equ = crossp(R_temp, V_temp);
		vars->incl_fr = acos(H_equ.z / length(H_equ));
		vars->v_EI = length(sv_r.V);

		if (vars->LunarFlybyIndicator)
		{
			double dlng;
			RNTSIM(sv_r.R, sv_r.V, sv_r.GMT, Constants.lambda_IP, vars->lat_ip, vars->lng_ip, dlng);
			outarray.GMT_ip = sv_r.GMT + Reentry_dt;
		}

		arr[4] = vars->h_pl / R_E;
		arr[3] = vars->incl_pl;
		arr[5] = vars->lat_pl;
		arr[6] = vars->incl_fr;
		arr[7] = vars->h_fr / R_E;
		return false;
	}
}

bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((TLMCCProcessor*)data)->ConicMissionComputer(var, varPtr, arr, mode);
}

bool TLMCCProcessor::ConicMissionComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	//Independent variables:
	//0: MCC delta velocity in Er/hr
	//1: MCC delta flight-path angle in rad
	//2: MCC delta azimuth in rad
	//3: LOI flight-path angle in rad
	//4: LOI delta azimuth in rad
	//5: Delta time between LOI and LLS in hours
	//6: Delta time between LOI and TEI in hours
	//7: TEI delta velocity in Er/h
	//8: TEI delta flight-path angle in rad
	//9: TEI delta azimuth in rad
	//Dependent variables:
	//0: Height of perilune in Er
	//1: Latitude of perilune in rad
	//2: Inclination of perilune in rad
	//3: Height of free return in Er
	//4: Inclination of free return in rad
	//5: theta in rad
	//6: Delta height node in Er
	//7: Height of lunar parking orbit in Er
	//8: Latitude of landing site in rad
	//9: Longitude of landing site in rad
	//10: Azimuth at the landing site in rad
	//11: Height of powered return in Er
	//12: Inclination of powered return in rad
	//13: Time of transearth coast in sec
	//14: Delta splashdown longitude in rad
	//15: Mass after MCC
	//16: Mass after LOI
	//17: Mass after DOI
	//18: Mass after LOPC
	//19: Mass after TEI
	//20: Translunar flight time in hours

	TLMCCGeneralizedIteratorArray *vars;
	vars = static_cast<TLMCCGeneralizedIteratorArray*>(varPtr);
	EphemerisData sv0;
	VECTOR3 RF, VF, R_patch, V_patch, R_pl, V_pl, R_temp, V_temp, HH_pl, H;
	double dv_mcc, dgamma_mcc, dpsi_mcc, dv_tei;
	double mfm0, beta, GMT_patch, ainv, gamma;
	double ainv_pl, a, e, i, n, P, eta, dpsi_lopc, DV_R;

	//Store in array
	vars->dv_mcc = var[0];
	vars->dgamma_mcc = var[1];
	vars->dpsi_mcc = var[2];
	vars->dv_tei = var[7];

	dv_mcc = var[0] * R_E / 3600.0;
	dgamma_mcc = var[1];
	dpsi_mcc = var[2];
	vars->gamma_nd = var[3];
	vars->dpsi_loi = var[4];
	vars->dt_lls = var[5] * 3600.0;
	vars->T_lo = var[6] * 3600.0;
	dv_tei = var[7] * R_E / 3600.0;
	vars->dgamma_tei = var[8];
	vars->dpsi_tei = var[9];

	sv0 = vars->sv0;

	if (vars->MidcourseCorrectionIndicator == false)
	{
		if (vars->TLIIndicator == true)
		{
			//Set mass
			//Call TLIBRN
			//Call ELEMT
			return false;
		}
		else
		{
			goto TLMCC_Conic_F5;
		}
	}

	BURN(sv0.R, sv0.V, dv_mcc, dgamma_mcc, dpsi_mcc, isp_SPS, mfm0, RF, VF);
	outarray.M_mcc = outarray.M_i * mfm0;

	//TBD: MCOMP?
	
	if (KREF_MCC == 1)
	{
		R_patch = RF;
		V_patch = VF;
		GMT_patch = sv0.GMT;
		if (PATCH(R_patch, V_patch, GMT_patch, 1, 1))
		{
			return true;
		}
	}
	else
	{
		R_patch = RF;
		V_patch = VF;
		GMT_patch = sv0.GMT;
	}
	beta = EBETA(R_patch, V_patch, mu_M, ainv);
	ainv_pl = ainv;
	XBETA(R_patch, V_patch, GMT_patch, beta, 2, R_pl, V_pl, vars->GMT_pl);
	R_temp = R_pl;
	V_temp = V_pl;
	LIBRAT(R_temp, V_temp, vars->GMT_pl, 4);
	double r_pl, gamma_temp;
	RVIO(true, R_temp, V_temp, r_pl, vars->v_pl, vars->lng_pl, vars->lat_pl, gamma_temp, vars->psi_pl);
	vars->h_pl = r_pl - DataTable.rad_lls;
	HH_pl = crossp(R_temp, V_temp);
	vars->incl_pl = acos(HH_pl.z / length(HH_pl));

	if (vars->FreeReturnIndicator)
	{
		MATRIX3 Rot;
		VECTOR3 R_patch2, V_patch2, R_pg, V_pg, R_reentry, V_reentry, R_equ, V_equ, H_equ;
		double GMT_patch2, GMT_pg, H, E, beta, e, GMT_reentry;

		R_patch2 = R_pl;
		V_patch2 = V_pl;
		GMT_patch2 = vars->GMT_pl;

		if (PATCH(R_patch2, V_patch2, GMT_patch2, 1, 2))
		{
			return true;
		}
		beta = EBETA(R_patch2, V_patch2, mu_E, ainv);
		XBETA(R_patch2, V_patch2, GMT_patch2, beta, 1, R_pg, V_pg, GMT_pg);
		DGAMMA(length(R_pg), ainv, gamma_reentry, H, E, beta, e);
		XBETA(R_pg, V_pg, GMT_pg, beta, 1, R_reentry, V_reentry, GMT_reentry);
		vars->h_fr = length(R_reentry) - R_E;
		Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, MEDQuantities.GMTBase + GMT_reentry / 24.0 / 3600.0);
		R_equ = rhtmul(Rot, R_reentry);
		V_equ = rhtmul(Rot, V_reentry);
		H_equ = crossp(R_equ, V_equ);
		vars->incl_fr = acos(H_equ.z / length(H_equ));

		if (vars->FreeReturnOnlyIndicator)
		{
			arr[0] = vars->h_pl / R_E;
			arr[1] = vars->lat_pl;
			arr[2] = vars->incl_pl;
			arr[4] = vars->incl_fr;
			arr[3] = vars->h_fr / R_E;
			arr[15] = outarray.M_mcc;
			return false;
		}
	}

	VECTOR3 R_LOI, V_LOI;

	if (vars->gamma_nd == 0.0)
	{
		R_LOI = R_pl;
		V_LOI = V_pl;
		vars->GMT_nd = vars->GMT_pl;
	}
	else
	{
		double H, E, e;
		DGAMMA(length(R_pl), ainv_pl, vars->gamma_nd, H, E, beta, e);
		XBETA(R_pl, V_pl, vars->GMT_pl, beta, 2, R_LOI, V_LOI, vars->GMT_nd);
	}

	vars->sv_loi.R = R_LOI;
	vars->sv_loi.V = V_LOI;
	vars->sv_loi.GMT = vars->GMT_nd;

	//Apply offset
	V_LOI += outarray.LOIOffset;

	VECTOR3 H_LOI;
	double i_EMP;

	vars->h_nd = length(R_LOI) - DataTable.rad_lls;
	R_temp = R_LOI;
	V_temp = V_LOI;
	LIBRAT(R_temp, V_temp, vars->GMT_nd, 4);
	H_LOI = crossp(R_temp, V_temp);
	OrbMech::latlong_from_r(R_temp, vars->lat_nd, vars->lng_nd);
	if (vars->lng_nd < 0) vars->lng_nd += PI2;
	i_EMP = acos(H_LOI.z / length(H_LOI));
	vars->t_tl = vars->GMT_nd - sv0.GMT;

	if (vars->FixedOrbitIndicator)
	{
		goto TLMCC_Conic_A1;
	}

	double dv, GMT_LPO;
	dv = Constants.V_pcynlo - length(V_LOI);
	BURN(R_LOI, V_LOI, dv, -vars->gamma_nd, vars->dpsi_loi, isp_MCC, mfm0, RF, VF);
	outarray.M_loi = mfm0 * outarray.M_mcc;
	outarray.M_cir = MCOMP(157.8*0.3048, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_loi);
	vars->GMT_nd += vars->dt_bias_conic_prec*3600.0;
	GMT_LPO = vars->GMT_nd + Constants.dt_bias*3600.0;

	goto TLMCC_Conic_C4;

TLMCC_Conic_A1:

	VECTOR3 U_H, R_N, U_DS;
	double r_N, v_H, theta, phi, gamma_H, psi, DV_LOI;

	RVIO(true, R_LOI, V_LOI, r_N, v_H, theta, phi, gamma_H, psi);
	R_N = unit(R_LOI);
	U_H = unit(crossp(R_LOI, V_LOI));

	if (vars->FirstSelect)// || (!mode && vars->FirstOptimize))
	{
		if (!mode && vars->FirstOptimize)
		{
			vars->GMT_nd += vars->dt_bias_conic_prec*3600.0;
		}
		PRCOMP(R_N, U_H, vars->GMT_nd, vars->RA_LPO1, vars->A1, vars->E1, vars->gamma1, vars->V_L, vars->gamma_L, vars->V2, vars->dt_lls);
		var[5] = vars->dt_lls / 3600.0;
		if (vars->FirstSelect)
		{
			vars->FirstSelect = false;
		}
		if (!mode && vars->FirstOptimize)
		{
			vars->FirstOptimize = false;
		}
	}

	VECTOR3 Rtemp = vars->SGSLOI.R;
	VECTOR3 Vtemp = vars->SGSLOI.V;
	LIBRAT(Rtemp, Vtemp, vars->GMT_nd, 6);
	U_DS = unit(crossp(Rtemp, Vtemp));

	VECTOR3 U_PJ, gamma_vec;
	double R_NL, V1;

	U_PJ = unit(U_DS - R_N * dotp(U_DS, R_N));
	gamma_vec = unit(crossp(U_H, R_N));
	vars->theta = length(crossp(U_DS, U_PJ))*OrbMech::sign(dotp(crossp(U_DS, U_PJ), gamma_vec));

	R_NL = vars->A1 * (1.0 - vars->E1 * vars->E1) / (1.0 + vars->E1 * cos(MEDQuantities.TA_LOI));
	vars->DH_Node = R_NL - r_N;
	V1 = sqrt(mu_M*(2.0 / r_N - 2.0 / (r_N + vars->RA_LPO1)));
	DV_LOI = sqrt(v_H*v_H + V1 * V1 - 2.0*v_H*V1*(cos(gamma_H)*dotp(U_DS, U_H))) + 10.0*0.3048; //10 ft/s calibration DV to account for finite burn loss
	vars->dpsi_loi = acos(dotp(U_DS, U_H));

	outarray.M_loi = MCOMP(DV_LOI, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_mcc);

	gamma = vars->gamma_L - vars->gamma1;
	double DV_DOI = sqrt(vars->V2*vars->V2 + vars->V_L * vars->V_L - 2.0*vars->V_L*vars->V2*cos(gamma));
	outarray.M_cir = MCOMP(DV_DOI, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_loi);

	LIBRAT(vars->sv_lls1.R, vars->sv_lls1.V, vars->sv_lls1.GMT, 5);
	vars->sv_lls1.GMT = vars->GMT_nd + vars->dt_lls;
	LIBRAT(vars->sv_lls1.R, vars->sv_lls1.V, vars->sv_lls1.GMT, 6);

	if (vars->LLSStopIndicator)
	{
		goto TLMCC_Conic_Out;
	}

	outarray.M_lopc = outarray.M_cir - MEDQuantities.LMMass;

	goto TLMCC_Conic_F5;
TLMCC_Conic_C4:
	
	VECTOR3 R_LPO, V_LPO, R_LLS, V_LLS;
	double GMT_LLS, lat_S, lng_S;

	SCALE(RF, VF, 60.0*1852.0, R_LPO, V_LPO);
	GMT_LLS = vars->GMT_nd + vars->dt_lls;
	CTBODY(R_LPO, V_LPO, GMT_LPO, GMT_LLS, 2, mu_M, R_LLS, V_LLS);

	R_temp = R_LLS;
	V_temp = V_LLS;
	LIBRAT(R_temp, V_temp, GMT_LLS, 5);
	OrbMech::latlong_from_r(R_temp, lat_S, lng_S);
	double r, v;
	RVIO(true, R_temp, V_temp, r, v, theta, phi, gamma, vars->AZ_act);
	ELEMT(R_LLS, V_LLS, mu_M, H, a, e, i, n, P, eta);
	P = PI2 / (length(V_LLS) / length(R_LLS) + OrbMech::w_Moon);
	
	LOPC(R_LLS, V_LLS, GMT_LLS, unit(R_temp), Constants.m, Constants.n, P, vars->sv_lls2.R, vars->sv_lls2.V, vars->sv_lls2.GMT, mfm0, dpsi_lopc, outarray.DV_LOPC);
	if (vars->FixedOrbitIndicator == false && abs(dpsi_lopc) < 8e-3)
	{
		DV_R = 2.0*length(vars->sv_lls2.V)*(31.24975000037*dpsi_lopc*dpsi_lopc + 2.0000053333202e-3);
		mfm0 = exp(-DV_R / isp_MCC);
	}
	outarray.M_lopc = (outarray.M_cir - MEDQuantities.LMMass)*mfm0;

TLMCC_Conic_F5:

	MATRIX3 Rot;
	VECTOR3 R_TEI, V_TEI, R_TEC, V_TEC, R_pg, V_pg, R_reentry, V_reentry, R_equ, V_equ, H_equ;
	double GMT_TEC, GMT_pg, HH, E, GMT_reentry, dlng;

	vars->GMT_tei = vars->GMT_nd + vars->T_lo;
	CTBODY(vars->sv_lls2.R, vars->sv_lls2.V, vars->sv_lls2.GMT, vars->GMT_tei, 2, mu_M, R_TEI, V_TEI);
	BURN(R_TEI, V_TEI, dv_tei, vars->dgamma_tei, vars->dpsi_tei, isp_MCC, mfm0, R_TEC, V_TEC);
	vars->DV_TEI = V_TEC - V_TEI;
	outarray.M_tei = outarray.M_lopc * mfm0;
	GMT_TEC = vars->GMT_tei;
	if (PATCH(R_TEC, V_TEC, GMT_TEC, 1, 2))
	{
		return true;
	}
	beta = EBETA(R_TEC, V_TEC, mu_E, ainv);
	XBETA(R_TEC, V_TEC, GMT_TEC, beta, 1, R_pg, V_pg, GMT_pg);
	//TBD: If T_reentry < T_patch, adjust T_reentry
	DGAMMA(length(R_pg), ainv, gamma_reentry, HH, E, beta, e);
	XBETA(R_pg, V_pg, GMT_pg, beta, 1, R_reentry, V_reentry, GMT_reentry);
	vars->h_pr = length(R_reentry) - R_E;
	Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, MEDQuantities.GMTBase + GMT_reentry / 24.0 / 3600.0);
	R_equ = rhtmul(Rot, R_reentry);
	V_equ = rhtmul(Rot, V_reentry);
	H_equ = crossp(R_equ, V_equ);
	vars->incl_pr = acos(H_equ.z / length(H_equ));
	RNTSIM(R_reentry, V_reentry, GMT_reentry, Constants.lambda_IP, vars->lat_ip_pr, vars->lng_ip_pr, dlng);
	vars->T_te = GMT_reentry - vars->GMT_tei;
	vars->GMT_ip_pr = GMT_reentry + Reentry_dt;
	vars->v_EI_pr = length(V_reentry);

TLMCC_Conic_Out:

	arr[0] = vars->h_pl / R_E;
	arr[2] = vars->incl_pl;
	arr[3] = vars->h_fr / R_E;
	arr[4] = vars->incl_fr;
	arr[5] = vars->theta;
	arr[6] = vars->DH_Node / R_E;
	arr[7] = vars->h_nd / R_E;
	arr[8] = lat_S;
	arr[9] = lng_S;
	arr[10] = vars->AZ_act;
	arr[11] = vars->h_pr / R_E;
	arr[12] = vars->incl_pr;
	arr[13] = vars->T_te / 3600.0;
	arr[14] = dlng;
	arr[15] = vars->M_mcc;
	arr[16] = vars->M_loi;
	arr[17] = vars->M_cir;
	arr[18] = vars->M_lopc;
	arr[19] = vars->M_tei;
	arr[20] = vars->t_tl / 3600.0;

	return false;
}

double TLMCCProcessor::EBETA(VECTOR3 R, VECTOR3 V, double mu, double &ainv)
{
	double D0, r0, v0, a, beta;

	r0 = length(R);
	v0 = length(V);
	D0 = dotp(R, V);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	a = 1.0 / ainv;

	if (a > 0)
	{
		double ecos_E, esin_E, E;

		ecos_E = 1.0 - r0 / a;
		esin_E = D0 / sqrt(mu*a);
		E = atan2(esin_E, ecos_E);
		beta = -E * sqrt(a);
	}
	else if (a < 0)
	{
		double ecosh_H, esinh_H, H;

		ecosh_H = 1.0 - r0 / a;
		esinh_H = D0 / sqrt(mu*abs(a));
		H = log((ecosh_H + esinh_H) / sqrt(abs(ecosh_H*ecosh_H - esinh_H * esinh_H)));
		beta = -H * sqrt(abs(a));
	}
	else
	{
		beta = -D0 / sqrt(mu);
	}
	return beta;
}

bool TLMCCProcessor::PATCH(VECTOR3 &R, VECTOR3 &V, double &GMT, int Q, int KREF)
{
	//KREF = 1: Earth reference input, 2: Moon reference input

	VECTOR3 R_EM, V_EM, R_ES, R1, V1, R2, V2, R21, A2;
	double beta, Ratio_desired, GMTF, r1, r2, r21, mu1, mu2, d1, d2, v12, v22, DRatioDBeta, DDRatioDDBeta, Ratio, DRatio, dbeta;
	int i = 0;

	if (KREF == 1)
	{
		if (length(R) > 40.0*R_E)
		{
			return true;
		}
		else
		{
			if (RBETA(R, V, 40.0*R_E, Q, mu_E, beta))
			{
				return true;
			}
		}
		Ratio_desired = 0.275;
		mu1 = mu_E;
		mu2 = mu_M;
	}
	else
	{
		if (length(R) > 10.0*R_E)
		{
			return true;
		}
		else
		{
			if (RBETA(R, V, 10.0*R_E, Q, mu_M, beta))
			{
				return true;
			}
		}
		Ratio_desired = 1.0 / 0.275;
		mu1 = mu_M;
		mu2 = mu_E;
	}

	do
	{
		XBETA(R, V, GMT, beta, KREF, R1, V1, GMTF);
		if (EPHEM(GMTF, R_EM, V_EM, R_ES))
		{
			return true;
		}
		if (KREF == 1)
		{
			R2 = R1 - R_EM;
			V2 = V1 - V_EM;
			R21 = R_EM;
		}
		else
		{
			R2 = R1 + R_EM;
			V2 = V1 + V_EM;
			R21 = -R_EM;
		}
		r1 = length(R1);
		r2 = length(R2);
		/*if (KREF == 1)
		{
			if (r1 > 60.0*R_E) return false;
		}
		else
		{
			if (r1 > 15.0*R_E) return false;
		}*/
		Ratio = r2 / r1;
		DRatio = Ratio_desired - Ratio;
		if (abs(DRatio) < 10e-12)
		{
			break;
		}
		r21 = length(R21);
		d1 = dotp(R1, V1);
		d2 = dotp(R2, V2);
		v12 = dotp(V1, V1);
		v22 = dotp(V2, V2);
		A2 = -R1 * mu1 / pow(r1, 3) + R21 * (mu1 + mu2) / pow(r21, 3);
		DRatioDBeta = 1.0 / r2 / sqrt(mu1)*(d2 - r2 * r2*d1 / r1 / r1);
		DDRatioDDBeta = r1 / mu1 * (v22 + dotp(R2, A2)) / r2 - d1 * d2 / (mu1*r1*r2) - d2 * d2*r1 / (mu1*pow(r2, 3)) - r2 * v12 / r1 / mu1 + r2 / r1 / r1 + 2.0*d1*d1*r2 / (mu1*pow(r1, 3));
		if (DRatioDBeta*DRatioDBeta + 2.0*DRatio*DDRatioDDBeta < 0)
		{
			DDRatioDDBeta = 0.0;
		}
		dbeta = 2.0*DRatio / (DRatioDBeta + DRatioDBeta / abs(DRatioDBeta)*sqrt(DRatioDBeta*DRatioDBeta + 2.0*DRatio*DDRatioDDBeta));
		beta = beta + dbeta;
		i++;
	} while (i < 10);

	R = R2;
	V = V2;
	GMT = GMTF;

	return false;
}


bool TLMCCProcessor::RBETA(VECTOR3 R0, VECTOR3 V0, double r, int Q, double mu, double &beta)
{
	double D0, r0, v0, ainv, e, QD, temp;

	//Q: +1 = Solution will be ahead of initial position with respect to direction of motion, -1 = solution behind initial position
	if (Q == -1)
	{
		QD = -1.0;
	}
	else if (Q == 1)
	{
		QD = 1.0;
	}
	else
	{
		return true;
	}

	r0 = length(R0);
	v0 = length(V0);
	D0 = dotp(R0, V0);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	e = sqrt(pow(1.0 - r0 * ainv, 2) + D0 * D0*ainv / mu);
	if (e < 0.000001)
	{
		return true;
	}

	//Hyperbolic
	if (ainv < 0)
	{
		double H0;
		double cosh_H0 = 1.0 / e * (1.0 - r0 * ainv);
		double cosh_H = 1.0 / e * (1.0 - r * ainv);

		if (cosh_H0 < 1.0)
		{
			H0 = 0.0;
		}
		else
		{

			temp = cosh_H0 * cosh_H0 - 1.0;
			if (temp < 0)
			{
				return true;
			}
			H0 = D0 / abs(D0)*log(cosh_H0 + sqrt(temp));
		}
		temp = cosh_H * cosh_H - 1.0;
		if (temp < 0)
		{
			return true;
		}
		double H = log(cosh_H + sqrt(temp));
		double theta = H0 - QD * H;
		beta = QD * abs(theta)*sqrt(abs(1.0 / ainv));
	}
	//Elliptical
	else if (ainv > 0)
	{
		double cos_E0 = 1.0 / e * (1.0 - r0 * ainv);
		double cos_E = 1.0 / e * (1.0 - r * ainv);
		temp = max(0.0, 1.0 - cos_E0 * cos_E0);
		if (temp < 0)
		{
			return true;
		}
		double E0 = atan2(sqrt(temp), cos_E0);
		if (E0 < 0)
		{
			E0 += PI2;
		}
		E0 = D0 / abs(D0)*E0;
		temp = 1.0 - cos_E * cos_E;
		if (temp < 0)
		{
			return true;
		}
		double E = atan2(sqrt(temp), cos_E);
		if (E < 0)
		{
			E += PI2;
		}
		double theta = E0 - QD * E;
		beta = QD * abs(theta)*sqrt(1.0 / ainv);
	}
	//Parabolic
	else
	{
		beta = D0 / sqrt(mu) + QD * sqrt(D0*D0 / mu + 2.0*(r - r0));
	}

	return false;
}

void TLMCCProcessor::XBETA(VECTOR3 R0, VECTOR3 V0, double GMT0, double beta, int K, VECTOR3 &RF, VECTOR3 &VF, double &GMTF)
{
	double D0, r0, v0, ainv, mu, a, F1, F2, F3, F4, t, r, f, g, fdot, gdot;

	mu = GetMU(K);
	r0 = length(R0);
	v0 = length(V0);
	D0 = dotp(R0, V0);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	a = -beta * beta*ainv;
	FCOMP(a, F1, F2, F3, F4);
	t = (beta*beta*F1 + D0 * beta*F2 / sqrt(mu) + r0 * F3)*beta / sqrt(mu);
	GMTF = GMT0 + t;
	r = (D0*F3 / sqrt(mu) + beta * F2)*beta + r0 * F4;
	f = 1.0 - beta * beta*F2 / r0;
	g = t - pow(beta, 3)*F1 / sqrt(mu);
	fdot = -sqrt(mu)*beta*F3 / (r0 * r);
	gdot = 1.0 - beta * beta*F2 / r;
	RF = R0 * f + V0 * g;
	VF = R0 * fdot + V0 * gdot;
}

double TLMCCProcessor::GetMU(int k)
{
	if (k == 1)
	{
		return mu_E;
	}
	else
	{
		return mu_M;
	}
}

void TLMCCProcessor::FCOMP(double a, double &F1, double &F2, double &F3, double &F4)
{
	/*static const double xarr[] = { 2e-7,2e-5,2e-3,2e-2,2e-1,1,2,4,8,16,32,64,128,256,512 };
	static const int yarr[] = { 5,6,7,8,9,10,11,13,15,18,21,25,30,38,46 };

	unsigned n;
	unsigned i = 0;

	while (i < 14 && abs(a)>=xarr[i])
	{
		i++;
	}
	n = yarr[i];
	F1 = F2 = 0.0;

	for (i = 0;i < n;i++)
	{
		F1 = F1 + pow(a, i) / factorial(2 * i + 4 - 1);
		F2 = F2 + pow(a, i) / factorial(2 * i + 4 - 2);
	}*/

	F1 = OrbMech::stumpS(-a);
	F2 = OrbMech::stumpC(-a);

	F3 = a * F1 + 1.0;
	F4 = a * F2 + 1.0;
}

bool TLMCCProcessor::EPHEM(double GMT, VECTOR3 &R_EM, VECTOR3 &V_EM, VECTOR3 &R_ES)
{
	return pRTCC->PLEFEM(1, GMT / 3600.0, 0, R_EM, V_EM, R_ES);
}

bool TLMCCProcessor::LIBRAT(VECTOR3 &R, double GMT, int K)
{
	VECTOR3 V = _V(0, 0, 0);
	return LIBRAT(R, V, GMT, K);
}

bool TLMCCProcessor::LIBRAT(VECTOR3 &R, VECTOR3 &V, double GMT, int K)
{
	//Options
	//1: EMP to selenographic
	//2: Selenographic to EMP
	//3: EMP to selenocentric
	//4: Selenocentric to EMP
	//5: Selenocentric to selenographic
	//6: Selenographic to selenocentric

	if (K < 5)
	{
		MATRIX3 A;
		VECTOR3 R_ME, V_ME, R_ES, i, j, k;

		if (EPHEM(GMT, R_ME, V_ME, R_ES))
		{
			return true;
		}
		i = -unit(R_ME);
		k = unit(crossp(R_ME, V_ME));
		j = crossp(k, i);
		A = _M(i.x, j.x, k.x, i.y, j.y, k.y, i.z, j.z, k.z);

		if (K == 1 || K == 3)
		{
			R = mul(A, R);
			V = mul(A, V);
		}
		else
		{
			R = tmul(A, R);
			V = tmul(A, V);
		}

		if (K == 1 || K == 2)
		{
			MATRIX3 Rot;

			Rot = OrbMech::GetRotationMatrix(BODY_MOON, MEDQuantities.GMTBase + GMT / 24.0 / 3600.0);

			if (K == 1)
			{
				R = rhtmul(Rot, R);
				V = rhtmul(Rot, V);
			}
			else
			{
				R = rhmul(Rot, R);
				V = rhmul(Rot, V);
			}
		}
	}
	else
	{
		MATRIX3 Rot;

		Rot = OrbMech::GetRotationMatrix(BODY_MOON, MEDQuantities.GMTBase + GMT / 24.0 / 3600.0);

		if (K == 5)
		{
			R = rhtmul(Rot, R);
			V = rhtmul(Rot, V);
		}
		else
		{
			R = rhmul(Rot, R);
			V = rhmul(Rot, V);
		}
	}

	return false;
}

bool TLMCCProcessor::CTBODY(VECTOR3 R0, VECTOR3 V0, double GMT0, double GMTF, int K, double mu, VECTOR3 &RF, VECTOR3 &VF)
{
	double F1, F2, F3, F4, alpha;
	return CTBODY(R0, V0, GMT0, GMTF, K, mu, alpha, F1, F2, F3, F4, RF, VF);
}

bool TLMCCProcessor::CTBODY(VECTOR3 R0, VECTOR3 V0, double GMT0, double GMTF, int K, double mu, double &alpha, double &F1, double &F2, double &F3, double &F4, VECTOR3 &RF, VECTOR3 &VF)
{
	double dt;
	int i = 0;

	dt = GMTF - GMT0;

	if (abs(dt) < 3.6e-9)
	{
		RF = R0;
		VF = V0;
		return false;
	}

	double r0, v0, ainv, D0, beta, t, r;

	r0 = length(R0);
	v0 = length(V0);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	D0 = dotp(R0, V0);

	//Initial guess
	beta = 1.0 / 5.0*dt*sqrt(mu) / r0;

	do
	{
		alpha = -beta * beta*ainv;
		FCOMP(alpha, F1, F2, F3, F4);
		t = (beta*beta*F1 + D0 / sqrt(mu)*beta*F2 + r0 * F3)*beta / sqrt(mu);
		r = D0 / sqrt(mu)*beta*F3 + beta * beta*F2 + r0 * F4;
		beta = beta + (dt - t)*sqrt(mu) / r;
		i++;
		if (i == 20)
		{
			return true;
		}
	} while (abs((t - dt) / dt) >= 10e-12);

	double f, g, fdot, gdot;

	f = 1.0 - beta * beta*F2 / r0;
	g = t - pow(beta, 3)*F1 / sqrt(mu);
	fdot = -sqrt(mu)*beta*F3 / (r0 * r);
	gdot = 1.0 - beta * beta*F2 / r;
	RF = R0 * f + V0 * g;
	VF = R0 * fdot + V0 * gdot;

	if (K == 1 || ainv < 0.0)
	{
		return false;
	}

	VECTOR3 G0, Gdot0;

	G0 = RF;
	Gdot0 = VF;
	LIBRAT(G0, Gdot0, GMTF, 5);
	double n1, n2, n;

	n1 = Gdot0.z*G0.x - G0.z*Gdot0.x;
	n2 = Gdot0.z*G0.y - G0.z*Gdot0.y;
	n = sqrt(n1*n1 + n2 * n2);
	if (n <= 10e-12)
	{
		return false;
	}

	VECTOR3 H;
	double cos_Omega, sin_Omega, cos_i, sin_i, dOmega, d, rr, vv;

	cos_Omega = n1 / n;
	sin_Omega = n2 / n;
	H = unit(crossp(G0, Gdot0));
	cos_i = H.z;
	sin_i = sqrt(H.x*H.x + H.y*H.y);
	dOmega = -1.5*OrbMech::J2_Moon*R_M*R_M*sqrt(mu_M)*cos_i*pow(ainv, 3)*sqrt(ainv)*dt;
	d = dotp(G0, Gdot0);
	rr = dotp(G0, G0);
	vv = dotp(Gdot0, Gdot0);

	VECTOR3 N, M, G, Gdot;
	N = _V(cos_Omega*cos(dOmega) - sin_Omega * sin(dOmega), sin_Omega*cos(dOmega) + cos_Omega * sin(dOmega), 0.0);
	M = _V(-cos_i*(sin_Omega*cos(dOmega) + cos_Omega * sin(dOmega)), cos_i*(cos_Omega*cos(dOmega) - sin_Omega * sin(dOmega)), sin_i);
	G = N * (Gdot0.z*rr - G0.z*d) / n + M * G0.z*length(crossp(G0, Gdot0)) / n;
	Gdot = N * (Gdot0.z*d - G0.z*vv) / n + M * Gdot0.z*length(crossp(G0, Gdot0)) / n;
	LIBRAT(G, Gdot, GMTF, 6);
	RF = G;
	VF = Gdot;

	return false;
}

bool TLMCCProcessor::DGAMMA(double r0, double ainv, double gamma, double &H, double &E, double &beta, double &e)
{
	double c;

	H = 0.0;
	E = 0.0;
	e = 1.0 - r0 * ainv;
	c = sqrt(abs(2.0*r0*ainv - r0 * r0*ainv*ainv));

	if (ainv < 0)
	{
		H = log(c / e * tan(gamma) + sqrt(1.0 + pow(c*tan(gamma) / e, 2)));
		beta = H * sqrt(abs(1.0 / ainv));
	}
	else if (ainv > 0)
	{
		double sin_E = c * tan(gamma) / e;
		if (abs(sin_E) > 1.0)
		{
			return true;
		}
		E = atan(sin_E / sqrt(1.0 - sin_E * sin_E));
		beta = E * sqrt(1.0 / ainv);
	}
	else
	{
		beta = sin(gamma) / cos(gamma)*sqrt(2.0*r0);
	}

	return false;
}

void TLMCCProcessor::BURN(VECTOR3 R, VECTOR3 V, double dv, double dgamma, double dpsi, double isp, double &mfm0, VECTOR3 &RF, VECTOR3 &VF)
{
	double v_c, dv_r;
	BURN(R, V, 3, 0.0, 0.0, dv, dgamma, dpsi, isp, 0.0, v_c, dv_r, mfm0, RF, VF);
}

void TLMCCProcessor::BURN(VECTOR3 R, VECTOR3 V, int opt, double gamma0, double v_pl, double dv, double dgamma, double dpsi, double isp, double mu, double &v_c, double &dv_R, double &mfm0, VECTOR3 &RF, VECTOR3 &VF)
{
	double r, v;
	RF = R;

	r = length(R);
	v = length(V);

	if (opt == 1)
	{
		v_c = sqrt(mu / r);
		dv = v_c - v;
		dgamma = -gamma0;
	}
	else if (opt == 2)
	{
		dv = v_pl - v;
		dgamma = -gamma0;
	}

	VECTOR3 Rdot1, Rdot2;
	double d, h;

	d = dotp(R, V);
	h = length(crossp(R, V));
	Rdot1 = V * cos(dgamma) + (R*v*v - V * d) / h * sin(dgamma);
	Rdot2 = R * 2.0*dotp(R, Rdot1) / r / r * pow(sin(dpsi / 2.0), 2) + Rdot1 * cos(dpsi) - crossp(R, Rdot1) / r * sin(dpsi);
	VF = Rdot2 * (1.0 + dv / v);
	dv_R = sqrt(dv*dv + 4.0*v*(v + dv)*(pow(sin(dgamma / 2.0), 2) + (h*h*cos(dgamma) - h * d*sin(dgamma)) / (r*r*v*v)*pow(sin(dpsi / 2.0), 2)));
	mfm0 = exp(-dv_R / isp);
}

void TLMCCProcessor::RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double&psi)
{
	if (vecinp)
	{
		r = length(R);
		v = length(V);
		phi = asin(R.z / r);
		theta = atan2(R.y, R.x);
		if (theta < 0)
		{
			theta += PI2;
		}
		gamma = asin(dotp(R, V) / r / v);
		psi = atan2(R.x*V.y - R.y*V.x, V.z*r - R.z*dotp(R, V) / r);
		//if (psi < 0)
		//{
		//	psi += PI2;
		//}
	}
	else
	{
		R = _V(cos(phi)*cos(theta), cos(phi)*sin(theta), sin(phi))*r;
		V = mul(_M(cos(phi)*cos(theta), -sin(theta), -sin(phi)*cos(theta), cos(phi)*sin(theta), cos(theta), -sin(phi)*sin(theta), sin(phi), 0, cos(phi)), _V(sin(gamma), cos(gamma)*sin(psi), cos(gamma)*cos(psi))*v);
	}
}

double TLMCCProcessor::MCOMP(double dv, bool docked, bool useSPS, double m0)
{
	if (docked == false || useSPS)
	{
		return m0 * exp(-abs(dv) / isp_SPS);
	}
	else
	{
		return m0 * exp(-abs(dv) / isp_DPS);
	}
}

void TLMCCProcessor::RNTSIM(VECTOR3 R, VECTOR3 V, double GMT, double lng_L, double &lat, double &lng, double &dlng)
{
	MATRIX3 Rot;
	VECTOR3 P, S, S_equ;
	double r, v, theta;

	r = length(R);
	v = length(V);

	P = V / v / cos(gamma_reentry) - R / r * tan(gamma_reentry);
	theta = Reentry_range / 3443.933585;
	S = R / r * cos(theta) + P * sin(theta);
	Rot = OrbMech::GetRotationMatrix(BODY_EARTH, MEDQuantities.GMTBase + (GMT + Reentry_dt) / 24.0 / 3600.0);
	S_equ = rhtmul(Rot, S);
	OrbMech::latlong_from_r(S_equ, lat, lng);
	if (lng < 0)
	{
		lng += PI2;
	}
	dlng = lng - lng_L;
	if (dlng > PI)
	{
		dlng -= PI2;
	}
	if (dlng < -PI)
	{
		dlng += PI2;
	}
}

void TLMCCProcessor::LOPC(VECTOR3 R0, VECTOR3 V0, double GMT0, VECTOR3 L, int m, int n, double P, VECTOR3 &R3, VECTOR3 &V3, double &GMT3, double &mfm0, double &dpsi, VECTOR3 &DV)
{
	VECTOR3 R1, V1, R2, V2;
	double dt1, GMT1, GMT2, dt2;

	dt1 = P * (-0.25 + (double)m);
	GMT1 = GMT0 + dt1;
	CTBODY(R0, V0, GMT0, GMT1, 2, mu_M, R1, V1);
	dt2 = P * (double)(m + n);
	GMT2 = GMT0 + dt2;
	CTBODY(R0, V0, GMT0, GMT2, 2, mu_M, R2, V2);
	LIBRAT(R2, V2, GMT2, 5);
	dpsi = -asin(dotp(unit(crossp(R2, V2)), L));
	BURN(R1, V1, 0.0, 0.0, dpsi, isp_MCC, mfm0, R3, V3);
	DV = V3 - V1;
	GMT3 = GMT1;
}

void TLMCCProcessor::PRCOMP(VECTOR3 u_pc, VECTOR3 h_pc, double GMT_nd, double &RA_LPO1, double &A1, double &E1, double &gamma1, double &V_L, double &gamma_L, double &V2, double &DT_1st_pass)
{
	EphemerisData sv_LLS, sv_DOI, sv_DOI2, sv_preDOI, sv_1I;
	VECTOR3 u_lls_equ, u_lls, R_LLS_sg, V_LLS_sg, U, H;
	double R1, DR1, R2, RP_LPO1, a_lls, e_lls, a1, e1, dt1, dt2, dt3, eta2, da, DA, dw, dt, alpha1, deta, a, e, i, eta, n, P;
	int ITS;
	bool recycle = false;

	sv_LLS.RBI = BODY_MOON;

	//TIME from LOI targeting
	dw = MEDQuantities.site_rotation_LPO2;
	DR1 = modf(MEDQuantities.Revs_LPO1, &R1)*PI2;
	R2 = (double)MEDQuantities.Revs_LPO2;

	RA_LPO1 = MEDQuantities.H_A_LPO1 + DataTable.rad_lls;
	RP_LPO1 = MEDQuantities.H_P_LPO1 + DataTable.rad_lls;
	a_lls = DataTable.rad_lls + (MEDQuantities.H_A_LPO2 + MEDQuantities.H_P_LPO2) / 2.0;
	e_lls = (MEDQuantities.H_A_LPO2 + DataTable.rad_lls) / a_lls - 1.0;

	a1 = (RA_LPO1 + RP_LPO1) / 2.0;
	e1 = RA_LPO1 / a1 - 1.0;

	dt2 = DELTAT(a1, e1, MEDQuantities.TA_LOI, DR1);
	eta2 = MEDQuantities.TA_LOI + DR1;
	while (eta2 < -PI)
	{
		eta2 += PI2;
	}
	while (eta2 > PI)
	{
		eta2 -= PI2;
	}

	dt1 = PI2 * R1*sqrt(pow(a1, 3) / mu_M) + PI2 * R2*sqrt(pow(a_lls, 3) / mu_M);
	sv_LLS.GMT = GMT_nd + dt1 + dt2;
	u_lls_equ = OrbMech::r_from_latlong(DataTable.lat_lls, DataTable.lng_lls);
LOI_PRCOMP2_TIME:
	u_lls = u_lls_equ;
	LIBRAT(u_lls, sv_LLS.GMT, 6);
	da = acos(dotp(u_lls, u_pc));
	if (dotp(crossp(u_lls, u_pc), h_pc) > 0)
	{
		da = PI2 - da;
	}
	if (da > DR1)
	{
		DA = da - DR1;
	}
	else
	{
		DA = PI2 - (DR1 - da);
	}
	dt3 = DELTAT(a_lls, e_lls, dw < 0 ? -dw : PI2 - dw, -DA);
	sv_LLS.GMT = GMT_nd + (dt1 + dt2 - dt3);
	if (recycle == false)
	{
		recycle = true;
		goto LOI_PRCOMP2_TIME;
	}

	//BACKUP from LOI targeting
	//compute LPO at LLS
	VECTOR3 U_PJ;
	double p_lls, r, v, cos_gamma, gamma, a_L, e_L, R_L,  theta_L, phi_L, psi_L, V1, A, E, dt_CORR;
	p_lls = a_lls * (1.0 - e_lls * e_lls);
	r = p_lls / (1.0 + e_lls * cos(dw));
	v = sqrt(mu_M*(2.0 / r - 1.0 / a_lls));
	cos_gamma = sqrt(mu_M*p_lls) / r / v;
	if (cos_gamma > 1.0)
	{
		cos_gamma = 1.0;
	}
	gamma = -OrbMech::sign(dw)*acos(cos_gamma);
	RVIO(false, R_LLS_sg, V_LLS_sg, r, v, DataTable.lng_lls, DataTable.lat_lls, gamma, DataTable.psi_lls);
	recycle = false;
LOI_PRCOMP2_BACKUP:
	sv_LLS.R = R_LLS_sg;
	sv_LLS.V = V_LLS_sg;
	LIBRAT(sv_LLS.R, sv_LLS.V, sv_LLS.GMT, 6);
	sv_DOI.GMT = sv_LLS.GMT + (dt3 - R2 * PI2*sqrt(pow(a_lls, 3) / mu_M));
	dt = sv_DOI.GMT - sv_LLS.GMT;
	pRTCC->PMMCEN(sv_LLS, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_DOI, ITS);
	ELEMT(sv_DOI.R, sv_DOI.V, mu_M, H, a, e, i, n, P, eta);
	alpha1 = acos(dotp(unit(sv_DOI.R), u_lls));
	U = unit(H);
	if (dotp(crossp(unit(sv_DOI.R), u_lls), U) <= 0.0)
	{
		alpha1 = PI2 - alpha1;
	}
	deta = alpha1 - DA;
	dt = DELTAT(a, e, eta, deta);
	pRTCC->PMMCEN(sv_DOI, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_DOI2, ITS);
	ELEMT(sv_DOI2.R, sv_DOI2.V, mu_M, H, a_L, e_L, i, n, P, eta);
	RVIO(true, sv_DOI2.R, sv_DOI2.V, R_L, V_L, theta_L, phi_L, gamma_L, psi_L);
	r = R_L - MEDQuantities.dh_bias;
	E = (RA_LPO1 - r) / (RA_LPO1 + r *cos(eta2));
	A = RA_LPO1 / (1.0 + E);
	V1 = sqrt(mu_M*(2.0 / r - 1.0 / A));
	V2 = V1;
	cos_gamma = sqrt(mu_M*A*(1.0 - E * E)) / (r * V1);
	if (cos_gamma > 1.0)
	{
		cos_gamma = 1.0;
	}
	gamma1 = OrbMech::sign(eta2)*acos(cos_gamma);
	sv_preDOI = sv_DOI2;
	RVIO(false, sv_preDOI.R, sv_preDOI.V, r, V1, theta_L, phi_L, gamma1, psi_L);
	dt = GMT_nd - sv_DOI2.GMT;
	pRTCC->PMMCEN(sv_preDOI, 0.0, 10.0*24.0*3600.0, 1, dt, 1.0, sv_1I, ITS);
	ELEMT(sv_1I.R, sv_1I.V, mu_M, H, a, e, i, n, P, eta);
	U = unit(H);
	U_PJ = unit(u_pc - U * dotp(u_pc, U));
	deta = OrbMech::sign(dotp(crossp(U_PJ, unit(sv_1I.R)), U))*acos(dotp(U_PJ, unit(sv_1I.R)));
	dt_CORR = DELTAT(a, e, eta, deta);
	if (recycle == false)
	{
		recycle = true;
		sv_LLS.GMT += dt_CORR;
		u_lls = u_lls_equ;
		LIBRAT(u_lls, sv_LLS.GMT, 6);
		goto LOI_PRCOMP2_BACKUP;
	}
	A1 = a;
	E1 = e;
	DT_1st_pass = sv_LLS.GMT - GMT_nd;

	outarray.sv_lls1.R = sv_LLS.R;
	outarray.sv_lls1.V = sv_LLS.V;
	outarray.sv_lls1.GMT = sv_LLS.GMT;

	outarray.SGSLOI.R = sv_1I.R;
	outarray.SGSLOI.V = sv_1I.V;
	LIBRAT(outarray.SGSLOI.R, outarray.SGSLOI.V, GMT_nd, 5);
}

void TLMCCProcessor::ELEMT(VECTOR3 R, VECTOR3 V, double mu, VECTOR3 &H, double &a, double &e, double &i, double &n, double &P, double &eta)
{
	double ainv, r, v, h;

	r = length(R);
	v = length(V);
	ainv = 2.0 / r - v * v / mu;
	a = 1.0 / ainv;
	e = sqrt(pow(1.0 - r * ainv, 2) + pow(dotp(R, V), 2)*ainv / mu);
	H = crossp(R, V);
	h = length(H);
	i = acos(H.z / h);
	n = pow(mu, 0.5) / pow(abs(a), 1.5);
	if (e != 0.0)
	{
		eta = atan2(h*dotp(R, V), pow(h, 2) - mu * r);
		if (eta < 0)
		{
			eta += PI2;
		}
	}
	else
	{
		eta = 0.0;
	}
	if (e < 1.0)
	{
		P = PI2 / n;
	}
}

void TLMCCProcessor::SCALE(VECTOR3 R0, VECTOR3 V0, double h, VECTOR3 &RF, VECTOR3 &VF)
{
	VECTOR3 H;

	RF = R0 * ((h + DataTable.rad_lls) / length(R0));
	H = unit(crossp(R0, V0));
	VF = unit(crossp(H, RF))*sqrt(mu_M / (h + DataTable.rad_lls));
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

EphemerisData TLMCCProcessor::TLIBRN(EphemerisData sv, double C3, double sigma, double delta, double FW, double W_I, double F_I, double F, double W_dot, double T_MRS)
{
	EphemerisData out;
	double DV_I, DT_B1, V_I, R_I, phi_dot_I, dphi_B1, DT_B2, dphi_B2, delta0, ddelta, W;

	static const double mu = OrbMech::mu_Earth*pow(3600.0, 2) / pow(6378.165*1000.0, 3);

	R_I = length(sv.R) / 6378.165;
	V_I = length(sv.V)*3600.0 / 6378.165;
	DV_I = sqrt(mu / R_I) - V_I;
	DT_B1 = DV_I / (F / W_I);
	phi_dot_I = V_I / R_I;
	dphi_B1 = phi_dot_I * DT_B1;
	DT_B2 = (1.0 - F_I / F)*T_MRS;
	dphi_B2 = phi_dot_I * DT_B2;
	W = W_I - W_dot * DT_B1;

	if (C3 < -45.0)
	{
		C3 = -45.0;
	}
	else if (C3 > -0.5)
	{
		C3 = -0.5;
	}
	if (delta > 2.0*RAD)
	{
		delta0 = 2.0*RAD;
		ddelta = delta - 2.0*RAD;
	}
	else if (delta < -2.0*RAD)
	{
		delta0 = -2.0*RAD;
		ddelta = -2.0*RAD - delta;
	}
	else
	{
		delta0 = delta;
		ddelta = 0.0;
	}

	double alpha0, beta0, etaalpha0, R_p0, DV0;

	if (C3 > -5.0)
	{
		//Nominal mission polynomial
		double a[5][26];
		
		a[0][0] = 0.61967804e-1;
		a[0][1] = 0.86219648e-2;
		a[0][2] = -0.2437182e2;
		a[0][3] = 0.41004848e4;
		a[0][4] = -0.99229657e6;
		a[0][5] = 0.14267564e9;
		a[0][6] = -0.54688962;
		a[0][7] = -0.78766288;
		a[0][8] = 0.10261969e2;
		a[0][9] = 0.52445599e1;
		a[0][10] = -0.15527983e5;
		a[0][11] = 0.51931839e7;
		a[0][12] = 0.18005069;
		a[0][13] = 0.97069489e-1;
		a[0][14] = 0.61442230e1;
		a[0][15] = -0.87765197e3;
		a[0][16] = -0.16502383;
		a[0][17] = 0.63224468;
		a[0][18] = 0.81844028e3;
		a[0][19] = -0.33505204;
		a[0][20] = -0.92426341e-1;
		a[0][21] = -0.18131458e4;
		a[0][22] = -0.39193696e4;

		a[1][0] = 0.51541772;
		a[1][1] = -0.15528032;
		a[1][2] = 0.27185659e2;
		a[1][3] = 0.18763984e3;
		a[1][4] = -0.92712145e6;
		a[1][5] = 0.21114994e9;
		a[1][6] = -0.56424215;
		a[1][7] = 0.95105384e1;
		a[1][8] = -0.15294910e2;
		a[1][9] = 0.33896643e2;
		a[1][10] = -0.26903240e5;
		a[1][11] = 0.12131396e8;
		a[1][12] = 0.25371175;
		a[1][13] = 0.22036833;
		a[1][14] = -0.22601576e2;
		a[1][15] = 0.14378586e4;
		a[1][16] = 0.31264540;
		a[1][17] = -0.64046690e1;
		a[1][18] = -0.39254760e4;
		a[1][19] = -0.57987931;
		a[1][20] = -0.2290591;
		a[1][21] = 0.12621438e4;
		a[1][22] = 0.70516077e4;
		a[1][23] = -0.76940409e-4;
		a[1][24] = 0.64393915e-4;
		a[1][25] = 0.48483478e-4;

		a[2][0] = 0.48329414;
		a[2][1] = 0.18759385e-2;
		a[2][2] = 0.14031932e1;
		a[2][3] = -0.13933485e3;
		a[2][4] = 0.40515931e5;
		a[2][5] = -0.48676865e7;
		a[2][6] = -0.10155877e1;
		a[2][7] = 0.83266987e-1;
		a[2][8] = -0.28021958e1;
		a[2][9] = 0.21207686;
		a[2][10] = 0.98814614e3;
		a[2][11] = -0.17699125e6;
		a[2][12] = 0.30964851;
		a[2][13] = 0.13152495;
		a[2][14] = 0.92808415;
		a[2][15] = -0.32524984e2;
		a[2][16] = 0.44675108e-2;
		a[2][17] = -0.59053312e-3;
		a[2][18] = -0.10061669e3;
		a[2][19] = -0.60405621;
		a[2][20] = 0.96317404e-2;
		a[2][21] = 0.18026336e3;
		a[2][22] = 0.81684373e2;

		double DV_M, DI, X1, X2, X3, X4, X5;

		DV_M = sqrt(C3 + 2.0*mu / R_I) - sqrt(mu / R_I);
		DI = asin(sin(abs(delta0)) / sin(sigma + 0.314));
		X1 = DV_M* - 1.75;
		X2 = DI * DI - 0.0027;
		X3 = sigma - 0.148;
		X4 = 1.0 / (F / W);
		X5 = R_I;

		double Y[5];

		for (int i = 0;i < 5;i++)
		{
			Y[i] = a[i][0] + a[i][1] * X1 + a[i][2] * X2 + a[i][3] * X2*X2 + a[i][4] * pow(X2, 3) + a[i][5] * pow(X2, 4) + a[i][6] * X3 + a[i][7] * pow(X3, 2) + a[i][8] * X1*X2 +
				a[i][9] * X2*X3 + a[i][10] * pow(X2, 2)*X3 + a[i][11] * pow(X2, 3)*X3 + a[i][12] * X4*a[i][13] * X1*X4 + a[i][14] * X2*X4 +
				a[i][15] * X2*X2*X4 + a[i][16] * X3*X4 + a[i][17] * X3*X3*X4 + a[i][18] * X2*X2*X3*X4 + a[i][19] * X5 + a[i][20] * X1*X3 +
				a[i][21] * X1*X2*X2 + a[i][22] * X1*X2*X2*X3;
		}

		alpha0 = Y[0];
		beta0 = Y[1] + (X4*X4*(a[1][23] + a[1][24] * X1 + a[1][25] * X4*X4)) / (pow(X3 + 0.148, 2) + 4.0*pow(X2 + 0.0027, 2));
		etaalpha0 = Y[2];
		R_p0 = Y[3];
		DV0 = Y[4];
	}
	else
	{
		//Alternate mission polynomial
	}

	VECTOR3 R_I_u, N_I_u, S, T, N_c;
	double alpha, beta, etaalpha, R_p, DV, eta, T_B, C1, p, e, R_c, V_c, gamma_c;

	alpha = alpha0 + 4.66*ddelta;
	beta = beta0 - 2.15*ddelta;
	etaalpha = etaalpha0 + 0.923*ddelta;
	R_p = R_p0 - 0.442*ddelta;
	DV = DV0 + 6.33*ddelta;

	alpha += dphi_B1 + dphi_B2;
	beta += 3.0 / 4.0*dphi_B1 + dphi_B2;
	eta = etaalpha - alpha;
	DV += DV + DV_I;

	T_B = W_I / W_dot * (1.0 - exp(-(DV*W_dot / F))) + DT_B2;

	R_I_u = unit(sv.R);
	N_I_u = unit(crossp(sv.R, sv.V));
	T = R_I_u * cos(delta)*cos(alpha) + crossp(N_I_u, R_I_u)*cos(delta)*sin(alpha) + N_I_u * sin(delta);
	S = R_I_u * cos(beta) + crossp(N_I_u, R_I_u)*sin(beta);
	N_c = unit(crossp(T, S));
	C1 = R_p * sqrt(2.0*mu / R_p + C3);
	p = C1 * C1 / mu;
	e = sqrt(1.0 + C3 / mu * p);
	R_c = p / (1.0 + e * cos(eta));
	V_c = mu / C1 * sqrt(1.0 + 2.0*e*cos(eta) + e * e);
	gamma_c = atan2(e*sin(eta), 1.0 + e * cos(eta));
	out.R = (T*cos(sigma + eta) + crossp(N_c, T)*sin(sigma + eta))*R_c;
	out.V = (-T * sin(sigma + eta - gamma_c) + crossp(N_c, T)*cos(sigma + eta - gamma_c))*V_c;
	out.GMT = sv.GMT + T_B;
	out.RBI = sv.RBI;

	return out;
}

double TLMCCProcessor::DELTAT(double a, double e, double eta, double deta)
{
	double K_e, E, eta_apo, dE, E_apo, DE, DM;

	if (eta < 0)
	{
		eta += PI2;
	}
	K_e = sqrt((1.0 - e) / (1.0 + e));
	E = atan2(sqrt(1.0 - e * e)*sin(eta), cos(eta) + e);
	if (E < 0)
	{
		E += PI2;
	}
	eta_apo = eta + deta;
	if (eta_apo >= 0 && eta_apo < PI2)
	{
		dE = 0.0;
	}
	else
	{
		dE = 0.0;
		if (eta_apo < 0)
		{
			while (eta_apo < 0)
			{
				eta_apo += PI2;
				dE += PI2;
			}
			dE = -(dE); //plus PI2???
		}
		else
		{
			while (eta_apo >= PI2)
			{
				eta_apo -= PI2;
				dE += PI2;
			}
		}
	}
	E_apo = atan2(sqrt(1.0 - e * e)*sin(eta_apo), cos(eta_apo) + e);
	while (E_apo < 0)
	{
		E_apo += PI2;
	}
	while (E_apo >= PI2)
	{
		E_apo -= PI2;
	}
	DE = E_apo - E + dE;
	DM = DE + e * (sin(E) - sin(E_apo));
	return sqrt(pow(a, 3) / OrbMech::mu_Moon)*DM;
}