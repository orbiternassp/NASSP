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

#include "TLTrajectoryComputers.h"

struct TLMCCDataTable
{
	TLMCCDataTable::TLMCCDataTable()
	{
		for (int i = 0;i < 25; i++)
		{
			data[i] = 0.0;
		}
	}

	//Time at which block was generated
	double GMTTimeFlag = 0.0;
	//TLMCC Mode that was used to generate block
	int mode = 0;
	double dt_upd_nom = 0.0;//???

	union
	{
		double data[25];
		struct
		{
			//EMP latitude of TLI pericynthion
			double lat_pc1;
			//EMP latitude of LOI pericynthion
			double lat_pc2;
			//EMP longitude of TLI pericynthion
			double lng_pc1;
			//EMP longitude of LOI pericynthion
			double lng_pc2;
			//Height of TLI pericynthion
			double h_pc1;
			//Height of LOI pericynthion
			double h_pc2;
			//GET of TLI Ignition
			double GET_TLI;
			//GMT of Node
			double GMT_nd;
			//Latitude of Node
			double lat_nd;
			//Longitude of Node
			double lng_nd;
			//Height of Node
			double h_nd;
			//Delta Azimuth of LOI
			double dpsi_loi;
			//Flight Path Angle of LOI
			double gamma_loi;
			//DT of LPO (LOI to TEI)
			double T_lo;
			//DT of LLS (LOI to LLS first pass)
			double dt_lls;
			//Azimuth of LLS
			double psi_lls;
			//Latitude of LLS
			double lat_lls;
			//Longitude of LLS
			double lng_lls;
			//Radius of LLS
			double rad_lls;
			//Delta azimuth of TEI
			double dpsi_tei;
			//Delta V of TEI
			double dv_tei;
			//DT of Transearth (TEI to EI)
			double T_te;
			//Inclination of free return
			double incl_fr;
			//GMT of TLI pericynthion
			double GMT_pc1;
			//GMT of LOI pericynthion
			double GMT_pc2;
		};
	};
};

struct TLMCCMEDQuantities
{
	int Mode;
	double T_MCC;
	EphemerisData sv0;
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

class TLMCCProcessor : public TLTrajectoryComputers
{
public:
	TLMCCProcessor(RTCC *r);
	void Init(TLMCCDataTable data, TLMCCMEDQuantities med, TLMCCMissionConstants cst, double GMTBase);

	void Main(TLMCCOutputData &out);

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
	void IntegratedXYZTTrajectory(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double R_nd, double lat_nd, double lng_nd, double GMT_node);
	void ConicFreeReturnFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl);
	void ConicFreeReturnInclinationFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_pg, double lat_pl_min = 0, double lat_pl_max = 0);
	void ConicFreeReturnOptimizedInclinationFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double inc_pg_min, double inc_pg_max, int inc_class);
	void IntegratedFreeReturnFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double lat_pl);
	void IntegratedFreeReturnInclinationFlyby(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double H_pl, double inc_fr);
	void ConicFreeReturnOptimizedFixedOrbitToLLS(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi);
	void ConicNonfreeReturnOptimizedFixedOrbitToLLS(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_min, double T_max);
	void ConicFreeReturnOptimizedFreeOrbitToLOPC(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double dpsi_loi, double DT_lls, double AZ_min, double AZ_max);
	void ConicNonfreeReturnOptimizedFreeOrbitToLOPC(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double T_min, double T_max, double h_pl);
	void ConicFullMissionFreeOrbit(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double h_pl, double gamma_loi, double dpsi_loi, double dt_lls, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double AZ_min, double AZ_max, double mass, bool freereturn, double T_min = 0.0, double T_max = 0.0);
	void ConicFullMissionFixedOrbit(EphemerisData sv0, double dv_guess, double dgamma_guess, double dpsi_guess, double gamma_loi, double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, double mass, bool freereturn, double T_min = 0.0, double T_max = 0.0);
	void ConicTransEarthInjection(double T_lo, double dv_tei, double dgamma_tei, double dpsi_tei, double T_te, bool lngiter);
	VECTOR3 CalcLOIDV(EphemerisData sv_MCC_apo, double gamma_nd);

	//Subroutines	
	double DDELTATIME(double a, double dt_apo, double xm, double betam, double dt);
	EphemerisData PPC(EphemerisData SIN, double lat1, double lng1, double azi1, int RT1, int INTL, double &DVS);
	
	EphemerisData sv_MCC; //In TLMCC coordinate system
	EphemerisData sv_MCC_SOI; //In the "correct" coordinate system
	
	VECTOR3 DV_MCC;

	TLMCCDataTable DataTable;
	TLMCCMEDQuantities MEDQuantities;

	TLMCCDataTable outtab;
};