/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Trajectory Computers for TLI and MCC Processors (Header)

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

#pragma once

#include "RTCCModule.h"
#include "RTCCTables.h"

struct TLMCCGeneralizedIteratorArray
{
	//Input
	EphemerisData sv0;
	int KREF_MCC;
	double lat_lls;
	double lng_lls;
	double rad_lls; //Radius of lunar landing site
	double psi_lls;
	VECTOR3 LOIOffset = _V(0, 0, 0);
	bool Config; //false = undocked, true = docked
	bool useSPS;
	double TA_LOI;
	double LMMass;
	double site_rotation_LPO2;
	double Revs_LPO1;
	double H_A_LPO1;
	double H_P_LPO1;
	int Revs_LPO2;
	double H_A_LPO2;
	double H_P_LPO2;
	double dh_bias;
	//For first guess logic
	bool TLMCIntegrating;
	//For integrating trajectory computer
	bool NodeStopIndicator;
	bool EllipticalCaseIndicator;
	bool TLIOnlyIndicator = false;
	//For conic trajectory computer
	bool FreeReturnIndicator;
	bool FreeReturnOnlyIndicator;
	bool TLIIndicator = false;
	bool FixedOrbitIndicator;
	bool FirstSelect;
	bool FirstOptimize;
	//For conic and integrated
	bool LunarFlybyIndicator;
	bool MidcourseCorrectionIndicator = true;
	bool LLSStopIndicator;

	//Output
	double dv_mcc;
	double dgamma_mcc;
	double dpsi_mcc;
	VECTOR3 V_MCC;
	double t_tl;
	//All EMP
	double lat_nd;
	double lng_nd;
	double h_nd;
	double gamma_nd;
	double GMT_nd;
	double h_pl;
	double v_pl;
	double gamma_pl;
	double psi_pl;
	double lat_pl;
	double lng_pl;
	double GMT_pl;
	double incl_pl;
	double dpsi_loi;
	EphemerisData sv_loi;
	double dt_lls;
	double AZ_act;
	EphemerisData sv_lls1;
	EphemerisData sv_lls2;
	double T_lo;
	VECTOR3 DV_LOPC;
	double GMT_tei;
	double dv_tei;
	double dgamma_tei;
	double dpsi_tei;
	VECTOR3 DV_TEI;
	double v_EI;
	double h_fr;
	double v_EI_pr;
	double h_pr;
	//Inclination of free return
	double incl_fr;
	//Inclination of powered return
	double incl_pr;
	double T_te;
	double lat_ip;
	double lng_ip;
	double GMT_ip;
	double lat_ip_pr;
	double lng_ip_pr;
	double GMT_ip_pr;
	double theta;
	double DH_Node;
	//Time bias in hours
	double dt_bias_conic_prec = 0.0;
	EphemerisData sv_tli_ign, sv_tli_cut;

	//Masses
	//Initial mass
	double M_i;
	//After TLI
	double M_tli;
	//After MCC
	double M_mcc;
	//After LOI
	double M_loi;
	//After LOI-2 or DOI
	double M_cir;
	//After LOPC
	double M_lopc;
	//After TEI
	double M_tei;

	//PRCOMP data
	EphemerisData SGSLOI;
	double RA_LPO1;
	//New Version
	double gamma_L;
	double A1;
	double E1;
	double gamma1;
	double V2;
	double V_L;

	//TLI only
	double dv_TLI;
	double C3_TLI; //Er^2/hr^2
	double dt_EPO; //sec
	double delta_TLI;
	double sigma_TLI;
};

struct TLMCCMissionConstants
{
	double delta, sigma;
	int n, m;
	double lambda_IP;
	double V_pcynlo;
	//Time bias for LPO1 in hours
	double dt_bias;
	double T_t1_min_dps;
	double T_t1_max_dps;
	double H_LPO;
	double INCL_PR_MAX;
	double Reentry_range;
};

class TLTrajectoryComputers : public RTCCModule
{
public:
	TLTrajectoryComputers(RTCC *r);
	virtual ~TLTrajectoryComputers();

	//The trajectory computers
	bool FirstGuessTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool ConicMissionComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool IntegratedTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
protected:

	void Init(TLMCCMissionConstants constants, double GMTBase);

	//Subroutines
	void BURN(VECTOR3 R, VECTOR3 V, double dv, double dgamma, double dpsi, double isp, double &mfm0, VECTOR3 &RF, VECTOR3 &VF);
	void BURN(VECTOR3 R, VECTOR3 V, int opt, double gamma0, double v_pl, double dv, double dgamma, double dpsi, double isp, double mu, double &v_c, double &dv_R, double &mfm0, VECTOR3 &RF, VECTOR3 &VF);
	bool CTBODY(VECTOR3 R0, VECTOR3 V0, double GMT0, double GMTF, int K, double mu, VECTOR3 &RF, VECTOR3 &VF);
	bool CTBODY(VECTOR3 R0, VECTOR3 V0, double GMT0, double GMTF, int K, double mu, double &alpha, double &F1, double &F2, double &F3, double &F4, VECTOR3 &RF, VECTOR3 &VF);
	double DELTAT(double a, double e, double eta, double deta);
	bool DGAMMA(double r0, double ainv, double gamma, double &H, double &E, double &beta, double &e);
	double EBETA(VECTOR3 R, VECTOR3 V, double mu, double &ainv);
	void ELEMT(VECTOR3 R, VECTOR3 V, double mu, VECTOR3 &H, double &a, double &e, double &i, double &n, double &P, double &eta);
	bool EPHEM(double GMT, VECTOR3 &R_EM, VECTOR3 &V_EM, VECTOR3 &R_ES);
	void FCOMP(double a, double &F1, double &F2, double &F3, double &F4);
	bool LIBRAT(VECTOR3 &R, double GMT, int K);
	bool LIBRAT(VECTOR3 &R, VECTOR3 &V, double GMT, int K);
	void LOPC(VECTOR3 R0, VECTOR3 V0, double GMT0, VECTOR3 L, int m, int n, double P, VECTOR3 &R3, VECTOR3 &V3, double &GMT3, double &mfm0, double &dpsi, VECTOR3 &DV);
	double MCOMP(double dv, bool docked, bool useSPS, double m0);
	bool PATCH(VECTOR3 &R, VECTOR3 &V, double &GMT, int Q, int KREF);
	void PRCOMP(TLMCCGeneralizedIteratorArray *vars, VECTOR3 u_pc, VECTOR3 h_pc, double GMT_nd, double &RA_LPO1, double &A1, double &E1, double &gamma1, double &V_L, double &gamma_L, double &V2, double &DT_1st_pass);
	bool RBETA(VECTOR3 R0, VECTOR3 V0, double r, int Q, double mu, double &beta);
	void RNTSIM(VECTOR3 R, VECTOR3 V, double GMT, double lng_L, double &lat, double &lng, double &dlng);
	void RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double& psi);
	void SCALE(VECTOR3 R0, VECTOR3 V0, double rad, double h, VECTOR3 &RF, VECTOR3 &VF);
	SV2 TLIBRN(SV2 state, double C3, double sigma, double delta, double F_I, double F, double WDOT, double T_MRS, bool nominal, double &DV);
	void XBETA(VECTOR3 R0, VECTOR3 V0, double GMT0, double beta, int K, VECTOR3 &RF, VECTOR3 &VF, double &GMTF);
	double GetMU(int k);

	TLMCCGeneralizedIteratorArray outarray;
	TLMCCMissionConstants MissionConstants;
	double GMTBASE;

	//Constants
	double R_E, R_M, mu_E, mu_M;
	double isp_SPS, isp_DPS, isp_MCC;
	double F_I_SIVB; //S-IVB thrust magnitude from ignition to MRS
	double F_SIVB; //S-IVB thrust magnitude from MRS to cutoff
	double WDOT_SIVB; //Mass flow rate of S-IVB
	double T_MRS_SIVB; //Estimated time of MRS, measured from ignition
	double gamma_reentry;
	double Reentry_dt;
};