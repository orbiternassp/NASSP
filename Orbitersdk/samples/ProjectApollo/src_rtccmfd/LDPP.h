/****************************************************************************
  This file is part of Shuttle FDO MFD for Orbiter Space Flight Simulator
  Copyright (C) 2019 Niklas Beug

  Launch Descent Planning Processor (Header)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
  **************************************************************************/

#pragma once

#include "OrbMech.h"

struct LDPPOptions
{
	LDPPOptions();
	//Maneuver routine flag
	int MODE;
	//Maneuver sequence flag
	int IDO;
	//Powered-descent simulation flag
	int I_PD;
	//Descent azimuth flag: 0 = descent azimuth is not specified, 1 = descent azimuth is specified
	int I_AZ;
	//Powered-descent time flag
	int I_TPD;
	//Time for powered descent ignition
	double T_PD;
	//Table of threshold times
	double TH[4];
	//Number of dwell orbits desired between DOI and powered-descent ignition
	int M;
	//Radius of the desired landing site
	double R_LS;
	//Latitude of the desired landing site
	double Lat_LS;
	//Longitude of the desired landing site
	double Lng_LS;
	//Altitude of point of descent ignition
	double H_DP;
	//Powered-flight arc of descent
	double theta_D;
	//Powered-flight time of descent
	double t_D;
	//Descent azimuth desired
	double azi_nom;
	//Altitude wanted at apsis
	double H_W;
	//Average specific impulse of LM descent engine
	double I_SP;
	//Initial weight of LM
	double W_LM;
	//CSM state vector and time
	SV sv0;
	//Liftoff time
	double GETbase;
};

struct LDPPResults
{
	LDPPResults();
	//Number of maneuver computed
	int i;
	//Delta V costs of each maneuver
	double DV[4];
	//Yaw angle of each maneuver
	double Y[4];
	//Pitch angle of each maneuver
	double P[4];
	//Time of each maneuver
	double T_M[4];
};

class LDPP
{
public:
	LDPP();
	void Init(const LDPPOptions &in);
	void LDPPMain(LDPPResults &out);
protected:
	
	VECTOR3 SAC(int L, double h_W, int J, SV sv_CSM);
	void CHAPLA(SV sv_CSM, int IWA, int IGO, int I, double &t_m, VECTOR3 &DV);
	void LLTPR(double T_H, SV sv_CSM, double &t_DOI, double &t_IGN);
	double ArgLat(VECTOR3 R, VECTOR3 V);
	void CNODE(SV sv_A, SV sv_P, double &t_m, VECTOR3 &dV_LVLH);
	//Subroutine that iterates to find an upcoming apsis point
	SV STAP(SV sv0);
	//Subroutine that iterates to find a specified radius in a given orbit
	SV STCIR(SV sv0, double h_W);
	SV TIMA(SV sv0, double u);
	SV APPLY(SV sv0, VECTOR3 dV_LVLH);
	VECTOR3 LATLON(double MJD);
	double mu;
	OBJHANDLE hMoon;
	//Number of the plane-change maneuver
	int I_PC;
	//Number of maneuver
	int i;
	int IRUT;

	double t_M[4];
	double DV_M[4];
	double Y_M[4];
	double P_M[4];

	LDPPOptions opt;

	//Angular iteration tolerance
	static const double zeta_theta;
	//time iteration tolerance
	static const double zeta_t;
};