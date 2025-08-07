/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Trajectory Computers for TLI and MCC Processors

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
#include "rtcc.h"
#include "TLTrajectoryComputers.h"

TLTrajectoryComputers::TLTrajectoryComputers(RTCC *r) : RTCCModule(r)
{
	R_E = 6378165.0; // OrbMech::R_Earth;
	R_M = OrbMech::R_Moon;
	mu_E = OrbMech::mu_Earth;
	mu_M = OrbMech::mu_Moon;

	isp_SPS = 3080.0;
	isp_DPS = 3107.0;

	F_I_SIVB = 179847.1544797684; //LBF, 800000.0 Newton
	F_SIVB = 202328.0487897395; //LBF, 900000.0 Newton
	WDOT_SIVB = 1677750.443641722; //LBS per hours, 211.393 kg/s
	T_MRS_SIVB = 50.0 / 3600.0; //Hours

	gamma_reentry = -6.52*RAD;
	Reentry_dt = 500.0;
}

TLTrajectoryComputers::~TLTrajectoryComputers()
{

}

void TLTrajectoryComputers::Init(TLMCCMissionConstants constants, double GMTBase)
{
	MissionConstants = constants;
	GMTBASE = GMTBase;
}

bool ConvergeTLMCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((TLTrajectoryComputers*)data)->FirstGuessTrajectoryComputer(var, varPtr, arr, mode);
}

bool TLTrajectoryComputers::FirstGuessTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
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
	vars->h_pl = var[4] * R_E - vars->rad_lls;
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
		//Convert if necessary
		if (vars->KREF_MCC == 1 && sv_mcc_temp.RBI == BODY_MOON)
		{
			pRTCC->ELVCNV(sv_mcc_temp, 0, sv_mcc_temp);
			sv_mcc_temp.RBI = BODY_EARTH;
		}
		else if (vars->KREF_MCC == 2 && sv_mcc_temp.RBI == BODY_EARTH)
		{
			pRTCC->ELVCNV(sv_mcc_temp, 2, sv_mcc_temp);
			sv_mcc_temp.RBI = BODY_MOON;
		}
	}
	else
	{
		if (vars->KREF_MCC == 1)
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

bool ConicMissionComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((TLTrajectoryComputers*)data)->ConicMissionComputer(var, varPtr, arr, mode);
}

bool TLTrajectoryComputers::ConicMissionComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
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
	//10: TLI energy in Er^2/^hr^2
	//11: TLI plane change in rad
	//12: TLI perigee angle in rad
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
	//21: Apogee height in Er
	//22: Mass after TLI

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
			SV2 state_TLI1, state_TLI2;

			state_TLI1.sv.R = vars->sv0.R / R_E;
			state_TLI1.sv.V = vars->sv0.V*3600.0 / R_E;
			state_TLI1.sv.GMT = vars->sv0.GMT / 3600.0;
			state_TLI1.Mass = outarray.M_i / 0.45359237;

			//Store in array
			vars->C3_TLI = var[10];
			vars->delta_TLI = var[11];
			vars->sigma_TLI = var[12];

			state_TLI2 = TLIBRN(state_TLI1, vars->C3_TLI, vars->sigma_TLI, vars->delta_TLI, F_I_SIVB, F_SIVB, WDOT_SIVB, T_MRS_SIVB, false, vars->dv_TLI);

			//Convert back to SI
			vars->M_tli = state_TLI2.Mass*0.45359237;
			vars->dv_TLI *= R_E / 3600.0;

			ELEMT(state_TLI2.sv.R*R_E, state_TLI2.sv.V*R_E / 3600.0, mu_E, H, a, e, i, n, P, eta);
			arr[21] = (a * (1.0 + e) - R_E) / R_E; //Calculated apogee height
			arr[22] = vars->M_tli;
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

	if (vars->KREF_MCC == 1)
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
	vars->h_pl = r_pl - vars->rad_lls;
	HH_pl = crossp(R_temp, V_temp);
	vars->incl_pl = acos(HH_pl.z / length(HH_pl));

	if (vars->FreeReturnIndicator)
	{
		VECTOR3 R_patch2, V_patch2, R_pg, V_pg, R_reentry, V_reentry, H_reentry;
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
		vars->h_fr = length(R_reentry) - OrbMech::R_Earth;
		H_reentry = crossp(R_reentry, V_reentry);
		vars->incl_fr = acos(H_reentry.z / length(H_reentry));

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

	vars->h_nd = length(R_LOI) - vars->rad_lls;
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
	dv = MissionConstants.V_pcynlo - length(V_LOI);
	BURN(R_LOI, V_LOI, dv, -vars->gamma_nd, vars->dpsi_loi, isp_MCC, mfm0, RF, VF);
	outarray.M_loi = mfm0 * outarray.M_mcc;
	outarray.M_cir = MCOMP(157.8*0.3048, vars->Config, vars->useSPS, outarray.M_loi);
	vars->GMT_nd += vars->dt_bias_conic_prec*3600.0;
	GMT_LPO = vars->GMT_nd + MissionConstants.dt_bias*3600.0;

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
		PRCOMP(vars, R_N, U_H, vars->GMT_nd, vars->RA_LPO1, vars->A1, vars->E1, vars->gamma1, vars->V_L, vars->gamma_L, vars->V2, vars->dt_lls);
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

	R_NL = vars->A1 * (1.0 - vars->E1 * vars->E1) / (1.0 + vars->E1 * cos(vars->TA_LOI));
	vars->DH_Node = R_NL - r_N;
	V1 = sqrt(mu_M*(2.0 / r_N - 2.0 / (r_N + vars->RA_LPO1)));
	DV_LOI = sqrt(v_H*v_H + V1 * V1 - 2.0*v_H*V1*(cos(gamma_H)*dotp(U_DS, U_H))) + 10.0*0.3048; //10 ft/s calibration DV to account for finite burn loss
	vars->dpsi_loi = acos(dotp(U_DS, U_H));

	outarray.M_loi = MCOMP(DV_LOI, vars->Config, vars->useSPS, outarray.M_mcc);

	gamma = vars->gamma_L - vars->gamma1;
	double DV_DOI = sqrt(vars->V2*vars->V2 + vars->V_L * vars->V_L - 2.0*vars->V_L*vars->V2*cos(gamma));
	outarray.M_cir = MCOMP(DV_DOI, vars->Config, vars->useSPS, outarray.M_loi);

	LIBRAT(vars->sv_lls1.R, vars->sv_lls1.V, vars->sv_lls1.GMT, 5);
	vars->sv_lls1.GMT = vars->GMT_nd + vars->dt_lls;
	LIBRAT(vars->sv_lls1.R, vars->sv_lls1.V, vars->sv_lls1.GMT, 6);

	if (vars->LLSStopIndicator)
	{
		goto TLMCC_Conic_Out;
	}

	outarray.M_lopc = outarray.M_cir - vars->LMMass;

	goto TLMCC_Conic_F5;
TLMCC_Conic_C4:

	VECTOR3 R_LPO, V_LPO, R_LLS, V_LLS;
	double GMT_LLS, lat_S, lng_S;

	SCALE(RF, VF, vars->rad_lls, 60.0*1852.0, R_LPO, V_LPO);
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

	LOPC(R_LLS, V_LLS, GMT_LLS, unit(R_temp), MissionConstants.m, MissionConstants.n, P, vars->sv_lls2.R, vars->sv_lls2.V, vars->sv_lls2.GMT, mfm0, dpsi_lopc, outarray.DV_LOPC);
	if (vars->FixedOrbitIndicator == false && abs(dpsi_lopc) < 8e-3)
	{
		DV_R = 2.0*length(vars->sv_lls2.V)*(31.24975000037*dpsi_lopc*dpsi_lopc + 2.0000053333202e-3);
		mfm0 = exp(-DV_R / isp_MCC);
	}
	outarray.M_lopc = (outarray.M_cir - vars->LMMass)*mfm0;

TLMCC_Conic_F5:

	VECTOR3 R_TEI, V_TEI, R_TEC, V_TEC, R_pg, V_pg, R_reentry, V_reentry, H_reentry;
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
	vars->h_pr = length(R_reentry) - OrbMech::R_Earth;
	H_reentry = crossp(R_reentry, V_reentry);
	vars->incl_pr = acos(H_reentry.z / length(H_reentry));
	RNTSIM(R_reentry, V_reentry, GMT_reentry, MissionConstants.lambda_IP, vars->lat_ip_pr, vars->lng_ip_pr, dlng);
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

bool IntegratedTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((TLTrajectoryComputers*)data)->IntegratedTrajectoryComputer(var, varPtr, arr, mode);
}

bool TLTrajectoryComputers::IntegratedTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	//Independent Variables:
	//0: Delta velocity in Er/hr.
	//1: Delta flight path angle in rad
	//2: Delta azimuth in rad
	//3: Delta time to node in hr.
	//4: TLI energy in (Er/hr)^2
	//5: Time in EPO in hours
	//6: TLI plane change in rad
	//Dependent variables:
	//0: Radius of node in Er
	//1: Latitude of node in rad
	//2: Longitude of node in rad
	//3: Inclination at perilune in rad
	//4: Height at perilune in Er
	//5: Latitude at perilune in rad
	//6: Inclination of return in rad
	//7: Height of return in Er
	//8: Apogee height in Er
	//9: TLI Delta V in m/s

	TLMCCGeneralizedIteratorArray *vars;
	vars = static_cast<TLMCCGeneralizedIteratorArray*>(varPtr);
	EphemerisData sv0, sv0_apo, sv_nd;
	VECTOR3 H_pg;
	double dv_mcc, dgamma_mcc, dpsi_mcc, mfm0, R_nd, lat_nd, lng_nd, inc_pg, dt_node;
	INT ITS;

	if (vars->TLIIndicator)
	{
		SV2 state_TLI1, state_TLI2;
		EphemerisData sv_ap;

		//Store in array
		vars->C3_TLI = var[4];
		vars->dt_EPO = var[5] * 3600.0;
		vars->delta_TLI = var[6];

		sv0 = vars->sv0;

		//Propagate to TLI ignition
		pRTCC->PMMCEN(sv0, 0.0, 0.0, 1, vars->dt_EPO, 1.0, state_TLI1.sv, ITS); //TBD: Use EMMENI instead for venting

		vars->sv_tli_ign = state_TLI1.sv;

		//Convert to correct units
		state_TLI1.sv.R /=  R_E;
		state_TLI1.sv.V *= 3600.0 / R_E;
		state_TLI1.sv.GMT /= 3600.0;
		state_TLI1.Mass = outarray.M_i / 0.45359237;

		//Simulate TLI burn
		state_TLI2 = TLIBRN(state_TLI1, vars->C3_TLI, vars->sigma_TLI, vars->delta_TLI, F_I_SIVB, F_SIVB, WDOT_SIVB, T_MRS_SIVB, vars->EllipticalCaseIndicator == false, vars->dv_TLI);

		//Convert back
		state_TLI2.sv.R *= R_E;
		state_TLI2.sv.V *= R_E / 3600.0;
		state_TLI2.sv.GMT *= 3600.0;
		state_TLI2.Mass *= 0.45359237;
		vars->dv_TLI *= R_E / 3600.0;

		vars->sv_tli_cut = state_TLI2.sv;
		arr[9] = vars->dv_TLI;

		//Simulate only TLI?
		if (vars->TLIOnlyIndicator) return false;
		//Simulate through apogee?
		if (vars->EllipticalCaseIndicator)
		{
			//Propagate state vector to apogee
			pRTCC->PMMCEN(state_TLI2.sv, 0.0, 100.0*3600.0, 2, 0.0, 1.0, sv_ap, ITS);

			//Error: Did not find apogee or reached lunar SOI
			if (ITS != 2 || sv_ap.RBI != BODY_EARTH)
			{
				return true;
			}

			arr[8] = (length(sv_ap.R) - R_E) / R_E;
			return false;
		}

		sv0_apo = vars->sv_tli_cut;
		
		if (vars->NodeStopIndicator)
		{
			dt_node = vars->GMT_nd - vars->sv_tli_cut.GMT;
		}
	}
	else
	{
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
	}

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
		vars->h_pl = r_pl - vars->rad_lls;
		vars->GMT_pl = GMT_pl;

		return false;
	}
	else
	{
		EphemerisData sv_pl, sv_r;
		VECTOR3 R_temp, V_temp, HH_pl, H_reentry;
		int ITS;

		//Propagate state vector to perilune
		pRTCC->PMMCEN(sv0_apo, 0.0, 100.0*3600.0, 2, 0.0, 1.0, sv_pl, ITS);

		R_temp = sv_pl.R;
		V_temp = sv_pl.V;
		vars->GMT_pl = sv_pl.GMT;
		LIBRAT(R_temp, V_temp, vars->GMT_pl, 4);
		vars->h_pl = length(R_temp) - vars->rad_lls;
		OrbMech::latlong_from_r(R_temp, vars->lat_pl, vars->lng_pl);
		if (vars->lng_pl < 0)
		{
			vars->lng_pl += PI2;
		}
		HH_pl = crossp(R_temp, V_temp);
		vars->incl_pl = acos(HH_pl.z / length(HH_pl));

		//Propagate state vector to reentry
		pRTCC->PMMCEN(sv_pl, 0.0, 100.0*3600.0, 2, sin(gamma_reentry), 1.0, sv_r, ITS);

		vars->h_fr = length(sv_r.R) - OrbMech::R_Earth;
		H_reentry = crossp(sv_r.R, sv_r.V);
		vars->incl_fr = acos(H_reentry.z / length(H_reentry));
		vars->v_EI = length(sv_r.V);

		if (vars->LunarFlybyIndicator)
		{
			double dlng;
			RNTSIM(sv_r.R, sv_r.V, sv_r.GMT, MissionConstants.lambda_IP, vars->lat_ip, vars->lng_ip, dlng);
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

void TLTrajectoryComputers::BURN(VECTOR3 R, VECTOR3 V, double dv, double dgamma, double dpsi, double isp, double &mfm0, VECTOR3 &RF, VECTOR3 &VF)
{
	double v_c, dv_r;
	BURN(R, V, 3, 0.0, 0.0, dv, dgamma, dpsi, isp, 0.0, v_c, dv_r, mfm0, RF, VF);
}

void TLTrajectoryComputers::BURN(VECTOR3 R, VECTOR3 V, int opt, double gamma0, double v_pl, double dv, double dgamma, double dpsi, double isp, double mu, double &v_c, double &dv_R, double &mfm0, VECTOR3 &RF, VECTOR3 &VF)
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

bool TLTrajectoryComputers::CTBODY(VECTOR3 R0, VECTOR3 V0, double GMT0, double GMTF, int K, double mu, VECTOR3 &RF, VECTOR3 &VF)
{
	double F1, F2, F3, F4, alpha;
	return CTBODY(R0, V0, GMT0, GMTF, K, mu, alpha, F1, F2, F3, F4, RF, VF);
}

bool TLTrajectoryComputers::CTBODY(VECTOR3 R0, VECTOR3 V0, double GMT0, double GMTF, int K, double mu, double &alpha, double &F1, double &F2, double &F3, double &F4, VECTOR3 &RF, VECTOR3 &VF)
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
	M = _V(-cos_i * (sin_Omega*cos(dOmega) + cos_Omega * sin(dOmega)), cos_i*(cos_Omega*cos(dOmega) - sin_Omega * sin(dOmega)), sin_i);
	G = N * (Gdot0.z*rr - G0.z*d) / n + M * G0.z*length(crossp(G0, Gdot0)) / n;
	Gdot = N * (Gdot0.z*d - G0.z*vv) / n + M * Gdot0.z*length(crossp(G0, Gdot0)) / n;
	LIBRAT(G, Gdot, GMTF, 6);
	RF = G;
	VF = Gdot;

	return false;
}

double TLTrajectoryComputers::DELTAT(double a, double e, double eta, double deta)
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

bool TLTrajectoryComputers::DGAMMA(double r0, double ainv, double gamma, double &H, double &E, double &beta, double &e)
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

double TLTrajectoryComputers::EBETA(VECTOR3 R, VECTOR3 V, double mu, double &ainv)
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

void TLTrajectoryComputers::ELEMT(VECTOR3 R, VECTOR3 V, double mu, VECTOR3 &H, double &a, double &e, double &i, double &n, double &P, double &eta)
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

bool TLTrajectoryComputers::EPHEM(double GMT, VECTOR3 &R_EM, VECTOR3 &V_EM, VECTOR3 &R_ES)
{
	return pRTCC->PLEFEM(1, GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);
}

void TLTrajectoryComputers::FCOMP(double a, double &F1, double &F2, double &F3, double &F4)
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

bool TLTrajectoryComputers::LIBRAT(VECTOR3 &R, double GMT, int K)
{
	VECTOR3 V = _V(0, 0, 0);
	return LIBRAT(R, V, GMT, K);
}

bool TLTrajectoryComputers::LIBRAT(VECTOR3 &R, VECTOR3 &V, double GMT, int K)
{
	//Options
	//1: EMP to selenographic
	//2: Selenographic to EMP
	//3: EMP to selenocentric
	//4: Selenocentric to EMP
	//5: Selenocentric to selenographic
	//6: Selenographic to selenocentric

	int in, out;

	switch (K)
	{
	case 1:
		in = 4; out = 3;
		break;
	case 2:
		in = 3; out = 4;
		break;
	case 3:
		in = 4; out = 2;
		break;
	case 4:
		in = 2; out = 4;
		break;
	case 5:
		in = 2; out = 3;
		break;
	case 6:
		in = 3; out = 2;
		break;
	default:
		return true;
	}

	EphemerisData2 sv;

	sv.R = R;
	sv.V = V;
	sv.GMT = GMT;

	if (pRTCC->ELVCNV(sv, in, out, sv))
	{
		return true;
	}

	R = sv.R;
	V = sv.V;

	return false;
}

void TLTrajectoryComputers::LOPC(VECTOR3 R0, VECTOR3 V0, double GMT0, VECTOR3 L, int m, int n, double P, VECTOR3 &R3, VECTOR3 &V3, double &GMT3, double &mfm0, double &dpsi, VECTOR3 &DV)
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

double TLTrajectoryComputers::MCOMP(double dv, bool docked, bool useSPS, double m0)
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

bool TLTrajectoryComputers::PATCH(VECTOR3 &R, VECTOR3 &V, double &GMT, int Q, int KREF)
{
	//KREF = 1: Earth reference input, 2: Moon reference input

	VECTOR3 R_EM, V_EM, R_ES, R1, V1, R2, V2, R21, A2;
	double beta, Ratio_desired, GMTF, r1, r2, r21, mu1, mu2, d1, d2, v12, v22, DRatioDBeta, DDRatioDDBeta, Ratio, DRatio, dbeta;
	int i = 0;

	if (KREF == 1)
	{
		//Is position magnitude larger than 40 e.r.?
		if (length(R) > 40.0*R_E)
		{
			VECTOR3 R_p, V_p;
			double ainv, GMT_p;
			beta = EBETA(R, V, mu_E, ainv);
			XBETA(R, V, GMT, beta, KREF, R_p, V_p, GMT_p);
			//Is radius of periapsis larger than 40 e.r. as well?
			if (length(R_p) > 40.0*R_E)
			{
				return true;
			}
			//Initial guess 40 e.r., with reverse direction indication
			if (RBETA(R, -V, 40.0*R_E, -Q, mu_E, beta))
			{
				return true;
			}
		}
		else
		{
			//Initial guess 40 e.r.
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
		//Is position magnitude larger than 10 e.r.?
		if (length(R) > 10.0*R_E)
		{
			VECTOR3 R_p, V_p;
			double ainv, GMT_p;
			beta = EBETA(R, V, mu_M, ainv);
			XBETA(R, V, GMT, beta, KREF, R_p, V_p, GMT_p);
			//Is radius of periapsis larger than 10 e.r. as well?
			if (length(R_p) > 10.0*R_E)
			{
				return true;
			}
			//Initial guess 10 e.r., with reverse direction indication
			if (RBETA(R, -V, 10.0*R_E, -Q, mu_E, beta))
			{
				return true;
			}
		}
		else
		{
			//Initial guess 10 e.r.
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
			if (r1 > 60.0*R_E) return true;
		}
		else
		{
			if (r1 > 15.0*R_E) return true;
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

void TLTrajectoryComputers::PRCOMP(TLMCCGeneralizedIteratorArray *vars, VECTOR3 u_pc, VECTOR3 h_pc, double GMT_nd, double &RA_LPO1, double &A1, double &E1, double &gamma1, double &V_L, double &gamma_L, double &V2, double &DT_1st_pass)
{
	EphemerisData sv_LLS, sv_DOI, sv_DOI2, sv_preDOI, sv_1I;
	VECTOR3 u_lls_equ, u_lls, R_LLS_sg, V_LLS_sg, U, H;
	double R1, DR1, R2, RP_LPO1, a_lls, e_lls, a1, e1, dt1, dt2, dt3, eta2, da, DA, dw, dt, alpha1, deta, a, e, i, eta, n, P;
	int ITS;
	bool recycle = false;

	sv_LLS.RBI = BODY_MOON;

	//TIME from LOI targeting
	dw = vars->site_rotation_LPO2;
	DR1 = modf(vars->Revs_LPO1, &R1)*PI2;
	R2 = (double)vars->Revs_LPO2;

	RA_LPO1 = vars->H_A_LPO1 + vars->rad_lls;
	RP_LPO1 = vars->H_P_LPO1 + vars->rad_lls;
	a_lls = vars->rad_lls + (vars->H_A_LPO2 + vars->H_P_LPO2) / 2.0;
	e_lls = (vars->H_A_LPO2 + vars->rad_lls) / a_lls - 1.0;

	a1 = (RA_LPO1 + RP_LPO1) / 2.0;
	e1 = RA_LPO1 / a1 - 1.0;

	dt2 = DELTAT(a1, e1, vars->TA_LOI, DR1);
	eta2 = vars->TA_LOI + DR1;
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
	u_lls_equ = OrbMech::r_from_latlong(vars->lat_lls, vars->lng_lls);
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
	double p_lls, r, v, cos_gamma, gamma, a_L, e_L, R_L, theta_L, phi_L, psi_L, V1, A, E, dt_CORR;
	p_lls = a_lls * (1.0 - e_lls * e_lls);
	r = p_lls / (1.0 + e_lls * cos(dw));
	v = sqrt(mu_M*(2.0 / r - 1.0 / a_lls));
	cos_gamma = sqrt(mu_M*p_lls) / r / v;
	if (cos_gamma > 1.0)
	{
		cos_gamma = 1.0;
	}
	gamma = -OrbMech::sign(dw)*acos(cos_gamma);
	RVIO(false, R_LLS_sg, V_LLS_sg, r, v, vars->lng_lls, vars->lat_lls, gamma, vars->psi_lls);
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
	r = R_L - vars->dh_bias;
	E = (RA_LPO1 - r) / (RA_LPO1 + r * cos(eta2));
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

bool TLTrajectoryComputers::RBETA(VECTOR3 R0, VECTOR3 V0, double r, int Q, double mu, double &beta)
{
	double D0, r0, v0, ainv, e, QD, temp, D0sign;

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
	if (D0 >= 0)
	{
		D0sign = 1.0;
	}
	else
	{
		D0sign = -1.0;
	}
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
			H0 = D0sign * log(cosh_H0 + sqrt(temp));
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
		E0 = D0sign * E0;
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

void TLTrajectoryComputers::RNTSIM(VECTOR3 R, VECTOR3 V, double GMT, double lng_L, double &lat, double &lng, double &dlng)
{
	VECTOR3 P, S;
	double r, v, theta, alpha_L, alpha_G;

	r = length(R);
	v = length(V);

	P = V / v / cos(gamma_reentry) - R / r * tan(gamma_reentry);
	theta = MissionConstants.Reentry_range / 3443.933585;
	S = R / r * cos(theta) + P * sin(theta);

	lat = atan2(S.z, sqrt(S.x*S.x + S.y*S.y));
	alpha_L = atan2(S.y, S.x);
	alpha_G = pRTCC->PIAIES((GMT + Reentry_dt) / 3600.0);

	dlng = alpha_L - alpha_G - lng_L;
	OrbMech::normalizeAngle(dlng);
	lng = alpha_L - alpha_G;
	OrbMech::normalizeAngle(lng);

	if (dlng > PI)
	{
		dlng -= PI2;
	}
	if (dlng < -PI)
	{
		dlng += PI2;
	}
}

void TLTrajectoryComputers::RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double&psi)
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

void TLTrajectoryComputers::SCALE(VECTOR3 R0, VECTOR3 V0, double rad, double h, VECTOR3 &RF, VECTOR3 &VF)
{
	VECTOR3 H;

	RF = R0 * ((h + rad) / length(R0));
	H = unit(crossp(R0, V0));
	VF = unit(crossp(H, RF))*sqrt(mu_M / (h + rad));
}

void TLIAlternateMissionPolynomial(double C3, double sigma, double delta, double FW, double R_I, double mu, double& DV, double& alpha, double& beta, double& eta_alpha, double& R_P)
{
	//INPUT:
	//C3 - twice vis-viva energy desired at cutoff, (e.r./hr)^2
	//sigma - radians
	//delta - radians
	//FW - lbf/lbm
	//R_I - e.r.

	static const double TLI_POLY[5][28] = { {-0.46115370, 0.22773466, 0.19555311e-2, -0.34314288e1, 0.10972731e2, -0.61556301e3, 0.68371262e1, 0.27942824e2, -0.21180981e1,
		-0.11490251e3, 0.14027177e4, -0.10215248e1, -0.11176447, -0.28441985e-1, 0.15194679e2, 0.17363458e1, -0.29872899e3, 0.20685458e4, 0.23445758e3,
		-0.16844313e-2, 0.11073411, -0.12723911e-1, -0.9793999, 0.46225223, 0.24985093e2, 0.73621917e1, 0.67955599e-1, -0.23841464},
	{-0.30106979, 0.71115079, -0.68187747e-1, 0.30524303e1, -0.77950375e2, -0.86691181e2, -0.43251893e1, -0.43827950e2, 0.19927943e1,
	0.13794269e3, -0.19130249e3, -0.81325347e-1, -0.19995333, -0.10653570, 0.19806250, 0.12671075e2, -0.12117350e3, 0.10037573e4, -0.64043756e3,
	0.19854365e-1, 0.54047018e-1, 0.34129362e-1, -0.19836786, 0.46879377, 0.41743162e2, 0.14016618e2, 0.23463883, -0.59041779},
	{-0.34400710, 0.35035981, -0.69479865e-2, 0.33711079, 0.60217170e1, -0.47852738e1, -0.57183248, -0.6708471, 0.19946202,
		0.16108986e1, -0.32165452e2, -0.10019443e1, -0.29683074e-3, 0.17184573e-2, 0.78000508e-1, -0.52481997e-1, 0.13599529e2, -0.42908334e2, -0.28623014e2,
	0.11456195e-1, 0.19812576, -0.16722383e-1, -0.75046623, 0.29063760, 0.63805218, -0.20020239, -0.70863131e-1, -0.31706871},
	{0.45577371e-2, 0.0, -0.50707834e-3, 0.13575870e-1, -0.18127728e1, 0.73544148e1, 0.69169491e-1, 0.21581974, -0.30721717e-1,
	0.12382241, 0.26043273e1, -0.15620168e-2, -0.30462187e-2, -0.11997834e-3, 0.14836169, 0.13979050, -0.19255400e1, 0.12275655e2, 0.32929342, 
	-0.35659736e-3, 0.59006873e-3, 0.12711563e-2, -0.11267094e-1, -0.23112987e-1, 0.45292900, 0.12662005, 0.99587755, -0.11403328e-2},
	{0.44538134e-1, 0.98663545, -0.56569620e-2, 0.65162585, 0.27300140e2, -0.56837040e1, -0.12413913e1, -0.36245037e1, 0.41329476,
	0.89385858e1, -0.19328518e3, 0.38846744e-2, 0.15599869e-1, 0.37744704e-2, -0.16105487e1, -0.21292248e1, 0.75144900e2, -0.40806938e3, -0.13406229e1,
	0.35304466e-2, -0.85815036e-2, 0.61631299e-2, 0.60883411e-1, 0.12879841, -0.58643257e1, -0.1908357e1, -0.48126707e-1, 0.23212768e-1} };

	static const double TLI_POLY_BETA[4] = { 0.31853394e-3, -0.63648369e-4, 0.13602553e-3, -0.26753068e-2 };

	double DV_M, DI, X1, X2, X3, X4, X5;

	DV_M = sqrt(C3 + 2.0 * mu / R_I) - sqrt(mu / R_I);
	DI = asin(sin(abs(delta)) / sin(sigma + 0.13));

	X1 = DV_M;
	X2 = DI * DI / (abs(DI) + 0.006*X1*X1);
	X3 = sigma - 0.4;
	X4 = 1.0 / FW;
	X5 = R_I;

	double Y[5];

	for (int i = 0; i < 5; i++)
	{
		Y[i] = TLI_POLY[i][0] + TLI_POLY[i][1] * X1 + TLI_POLY[i][2] * pow(X1, 2) + TLI_POLY[i][3] * X2 + TLI_POLY[i][4] * pow(X2, 2) + TLI_POLY[i][5] * pow(X2, 3) + TLI_POLY[i][6] * X1*X2 + TLI_POLY[i][7] * pow(X1, 2)*pow(X2, 2) +
			TLI_POLY[i][8] * pow(X1, 2)*X2 + TLI_POLY[i][9] * X1*pow(X2, 2) + TLI_POLY[i][10] * X1*pow(X2, 3) + TLI_POLY[i][11] * X3 + TLI_POLY[i][12] * pow(X3, 2) + TLI_POLY[i][13] * X1*X3 + TLI_POLY[i][14] * X2*X3 +
			TLI_POLY[i][15] * X1*X2*X3 + TLI_POLY[i][16] * pow(X2, 2)*X3 + TLI_POLY[i][17] * X1*pow(X2, 3)*X3 + TLI_POLY[i][18] * pow(X1, 2)*pow(X2, 3)*X3 + TLI_POLY[i][19] * X4 + TLI_POLY[i][20] * X1*X4 +
			TLI_POLY[i][21] * pow(X1, 2)*X4 + TLI_POLY[i][22] * X1*X2*X3*X4 + TLI_POLY[i][23] * pow(X1, 2)*X2*X3*X4 + TLI_POLY[i][24] * pow(X1, 2)*pow(X2, 2)*X3*X4 + TLI_POLY[i][25] * pow(X1, 2)*X2*pow(X3, 2)*X4 +
			TLI_POLY[i][26] * X5 + TLI_POLY[i][27] * X1*X5;
	}

	alpha = Y[0];
	beta = Y[1] + (X1 * X4 * (TLI_POLY_BETA[0] + TLI_POLY_BETA[1] * X1 + TLI_POLY_BETA[2] * X1 * X4 + TLI_POLY_BETA[3] * X3*X3)) / (pow(X3 + 0.4, 2) + 3.0 * X2*X2);
	eta_alpha = Y[2];
	R_P = Y[3];
	DV = Y[4];
}

void TLINominalMissionPolynomial(double C3, double sigma, double delta, double FW, double R_I, double mu, double& DV, double& alpha, double& beta, double& eta_alpha, double& R_P)
{
	//INPUT:
	//C3 - twice vis-viva energy desired at cutoff, (e.r./hr)^2
	//sigma - radians
	//delta - radians
	//FW - lbf/lbm
	//R_I - e.r.

	static const double TLI_POLY[5][26] = { { 0.61967804e-1, 0.86219648e-2, -0.24371820e2, 0.41004848e4, -0.99229657e6, 0.14267564e9, -0.54688962e0, -0.78766288e0, 0.10261969e2, 0.52445599e1, -0.15527983e5,
		0.51931839e7, 0.18005069e0, 0.97069489e-1, 0.61442230e1, -0.87765197e3, -0.16502383e0,  0.63224468e0, 0.81844028e3,  -0.33505204e0, -0.92426341e-1, -0.18131458e4, -0.39193696e4},
	{ 0.51541772e0, -0.15528032e0, 0.27185659e2, 0.18763984e3, -0.92712145e6, 0.21114994e9, -0.56424215e0, 0.95105384e1, -0.15294910e2, 0.33896643e2, -0.26903240e5, 0.12131396e8, 0.25371175e0,
		0.22036833e0,  -0.22601576e2, 0.14378586e4, 0.31264540e0, -0.64046690e1,-0.39254760e4, -0.57987931e0, -0.22905910e0, 0.12621438e4, 0.70516077e4, -0.76940409e-4, 0.64393915e-4, 0.48483478e-4 },
	{ 0.48329414e0, 0.18759385e-2, 0.14031932e1, -0.13933485e3, 0.40515931e5, -0.48676865e7, -0.10155877e1, 0.83266987e-1, -0.28021958e1, 0.21207686, 0.98814614e3, -0.17699125e6,
		0.30964851, 0.13152495, 0.92808415, -0.32524984e2, 0.44675108e-2, -0.59053312e-3, -0.10061669e3, -0.60405621, 0.96317404e-2, 0.18026336e3, 0.81684373e2 },
	{ 0.46986962e-2, -0.44724715e-2, -0.17477015e1, 0.16880382e2, 0.14554490e5, -0.27167564e7, 0.25758493e-1, -0.77608981e-1, 0.57075666e0, 0.36716041e1, -0.96377142e3, -0.56658473e5,
		0.61201761e-2, 0.72885987e-2, 0.55059398e0, 0.44179572e1, -0.10348462e-1, 0.49107017e-1,0.44830843e3, 0.98814646e0, -0.73420686e-2, -0.27739134e2, 0.29172916e3},
	{ 0.18679213e1, 0.98320266e0, 0.25028715e2, -0.17104963e4, 0.37348295e6, -0.51521225e8, -0.21640574e0, 0.41744541e0,-0.65859807e1, -0.57578939e1, 0.99505664e4, -0.20041678e7,
		 -0.76178399e-2, -0.14737246e-2, -0.43988981e1, 0.20554193e3, 0.74293949e-1, -0.29415058e0, -0.16188456e4, -0.30007513e-1, 0.28463657e-1, 0.79392771e3, 0.12182700e4 } };

	double DV_M, DI, X1, X2, X3, X4, X5;

	DV_M = sqrt(C3 + 2.0 * mu / R_I) - sqrt(mu / R_I);
	DI = asin(sin(abs(delta)) / sin(sigma + 0.314));
	X1 = DV_M - 1.75;
	X2 = DI * DI - 0.0027;
	X3 = sigma - 0.148;
	X4 = 1.0 / FW;
	X5 = R_I;

	double Y[5];

	for (int i = 0; i < 5; i++)
	{
		Y[i] = TLI_POLY[i][0] + TLI_POLY[i][1] * X1 + TLI_POLY[i][2] * X2 + TLI_POLY[i][3] * pow(X2, 2) + TLI_POLY[i][4] * pow(X2, 3) + TLI_POLY[i][5] * pow(X2, 4) +
			TLI_POLY[i][6] * X3 + TLI_POLY[i][7] * pow(X3, 2) + TLI_POLY[i][8] * X1 * X2 + TLI_POLY[i][9] * X2 * X3 + TLI_POLY[i][10] * pow(X2, 2) * X3 +
			TLI_POLY[i][11] * pow(X2, 3) * X3 + TLI_POLY[i][12] * X4 + TLI_POLY[i][13] * X1 * X4 + TLI_POLY[i][14] * X2 * X4 + TLI_POLY[i][15] * X2 * X2 * X4 +
			TLI_POLY[i][16] * X3 * X4 + TLI_POLY[i][17] * X3 * X3 * X4 + TLI_POLY[i][18] * X2 * X2 * X3 * X4 + TLI_POLY[i][19] * X5 + TLI_POLY[i][20] * X1 * X3 +
			TLI_POLY[i][21] * X1 * X2 * X2 + TLI_POLY[i][22] * X1 * X2 * X2 * X3;
	}

	alpha = Y[0];
	beta = Y[1] + (X4 * X4 * (TLI_POLY[1][23] + TLI_POLY[1][24] * X1 + TLI_POLY[1][25] * X4 * X4)) / (pow(X3 + 0.148, 2) + 4.0 * pow(X2 + 0.0027, 2));
	eta_alpha = Y[2];
	R_P = Y[3];
	DV = Y[4];
}

SV2 TLTrajectoryComputers::TLIBRN(SV2 state, double C3, double sigma, double delta, double F_I, double F, double WDOT, double T_MRS, bool nominal, double &DV)
{
	//This function uses entirely units of Er, hours and pounds

	SV2 out;
	double W_I, DV_I, DT_B1, V_I, R_I, phi_dot_I, dphi_B1, DT_B2, dphi_B2, delta0, ddelta, W;

	static const double mu = 19.90941651408238;

	R_I = length(state.sv.R);
	V_I = length(state.sv.V);
	W_I = state.Mass;

	DV_I = sqrt(mu / R_I) - V_I;
	DT_B1 = DV_I / (F / W_I);

	phi_dot_I = V_I / R_I;
	dphi_B1 = phi_dot_I * DT_B1;

	DT_B2 = (1.0 - F_I / F) * T_MRS;
	dphi_B2 = phi_dot_I * DT_B2;
	W = W_I - WDOT * DT_B1;

	if (delta > 2.0 * RAD)
	{
		delta0 = 2.0 * RAD;
		ddelta = delta - 2.0 * RAD;
	}
	else if (delta < -2.0 * RAD)
	{
		delta0 = -2.0 * RAD;
		ddelta = -2.0 * RAD - delta;
	}
	else
	{
		delta0 = delta;
		ddelta = 0.0;
	}

	double DV0, alpha0, beta0, eta_alpha0, R_P0;

	if (nominal)
	{
		TLINominalMissionPolynomial(C3, sigma, delta, F / W, R_I, mu, DV0, alpha0, beta0, eta_alpha0, R_P0);
	}
	else
	{
		TLIAlternateMissionPolynomial(C3, sigma, delta, F / W, R_I, mu, DV0, alpha0, beta0, eta_alpha0, R_P0);
	}

	VECTOR3 R_I_u, N_I_u, S, T, N_c;
	double alpha, beta, eta_alpha, R_P, eta, T_B, C1, p, e, R_c, V_c, gamma_c;

	//Account for possible out-of-plane
	alpha = alpha0 + 4.66 * ddelta;
	beta = beta0 - 2.15 * ddelta;
	eta_alpha = eta_alpha0 + 0.923 * ddelta;
	R_P = R_P0 - 0.442 * ddelta;
	DV = DV0 + 6.33 * ddelta;

	//Account for mixture ratio shift
	alpha += dphi_B1 + dphi_B2;
	beta += 3.0 / 4.0 * dphi_B1 + dphi_B2;
	eta = eta_alpha - alpha;
	DV += DV_I;

	T_B = W_I / WDOT * (1.0 - exp(-(DV / (9.80665 * 3600.0*3600.0 / R_E) * WDOT / F))) + DT_B2;

	R_I_u = unit(state.sv.R);
	N_I_u = unit(crossp(state.sv.R, state.sv.V));
	T = R_I_u * cos(delta)*cos(alpha) + crossp(N_I_u, R_I_u)*cos(delta)*sin(alpha) + N_I_u * sin(delta);
	S = R_I_u * cos(beta) + crossp(N_I_u, R_I_u)*sin(beta);
	N_c = unit(crossp(T, S));
	C1 = R_P * sqrt(2.0*mu / R_P + C3);
	p = C1 * C1 / mu;
	e = sqrt(1.0 + C3 / mu * p);
	R_c = p / (1.0 + e * cos(eta));
	V_c = mu / C1 * sqrt(1.0 + 2.0*e*cos(eta) + e * e);
	gamma_c = atan2(e*sin(eta), 1.0 + e * cos(eta));
	out.sv.R = (T*cos(sigma + eta) + crossp(N_c, T)*sin(sigma + eta))*R_c;
	out.sv.V = (-T * sin(sigma + eta - gamma_c) + crossp(N_c, T)*cos(sigma + eta - gamma_c))*V_c;
	out.sv.GMT = state.sv.GMT + T_B;
	out.sv.RBI = state.sv.RBI;
	out.Mass = W - WDOT * (T_B - DT_B2);

	return out;
}

void TLTrajectoryComputers::XBETA(VECTOR3 R0, VECTOR3 V0, double GMT0, double beta, int K, VECTOR3 &RF, VECTOR3 &VF, double &GMTF)
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

double TLTrajectoryComputers::GetMU(int k)
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