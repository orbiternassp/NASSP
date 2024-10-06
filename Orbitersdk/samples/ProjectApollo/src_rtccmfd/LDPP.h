/****************************************************************************
  This file is part of Shuttle FDO MFD for Orbiter Space Flight Simulator
  Copyright (C) 2019 Niklas Beug

  Lunar Descent Planning Processor (Header)

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
#include "RTCCModule.h"
#include "RTCCTables.h"

struct LDPPOptions
{
	LDPPOptions();
	//Maneuver routine flag
	//1 = CSM phase change
	int MODE;
	//Maneuver sequence flag
	//Mode 1: -1 = plane-change only, 0 = plane change and circularization, 1 = plane change combined with first maneuver of a CSM two-maneuver sequence to circularize the CSM orbit at an input altitude
	//Mode 2: 0 = compute CSM maneuver to establish an apsis and an input altitude at the DOI maneuver point, 1 = compute CSM maneuver to circularize orbit at an input altitude
	//Mode 3: -1 = compute CSM two-maneuver sequence with the first maneuver performed at an input time and the second maneuver performed at an input altitude to circularize the orbit
	//		   1 = compute CSM two-maneuver sequence with the first maneuver performed at an apsis and the second maneuver performed at an input altitude to circularize the orbit
	//Mode 4: -1 = DOI alone, 0 = DOI with plane change, 1 = Integrated DOI, 2 = Integrated DOI with plane change
	//Mode 5: -1 = compute CSM three-maneuver sequence so that the first maneuver is a plane change and the following pair is a double Hohmann to a circular orbit at an input altitude
	//		  0 = compute CSM three-maneuver sequence so that the first maneuver initiates a double Hohmann, the second is a plane change, and the third completes the double Hohmann to a circular orbit at an input altitude.
	//		  1 = compute CSM three-maneuver sequence so that the first two maneuvers constitute a double Hohmann to a circular orbit at an input altitude and the third is a plane change.
	//Mode 6: Powered descent only
	//Mode 7: CSM prelaunch plane change
	int IDO;
	//Powered-descent simulation flag (false = simulate powered descent, true = do not simulate powered descent)
	bool I_PD;
	//Descent azimuth flag: false = descent azimuth is not specified, true = descent azimuth is specified
	bool I_AZ;
	//Powered-descent time flag: false = let powered descent compute time to ignite, true = input time for powered-descent ignition
	bool I_TPD;
	//Time for powered descent ignition (GMT)
	double T_PD;
	//Table of threshold times (GMT)
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
	//Angle from PDI to landing site
	double theta_PDI;
	//Descent azimuth desired
	double azi_nom;
	//Altitude wanted at apsis
	double H_W;
	//Average specific impulse of LM descent engine
	double I_SP;
	//Initial weight of LM
	double W_LM;
	//CSM state vector and time
	EphemerisData sv0;
};

struct LDPPResults
{
	LDPPResults();
	//Delta V vector in LVLH coordinates
	VECTOR3 DeltaV_LVLH[4];
	//Time of each maneuver
	double T_M[4];
	//Number of maneuvers in plan
	int I_Num;
	//Time of PDI threshold (if calculated)
	double t_PDI_TH;
	//Time of PDI (if calculated)
	double t_PDI;
	//Time of touchdown (if calculated)
	double t_Land;
	//Azimuth at landing site
	double azi;
	EphemerisData sv_before[4];
	VECTOR3 V_after[4];
	//Error codes (bitset):
	//Bit 0: Unrecoverable AEG error (includes STAP errors). Processing halted.
	//Bit 1: Error in backing through maneuver(s) in PCPLCH (aka CHAPLA). Processing halted.
	//Bit 2: Error in trying to obtain LM weight from PLAWDT (or LM weight is zero). Processing continued.
	//Bit 3: Failed to converge in ASH maneuver. Processing continued.
	//Bit 4: Failed to converge on closest approach in PCPLCH (aka CHAPLA). Processing continued.
	//Bit 5: Failed to converge on landing site. Processing continued.
	//Bit 6: Failed to converge in DOI maneuver. Processing continued.
	//Bit 7: Failed to converge on a height maneuver in PCSAC (SAC). Processing continued.
	//Bit 8: Failed to converge on a common node. Processing continued.
	//Bit 9: Delta V for plane change maneuver less than 1 ft/s. No maneuver computed, processing continued.
	//Bit 10: Error from PMMAPD. Processing halted.
	//Bit 11: Failed to converge on time of landing site longitude crossing PCTLLC (aka LLTPR) - Processing continued.
	int Error;
};

class LDPP : public RTCCModule
{
public:
	LDPP(RTCC *r);
	void LDPPMain(const LDPPOptions &in, LDPPResults &out);
protected:
	//CSM phase change
	void Mode1();
	void Mode1_1();
	void Mode1_2();
	//Single CSM maneuver sequence
	void Mode2();
	void Mode2_1();
	void Mode2_2();
	//Double CSM maneuver sequence
	void Mode3();
	//DOI
	void Mode4();
	//Double Hohmann plane change DOI maneuver sequence
	void Mode5();
	//PDI
	void Mode6();
	//CSM prelaunch plane change maneuver
	void Mode7();

	//Compute a maneuver to shift the line-of-apsides and change apocynthion and pericynthion or circularize the CSM orbit
	VECTOR3 SAC(double h_W, bool J, EphemerisData sv_L, int Integrated = false);
	//Compute a maneuver to place CSM orbital track over a desired landing site with or without a specified azimuth
	void CHAPLA(EphemerisData sv_L, bool IWA, bool IGO, double TH, double &t_m, VECTOR3 &DV);
	void CHAPLA_FixedTIG(EphemerisData sv_TIG, EphemerisData sv_L, double TH, double &deltaw_s, VECTOR3 &DV) const;
	//Compute the time of the DOI maneuver based on a desired landing site and a CSM vector before the maneuver
	void LLTPR(double T_H, EphemerisData sv_L, EphemerisData &sv_DOI, VECTOR3 &DV_LVLH, double &t_IGN, double &t_TD, bool Integrated = false);
	//Calculate argument of latitude
	double ArgLat(VECTOR3 R, VECTOR3 V) const;
	//Subroutine that searches for a common node between two orbits
	void CNODE(EphemerisData sv_A, EphemerisData sv_P, double &t_m, VECTOR3 &dV_LVLH) const;
	//Subroutine that iterates to find an upcoming apsis point
	EphemerisData STAP(EphemerisData sv0);
	//Subroutine that iterates to find a specified radius in a given orbit
	bool STCIR(EphemerisData sv0, double h_W, bool ca_flag, EphemerisData &sv_out);
	//Advance state to argument of latitude
	EphemerisData TIMA(EphemerisData sv0, double U0, double UD, bool &error);
	//Add a LVLH Delta V vector to state
	EphemerisData APPLY(EphemerisData sv0, VECTOR3 dV_LVLH);
	//Inertial (MCI) landing site vector at time GMT
	VECTOR3 LATLON(double GMT) const;
	//Utility functions
	EphemerisData coast_u(EphemerisData sv0, double dt, double U0, double &U1) const;
	EphemerisData coast(EphemerisData sv1, double dt, bool Integrated = false) const;
	EphemerisData PMMLAEG(EphemerisData sv0, int opt, double param, bool &error, double DN = 0.0) const;
	EphemerisData PositionMatch(EphemerisData sv_A, EphemerisData sv_P, double mu) const;
	double P29TimeOfLongitude(VECTOR3 R0, VECTOR3 V0, double GMT, double phi_d) const;
	EphemerisData SaveElements(EphemerisData sv, int n, VECTOR3 DV);
	EphemerisData LoadElements(int n, bool before) const;
	double OutOfPlaneError(EphemerisData sv) const;
	double OrbitalPeriod(EphemerisData sv) const;
	//Converges on closest approach to the landing site after TH
	bool LSClosestApproach(EphemerisData sv, double TH, EphemerisData &sv_CA) const;
	//Calculate DOI maneuver
	int DOIManeuver(int i_DOI, bool Integrated = false);
	//PDI Calculations
	void PDICalculations();
	void OutputCalculations();
	void SetError(int bit);
	bool IsErrorUnrecoverable() const;

	double mu;
	OBJHANDLE hMoon;
	//Number of the plane-change maneuver
	int I_PC;
	//Number of maneuvers in sequence
	int I_Num;
	//Closest approach to landing site in CHAPLA
	double GMT_LS_CA;

	//Time of DOI, PDI and touchdown
	double t_DOI, t_IGN, t_TD;
	//Stored state vectors
	EphemerisData sv_CSM, sv_V, sv_LM;
	//State vector index
	//0-3: maneuvers
	//0-1: before or after
	//0-1: position and velocity
	VECTOR3 LDPP_SV_E[4][2][2];
	//Time of maneuvers
	double t_M[4];
	//Delta V of maneuvers
	VECTOR3 DeltaV_LVLH[4];

	LDPPOptions opt;
	LDPPResults outp;

	//Angular iteration tolerance
	static const double zeta_theta;
	//time iteration tolerance
	static const double zeta_t;
};