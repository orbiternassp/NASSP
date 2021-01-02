/****************************************************************************
This file is part of Project Apollo - NASSP

Translunar Midcourse Correction Processor (Header)

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

struct TLMCCDataTable
{
	//Time at which block was generated
	double GMTTimeFlag = 0.0;
	//TLMCC Mode that was used to generate block
	int mode = 0;
	//EMP latitude of TLI pericynthion
	double lat_pc1 = 0.0;
	//EMP latitude of LOI pericynthion
	double lat_pc2 = 0.0;
	//EMP longitude of TLI pericynthion
	double lng_pc1 = 0.0;
	//EMP longitude of LOI pericynthion
	double lng_pc2 = 0.0;
	//Height of TLI pericynthion
	double h_pc1 = 0.0;
	//Height of LOI pericynthion
	double h_pc2 = 0.0;
	//GET of TLI Ignition
	double GET_TLI = 0.0;
	//GMT of Node
	double GMT_nd = 0.0;
	//Latitude of Node
	double lat_nd = 0.0;
	//Longitude of Node
	double lng_nd = 0.0;
	//Height of Node
	double h_nd = 0.0;
	//Delta Azimuth of LOI
	double dpsi_loi = 0.0;
	//Flight Path Angle of LOI
	double gamma_loi = 0.0;
	//DT of LPO (LOI to TEI)
	double T_lo = 0.0;
	//DT of LLS (LOI to LLS first pass)
	double dt_lls = 0.0;
	//Azimuth of LLS
	double psi_lls = 0.0;
	//Latitude of LLS
	double lat_lls = 0.0;
	//Longitude of LLS
	double lng_lls = 0.0;
	//Radius of LLS
	double rad_lls = 0.0;
	//Delta azimuth of TEI
	double dpsi_tei = 0.0;
	//Delta V of TEI
	double dv_tei = 0.0;
	//DT of Transearth (TEI to EI)
	double T_te = 0.0;
	//Inclination of free return
	double incl_fr = 0.0;
	//GMT of TLI pericynthion
	double GMT_pc1 = 0.0;
	//GMT of LOI pericynthion
	double GMT_pc2 = 0.0;
	double dt_upd_nom = 0.0;//???
};

struct TLMCCMEDQuantities
{
	int Mode;
	double T_MCC;
	double GMTBase;
	double GETBase;
	MPTSV sv0;
	double CSMMass;
	double LMMass;
	bool Config; //false = undocked, true = docked
	bool useSPS;
	double AZ_min;
	double AZ_max;
	double H_pl;
	double INCL_fr;
	double H_pl_TLMC;
	double GET_nd_min;
	double GET_nd_max;
	double lat_bias;
	double H_pl_min;
	double H_pl_max;
	double H_A_LPO1;
	double H_P_LPO1;
	double Revs_LPO1;
	double H_A_LPO2;
	double H_P_LPO2;
	int Revs_LPO2;
	double site_rotation_LPO2;
	int Revs_circ;
	double H_T_circ;
	double TA_LOI;
	double T_min_sea;
	double T_max_sea;
	double dh_bias = 0.0;
	double H_pl_mode5;
};

struct TLMCCMissionConstants
{
	int n, m;
	double lambda_IP;
	double V_pcynlo;
	double dt_bias;
	double T_t1_min_dps;
	double T_t1_max_dps;
	double H_LPO;
	double INCL_PR_MAX;
};

struct TLMCCDisplayData
{
	int Mode = 0;
	int Return = 0;
	double AZ_min = 0.0;
	double AZ_max = 0.0;
	int Config = 0;
	double GET_MCC = 0.0;
	VECTOR3 DV_MCC;
	double YAW_MCC = 0.0;
	double h_PC = 0.0;
	double GET_LOI = 0.0;
	VECTOR3 DV_LOI = _V(0, 0, 0);
	double AZ_act = 0.0;
	double H_bo = 0.0;
	double delta_lat = 0.0;
	double HA_LPO = 0.0;
	double HP_LPO = 0.0;
	VECTOR3 DV_LOPC = _V(0, 0, 0);
	double GET_TEI = 0.0;
	VECTOR3 DV_TEI = _V(0, 0, 0);
	double DV_REM = 0.0;
	double incl_fr = 0.0;
	double incl_pr = 0.0;
	double v_EI = 0.0;
	double gamma_EI = 0.0;
	//Landing computed at Earth
	double GET_LC = 0.0;
	double lat_IP = 0.0;
	double lng_IP = 0.0;
	std::string STAID;
	double GMTV = 0.0;
	double GETV = 0.0;
	double CSMWT = 0.0;
	double LMWT = 0.0;
};

struct TLMCCOutputData
{
	TLMCCDisplayData display;
	//For Data Table
	TLMCCDataTable outtab;
	//For MPT
	VECTOR3 R_MCC;
	VECTOR3 V_MCC;
	double GMT_MCC;
	int RBI;
	VECTOR3 V_MCC_apo;
};

struct TLMCCGeneralizedIteratorArray
{
	//Input
	MPTSV sv0;
	VECTOR3 LOIOffset = _V(0, 0, 0);
	//For first guess logic
	bool TLMCIntegrating;
	//For integrating trajectory computer
	bool NodeStopIndicator;
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
	double MJD_nd;
	double h_pl;
	double v_pl;
	double gamma_pl;
	double psi_pl;
	double lat_pl;
	double lng_pl;
	double MJD_pl;
	double incl_pl;
	double dpsi_loi;
	MPTSV sv_loi;
	double dt_lls;
	double AZ_act;
	MPTSV sv_lls1;
	MPTSV sv_lls2;
	double T_lo;
	VECTOR3 DV_LOPC;
	double MJD_tei;
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
	double MJD_ip;
	double lat_ip_pr;
	double lng_ip_pr;
	double MJD_ip_pr;
	double theta;
	double DH_Node;
	double dt_bias_conic_prec = 0.0;

	//Masses
	//Initial mass
	double M_i;
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
	MPTSV SGSLOI;
	double RA_LPO1;
	//New Version
	double gamma_L;
	double A1;
	double E1;
	double gamma1;
	double V2;
	double V_L;
};

class TLMCCProcessor : public RTCCModule
{
public:
	TLMCCProcessor(RTCC *r);
	void Init(TLMCCDataTable data, TLMCCMEDQuantities med, TLMCCMissionConstants cst);
	void Main(TLMCCOutputData &out);

	//The trajectory computers
	bool FirstGuessTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool ConicMissionComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool IntegratedTrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);

protected:

	//The Options
	void Option1();
	void Option2();
	void Option3();
	void Option4();
	void Option5();
	void Option6();
	void Option7();
	void Option8();
	void Option9A();
	void Option9B();

	//These appear as the boxes in the main program flow
	bool ConvergeTLMC(double V, double azi, double lng, double lat, double r, double GMT_pl, bool integrating);
	void IntegratedXYZTTrajectory(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double R_nd, double lat_nd, double lng_nd, double GMT_node);
	void ConicFreeReturnFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl);
	void ConicFreeReturnInclinationFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_pg, double lat_pl_min = 0, double lat_pl_max = 0);
	void ConicFreeReturnOptimizedInclinationFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double inc_pg_min, double inc_pg_max, int inc_class);
	void IntegratedFreeReturnFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl);
	void IntegratedFreeReturnInclinationFlyby(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_fr);
	void ConicFreeReturnOptimizedFixedOrbitToLLS(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi);
	void ConicNonfreeReturnOptimizedFixedOrbitToLLS(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_min, double T_max);
	void ConicFreeReturnOptimizedFreeOrbitToLOPC(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double dpsi_loi, double DT_lls, double AZ_min, double AZ_max);
	void ConicNonfreeReturnOptimizedFreeOrbitToLOPC(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double T_min, double T_max, double h_pl);
	void ConicFullMissionFreeOrbit(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double h_pl, double gamma_loi, double dpsi_loi, double dt_lls, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double AZ_min, double AZ_max, double mass, bool freereturn, double T_min = 0.0, double T_max = 0.0);
	void ConicFullMissionFixedOrbit(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double mass, bool freereturn, double T_min = 0.0, double T_max = 0.0);
	void ConicTransEarthInjection(double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, bool lngiter);
	VECTOR3 CalcLOIDV(MPTSV sv_MCC_apo, double gamma_nd);

	//Subroutines
	bool PATCH(VECTOR3 &R, VECTOR3 &V, double &MJD, int Q, int KREF);
	bool LIBRAT(VECTOR3 &R, double MJD, int K);
	bool LIBRAT(VECTOR3 &R, VECTOR3 &V, double MJD, int K);
	double EBETA(VECTOR3 R, VECTOR3 V, double mu, double &ainv);
	bool RBETA(VECTOR3 R0, VECTOR3 V0, double r, int Q, double mu, double &beta);
	void XBETA(VECTOR3 R0, VECTOR3 V0, double MJD0, double beta, int K, VECTOR3 &RF, VECTOR3 &VF, double &MJDF);
	double GetMU(int k);
	void FCOMP(double a, double &F1, double &F2, double &F3, double &F4);
	bool EPHEM(double MJD, VECTOR3 &R_EM, VECTOR3 &V_EM, VECTOR3 &R_ES);
	bool CTBODY(VECTOR3 R0, VECTOR3 V0, double MJD0, double MJDF, int K, double mu, VECTOR3 &RF, VECTOR3 &VF);
	bool CTBODY(VECTOR3 R0, VECTOR3 V0, double MJD0, double MJDF, int K, double mu, double &alpha, double &F1, double &F2, double &F3, double &F4, VECTOR3 &RF, VECTOR3 &VF);
	bool DGAMMA(double r0, double ainv, double gamma, double &H, double &E, double &beta, double &e);
	void BURN(VECTOR3 R, VECTOR3 V, double dv, double dgamma, double dpsi, double isp, double &mfm0, VECTOR3 &RF, VECTOR3 &VF);
	void BURN(VECTOR3 R, VECTOR3 V, int opt, double gamma0, double v_pl, double dv, double dgamma, double dpsi, double isp, double mu, double &v_c, double &dv_R, double &mfm0, VECTOR3 &RF, VECTOR3 &VF);
	void RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double& psi);
	double MCOMP(double dv, bool docked, bool useSPS, double m0);
	void RNTSIM(VECTOR3 R, VECTOR3 V, double MJD, double lng_L, double &lat, double &lng, double &dlng);
	void LOPC(VECTOR3 R0, VECTOR3 V0, double MJD0, VECTOR3 L, int m, int n, double P, VECTOR3 &R3, VECTOR3 &V3, double &MJD3, double &mfm0, double &dpsi, VECTOR3 &DV);
	void ELEMT(VECTOR3 R, VECTOR3 V, double mu, VECTOR3 &H, double &a, double &e, double &i, double &n, double &P, double &eta);
	void PRCOMP(VECTOR3 u_pc, VECTOR3 h_pc, double MJD_nd, double &RA_LPO1, double &A1, double &E1, double &gamma1, double &V_L, double &gamma_L, double &V2, double &DT_1st_pass);
	double DDELTATIME(double a, double dt_apo, double xm, double betam, double dt);
	void SCALE(VECTOR3 R0, VECTOR3 V0, double h, VECTOR3 &RF, VECTOR3 &VF);
	MPTSV PPC(MPTSV SIN, double lat1, double lng1, double azi1, int RT1, int INTL, double &DVS);
	MPTSV TLIBRN(MPTSV sv, double C3, double sigma, double delta, double FW, double W_I, double F_I, double F, double W_dot, double T_MRS);
	double DELTAT(double a, double e, double eta, double deta);

	double R_E, R_M, mu_E, mu_M;

	MPTSV sv_MCC;
	double isp_SPS, isp_DPS, isp_MCC, Wdot;
	int KREF_MCC;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 DV_MCC;

	double gamma_reentry;
	double Reentry_range; //In NM
	double Reentry_dt;

	TLMCCDataTable DataTable;
	TLMCCMEDQuantities MEDQuantities;
	TLMCCMissionConstants Constants;

	TLMCCGeneralizedIteratorArray outarray;
	TLMCCDataTable outtab;
};