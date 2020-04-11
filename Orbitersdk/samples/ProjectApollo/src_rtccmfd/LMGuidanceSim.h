/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

LM Guidance Simulation for the RTCC (Header)

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

class AscentGuidance
{
public:
	AscentGuidance();
	void Init(VECTOR3 R_C, VECTOR3 V_C, double m0, double rls, double v_hor, double v_rad, bool aps = true);
	void Guidance(VECTOR3 R, VECTOR3 V, double M, double t_cur, VECTOR3 &U_FDP, double &ttgo, double &Thrust, double &isp);
	void SetThrustParams(bool aps);
	void SetTGO(double tgo);
protected:
	//Thrust magnitude of DPS and APS
	static const double F_DPS, F_APS;
	//Specific Impulse
	static const double Isp_APS, Isp_DPS;
	//Lunar gravitational constant
	static const double mu_M;
	//Time to go at which guidance parameters are maintained at last computed value
	static const double t_2;
	//Time to go at which spacecraft loses position control at injection
	static const double t_3;
	//Limit on B
	static const double PRLIMIT;

	//Unit vector perpendicular to the CSM plane
	VECTOR3 Q;
	//Estimated time to ascent injection
	double t_go;
	//Initial rise phase
	bool FLVP;
	//Cutoff command issued
	bool FLENG2;
	//Landing site radius
	double r_LS;
	//Current thrust setting
	double F;
	//Current specific impulse
	double Isp;
	//Cutoff time
	double t_cut;

	double m_dot;
	double a_T;
	double v_e;
	double tau;
	double r, h;
	VECTOR3 U_R, U_Y, U_Z;
	double R_dot, Y_dot, Z_dot;
	double Y;
	VECTOR3 V_G;
	double g_eff;

	double R_D, Y_D, R_D_dot, Y_D_dot, Z_D_dot;
	double L, D_12, D_21, A, B, C, D, E;
	double a_TR, a_TY, a_TP, a_H;
	VECTOR3 A_H, A_T;
};

struct LGCDescentConstants
{
	LGCDescentConstants();

	VECTOR3 RBRFG;
	VECTOR3 VBRFG;
	VECTOR3 ABRFG;
	double JBRFGZ;
	VECTOR3 RARFG;
	VECTOR3 VARFG;
	VECTOR3 AARFG;
	double JARFGZ;
};

class DescentGuidance
{
public:
	DescentGuidance();
	void Init(VECTOR3 R_C, VECTOR3 V_C, double m0, double t_I, MATRIX3 REFS, VECTOR3 R_LSP_init, double t_P, VECTOR3 W, double ttgo, LGCDescentConstants *consts);
	void Guidance(VECTOR3 R, VECTOR3 V, double M, double t_cur, VECTOR3 &U_FDI, double &ttgo, double &Thrust, double &isp);
protected:
	//Descent phase
	// -2: preignition
	// -1: ullage and trim
	//  0: braking
	//  1: approach
	//  2: vertical descent
	int PHASE;
	//time of ignition
	double t_IG;
	//Ullage time
	static const double UT;
	//Time at 10% thrust
	static const double TRMT;
	//Ullage thrust
	static const double THRUL;
	//Thrust during trim maneuver
	static const double THRTRM;
	//Max thrust setting
	static const double THRMAX;
	static const double THRMIN;
	//Ullage specific impulse
	static const double ULISP;
	//DPS specific impulse
	static const double XKISP;
	//Lunar gravitational constant
	static const double mu_M;
	//Maximum force in throttable region (P66)
	static const double MAXFORCE;
	//Minimum force in throttable region (P66)
	static const double MINFORCE;
	//Lower end of throttlable upper limit
	static const double LOWCRIT;
	//Upper end of throttlable upper limit
	static const double HIGHCRIT;
	//Change in actual thrust with respect to time from braking phase initiation,
	//for the DPS due to an eroding engine nozzle
	static const double DELHTR;
	static const double DELISP;
	static const double XISP0;
	static const double XISP1;
	static const double XISP2;
	//time to go in current phase
	double t_go;
	VECTOR3 RDG, VDG, ADG, JDG;
	double eps;
	double DELTGO;
	MATRIX3 REFSMMAT;
	double t_pip, t_pipold;
	VECTOR3 R_LSP;
	VECTOR3 RP, VP, WP, GP;
	double r_LS;
	VECTOR3 ULP, UXGP, UYGP, UZGP, WXR, RG, VG, ACG, A_FDP;
	MATRIX3 C_GP;
	double FC;
	//Thrust of previous cycle
	double Thrust_old;
	LGCDescentConstants *desc_const;
};

class AscDescIntegrator
{
public:
	AscDescIntegrator();
	void Init(VECTOR3 U_TD_init);
	bool Integration(VECTOR3 &R, VECTOR3 &V, double &mnow, double &t_total, VECTOR3 U_TD, double t_remain, double Thrust, double Isp);
	VECTOR3 GetCurrentTD() { return U_TD_cur; }
private:
	VECTOR3 gravity(VECTOR3 R);
protected:
	//Lunar gravitational constant
	static const double mu_M;
	//Timestep
	double dt, dt_max, max_rate;
	//Accumulated DV over the timestep
	VECTOR3 DVDT;
	//Gravity at begin and end of timestep
	VECTOR3 G_P, G_PDT;
	//Current thrust direction
	VECTOR3 U_TD_cur;
};