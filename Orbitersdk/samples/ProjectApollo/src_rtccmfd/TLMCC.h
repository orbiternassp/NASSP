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

struct TLMCCDataTable
{
	double GET_TLI;
	double GMT_pc1;
	double GMT_pc2;
	double GMT_nd;
	double H_nd;
	double lat_nd;
	double lng_nd;
	double H_pc1;
	double H_pc2;
	double lat_pc1;
	double lat_pc2;
	double lng_pc1;
	double lng_pc2;
	double gamma_loi;
	double dpsi_loi;
	double dt_lls;
	double T_lo;
	double psi_lls;
	double dpsi_tei;
	double dv_tei;
	double T_te;
	double lat_lls;
	double lng_lls;
	double R_lls;
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
	double Revs_LPO2;
	double site_rotation_LPO2;
	double Revs_circ;
	double H_T_circ;
	double TA_LOI;
};

struct TLMCCMissionConstants
{
	int n, m;
	double lambda_IP;
	double V_pcynlo;
};

struct TLMCCOutputData
{
	//For Display
	int Mode;
	int Return;
	double AZ_min;
	double AZ_max;
	int Config;
	double GET_MCC;
	VECTOR3 DV_MCC;
	double YAW_MCC;
	double h_PC;
	double GET_LOI;
	VECTOR3 DV_LOI;
	double AZ_act;
	double H_bo;
	double delta_lat;
	double HA_LPO;
	double HP_LPO;
	VECTOR3 DV_LOPC;
	double GET_TEI;
	VECTOR3 DV_TEI;
	double DV_REM;
	//Landing computed at Earth
	double GET_LC;
	double lat_IP;
	double lng_IP;
	std::string STAID;
	double GMTV;
	double GETV;
	double CSMWT;
	double LMWT;
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
	//For first guess logic
	bool TLMCIntegrating;
	//For integrating trajectory computer
	bool MidcourseCorrectionIndicator = true;
	bool NodeStopIndicator;
	bool LunarFlybyIndicator;
	bool FreeReturnInclinationIndicator;

	//Output
	double dv_mcc;
	double dgamma_mcc;
	double dpsi_mcc;
	VECTOR3 V_MCC;
	//All EMP
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
	double dgamma_loi;
	double dpsi_loi;
	double dt_lls;
	double AZ_act;

	//Masses
	double M_i;
	double M_mcc;
	double M_loi;
	double M_lc;
	double M_lopc;
	double M_tei;
};

struct TLMCCConicTrajectoryVars
{
	MPTSV sv0;
	bool FreeReturnIndicator;
	bool FreeReturnOnlyIndicator;
	bool FixedOrbitIndicator;
	bool FirstSelect;
	bool FreeReturnInclinationIndicator;
	bool OptimizeIndicator = false;
	bool MCCIndicator = true;
	bool TLIIndicator = false;
	bool AfterLOIStopIndicator = false;
	bool HasLOI2Indicator = true;
};

class TLMCCProcessor
{
public:
	TLMCCProcessor();
	void Init(PZEFEM *ephem, TLMCCDataTable data, TLMCCMEDQuantities med, TLMCCMissionConstants cst);
	void Main(TLMCCOutputData &out);

	//The trajectory computers
	bool FirstGuessTrajectoryComputer(std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool ConicMissionComputer(std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool IntegratedNodeComputer(std::vector<double> var, void *varPtr, std::vector<double>& arr);

protected:

	//The Options
	void Option1();
	/*void Option2();
	void Option3();
	void Option4();
	void Option5();
	void Option6();
	void Option7();
	void Option8();
	void Option9A();
	void Option9B();*/

	//These appear as the boxes in the main program flow
	void ConvergeTLMC(double V, double azi, double lng, double lat, double r, double GMT_pl, bool integrating);
	void IntegratedXYZTTrajectory(MPTSV sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double R_nd, double lat_nd, double lng_nd, double GMT_node);
	VECTOR3 ConicFreeReturnFlyby(MPTSV sv0, double H_pl, double lat_pl, VECTOR3 guess);
	VECTOR3 ConicFreeReturnInclinationFlyby(MPTSV sv0, double H_pl, double inc_pg, VECTOR3 guess, double lat_pl_min = 0, double lat_pl_max = 0);
	VECTOR3 ConicFreeReturnOptimizedInclinationFlyby(MPTSV sv0, double inc_pg_min, double inc_pg_max, int inc_class, VECTOR3 guess);
	VECTOR3 IntegratedFreeReturnFlyby(MPTSV sv0, double H_pl, double lat_pl, VECTOR3 guess);
	VECTOR3 IntegratedFreeReturnInclinationFlyby(MPTSV sv0, double H_pl, double inc_fr, VECTOR3 guess);
	VECTOR3 ConicFreeReturnOptimizedFixedOrbitBAP(MPTSV sv0, VECTOR3 guess);
	void ConicFreeReturnFixedOrbitLOI2BAP(MPTSV sv0, std::vector<double> guess, std::vector<double> &x_res, std::vector<double> &y_res, bool optimize = false, double mass = 0.0);
	VECTOR3 CalcLOIDV(MPTSV sv_MCC_apo, double gamma_nd);

	//Subroutines
	void EmpiricalFirstGuess(double r, double lng, double dt, double &V, double &lambda);
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
	void RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double&psi);
	double MCOMP(double dv, bool docked, bool useSPS, double m0);
	void RNTSIM(VECTOR3 R, VECTOR3 V, double MJD, double lng_L, double &lat, double &lng, double &dlng);
	void LOPC(VECTOR3 R0, VECTOR3 V0, double MJD0, VECTOR3 L, int m, int n, double P, VECTOR3 &R3, VECTOR3 &V3, double &MJD3, double &mfm0, double &dpsi);
	void ELEMT(VECTOR3 R, VECTOR3 V, double mu, VECTOR3 &H, double &a, double &e, double &i, double &n, double &P, double &eta);
	void PRCOMP(VECTOR3 R_nd, VECTOR3 V_nd, double MJD_nd, double &RA_LPO1, double &A_L, double &E_L, VECTOR3 &u_pl, double &dw_p, double &dh_a, double &dh_p, double &DT, double &DT_1st_pass, MPTSV &SGSLOI);
	double DDELTATIME(double a, double dt_apo, double xm, double betam, double dt);
	void SCALE(VECTOR3 R0, VECTOR3 V0, double H_A, double H_P, VECTOR3 &RF, VECTOR3 &VF);

	double R_E, R_M, mu_E, mu_M;

	PZEFEM *ephemeris;

	MPTSV sv_MCC;
	double isp_SPS, isp_DPS, isp_MCC, Wdot;
	int KREF_MCC;
	OBJHANDLE hMoon;
	VECTOR3 DV_MCC;

	double gamma_reentry;
	double Reentry_range; //In NM
	double Reentry_dt;

	TLMCCDataTable DataTable;
	TLMCCMEDQuantities MEDQuantities;
	TLMCCMissionConstants Constants;

	TLMCCGeneralizedIteratorArray outarray;
	double lat_pl_stored;
	double inc_fr_stored;
	double MJD_pl_stored;
	double h_pl_stored;
	double mass_stored;
};