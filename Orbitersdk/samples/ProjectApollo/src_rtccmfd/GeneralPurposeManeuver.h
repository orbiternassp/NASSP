/****************************************************************************
This file is part of Project Apollo - NASSP

General Purpose Maneuver Processor, RTCC Module PMMGPM (Header)

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
#include "AnalyticEphemerisGenerator.h"
#include "RTCCModule.h"
#include "RTCCTables.h"

#define RTCC_GMP_PCE 1
#define RTCC_GMP_PCL 2
#define RTCC_GMP_PCT 3
#define RTCC_GMP_CRL 4
//Circularization at a specified height
#define RTCC_GMP_CRH 5
#define RTCC_GMP_HOL 6
#define RTCC_GMP_HOT 7
#define RTCC_GMP_HAO 8
#define RTCC_GMP_HPO 9
#define RTCC_GMP_NST 10
#define RTCC_GMP_NSO 11
//Maneuver to change both apogee and perigee at a specified time
#define RTCC_GMP_HBT 12
#define RTCC_GMP_HBH 13
#define RTCC_GMP_HBO 14
#define RTCC_GMP_FCT 15
#define RTCC_GMP_FCL 16
#define RTCC_GMP_FCH 17
#define RTCC_GMP_FCA 18
#define RTCC_GMP_FCP 19
#define RTCC_GMP_FCE 20
#define RTCC_GMP_NHT 21
//Combination maneuver to change both apogee and perigee and shift the node at a specified longitude
#define RTCC_GMP_NHL 22
//Maneuver to shift line-of-apsides some angle at a specified longitude
#define RTCC_GMP_SAL 23
//Maneuver to shift line-of-apsides to a specified longitude
#define RTCC_GMP_SAA 24
#define RTCC_GMP_PHL 25
#define RTCC_GMP_PHT 26
#define RTCC_GMP_PHA 27
#define RTCC_GMP_PHP 28
#define RTCC_GMP_CPL 29
//Combination circularization maneuver and a plane change at a specified altitude
#define RTCC_GMP_CPH 30
#define RTCC_GMP_SAT 31
//Maneuver to shift line-of-apsides some angle and keep the same apogee and perigee altitudes
#define RTCC_GMP_SAO 32
//Maneuver to change both apogee and perigee at a specified longitude
#define RTCC_GMP_HBL 33
#define RTCC_GMP_CNL 34
#define RTCC_GMP_CNH 35
//Height maneuver and node shift at a specified longitude
#define RTCC_GMP_HNL 36
#define RTCC_GMP_HNT 37
#define RTCC_GMP_HNA 38
#define RTCC_GMP_HNP 39
#define RTCC_GMP_CRT 40
#define RTCC_GMP_CRA 41
#define RTCC_GMP_CRP 42
#define RTCC_GMP_CPT 43
#define RTCC_GMP_CPA 44
#define RTCC_GMP_CPP 45
#define RTCC_GMP_CNT 46
#define RTCC_GMP_CNA 47
#define RTCC_GMP_CNP 48
#define RTCC_GMP_PCH 49
#define RTCC_GMP_NSH 50
//Node shift at a longitude
#define RTCC_GMP_NSL 51
#define RTCC_GMP_HOH 52
//Maneuver to change both apogee and perigee and place perigee at a certain longitude a certain number of revs later
#define RTCC_GMP_HAS 53

struct GMPOpt
{
	int ManeuverCode;
	EphemerisData sv_in;		//State vector as input or without
	double Area = 0.0;
	double Weight = 1.0;
	double KFactor = 1.0;

	//maneuver parameters

	double TIG_GET;	//Threshold time or time of ignition
	double dW;		//Desired wedge angle change
	double long_D;	//Desired maneuver longitude
	double H_D;		//Desired maneuver height
	double dH_D;	//Desired change in height
	double dLAN;	//Desired change in the ascending node
	double H_A;		//Desired apogee height after the maneuver
	double H_P;		//Desired perigee height after the maneuver
	double dV;		//Input incremental velocity magnitude of the maneuver
	double Pitch;	//Input pitch of the maneuver
	double Yaw;		//Input yaw of the maneuver
	double dLOA;	//Line-of-apsides shift
	int N;			//Number of revolutions
};

class RTCCGeneralPurposeManeuverProcessor : public RTCCModule
{
public:
	RTCCGeneralPurposeManeuverProcessor(RTCC *r);
	int PCMGPM(const GMPOpt &IOPT);
protected:

	//Math program
	void PCGPMP();
	void DetermineManeuverPoint();
	void DetermineManeuverType();
	void GetSelenographicElements(const AEGDataBlock &sv, double &u, double &i, double &g, double &h);
	void GetSelenocentricElements(double i, double g, double h, AEGDataBlock &sv);
	void PlaneChange();
	void FlightControllerInput();
	void HeightManeuver(bool circ);
	int OptimumApsidesChange();
	void NodeShift();
	int ApsidesChange(bool limit = true);
	int OptimumPointForApsidesChange();
	void ApseLineShift(double dang);
	void ApsidesPlacementNRevsLater();
	void OptimumApseLineShift(double dang);

	void NormalizeAngle(double &ang); //0 to 2*pi
	void GetLatLongHeight(const AEGDataBlock &sv, double &alt, double &lat, double &lng);

	//SAVE INPUTS
	//Maneuver code
	int code;

	//INDICATORS
	//0 = Time, 1 = apogee, 2 = perigee, 3 = equatorial crossing, 4 = longitude, 5 = height, 6 = optimum nodal shift, 7 = optimum apogee/perigee change
	//8 = Optimum apse line rotation, 9 = Apogee and perigee change + apse line rotation to specified longitude, 10 = roate apse line to longitude
	int ManeuverPoint;
	//0 = Flight controller input, 1 = plane change, 2 = circularization 3 = optimum height change at apogee/perigee, 4 height maneuver, 5 = node shift, 6 = maneuver to change apogee/perigee
	//7 = Combination node shift and apogee/perigee change, 8 = shift the line-of-apsides, 9 shift line-of-apsides to specific longitude, 10 = plane change and height maneuver
	//11 = Circularization and plane change, 12 = Circularization and node shift, 13 = Height change and node shift
	//14 = //Maneuver to change both apogee and perigee and place perigee at a certain longitude a certain number of revs later
	int ManeuverType;
	//1 = if maneuver is at apoapsis, -1 if at periapsis, 2 = indicator for apsides for output already having been calculated
	int K3;
	//Combination maneuver indicator
	int K5;

	//TEMPORARY VARIABLES
	//State vector at threshold time
	EphemerisData sv1;
	//AEG block at threshold time
	AEGBlock aeg;
	//AEG data block before maneuver
	AEGDataBlock sv_b_apo;
	//AEG data block before maneuver (can be overlayed for a combination maneuver)
	AEGDataBlock sv_b;
	//AEG data block after maneuver
	AEGDataBlock sv_a;
	//Temporary AEG block
	AEGDataBlock sv_temp;
	//AEG data blocks for apogee and perigee data
	AEGDataBlock sv_AP, sv_PE;
	//Apogee/perigee data returned by PMMAPD
	double INFO[10];
	//Apogee/perigee data returned by PMMAPD with option HAS, N revs later
	double INFO_HAS[10];
	//Radius of body
	double R_E;
	//Gravitational parameter of body
	double mu;
	//
	double DW;
	int I;
	double dLOA_temp;

	//OPTIONS
	const GMPOpt *opt;

	//OUTPUTS
	VECTOR3 DV_Vector;
	double Pitch_Man, Yaw_Man, Node_Ang, Del_G, DV, H_man, lat_man, lng_man;
	//0 = good maneuver, 1 = Maneuver cannot be performed in this orbit, 2 = Maneuver cannot be performed at this point in the orbit, 3 = AEG failed to converge, 4 = Unrecoverable AEG error
	//5 = Unable to obtain libration matrix, 6 = Unable to advance to selenographic argument of latitude, 7 = PMMAPD error for current apo/peri, 8 = PMMAPD error for resultant apo/peri
	int ErrorIndicator;

	//CONSTANTS

	//Height maneuver tolerance
	const double eps1 = 0.1*1852.0;
};