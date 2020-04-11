/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2020

Lunar Orbit Insertion Targeting (Header)

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

#include "Orbitersdk.h"
#include "RTCCTables.h"

namespace rtcc
{

	struct LOIOptions
	{
		//State vector at hyperbolic perilune
		EphemerisData SPH;
		double HA_LPO;
		double HP_LPO;
		double psi_mx;
		double psi_DS;
		double psi_mn;
		double DV_maxp;
		double DV_maxm;
		double HA_LLS;
		double HP_LLS;
		double R_LLS;
		double REVS1;
		int REVS2;
		double eta1;
		double dh_bias;
		double DW;
		double lat_LLS;
		double lng_LLS;
		double GMTBASE;
		bool usePlaneSolnForInterSoln = true;
		//Convergence criterium for DELV2
		double RARPGT = 10.0*1852.0;
	};

	struct LOIDisplayData
	{
		//Height at node
		double H_ND = 0.0;
		//True anomaly on hyperbola at node
		double eta_N = 0.0;
		//Angle between desired and maximum lunar orbit plane (?)
		double delta_op = 0.0;
		//Angle between desired lunar orbit plane and achieved lunar orbit plane
		double theta = 0.0;
		//Height of perilune of LOI ellipse after LOI
		double h_P = 0.0;
		//True anomaly on LOI1 ellipse after LOI
		double W_P = 0.0;
		//LOI-2/DOI maneuver DV
		double dv_LOI2 = 0.0;
	};

	struct LOIOutputDataSet
	{
		LOIDisplayData display;
		//For MPT
		VECTOR3 R_LOI = _V(0, 0, 0);
		VECTOR3 V_LOI = _V(0, 0, 0);
		double GMT_LOI = 0.0;
		VECTOR3 V_LOI_apo = _V(0, 0, 0);
		double dv_LOI = 0.0;
		double dgamma_LOI = 0.0;
		double dpsi_LOI = 0.0;
		//Just for temporary storage really
		//Unit angular momentum vector to the plane of the solution
		VECTOR3 USSAV;
	};

	struct LOIOutputData
	{
		LOIOutputDataSet data[8];
		double T_LLS;
		double eta_MX;
		double eta_MN;
	};

	class LOITargeting
	{
	public:
		LOITargeting(LOIOptions o);
		bool MAIN();

		LOIOutputData out;
	protected:
		double TIME(double a_LLS, double e_LLS, double lng_LLS, double lat_LLS, double &DA, double &eta2, double &dt3);
		void BACKUP(VECTOR3 R_LLS, VECTOR3 V_LLS, double a_LLS, double T_LLS, double DA, double eta2, double dt3, double &dh_a, double &dh_p, double &dw_p, double &a_L, double &e_L, VECTOR3 &U_PL, VECTOR3 &R_LOI, VECTOR3 &V_LOI);
		//To compute a conic DT to go from an input true anomaly in an ellipse through a delta true anomaly
		double DELTAT(double a, double e, double eta, double deta);
		//To compute +/- coplanar solns, pre-pcyn
		void COPLNR(VECTOR3 U_DS);
		//To compute a maneuver DV or the cosine of the allowable plane change within a given DV
		double DVDA(VECTOR3 R_N, VECTOR3 V_N, double soln, bool dv, double cos_dpsi_N, double DV);
		//To compute DV, dgamma, dpsi of the solution and certain display quantities and the conic state after LOI
		void CANS(VECTOR3 R_N, VECTOR3 V_N, double T_N, VECTOR3 U_S, double R_P, double theta, double SGN, int soln);
		//To compute + and - minimum theta solns
		void MINTHT(VECTOR3 U_DS);
		//To compute + or - intersection solns
		void INTER(VECTOR3 R_N, VECTOR3 V_N, double T_N, VECTOR3 U_L, VECTOR3 U_S, bool soln);
		//To compute the dh between R_p of the first LPO at DOI time and the R on the second LPO
		double DELTAH(double R_p, double r_N, double dw_a, VECTOR3 U_L, VECTOR3 R_N_u, VECTOR3 U_S, double SGN);
		//To compute the DV for the maneuver after LOI
		double DELV2(VECTOR3 R_N, VECTOR3 U_S, double R_a, double R_p, double W_P);

		LOIOptions opt;
		OBJHANDLE hMoon;
		//Unit position vector to hyperbolic perilune
		VECTOR3 U_PC;
		//Unit angular momentum vector to SPH
		VECTOR3 U_H;
		//Apolune and perilune radii of the first LPO
		double RA_LPO, RP_LPO;
		//Hyperbolic pericynthion radius
		double R_PHYP;
		//a, e of the hyperbola at perilune
		double a_H, e_H;
		//unit(crossp(U_DS, U_XTRA))
		VECTOR3 U_N;
		//a, e of second LPO at DOI
		double ALSAV, ELSAV;
		//Unit inertial vector to LPO-2 perilune at LOI-2 time
		VECTOR3 UPLSAV;
		//Error in the estimated time at the LLS
		double DT_CORR;
		//Estimate of the change of apolune altitude, perilune altitude, and perilune position due to propagation in LPO-1
		double DHASAV, DHPSAV, DWPSAV;
	};

}