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
#include "TLMCC.h"

TLMCCProcessor::TLMCCProcessor()
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

void TLMCCProcessor::Init(PZEFEM *ephem, TLMCCDataTable data, TLMCCMEDQuantities med, TLMCCMissionConstants cst)
{
	ephemeris = ephem;
	
	hMoon = oapiGetObjectByName("Moon");

	DataTable = data;
	MEDQuantities = med;
	Constants = cst;

	outarray.sv_lls1.gravref = hMoon;
	outarray.sv_loi.gravref = hMoon;
	outarray.SGSLOI.gravref = hMoon;
	outarray.sv_lls2.gravref = hMoon;
}

void TLMCCProcessor::Main(TLMCCOutputData &out)
{
	sv_MCC = OrbMech::coast(MEDQuantities.sv0, MEDQuantities.T_MCC - OrbMech::GETfromMJD(MEDQuantities.sv0.MJD, MEDQuantities.GMTBase));
	if (sv_MCC.gravref == oapiGetObjectByName("Earth"))
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
		MPTSV sv_MCC_apo;
		sv_MCC_apo = sv_MCC;
		sv_MCC_apo.V += DV_MCC;
		DV_LOI = CalcLOIDV(sv_MCC_apo, outarray.gamma_nd);
	}
	//Calc MPT parameters
	out.R_MCC = sv_MCC.R;
	out.V_MCC = sv_MCC.V;
	out.GMT_MCC = OrbMech::GETfromMJD(sv_MCC.MJD, MEDQuantities.GMTBase);
	if (sv_MCC.gravref == oapiGetObjectByName("Earth"))
	{
		out.RBI = BODY_EARTH;
	}
	else
	{
		out.RBI = BODY_MOON;
	}
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
	out.display.GET_MCC = OrbMech::GETfromMJD(sv_MCC.MJD, MEDQuantities.GETBase) + T_start;
	out.display.DV_MCC = DV_MCC;
	out.display.h_PC = outarray.h_pl;
	
	if (out.display.Mode <= 5)
	{
		dv = length(DV_LOI);
		T_start = -outarray.M_mcc / Wdot * (1.0 + (exp(-dv / isp_MCC) - 1.0) / (dv / isp_MCC));
		out.display.GET_LOI = OrbMech::GETfromMJD(outarray.MJD_nd, MEDQuantities.GETBase) + T_start;
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
		out.display.GET_TEI = OrbMech::GETfromMJD(outarray.MJD_tei, MEDQuantities.GETBase) + T_start;
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
			out.display.GET_LC = OrbMech::GETfromMJD(outarray.MJD_ip, MEDQuantities.GETBase);
			out.display.lat_IP = outarray.lat_ip;
			out.display.lng_IP = outarray.lng_ip;
			out.display.v_EI = outarray.v_EI;
		}
		else
		{
			out.display.GET_LC = OrbMech::GETfromMJD(outarray.MJD_ip_pr, MEDQuantities.GETBase);
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
	out.display.GMTV = OrbMech::GETfromMJD(MEDQuantities.sv0.MJD, MEDQuantities.GMTBase);
	out.display.GETV = OrbMech::GETfromMJD(MEDQuantities.sv0.MJD, MEDQuantities.GETBase);
	out.display.CSMWT = MEDQuantities.CSMMass;
	out.display.LMWT = MEDQuantities.LMMass;

	VECTOR3 X_PHV, Y_PHV, Z_PHV;
	Z_PHV = unit(-sv_MCC.R);
	Y_PHV = unit(crossp(sv_MCC.V, sv_MCC.R));
	X_PHV = crossp(Y_PHV, Z_PHV);
	out.display.YAW_MCC = atan(dotp(Y_PHV, unit(DV_MCC)) / dotp(X_PHV, unit(DV_MCC)));

	out.outtab = outtab;
}

VECTOR3 TLMCCProcessor::CalcLOIDV(MPTSV sv_MCC_apo, double gamma_nd)
{
	VECTOR3 U1, U2, RF, VF;
	double mfm0;

	MPTSV sv_nd = OrbMech::PMMCEN(sv_MCC_apo, 0.0, 10.0*24.0*3600.0, 2, sin(gamma_nd), 1.0);
	VECTOR3 h1 = unit(crossp(sv_nd.R, sv_nd.V));
	double R = 1.0;
	double V = 1.0;
	double gamma = 0.0;
	RVIO(false, U1, U2, R, V, DataTable.lng_lls, DataTable.lat_lls, gamma, outarray.AZ_act);
	LIBRAT(U1, U2, sv_nd.MJD + DataTable.dt_lls / 24.0 / 3600.0, 6);
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
	double GMT_pl3 = OrbMech::GETfromMJD(outarray.MJD_pl, MEDQuantities.GMTBase) / 3600.0;
	double gamma_loi3 = outarray.gamma_nd;
	double dt_lls3 = outarray.dt_lls;

	double dt = -OrbMech::period(outarray.sv_lls1.R, outarray.sv_lls1.V, mu_M)*(double)(MEDQuantities.Revs_circ + 1);
	MPTSV sv_circ;
	OrbMech::oneclickcoast(outarray.sv_lls1.R, outarray.sv_lls1.V, outarray.sv_lls1.MJD, dt, sv_circ.R, sv_circ.V, hMoon, hMoon);
	sv_circ.MJD = outarray.sv_lls1.MJD + dt / 24.0 / 3600.0;
	dt = OrbMech::time_radius(sv_circ.R, sv_circ.V, DataTable.rad_lls+MEDQuantities.H_T_circ, 1.0, mu_M);
	OrbMech::oneclickcoast(sv_circ.R, sv_circ.V, sv_circ.MJD, dt, sv_circ.R, sv_circ.V, hMoon, hMoon);
	sv_circ.MJD = sv_circ.MJD + dt / 24.0 / 3600.0;
	sv_circ.V = unit(sv_circ.V)*sqrt(mu_M / (DataTable.rad_lls + MEDQuantities.H_T_circ));
	sv_circ.gravref = hMoon;

	double DV_PPC;
	MPTSV sv_lls2 = PPC(sv_circ, DataTable.lat_lls, DataTable.lng_lls, DataTable.psi_lls, Constants.n, 0, DV_PPC);
	outarray.sv_lls2.R = sv_lls2.R;
	outarray.sv_lls2.V = sv_lls2.V;
	outarray.sv_lls2.MJD = sv_lls2.MJD;
	outarray.M_lopc = MCOMP(DV_PPC, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_cir);

	double lat_TEI, lng_TEI, MJD_TEI, dlng, mfm0;
	MPTSV sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;

	MJD_TEI = outarray.MJD_nd + DataTable.T_lo / 24.0 / 3600.0;
	sv_TEI1 = OrbMech::coast(outarray.sv_lls2, (MJD_TEI - outarray.sv_lls2.MJD)*24.0*3600.0);
	sv_TEI2 = sv_TEI1;

	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.MJD, 4);
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
		sv_TEI2 = OrbMech::coast(sv_TEI2, dt);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp;

	dt_LPO = (sv_TEI2.MJD - sv_TEI1.MJD)*24.0*3600.0;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.MJD, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI2)
	{
		dpsi_tei -= PI2;
	}

	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
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
	double GMT_pl6 = OrbMech::GETfromMJD(outarray.MJD_pl, MEDQuantities.GMTBase) / 3600.0;
	//With the trajectory computer, the time used for the state vector at the start of LPO is biased by the difference between the time of perilune passage
	//in step 6 (integrated) and step 3 (conic).
	outarray.dt_bias_conic_prec = GMT_pl6 - GMT_pl3;
	VECTOR3 DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	VECTOR3 DV_temp = VF - sv_MCC.V;

	MPTSV S_apo;
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

	outtab.GMT_pc2 = OrbMech::GETfromMJD(outarray.MJD_pl, MEDQuantities.GMTBase);
	outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc2 = outarray.lng_pl;
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

	MPTSV sv_MCC_apo = sv_MCC;
	sv_MCC_apo.V = VF;
	//Step 9
	MPTSV sv_nd = OrbMech::PMMCEN(sv_MCC_apo, 0.0, 10.0*24.0*3600.0, 2, sin(outarray.gamma_nd), 1.0);
	LIBRAT(sv_nd.R, sv_nd.MJD, 4);
	OrbMech::latlong_from_r(sv_nd.R, outtab.lat_nd, outtab.lng_nd);
	if (outtab.lng_nd < 0)
	{
		outtab.lng_nd += PI2;
	}
	outtab.h_nd = length(sv_nd.R) - DataTable.rad_lls;
	outtab.GMT_nd = OrbMech::GETfromMJD(sv_nd.MJD, MEDQuantities.GMTBase);
	outarray.MJD_nd = sv_nd.MJD;

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
	double GMT_pl3 = OrbMech::GETfromMJD(outarray.MJD_pl, MEDQuantities.GMTBase) / 3600.0;
	double gamma_loi3 = outarray.gamma_nd;
	double dpsi_loi3 = outarray.dpsi_loi;
	double dt_lls3 = outarray.dt_lls;

	double lat_TEI, lng_TEI, MJD_TEI, dlng, dt;
	MPTSV sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;

	MJD_TEI = outarray.MJD_nd + DataTable.T_lo / 24.0 / 3600.0;
	sv_TEI1 = OrbMech::coast(outarray.sv_lls2, (MJD_TEI - outarray.sv_lls2.MJD)*24.0*3600.0);
	sv_TEI2 = sv_TEI1; 
	
	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.MJD, 4);
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
		sv_TEI2 = OrbMech::coast(sv_TEI2, dt);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp, mfm0;

	dt_LPO = (sv_TEI2.MJD - sv_TEI1.MJD)*24.0*3600.0;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.MJD, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI2)
	{
		dpsi_tei -= PI2;
	}

	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
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
	double GMT_pl6 = OrbMech::GETfromMJD(outarray.MJD_pl, MEDQuantities.GMTBase) / 3600.0;
	//With the trajectory computer, the time used for the state vector at the start of LPO is biased by the difference between the time of perilune passage
	//in step 6 (integrated) and step 3 (conic).
	outarray.dt_bias_conic_prec = GMT_pl6 - GMT_pl3;
	VECTOR3 DV6 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(sv_MCC.R, sv_MCC.V, DV6.x*R_E / 3600.0, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	VECTOR3 DV_temp = VF - sv_MCC.V;

	MPTSV S_apo;
	VECTOR3 NewGuess, DV7, DV8;
	double r, v1, theta, phi, gamma1, psi1, v2, gamma2, psi2, v_c, dv_char7, dv_char8;

TLMCC_Option_3_E:

	S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;

	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	//Step 7
	ConicFullMissionFreeOrbit(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, gamma_loi3, dpsi_loi3, dt_lls3 / 3600.0, outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, outarray.T_te, MEDQuantities.AZ_min, MEDQuantities.AZ_max, outarray.M_tei, true);
	DV7 = _V(outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc);
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV7.x*R_E / 3600.0, DV7.y, DV7.z, isp_MCC, 0.0, v_c, dv_char7, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V((v2 - v1)*3600.0 / R_E, gamma2 - gamma1, psi2 - psi1);

	outtab.GMT_pc2 = OrbMech::GETfromMJD(outarray.MJD_pl, MEDQuantities.GMTBase);
	outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc2 = outarray.lng_pl;
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

	MPTSV sv_MCC_apo = sv_MCC;
	sv_MCC_apo.V = VF;
	//Step 9
	MPTSV sv_nd = OrbMech::PMMCEN(sv_MCC_apo, 0.0, 10.0*24.0*3600.0, 2, sin(outarray.gamma_nd), 1.0);
	LIBRAT(sv_nd.R, sv_nd.MJD, 4);
	OrbMech::latlong_from_r(sv_nd.R, outtab.lat_nd, outtab.lng_nd);
	if (outtab.lng_nd < 0)
	{
		outtab.lng_nd += PI2;
	}
	outtab.h_nd = length(sv_nd.R) - DataTable.rad_lls;
	outtab.GMT_nd = OrbMech::GETfromMJD(sv_nd.MJD, MEDQuantities.GMTBase);
	outarray.MJD_nd = sv_nd.MJD;
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
		double MJD_PC = OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase);
		double MJD_TLI = OrbMech::MJDfromGET(DataTable.GET_TLI + 328.0, MEDQuantities.GETBase);
		if ((sv_MCC.MJD - MJD_TLI)*24.0 > 20.0)
		{
			ddt = 0.0;
		}
		else
		{
			EPHEM(MJD_PC, R_EM, V_EM, R_ES);
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
	double GMT_nd2 = OrbMech::GETfromMJD(outarray.MJD_nd, MEDQuantities.GMTBase) / 3600.0;
	double gamma_loi2 = outarray.gamma_nd;
	double dt_lls2 = outarray.dt_lls;

	double dt = -OrbMech::period(outarray.sv_lls1.R, outarray.sv_lls1.V, mu_M)*(double)(MEDQuantities.Revs_circ + 1);
	MPTSV sv_circ;
	OrbMech::oneclickcoast(outarray.sv_lls1.R, outarray.sv_lls1.V, outarray.sv_lls1.MJD, dt, sv_circ.R, sv_circ.V, hMoon, hMoon);
	sv_circ.MJD = outarray.sv_lls1.MJD + dt / 24.0 / 3600.0;
	dt = OrbMech::time_radius(sv_circ.R, sv_circ.V, DataTable.rad_lls + MEDQuantities.H_T_circ, 1.0, mu_M);
	OrbMech::oneclickcoast(sv_circ.R, sv_circ.V, sv_circ.MJD, dt, sv_circ.R, sv_circ.V, hMoon, hMoon);
	sv_circ.MJD = sv_circ.MJD + dt / 24.0 / 3600.0;
	sv_circ.V = unit(sv_circ.V)*sqrt(mu_M / (DataTable.rad_lls + MEDQuantities.H_T_circ));
	sv_circ.gravref = hMoon;

	double DV_PPC;
	MPTSV sv_lls2 = PPC(sv_circ, DataTable.lat_lls, DataTable.lng_lls, DataTable.psi_lls, Constants.n, 0, DV_PPC);
	outarray.DV_LOPC = _V(DV_PPC, 0, 0);
	outarray.sv_lls2.R = sv_lls2.R;
	outarray.sv_lls2.V = sv_lls2.V;
	outarray.sv_lls2.MJD = sv_lls2.MJD;
	outarray.M_lopc = MCOMP(DV_PPC, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_cir) - MEDQuantities.LMMass;

	double lat_TEI, lng_TEI, MJD_TEI, dlng, mfm0;
	MPTSV sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;

	MJD_TEI = outarray.MJD_nd + DataTable.T_lo / 24.0 / 3600.0;
	sv_TEI1 = OrbMech::coast(outarray.sv_lls2, (MJD_TEI - outarray.sv_lls2.MJD)*24.0*3600.0);
	sv_TEI2 = sv_TEI1;

	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.MJD, 4);
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
		sv_TEI2 = OrbMech::coast(sv_TEI2, dt);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp;

	dt_LPO = (sv_TEI2.MJD - sv_TEI1.MJD)*24.0*3600.0;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.MJD, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI)
	{
		dpsi_tei -= PI2;
	}

	MPTSV S_apo, S3I;
	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	MPTSV S3C = outarray.sv_loi;
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
	ConicFullMissionFixedOrbit(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, gamma_loi2, outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, outarray.T_te, outarray.M_tei, false, dt_min, dt_max);
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
	GMT_nd = OrbMech::GETfromMJD(outarray.MJD_nd, MEDQuantities.GMTBase);
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
	double beta, ainv, MJD_pl;
	beta = EBETA(outarray.sv_loi.R, outarray.sv_loi.V, mu_M, ainv);
	XBETA(outarray.sv_loi.R, outarray.sv_loi.V, outarray.sv_loi.MJD, beta, 2, R_pl, V_pl, MJD_pl);
	outarray.AZ_act = DataTable.psi_lls;
	outtab.GMT_pc2 = OrbMech::GETfromMJD(MJD_pl, MEDQuantities.GMTBase);
	outtab.GMT_nd = GMT_nd;
	outtab.h_nd = outarray.h_nd;
	outtab.lat_nd = outarray.lat_nd;
	outtab.lng_nd = outarray.lng_nd;
	outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc2 = outarray.lng_pl;
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
		double MJD_PC = OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase);
		double MJD_TLI = OrbMech::MJDfromGET(DataTable.GET_TLI + 328.0, MEDQuantities.GETBase);
		if ((sv_MCC.MJD - MJD_TLI)*24.0 > 20.0)
		{
			ddt = 0.0;
		}
		else
		{
			EPHEM(MJD_PC, R_EM, V_EM, R_ES);
			ddt = DDELTATIME(1.0 / ainv / R_E, (sv_MCC.MJD - MJD_TLI)*24.0, length(R_EM) / R_E, PI - acos(dotp(unit(R_EM), unit(V_EM))), (MJD_PC - MJD_TLI)*24.0);
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
	ConicNonfreeReturnOptimizedFreeOrbitToLOPC(sv_MCC, outarray.dv_mcc, outarray.dgamma_mcc, outarray.dpsi_mcc, dt_min, dt_max);
	double h_nd2 = outarray.h_nd;
	double lat_nd2 = outarray.lat_nd;
	double lng_nd2 = outarray.lng_nd;
	double GMT_nd2 = OrbMech::GETfromMJD(outarray.MJD_nd, MEDQuantities.GMTBase) / 3600.0;
	double gamma_loi2 = outarray.gamma_nd;
	double dpsi_loi2 = outarray.dpsi_loi;
	double dt_lls2 = outarray.dt_lls;
	double psi_nd2 = outarray.psi_pl;
	double v_nd2 = outarray.v_pl*3600.0 / R_E;

	double lat_TEI, lng_TEI, MJD_TEI, dlng, dt;
	MPTSV sv_TEI1, sv_TEI2;
	VECTOR3 R_TEI_EMP, V_TEI_EMP;

	MJD_TEI = outarray.MJD_nd + DataTable.T_lo / 24.0 / 3600.0;
	sv_TEI1 = OrbMech::coast(outarray.sv_lls2, (MJD_TEI - outarray.sv_lls2.MJD)*24.0*3600.0);
	sv_TEI2 = sv_TEI1;

	do
	{
		R_TEI_EMP = sv_TEI2.R;
		LIBRAT(R_TEI_EMP, sv_TEI2.MJD, 4);
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
		sv_TEI2 = OrbMech::coast(sv_TEI2, dt);
	} while (abs(dt) > 0.1);

	VECTOR3 RF, VF;
	double dt_LPO, dpsi_tei, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp, mfm0;

	dt_LPO = (sv_TEI2.MJD - sv_TEI1.MJD)*24.0*3600.0;
	R_TEI_EMP = sv_TEI2.R;
	V_TEI_EMP = sv_TEI2.V;
	LIBRAT(R_TEI_EMP, V_TEI_EMP, sv_TEI2.MJD, 4);
	RVIO(true, R_TEI_EMP, V_TEI_EMP, r_emp, v_emp, theta_emp, phi_emp, gamma_emp, psi_emp);
	dpsi_tei = 270.0*RAD - psi_emp;
	if (dpsi_tei > PI2)
	{
		dpsi_tei -= PI2;
	}

	MPTSV S_apo, S3I;
	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	MPTSV S3C = outarray.sv_loi;
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
	ConicFullMissionFreeOrbit(S_apo, NewGuess.x, NewGuess.y, NewGuess.z, DataTable.gamma_loi, dpsi_loi2, dt_lls2 / 3600.0, outarray.T_lo / 3600.0, outarray.dv_tei, outarray.dgamma_tei, outarray.dpsi_tei, outarray.T_te, MEDQuantities.AZ_min, MEDQuantities.AZ_max, outarray.M_tei, false, dt_min, dt_max);
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
	GMT_nd = OrbMech::GETfromMJD(outarray.MJD_nd, MEDQuantities.GMTBase);
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
	double beta, ainv, MJD_pl;
	beta = EBETA(outarray.sv_loi.R, outarray.sv_loi.V, mu_M, ainv);
	XBETA(outarray.sv_loi.R, outarray.sv_loi.V, outarray.sv_loi.MJD, beta, 2, R_pl, V_pl, MJD_pl);
	outtab.GMT_pc2 = OrbMech::GETfromMJD(MJD_pl, MEDQuantities.GMTBase);
	outtab.GMT_nd = GMT_nd;
	outtab.h_nd = outarray.h_nd;
	outtab.lat_nd = outarray.lat_nd;
	outtab.lng_nd = outarray.lng_nd;
	outtab.h_pc2 = outarray.h_pl;
	outtab.lat_pc2 = outarray.lat_pl;
	outtab.lng_pc2 = outarray.lng_pl;
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

	double dt = (OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase) - sv_MCC.MJD)*24.0;
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
	double incl_pg, lat_split, MJD_pc;
	MJD_pc = OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase);
	EPHEM(MJD_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, MJD_pc), R_EM);
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

	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, DV5.x*R_E / 3600.0, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;
	MPTSV S_apo = sv_MCC;
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

	//Empirical first guess
	R = (MEDQuantities.H_pl + DataTable.rad_lls) / R_E;
	V = sqrt(0.184 + 0.553 / (R + 20.0*1852.0 / R_E));
	lng = PI;
	lat = DataTable.lat_pc1;
	psi = 270.0*RAD;
	T = DataTable.GMT_pc1 / 3600.0;

	double dt = (OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase) - sv_MCC.MJD)*24.0;
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
	double incl_pg, lat_split, MJD_pc;
	MJD_pc = OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase);
	EPHEM(MJD_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, MJD_pc), R_EM);
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
	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, DV5.x*R_E / 3600.0, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;
	MPTSV S_apo;
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

	double dt = (OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase) - sv_MCC.MJD)*24.0;
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
	double incl_pg, lat_split, MJD_pc;
	MJD_pc = OrbMech::MJDfromGET(DataTable.GMT_pc1, MEDQuantities.GMTBase);
	EPHEM(MJD_pc, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, MJD_pc), R_EM);
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

	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	MPTSV S_apo;
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

void TLMCCProcessor::ConvergeTLMC(double V, double azi, double lng, double lat, double r, double GMT_pl, bool integrating)
{
	void *constPtr;
	outarray.TLMCIntegrating = integrating;
	outarray.sv0 = sv_MCC;
	constPtr = &outarray;

	bool ConvergeTLMCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConvergeTLMCPointer;

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
	GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

void TLMCCProcessor::IntegratedXYZTTrajectory(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double R_nd, double lat_nd, double lng_nd, double GMT_node)
{
	void *constPtr;

	outarray.sv0 = sv0;
	outarray.NodeStopIndicator = true;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &IntegratedTrajectoryComputerPointer;

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

void TLMCCProcessor::ConicFreeReturnInclinationFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_pg, double lat_pl_min, double lat_pl_max)
{
	void *constPtr;

	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = true;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

void TLMCCProcessor::ConicFreeReturnOptimizedInclinationFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double inc_pg_min, double inc_pg_max, int inc_class)
{
	void *constPtr;

	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = true;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

void TLMCCProcessor::IntegratedFreeReturnFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl)
{
	void *constPtr;
	outarray.NodeStopIndicator = false;
	outarray.LunarFlybyIndicator = true;
	outarray.sv0 = sv0;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &IntegratedTrajectoryComputerPointer;

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

void TLMCCProcessor::ConicFreeReturnFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = true;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

void TLMCCProcessor::IntegratedFreeReturnInclinationFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_fr)
{
	void *constPtr;
	outarray.NodeStopIndicator = false;
	outarray.LunarFlybyIndicator = true;
	outarray.sv0 = sv0;
	constPtr = &outarray;

	bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &IntegratedTrajectoryComputerPointer;

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

void TLMCCProcessor::ConicFreeReturnOptimizedFixedOrbitToLLS(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = false;
	outarray.FixedOrbitIndicator = true;
	outarray.FirstSelect = true;
	outarray.LLSStopIndicator = true;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

void TLMCCProcessor::ConicNonfreeReturnOptimizedFixedOrbitToLLS(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_min, double T_max)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = false;
	outarray.FixedOrbitIndicator = true;
	outarray.FirstSelect = true;
	outarray.LLSStopIndicator = true;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

void TLMCCProcessor::ConicFreeReturnOptimizedFreeOrbitToLOPC(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double dpsi_loi, double DT_lls, double AZ_min, double AZ_max)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = true;
	outarray.FreeReturnOnlyIndicator = false;
	outarray.FixedOrbitIndicator = false;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

void TLMCCProcessor::ConicNonfreeReturnOptimizedFreeOrbitToLOPC(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double T_min, double T_max)
{
	void *constPtr;
	outarray.sv0 = sv0;
	outarray.MidcourseCorrectionIndicator = true;
	outarray.FreeReturnIndicator = false;
	outarray.FixedOrbitIndicator = false;

	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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
	block.DepVarSwitch[7] = true;
	block.DepVarSwitch[8] = true;
	block.DepVarSwitch[9] = true;
	block.DepVarSwitch[10] = true;
	block.DepVarSwitch[18] = true;
	block.DepVarSwitch[20] = true;
	block.DepVarLowerLimit[0] = (DataTable.h_pc2 - 0.1*1852.0) / R_E;
	block.DepVarLowerLimit[2] = 90.0*RAD;
	block.DepVarLowerLimit[7] = (Constants.H_LPO - 0.5*1852.0) / R_E;
	block.DepVarLowerLimit[8] = DataTable.lat_lls - 0.01*RAD;
	block.DepVarLowerLimit[9] = DataTable.lng_lls - 0.01*RAD;
	block.DepVarLowerLimit[10] = DataTable.psi_lls - 0.01*RAD;
	block.DepVarLowerLimit[18] = outarray.M_i + 6000.0*0.453;
	block.DepVarLowerLimit[20] = T_min / 3600.0 - 2.0;
	block.DepVarUpperLimit[0] = (DataTable.h_pc2 + 0.1*1852.0) / R_E;
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

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

void TLMCCProcessor::ConicFullMissionFreeOrbit(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double dpsi_loi, double dt_lls, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double AZ_min, double AZ_max, double mass, bool freereturn, double T_min, double T_max)
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

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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
	}
	else
	{
		block.DepVarSwitch[20] = true;
	}
	block.DepVarSwitch[7] = true;
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
		block.DepVarLowerLimit[0] = (DataTable.h_pc2 - 0.1*1852.0) / R_E;
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
		block.DepVarUpperLimit[0] = (DataTable.h_pc2 + 0.1*1852.0) / R_E;
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

void TLMCCProcessor::ConicFullMissionFixedOrbit(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double mass, bool freereturn, double T_min, double T_max)
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
	outarray.LLSStopIndicator = false;
	constPtr = &outarray;

	bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

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

bool ConvergeTLMCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr)
{
	return ((TLMCCProcessor*)data)->FirstGuessTrajectoryComputer(var, varPtr, arr);
}

bool TLMCCProcessor::FirstGuessTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr)
{
	TLMCCGeneralizedIteratorArray *vars;
	vars = static_cast<TLMCCGeneralizedIteratorArray*>(varPtr);
	VECTOR3 R, V, R_MCC, V_MCC;
	double MJD, v_pl, psi_pl, lng_pl, lat_pl, rad_pl, gamma_pl, GMT_pl;
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
	GMT_pl = var[6] * 3600.0;
	MJD = OrbMech::MJDfromGET(GMT_pl, MEDQuantities.GMTBase);
	vars->MJD_pl = MJD;

	RVIO(false, R, V, rad_pl, v_pl, lng_pl, lat_pl, gamma_pl, psi_pl);
	LIBRAT(R, V, MJD, 3);

	if (vars->TLMCIntegrating)
	{
		OrbMech::oneclickcoast(R, V, MJD, (vars->sv0.MJD - MJD)*24.0*3600.0, R_MCC, V_MCC, hMoon, vars->sv0.gravref);
	}
	else
	{
		if (KREF_MCC == 1)
		{
			VECTOR3 R_patch, V_patch;
			R_patch = R;
			V_patch = V;
			if (PATCH(R_patch, V_patch, MJD, -1, 2))
			{
				return true;
			}
			err = CTBODY(R_patch, V_patch, MJD, vars->sv0.MJD, 1, mu_E, R_MCC, V_MCC);
			if (err) return true;
		}
		else
		{
			err = CTBODY(R, V, MJD, vars->sv0.MJD, 2, mu_M, R_MCC, V_MCC);
			if (err) return true;
		}
	}

	arr[0] = R_MCC.x / R_E;
	arr[1] = R_MCC.y / R_E;
	arr[2] = R_MCC.z / R_E;
	vars->V_MCC = V_MCC;

	double r1, r2, v1, v2, theta, phi, gamma1, gamma2, psi1, psi2;

	RVIO(true, vars->sv0.R, vars->sv0.V, r1, v1, theta, phi, gamma1, psi1);
	RVIO(true, R_MCC, V_MCC, r2, v2, theta, phi, gamma2, psi2);
	vars->dv_mcc = (v2 - v1)*3600.0 / R_E;
	vars->dgamma_mcc = gamma2 - gamma1;
	vars->dpsi_mcc = psi2 - psi1;

	if (arr.size() > 3)
	{
		arr[3] = gamma2;
	}

	return false;
}

bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr)
{
	return ((TLMCCProcessor*)data)->IntegratedTrajectoryComputer(var, varPtr, arr);
}

bool TLMCCProcessor::IntegratedTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr)
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
	MPTSV sv0;
	VECTOR3 RF, VF, R_node, V_node, H_pg;
	double dv_mcc, dgamma_mcc, dpsi_mcc, mfm0, R_nd, lat_nd, lng_nd, MJD_node, inc_pg, dt_node;

	//Store in array
	vars->dv_mcc = var[0];
	vars->dgamma_mcc = var[1];
	vars->dpsi_mcc = var[2];

	dv_mcc = var[0] * R_E / 3600.0;
	dgamma_mcc = var[1];
	dpsi_mcc = var[2];
	dt_node = var[3] * 3600.0;
	sv0 = vars->sv0;

	BURN(sv0.R, sv0.V, dv_mcc, dgamma_mcc, dpsi_mcc, isp_SPS, mfm0, RF, VF);
	outarray.M_mcc = outarray.M_i*mfm0;
	if (vars->NodeStopIndicator)
	{
		VECTOR3 R_node_emp, V_node_emp, R_pl, V_pl;
		double V_nd, gamma_nd, psi_nd, beta, ainv, MJD_pl, r_pl, v_pl, lng_pl, lat_pl, gamma_pl, psi_pl;

		OrbMech::oneclickcoast(RF, VF, sv0.MJD, dt_node, R_node, V_node, sv0.gravref, hMoon);
		MJD_node = sv0.MJD + dt_node / 24.0 / 3600.0;
		vars->sv_loi.R = R_node;
		vars->sv_loi.V = V_node;
		vars->sv_loi.MJD = MJD_node;
		R_node_emp = R_node;
		V_node_emp = V_node;
		LIBRAT(R_node_emp, V_node_emp, MJD_node, 4);
		RVIO(true, R_node_emp, V_node_emp, R_nd, V_nd, lng_nd, lat_nd, gamma_nd, psi_nd);
		if (lng_nd < 0) lng_nd += PI2;
		H_pg = crossp(R_node_emp, V_node_emp);
		inc_pg = acos(H_pg.z / length(H_pg));

		beta = EBETA(R_node, V_node, mu_M, ainv);
		XBETA(R_node, V_node, MJD_node, beta, 2, R_pl, V_pl, MJD_pl);
		LIBRAT(R_pl, V_pl, MJD_pl, 4);
		RVIO(true, R_pl, V_pl, r_pl, v_pl, lng_pl, lat_pl, gamma_pl, psi_pl);

		arr[0] = R_nd / R_E;
		arr[1] = lat_nd;
		arr[2] = lng_nd;
		arr[3] = inc_pg;
		vars->gamma_nd = gamma_nd;
		vars->MJD_nd = MJD_node;
		vars->lat_pl = lat_pl;
		vars->lng_pl = lng_pl;
		vars->h_pl = r_pl - DataTable.rad_lls;
		vars->MJD_pl = MJD_pl;

		return false;
	}
	else
	{
		MPTSV sv1;
		MATRIX3 Rot;
		VECTOR3 R_temp, V_temp, HH_pl, H_equ;

		sv1.R = RF;
		sv1.V = VF;
		sv1.MJD = sv0.MJD;
		sv1.gravref = sv0.gravref;
		MPTSV sv_pl = OrbMech::PMMCEN(sv1, 0.0, 100.0*3600.0, 2, 0.0, 1.0);
	
		R_temp = sv_pl.R;
		V_temp = sv_pl.V;
		LIBRAT(R_temp, V_temp, sv_pl.MJD, 4);
		vars->MJD_pl = sv_pl.MJD;
		vars->h_pl = length(R_temp) - DataTable.rad_lls;
		OrbMech::latlong_from_r(R_temp, vars->lat_pl, vars->lng_pl);
		if (vars->lng_pl < 0)
		{
			vars->lng_pl += PI2;
		}
		HH_pl = crossp(R_temp, V_temp);
		vars->incl_pl = acos(HH_pl.z / length(HH_pl));

		MPTSV sv_reentry = OrbMech::PMMCEN(sv_pl, 0.0, 100.0*3600.0, 2, sin(gamma_reentry), 1.0);
		vars->h_fr = length(sv_reentry.R) - R_E;
		Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, sv_reentry.MJD);
		R_temp = rhtmul(Rot, sv_reentry.R);
		V_temp = rhtmul(Rot, sv_reentry.V);
		H_equ = crossp(R_temp, V_temp);
		vars->incl_fr = acos(H_equ.z / length(H_equ));
		vars->v_EI = length(sv_reentry.V);

		if (vars->LunarFlybyIndicator)
		{
			double dlng;
			RNTSIM(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, Constants.lambda_IP, vars->lat_ip, vars->lng_ip, dlng);
			outarray.MJD_ip = sv_reentry.MJD + Reentry_dt / 24.0 / 3600.0;
		}

		arr[4] = vars->h_pl / R_E;
		arr[3] = vars->incl_pl;
		arr[5] = vars->lat_pl;
		arr[6] = vars->incl_fr;
		arr[7] = vars->h_fr / R_E;
		return false;
	}
}

bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr)
{
	return ((TLMCCProcessor*)data)->ConicMissionComputer(var, varPtr, arr);
}

bool TLMCCProcessor::ConicMissionComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr)
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
	MPTSV sv0;
	VECTOR3 RF, VF, R_patch, V_patch, R_pl, V_pl, R_temp, V_temp, HH_pl, H;
	double dv_mcc, dgamma_mcc, dpsi_mcc, dv_tei;
	double mfm0, beta, MJD_patch, ainv;
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
		MJD_patch = sv0.MJD;
		if (PATCH(R_patch, V_patch, MJD_patch, 1, 1))
		{
			return true;
		}
	}
	else
	{
		R_patch = RF;
		V_patch = VF;
		MJD_patch = sv0.MJD;
	}
	beta = EBETA(R_patch, V_patch, mu_M, ainv);
	ainv_pl = ainv;
	XBETA(R_patch, V_patch, MJD_patch, beta, 2, R_pl, V_pl, vars->MJD_pl);
	R_temp = R_pl;
	V_temp = V_pl;
	LIBRAT(R_temp, V_temp, vars->MJD_pl, 4);
	double r_pl, gamma_temp;
	RVIO(true, R_temp, V_temp, r_pl, vars->v_pl, vars->lng_pl, vars->lat_pl, gamma_temp, vars->psi_pl);
	vars->h_pl = r_pl - DataTable.rad_lls;
	HH_pl = crossp(R_temp, V_temp);
	vars->incl_pl = acos(HH_pl.z / length(HH_pl));

	if (vars->FreeReturnIndicator)
	{
		MATRIX3 Rot;
		VECTOR3 R_patch2, V_patch2, R_pg, V_pg, R_reentry, V_reentry, R_equ, V_equ, H_equ;
		double MJD_patch2, MJD_pg, H, E, beta, e, MJD_reentry;

		R_patch2 = R_pl;
		V_patch2 = V_pl;
		MJD_patch2 = vars->MJD_pl;

		if (PATCH(R_patch2, V_patch2, MJD_patch2, 1, 2))
		{
			return true;
		}
		beta = EBETA(R_patch2, V_patch2, mu_E, ainv);
		XBETA(R_patch2, V_patch2, MJD_patch2, beta, 1, R_pg, V_pg, MJD_pg);
		DGAMMA(length(R_pg), ainv, gamma_reentry, H, E, beta, e);
		XBETA(R_pg, V_pg, MJD_pg, beta, 1, R_reentry, V_reentry, MJD_reentry);
		vars->h_fr = length(R_reentry) - R_E;
		Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, MJD_reentry);
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
		vars->MJD_nd = vars->MJD_pl;
	}
	else
	{
		double H, E, e;
		DGAMMA(length(R_pl), ainv_pl, vars->gamma_nd, H, E, beta, e);
		XBETA(R_pl, V_pl, vars->MJD_pl, beta, 2, R_LOI, V_LOI, vars->MJD_nd);
	}

	vars->sv_loi.R = R_LOI;
	vars->sv_loi.V = V_LOI;
	vars->sv_loi.MJD = vars->MJD_nd;

	//Apply offset
	V_LOI += outarray.LOIOffset;

	VECTOR3 H_LOI;
	double i_EMP;

	vars->h_nd = length(R_LOI) - DataTable.rad_lls;
	R_temp = R_LOI;
	V_temp = V_LOI;
	LIBRAT(R_temp, V_temp, vars->MJD_nd, 4);
	H_LOI = crossp(R_temp, V_temp);
	OrbMech::latlong_from_r(R_temp, vars->lat_nd, vars->lng_nd);
	if (vars->lng_nd < 0) vars->lng_nd += PI2;
	i_EMP = acos(H_LOI.z / length(H_LOI));
	vars->t_tl = (vars->MJD_nd - sv0.MJD)*24.0*3600.0;

	if (vars->FixedOrbitIndicator)
	{
		goto TLMCC_Conic_A1;
	}

	double dv, MJD_LPO;
	dv = Constants.V_pcynlo - length(V_LOI);
	BURN(R_LOI, V_LOI, dv, -vars->gamma_nd, vars->dpsi_loi, isp_MCC, mfm0, RF, VF);
	outarray.M_loi = mfm0 * outarray.M_mcc;
	outarray.M_cir = MCOMP(157.8*0.3048, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_loi);
	vars->MJD_nd += vars->dt_bias_conic_prec / 24.0;
	MJD_LPO = vars->MJD_nd + Constants.dt_bias / 24.0;

	goto TLMCC_Conic_C4;

TLMCC_Conic_A1:

	VECTOR3 U_H, R_N, U_DS;
	double r_N, v_H, theta, phi, gamma_H, psi, DV_LOI;

	RVIO(true, R_LOI, V_LOI, r_N, v_H, theta, phi, gamma_H, psi);
	R_N = unit(R_LOI);
	U_H = unit(crossp(R_LOI, V_LOI));

	if (vars->FirstSelect)
	{
		PRCOMP(R_N, U_H, vars->MJD_nd, vars->RA_LPO1, vars->A_L, vars->E_L, vars->dw_p, vars->dh_a, vars->dh_p, vars->DT_LOI_DOI, vars->dt_lls, vars->u_l, vars->SGSLOI);
		//PRCOMP(R_N, U_H, vars->MJD_nd, vars->RA_LPO1, vars->V2, vars->gamma_L, vars->V_L, vars->A1, vars->E1, vars->gamma1, vars->dt_lls, vars->SGSLOI);
		var[5] = vars->dt_lls / 3600.0;
		vars->FirstSelect = false;
	}

	VECTOR3 Rtemp = vars->SGSLOI.R;
	VECTOR3 Vtemp = vars->SGSLOI.V;
	LIBRAT(Rtemp, Vtemp, vars->MJD_nd, 6);
	U_DS = unit(crossp(Rtemp, Vtemp));

	VECTOR3 U_PJ, gamma_vec;
	double R_NL, V1;

	U_PJ = unit(U_DS - R_N * dotp(U_DS, R_N));
	gamma_vec = unit(crossp(U_H, R_N));
	vars->theta = length(crossp(U_DS, U_PJ))*OrbMech::sign(dotp(crossp(U_DS, U_PJ), gamma_vec));

	//Old Version
	VECTOR3 u_pL = vars->u_l;
	LIBRAT(u_pL, vars->MJD_nd + vars->DT_LOI_DOI / 24.0 / 3600.0, 6);
	double dw_a = acos(dotp(R_N, u_pL));
	double Dw = dw_a + vars->dw_p * OrbMech::sign(dotp(crossp(u_pL, R_N), U_DS));
	R_NL = vars->A_L * (1.0 - vars->E_L * vars->E_L) / (1.0 + vars->E_L * cos(Dw));
	double ddh = 0.0;
	vars->DH_Node = R_NL - (r_N + vars->dh_p) + ddh;
	//New Version
	//R_NL = vars->A1 * (1.0 - vars->E1 * vars->E1) / (1.0 + vars->E1 * cos(MEDQuantities.TA_LOI));
	//vars->DH_Node = R_NL - r_N;
	V1 = sqrt(mu_M*(2.0 / r_N - 2.0 / (r_N + vars->RA_LPO1)));
	DV_LOI = sqrt(v_H*v_H + V1 * V1 - 2.0*v_H*V1*(cos(gamma_H)*dotp(U_DS, U_H))) + 10.0*0.3048; //10 ft/s calibration DV to account for finite burn loss
	vars->dpsi_loi = acos(dotp(U_DS, U_H));

	outarray.M_loi = MCOMP(DV_LOI, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_mcc);

	//Old Version
	double V_L, V2, cos_gamma_L, DV_DOI;
	V_L = sqrt(mu_M*(2.0 / R_NL - 1.0 / vars->A_L));
	V2 = sqrt(mu_M*(2.0 / (r_N + vars->dh_p) - 2.0 / (r_N + vars->RA_LPO1 + vars->dh_a + vars->dh_p)));
	cos_gamma_L = min(1.0, sqrt(mu_M*(1.0 - vars->E_L*vars->E_L)*vars->A_L) / R_NL / V_L);
	DV_DOI = sqrt(V2*V2 + V_L * V_L - 2.0*V_L*V2*cos_gamma_L);
	//New Version
	//gamma = vars->gamma_L - vars->gamma1;
	//double DV_DOI = sqrt(vars->V2*vars->V2 + vars->V_L * vars->V_L - 2.0*vars->V_L*vars->V2*cos(gamma));
	outarray.M_cir = MCOMP(DV_DOI, MEDQuantities.Config, MEDQuantities.useSPS, outarray.M_loi);

	LIBRAT(vars->sv_lls1.R, vars->sv_lls1.V, vars->sv_lls1.MJD, 5);
	vars->sv_lls1.MJD = vars->MJD_nd + vars->dt_lls / 24.0 / 3600.0;
	LIBRAT(vars->sv_lls1.R, vars->sv_lls1.V, vars->sv_lls1.MJD, 6);

	if (vars->LLSStopIndicator)
	{
		goto TLMCC_Conic_Out;
	}

	outarray.M_lopc = outarray.M_cir - MEDQuantities.LMMass;

	goto TLMCC_Conic_F5;
TLMCC_Conic_C4:
	
	VECTOR3 R_LPO, V_LPO, R_LLS, V_LLS;
	double MJD_LLS, lat_S, lng_S;

	SCALE(RF, VF, 60.0*1852.0, R_LPO, V_LPO);
	MJD_LLS = vars->MJD_nd + vars->dt_lls / 24.0 / 3600.0;
	CTBODY(R_LPO, V_LPO, MJD_LPO, MJD_LLS, 2, mu_M, R_LLS, V_LLS);
	R_temp = R_LLS;
	V_temp = V_LLS;
	LIBRAT(R_temp, V_temp, MJD_LLS, 5);
	OrbMech::latlong_from_r(R_temp, lat_S, lng_S);
	double r, v, gamma;
	RVIO(true, R_temp, V_temp, r, v, theta, phi, gamma, vars->AZ_act);
	ELEMT(R_LLS, V_LLS, mu_M, H, a, e, i, n, P, eta);
	P = PI2 / (length(V_LLS) / length(R_LLS) + OrbMech::w_Moon);
	
	LOPC(R_LLS, V_LLS, MJD_LLS, unit(R_temp), Constants.m, Constants.n, P, vars->sv_lls2.R, vars->sv_lls2.V, vars->sv_lls2.MJD, mfm0, dpsi_lopc, outarray.DV_LOPC);
	if (vars->FixedOrbitIndicator == false && abs(dpsi_lopc) < 8e-3)
	{
		DV_R = 2.0*length(vars->sv_lls2.V)*(31.24975000037*dpsi_lopc*dpsi_lopc + 2.0000053333202e-3);
		mfm0 = exp(-DV_R / isp_MCC);
	}
	outarray.M_lopc = (outarray.M_cir - MEDQuantities.LMMass)*mfm0;

TLMCC_Conic_F5:

	MATRIX3 Rot;
	VECTOR3 R_TEI, V_TEI, R_TEC, V_TEC, R_pg, V_pg, R_reentry, V_reentry, R_equ, V_equ, H_equ;
	double MJD_TEC, MJD_pg, HH, E, MJD_reentry, dlng;

	vars->MJD_tei = vars->MJD_nd + vars->T_lo / 24.0 / 3600.0;
	CTBODY(vars->sv_lls2.R, vars->sv_lls2.V, vars->sv_lls2.MJD, vars->MJD_tei, 2, mu_M, R_TEI, V_TEI);
	BURN(R_TEI, V_TEI, dv_tei, vars->dgamma_tei, vars->dpsi_tei, isp_MCC, mfm0, R_TEC, V_TEC);
	vars->DV_TEI = V_TEC - V_TEI;
	outarray.M_tei = outarray.M_lopc * mfm0;
	MJD_TEC = vars->MJD_tei;
	if (PATCH(R_TEC, V_TEC, MJD_TEC, 1, 2))
	{
		return true;
	}
	beta = EBETA(R_TEC, V_TEC, mu_E, ainv);
	XBETA(R_TEC, V_TEC, MJD_TEC, beta, 1, R_pg, V_pg, MJD_pg);
	//TBD: If T_reentry < T_patch, adjust T_reentry
	DGAMMA(length(R_pg), ainv, gamma_reentry, HH, E, beta, e);
	XBETA(R_pg, V_pg, MJD_pg, beta, 1, R_reentry, V_reentry, MJD_reentry);
	vars->h_pr = length(R_reentry) - R_E;
	Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, MJD_reentry);
	R_equ = rhtmul(Rot, R_reentry);
	V_equ = rhtmul(Rot, V_reentry);
	H_equ = crossp(R_equ, V_equ);
	vars->incl_pr = acos(H_equ.z / length(H_equ));
	RNTSIM(R_reentry, V_reentry, MJD_reentry, Constants.lambda_IP, vars->lat_ip_pr, vars->lng_ip_pr, dlng);
	vars->T_te = (MJD_reentry - vars->MJD_tei)*24.0*3600.0;
	vars->MJD_ip_pr = MJD_reentry + Reentry_dt / 24.0 / 3600.0;
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

bool TLMCCProcessor::PATCH(VECTOR3 &R, VECTOR3 &V, double &MJD, int Q, int KREF)
{
	//KREF = 1: Earth reference input, 2: Moon reference input

	VECTOR3 R_EM, V_EM, R_ES, R1, V1, R2, V2, R21, A2;
	double beta, Ratio_desired, MJDF, r1, r2, r21, mu1, mu2, d1, d2, v12, v22, DRatioDBeta, DDRatioDDBeta, Ratio, DRatio, dbeta;
	int i = 0;

	if (KREF == 1)
	{
		if (RBETA(R, V, 40.0*R_E, Q, mu_E, beta))
		{
			return true;
		}
		Ratio_desired = 0.275;
		mu1 = mu_E;
		mu2 = mu_M;
	}
	else
	{
		if (RBETA(R, V, 10.0*R_E, Q, mu_M, beta))
		{
			return true;
		}
		Ratio_desired = 1.0 / 0.275;
		mu1 = mu_M;
		mu2 = mu_E;
	}

	do
	{
		XBETA(R, V, MJD, beta, KREF, R1, V1, MJDF);
		if (EPHEM(MJDF, R_EM, V_EM, R_ES))
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
		dbeta = 2.0*DRatio / (DRatioDBeta + DRatioDBeta / abs(DRatioDBeta)*sqrt(DRatioDBeta*DRatioDBeta + 2.0*DRatio*DDRatioDDBeta));
		beta = beta + dbeta;
		i++;
	} while (i < 10);

	R = R2;
	V = V2;
	MJD = MJDF;

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
		double E0 = D0 / abs(D0)*atan(sqrt(temp) / cos_E0);
		temp = 1.0 - cos_E * cos_E;
		if (temp < 0)
		{
			return true;
		}
		double E = atan2(sqrt(temp), cos_E);
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

void TLMCCProcessor::XBETA(VECTOR3 R0, VECTOR3 V0, double MJD0, double beta, int K, VECTOR3 &RF, VECTOR3 &VF, double &MJDF)
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
	MJDF = MJD0 + t / 24.0 / 3600.0;
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

bool TLMCCProcessor::EPHEM(double MJD, VECTOR3 &R_EM, VECTOR3 &V_EM, VECTOR3 &R_ES)
{
	return (OrbMech::PLEFEM(*ephemeris, MJD, R_EM, V_EM, R_ES) == false);
}

bool TLMCCProcessor::LIBRAT(VECTOR3 &R, double MJD, int K)
{
	VECTOR3 V = _V(0, 0, 0);
	return LIBRAT(R, V, MJD, K);
}

bool TLMCCProcessor::LIBRAT(VECTOR3 &R, VECTOR3 &V, double MJD, int K)
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

		if (EPHEM(MJD, R_ME, V_ME, R_ES))
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

			Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD);

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

		Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD);

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

bool TLMCCProcessor::CTBODY(VECTOR3 R0, VECTOR3 V0, double MJD0, double MJDF, int K, double mu, VECTOR3 &RF, VECTOR3 &VF)
{
	double F1, F2, F3, F4, alpha;
	return CTBODY(R0, V0, MJD0, MJDF, K, mu, alpha, F1, F2, F3, F4, RF, VF);
}

bool TLMCCProcessor::CTBODY(VECTOR3 R0, VECTOR3 V0, double MJD0, double MJDF, int K, double mu, double &alpha, double &F1, double &F2, double &F3, double &F4, VECTOR3 &RF, VECTOR3 &VF)
{
	double dt;
	int i = 0;

	dt = (MJDF - MJD0)*24.0*3600.0;

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

	OrbMech::rv_from_r0v0(R0, V0, dt, RF, VF, mu);
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
	LIBRAT(G0, Gdot0, MJDF, 5);
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
	LIBRAT(G, Gdot, MJDF, 6);
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

void TLMCCProcessor::RNTSIM(VECTOR3 R, VECTOR3 V, double MJD, double lng_L, double &lat, double &lng, double &dlng)
{
	MATRIX3 Rot;
	VECTOR3 P, S, S_equ;
	double r, v, theta;

	r = length(R);
	v = length(V);

	P = V / v / cos(gamma_reentry) - R / r * tan(gamma_reentry);
	theta = Reentry_range / 3443.933585;
	S = R / r * cos(theta) + P * sin(theta);
	Rot = OrbMech::GetRotationMatrix(BODY_EARTH, MJD + Reentry_dt / 24.0 / 3600.0);
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

void TLMCCProcessor::LOPC(VECTOR3 R0, VECTOR3 V0, double MJD0, VECTOR3 L, int m, int n, double P, VECTOR3 &R3, VECTOR3 &V3, double &MJD3, double &mfm0, double &dpsi, VECTOR3 &DV)
{
	VECTOR3 R1, V1, R2, V2;
	double dt1, MJD1, MJD2, dt2;

	dt1 = P * (-0.25 + (double)m);
	MJD1 = MJD0 + dt1 / 24.0 / 3600.0;
	CTBODY(R0, V0, MJD0, MJD1, 2, mu_M, R1, V1);
	dt2 = P * (double)(m + n);
	MJD2 = MJD0 + dt2 / 24.0 / 3600.0;
	CTBODY(R0, V0, MJD0, MJD2, 2, mu_M, R2, V2);
	LIBRAT(R2, V2, MJD2, 5);
	dpsi = -asin(dotp(unit(crossp(R2, V2)), L));
	BURN(R1, V1, 0.0, 0.0, dpsi, isp_MCC, mfm0, R3, V3);
	DV = V3 - V1;
	MJD3 = MJD1;
}

void TLMCCProcessor::PRCOMP(VECTOR3 u_pc, VECTOR3 h_pc, double MJD_nd, double &RA_LPO1, double &A_L, double &E_L, double &dw_p, double &dh_a, double &dh_p, double &DT, double &DT_1st_pass, VECTOR3 &u_l, MPTSV &SGSLOI)
{
	VECTOR3 u_lls, RR2, VV2, R_L, V_L, RR_LPO1, VV_LPO1, u_lls_equ;
	double RP_LPO1, a_lls, dt_LLS, MJD_LLS, da_apo, R1, DR1, da, DA, DA_apo, e, R2, phi2, lambda2, psi2, gamma2, eta2, dt3, dt, MJDI, dt2;
	double a_LPO1, e_LPO1, r, v, gamma, psi, theta, phi;
	//Old Version
	VECTOR3 u_pl;
	double V2, v_L, gamma_L, gamma1, A1, E1;
	//New Version
	//VECTOR3 u_pl;
	//double A_L, E_L;

	DR1 = modf(MEDQuantities.Revs_LPO1, &R1);

	RA_LPO1 = MEDQuantities.H_A_LPO1 + DataTable.rad_lls;
	RP_LPO1 = MEDQuantities.H_P_LPO1 + DataTable.rad_lls;
	a_lls = DataTable.rad_lls + (MEDQuantities.H_A_LPO2 + MEDQuantities.H_P_LPO2) / 2.0;

	a_LPO1 = (RA_LPO1 + RP_LPO1) / 2.0;
	e_LPO1 = (RA_LPO1 - RP_LPO1) / (RA_LPO1 + RP_LPO1);

	r = a_LPO1 * (1.0 - e_LPO1 * e_LPO1) / (1.0 + e_LPO1 * cos(MEDQuantities.TA_LOI));
	v = sqrt(mu_M*(2.0 / r - 1.0 / a_LPO1));
	if (MEDQuantities.TA_LOI != 0.0)
	{
		gamma = -OrbMech::sign(MEDQuantities.TA_LOI)*acos(sqrt(mu_M*a_LPO1*(1.0 - e_LPO1 * e_LPO1)) / r / v);
	}
	else
	{
		gamma = 0.0;
	}
	psi = 0.0;
	theta = 0.0;
	phi = 0.0;
	RVIO(false, RR_LPO1, VV_LPO1, r, v, theta, phi, gamma, psi);
	dt2 = OrbMech::time_theta(RR_LPO1, VV_LPO1, DR1, mu_M);
	eta2 = MEDQuantities.TA_LOI + DR1;
	if (eta2 < 0)
	{
		eta2 += PI2;
	}

	dt_LLS = PI2 / sqrt(mu_M)*(R1*pow((RA_LPO1 + RP_LPO1) / 2.0, 1.5) + (double)(MEDQuantities.Revs_LPO2) * pow(a_lls, 1.5)) + dt2;
	MJD_LLS = MJD_nd + dt_LLS / 24.0 / 3600.0;
	u_lls_equ = OrbMech::r_from_latlong(DataTable.lat_lls, DataTable.lng_lls);
	for (int i = 0;i < 2;i++)
	{
		u_lls = u_lls_equ;
		LIBRAT(u_lls, MJD_LLS, 6);
		da_apo = acos(dotp(u_lls, u_pc));
		if (dotp(crossp(u_lls, u_pc), h_pc) > 0)
		{
			da = PI2 - da_apo;
		}
		else
		{
			da = da_apo;
		}
		DA_apo = da - DR1;
		if (DA_apo > 0)
		{
			DA = DA_apo;
		}
		else
		{
			DA = DA_apo + PI2;
		}
		e = -1.0 + (MEDQuantities.H_A_LPO2 + DataTable.rad_lls) / a_lls;
		phi2 = DataTable.lat_lls;
		lambda2 = DataTable.lng_lls;
		psi2 = DataTable.psi_lls;
		R2 = a_lls * (1.0 - e * e) / (1.0 + e * cos(MEDQuantities.site_rotation_LPO2));
		V2 = sqrt(mu_M*(2.0 / R2 - 1.0 / a_lls));
		if (MEDQuantities.site_rotation_LPO2 != 0)
		{
			gamma2 = -OrbMech::sign(MEDQuantities.site_rotation_LPO2)*acos(sqrt(mu_M*a_lls*(1.0 - e * e)) / R2 / V2);
		}
		else
		{
			gamma2 = 0.0;
		}
		RVIO(false, RR2, VV2, R2, V2, lambda2, phi2, gamma2, psi2);

		dt3 = OrbMech::time_theta(RR2, -VV2, DA, mu_M);
		if (DA > PI)
		{
			dt3 += OrbMech::period(RR2, VV2, mu_M);
		}
		else if (DA < -PI)
		{
			dt3 -= OrbMech::period(RR2, VV2, mu_M);
		}
		MJD_LLS = MJD_nd + (dt_LLS + dt3) / 24.0 / 3600.0;
	}
	
	VECTOR3 RR2S, VV2S, am, u_pc_proj;
	VECTOR3 H_L, R1I, V1I, Rtemp, Vtemp, u_pl_apo, H1;
	double i_L, n, P, eta, RA2, RP2, r_L, E, A, V1, lambda, ddh;//, RA1, RP1;
	double i1;
	double RA1, RP1, deta_apo, DTCORR;
	bool recycle = false;

TLMCC_PRCOMP_1:
	RR2S = RR2;
	VV2S = VV2;
	LIBRAT(RR2S, VV2S, MJD_LLS, 6);

	outarray.sv_lls1.R = RR2S;
	outarray.sv_lls1.V = VV2S;
	outarray.sv_lls1.MJD = MJD_LLS;

	dt = -(OrbMech::period(RR2S, VV2S, mu_M)*(double)(MEDQuantities.Revs_LPO2) + dt3);
	OrbMech::oneclickcoast(RR2S, VV2S, MJD_LLS, dt, R_L, V_L, hMoon, hMoon);
	MJDI = MJD_LLS + dt / 24.0 / 3600.0;

	am = unit(crossp(R_L, V_L));
	u_pc_proj = unit(u_pc - am * dotp(u_pc, am));
	deta_apo = acos(dotp(u_pc_proj, unit(R_L)));
	DTCORR = OrbMech::time_theta(R_L, V_L, deta_apo, mu_M);

	OrbMech::oneclickcoast(R_L, V_L, MJDI, DTCORR, R_L, V_L, hMoon, hMoon);
	MJDI = MJDI + DTCORR / 24.0 / 3600.0;

	ELEMT(R_L, V_L, mu_M, H_L, A_L, E_L, i_L, n, P, eta);
	RVIO(true, R_L, V_L, r_L, v_L, lambda, phi, gamma, psi);
	//RA2 = (1.0 + E_L)*A_L;
	//RP2 = (1.0 - E_L)*A_L;
	gamma_L = asin(dotp(R_L, V_L) / r_L / v_L);
	u_l = unit(crossp(V_L, crossp(R_L, V_L)) / mu_M - R_L / length(R_L));
	LIBRAT(u_l, MJDI, 5);

	ddh = 0.0; //TBD

	E = (RA_LPO1 - (r_L - ddh)) / (RA_LPO1 + (r_L - ddh)*cos(eta2));
	A = RA_LPO1 / (1.0 + E);
	RA2 = (1.0 + E)*A;
	RP2 = (1.0 - E)*A;
	V1 = sqrt(mu_M * (2.0 / (r_L - ddh) - 1.0 / A));
	if (eta2 != 0.0)
	{
		gamma1 = OrbMech::sign(eta2)*acos(sqrt(mu_M*A*(1.0 - E * E)) / ((r_L - ddh)*V1));
	}
	else
	{
		gamma1 = 0.0;
	}
	RVIO(false, Rtemp, Vtemp, r_L, V1, lambda, phi, gamma1, psi);
	u_pl = unit(crossp(Vtemp, crossp(Rtemp, Vtemp)) / mu_M - Rtemp / length(Rtemp));
	dt = (MJD_nd - MJDI)*24.0*3600.0;
	OrbMech::oneclickcoast(Rtemp, Vtemp, MJDI, dt, R1I, V1I, hMoon, hMoon);
	u_pl_apo = unit(crossp(V1I, crossp(R1I, V1I)) / mu_M - R1I / length(R1I));
	ELEMT(R1I, V1I, mu_M, H1, A1, E1, i1, n, P, eta);
	
	OrbMech::periapo(R1I, V1I, mu_M, RA1, RP1);

	//VECTOR3 am, u_pc_proj;
	//double deta_apo, DTCORR;
	am = unit(crossp(R1I, V1I));
	u_pc_proj = unit(u_pc - am * dotp(u_pc, am));
	deta_apo = acos(dotp(u_pc_proj, unit(R1I)));
	DTCORR = OrbMech::time_theta(R1I, V1I, deta_apo, mu_M);
	MJD_LLS -= DTCORR / 24.0 / 3600.0;
	if (recycle == false)
	{
		recycle = true;
		goto TLMCC_PRCOMP_1;
	}

	dw_p = acos(dotp(u_pl, u_pl_apo))*OrbMech::sign(dotp(crossp(u_pl_apo, u_pl), crossp(R1I, V1I)));
	dh_a = RA2 - RA1;
	dh_p = RP2 - RP1;
	DT = (MJDI - MJD_nd)*24.0*3600.0;
	DT_1st_pass = (MJD_LLS - MJD_nd)*24.0*3600.0;

	SGSLOI.gravref = hMoon;
	SGSLOI.MJD = MJD_nd;
	SGSLOI.R = R1I;
	SGSLOI.V = V1I;
	LIBRAT(SGSLOI.R, SGSLOI.V, SGSLOI.MJD, 5);
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

MPTSV TLMCCProcessor::PPC(MPTSV SIN, double lat1, double lng1, double azi1, int RT1, int INTL, double &DVS)
{
	if (INTL != 0)
	{
		//goto TLMCC_PPC_E;
	}

	MPTSV S, SG, SLLG, SLLS, SMB, SMA;
	VECTOR3 H, H_apo, ND1, HSMA, TSMB, PSMB;
	double dt, GMT, PP, DV, r, v, theta, phi, gamma, psi, DL, D1, T1, T2, sin_az, DT, D, DAZ;
	int i, K;
	int IMAX = 1;

	S = SIN;
	GMT = OrbMech::GETfromMJD(S.MJD, MEDQuantities.GMTBase);
	PP = PI2 * length(S.R) / sqrt(mu_M / length(S.R));
	i = 1;
	DV = 0.0;
TLMCC_PPC_J:
	dt = PP * (double)(RT1 + 1);
	OrbMech::oneclickcoast(S.R, S.V, S.MJD, dt, SG.R, SG.V, hMoon, hMoon);
	SG.MJD = S.MJD + dt / 24.0 / 3600.0;
	LIBRAT(SG.R, SG.V, SG.MJD, 5);
	r = length(S.R);
	v = length(S.V);
	theta = lng1;
	phi = lat1;
	gamma = 0.0;
	psi = azi1;
	RVIO(false, SLLG.R, SLLG.V, r, v, theta, phi, gamma, psi);
	DL = atan2(length(crossp(SG.R, SLLG.R)), dotp(SG.R, SLLG.R));
	SLLG.MJD = S.MJD + (PP*(DL / PI2 + 1.0 + (double)(RT1))) / 24.0 / 3600.0;
TLMCC_PPC_C:
	SLLS = SLLG;
	LIBRAT(SLLS.R, SLLS.V, SLLS.MJD, 6);
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
	SMB.MJD = S.MJD + PP*(1.0-D1/PI2+(double)(RT1)) / 24.0 / 3600.0;
	if (SLLG.MJD < SMB.MJD)
	{
		SLLG.MJD += PP / 24.0 / 3600.0;
		goto TLMCC_PPC_C;
	}
TLMCC_PPC_D:
	if (K <= 5)
	{
		T1 = S.MJD + PP * (double)(RT1) / 24.0 / 3600.0;
		T2 = S.MJD + PP * (1.0 + (double)(RT1)) / 24.0 / 3600.0;
		if (SMB.MJD < T1)
		{
			SMB.MJD += PP / 24.0 / 3600.0;
		}
		else if (SMB.MJD > T2)
		{
			SMB.MJD -= PP / 24.0 / 3600.0;
		}
		T1 = SMB.MJD;
		T2 = SMB.MJD + PP / 24.0 / 3600.0;
		if (SLLG.MJD < T1)
		{
			SLLG.MJD += PP / 24.0 / 3600.0;
		}
		else if (SLLG.MJD > T2)
		{
			SLLG.MJD -= PP / 24.0 / 3600.0;
		}
	}
	SLLS.MJD = SLLG.MJD;
	SMA.MJD = SMB.MJD;
	CTBODY(S.R, S.V, S.MJD, SMB.MJD, 2, mu_M, SMB.R, SMB.V);
	CTBODY(SLLS.R, SLLS.V, SLLS.MJD, SMA.MJD, 2, mu_M, SMA.R, SMA.V);
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
		SLLG.MJD += (DT + D) / 24.0 / 3600.0;
		SMB.MJD += (DT) / 24.0 / 3600.0;
		SLLS = SLLG;
		LIBRAT(SLLS.R, SLLS.V, SLLS.MJD, 6);
		K++;
		goto TLMCC_PPC_D;
	}
TLMCC_PPC_I:
	DVS = DV;
	return SLLS;
}