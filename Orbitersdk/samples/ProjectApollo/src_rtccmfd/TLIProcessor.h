/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Translunar Injection Processor (Header)

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

struct TLIFirstGuessInputs
{
	double BHA; //Hour angle at midnight, radians
	double Hour; //Hour at which to start the search, hours

	double C1; //Declination of the input state vector, radians
	double FLO1; //Geographic longitude of the input orbit state vector, radians
	double AZ1; //Inertial azimuth of the input orbit state vector, radians
	double R1; //Radius of the input orbit state vector, nautical miles
	double ORBNUM; //Number of the inertial orbit revolution
	int IPOA; //1 = Pacific window (AZ2V <= 90°), 2 = Atlantic window (AZ2V > 90°)
	int IPERT; //0 = no perturbations, 1 = Earth oblateness, 2 = solar gravitation, 3 = both
	double XPC; //Longitude of pericynthion of the simulated trajectory, measured in a moon-centered MOP coordinate system
				//from the extension of the earth-moon axis on the back side of the moon, degrees
	double YPC; //Declination of pericynthion of the simulated trajectory, in a moon-centered MOP coordinate system
	double RPC; //Radius of pericynthion of the simulation trajectory, nautical miles
	int MAXIT = 50;
	double TOL = 0.001;
	double TTW = 0.7; //Valid for 0.63 to 0.8
	bool debug = true;
};

struct TLIFirstGuessOutputs
{
	double T1; //Time of launch from base time
	double CTIO; //Time in Earth orbit
	double C3; //Energy of TLI
	double S; //Angle sigma
	double TOPCY; //Time of PC from base time
	double DEL; //Latitude of the updated unit M TLI targeting vector relative to the dispersed orbit plane
};

class TLIFirstGuess : public RTCCModule
{
public:
	TLIFirstGuess(RTCC *r);

	void Calculate(const TLIFirstGuessInputs& in);

	void GetOutput(TLIFirstGuessOutputs& out);
	int GetError() { return CO1; }
private:
	void CIST(double RAGBT);
	void UPDATE(double TOIDIS);

	void PERCYN(double T);
	double ENERGY(int IPERT, double C4) const;
	double FLYTYM(int IPERT) const;
	void GEOLAT(double FI, double C, double RA, int I, double& A, double& B, double& AZ, double& RN) const;
	void GEOARG(double FI, double A, double RN, double& AZ, double& B, double& C, double& RA) const;
	void SUBB(int IPERT, double &BETA, double &AFNTMH) const;
	double SUBCL(int IPERT) const;
	void TLIMP(double W);
	double HELP(double X) const;
	double ANGLE(VECTOR3 VEC1, VECTOR3 VEC2, VECTOR3 VEC3) const;

	TLIFirstGuessInputs inp;
	TLIFirstGuessOutputs outp;

	VECTOR3 MH; //Hypersurface
	double WV;
	double FIV;
	double RNV;
	double A1;

	double FIVTL;
	double R4;
	double APS;
	double FTOCO;

	VECTOR3 HM; //Angular momentum vector of the Moon
	double EMR;
	double EMRDOT;
	double SMOPL;
	double A6;
	double FIM;
	double RNM;
	double WM;
	double AM;
	double DS;
	double DEL; //Latitude of the updated unit M TLI targeting vector relative to the dispersed orbit plane

	//Error indicator
	int CO1;

	//Earth rotational rate
	const double WE;
};

struct TLIMEDQuantities
{
	//1 = CSM, 3 = LEM
	int mpt;

	//1 = S-IVB hypersurface solution, 2 = integrated free-return, 3 = hybrid ellipse, 4 = E-type mission ellipse, 5 = non-free return
	int Mode;
	EphemerisData state;
	PLAWDTOutput WeightsTable;
	double GMT_TIG; //TIG or estimated TIG
	int Opportunity; //1 or 2

	//Mode 2/5
	int IPOA; //1 = Pacific window (AZ2V <= 90°), 2 = Atlantic window (AZ2V > 90°), for first guess logic
	double h_PC; //Pericynthion flyby altitude, meters
	double lat_PC; //Latitude of pericynthion (mode 2) or of node (mode 5)
	double lng_node; //Longitude of node (mode 5)
	double GMT_node; //Time at node (mode 5)

	//Mode 3
	double dv_available; //DV available for TLI

	//Mode 4
	double h_ap;	//Desired apogee altitude at TLI cutoff
};

struct SevenParameterUpdate
{
	double GMT_TIG = 0.0;
	double C3 = 0.0;
	double Inclination = 0.0;
	double e = 0.0;
	double alpha_D = 0.0;
	double f = 0.0;
	double theta_N = 0.0;
};

struct TLIOutputData
{
	int ErrorIndicator = 0;
	bool IsSevenParameters = true;
	EphemerisData sv_TLI_ign, sv_TLI_cut;
	SevenParameterUpdate uplink_data;
	double dv_TLI;
};

class TLIProcessor : public TLTrajectoryComputers
{
public:
	TLIProcessor(RTCC *r);

	void Main(TLIOutputData &out);
	void Init(TLIMEDQuantities med, TLMCCMissionConstants constants, double GMTBase);
protected:
	//Hypersurface
	void Option1();
	//Free return or non-free return
	void Option2_5(bool freereturn);
	//Hybrid ellipse
	void Option3();
	//Desired apogee
	void Option4();

	bool HybridMission(double C3_guess, double dv_TLI);
	bool ConicTLIIEllipse(double C3_guess, double h_ap);
	bool IntegratedTLIIEllipse(double C3_guess_ER, double h_ap);
	bool IntegratedTLIToNode(double C3_guess_ER, double T_c_hrs, double delta, double sigma, double GMT_nd, double lat_nd, double lng_nd);
	bool IntegratedTLIFlyby(double C3_guess_ER, double T_c_hrs, double delta, double sigma, double lat_pc);

	OELEMENTS LVTAR(OELEMENTS coe, double lng_PAD, double RAGL) const;

	TLIMEDQuantities MEDQuantities;
	int ErrorIndicator;
};