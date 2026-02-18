/***************************************************************************
  This file is part of Project Apollo - NASSP

  MCC Calculations (Header)

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

#include "../src_rtccmfd/RTCCModule.h"
#include "../src_rtccmfd/RTCCTables.h"
#include "../src_rtccmfd/OrbMech.h"

// A class with utility calculations for the MCC class, with access to the RTCC class
// Anything that does not belong in the RTCC class, because it does not correspond to real RTCC code
// The main purpose of this class is to reduce the complexity of the code required for the mission specific MCC calculations

class MCC_Calculations : public RTCCModule
{
public:
	MCC_Calculations(RTCC *r);

	//Ephemeris without drag
	bool CreateEphemeris(EphemerisData sv, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2 &ephem);
	//Ephemeris with VehicleDataBlock as input
	bool CreateEphemeris(VehicleDataBlock sv, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2& ephem);
	//Ephemeris with state vector and weights table as input
	bool CreateEphemeris(EphemerisData sv, PLAWDTOutput weights, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2& ephem);
	double EnvironmentChange(EphemerisDataTable2 &ephem, double gmt_estimate, int option, bool present, bool terminator);
	double Sunrise(EphemerisDataTable2 &ephem, double gmt_estimate);
	double TerminatorRise(EphemerisDataTable2 &ephem, double gmt_estimate);
	bool LongitudeCrossing(EphemerisDataTable2 &ephem, double lng, double gmt_estimate, double &gmt_cross);
	//Find orbital sunrise with initial guess
	double FindOrbitalSunrise(VehicleDataBlock sv, double t_sunrise_guess);
	//Find orbital sunrise with initial guess (for backwards compatibility)
	double FindOrbitalSunrise(SV sv, double t_sunrise_guess);
	//Find orbital sunset with initial guess
	double FindOrbitalSunset(VehicleDataBlock sv, double t_sunset_guess);
	//Find orbital sunset with initial guess (for backwards compatibility)
	double FindOrbitalSunset(SV sv, double t_sunset_guess);
	double FindOrbitalMidnight(SV sv, double t_TPI_guess);
	//Given an ephemeris, find AOS, maximum elevation and LOS
	int StationContactsGenerator(EphemerisDataTable2& ephem, double lat, double lng, double alt, int RBI, StationContact &contact);
	//General ground target pointing utility function
	int GroundTargetPointing(EphemerisDataTable2& ephem, MATRIX3 REFSMMAT, double gmt, double lat, double lng, double alt, int RBI, double yaw, double pitch, double omicron, VECTOR3 &Att);
	//General celestial target pointing utility function
	int CelestialTargetPointing(EphemerisDataTable2& ephem, MATRIX3 REFSMMAT, double gmt, int star, double yaw, double pitch, double omicron, VECTOR3& Att);
	//Utility function for pointing at the center of Earth, Sun or Moon
	int CelestialBodyPointing(EphemerisDataTable2& ephem, MATRIX3 REFSMMAT, double gmt, int option, double yaw, double pitch, double omicron, VECTOR3& Att);
	//Pointing direction to Earth, Moon or Sun
	int CelestialBodyPointingDirection(VECTOR3 R, double GMT, int CSI, int option, VECTOR3 &u_LOS);
	//Attitude from pointing direction
	VECTOR3 AttitudeFromPointingDirection(VECTOR3 R, VECTOR3 V, MATRIX3 REFSMMAT, VECTOR3 u_LOS, double yaw, double pitch, double omicron);
	void FindRadarAOSLOS(SV sv, double lat, double lng, double &GET_AOS, double &GET_LOS);
	double ComputeDVTO(double mass); //Computes SPS Tail-off
	int SPSRCSDecision(double a, VECTOR3 dV_LVLH);	//0 = SPS, 1 = RCS
	bool REFSMMATDecision(VECTOR3 Att); //true = everything ok, false = Preferred REFSMMAT necessary

	//ALIGNMENTS
	//Calculates backup GDC alignment angles and star set
	void BackupGDCAlignment(VehicleDataBlock sv, double GET, MATRIX3 REFSMMAT, int PrefGDCStars, VECTOR3 &GDCangles, char *SetStars);

	void PrelaunchMissionInitialization();
	//Returns the time in GET that the LVDC saved as the orbital insertion time
	double GetLVDCOrbitalInsertionTime(VESSEL *v);

	//Mission specific rendezvous plans
	void DMissionRendezvousPlan(SV sv_A0, double &t_TPI0);
	void FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double t_TIG, double t_TPI, double &t_Ins, double &CSI);

	//Returns true if the current Ground Elapsed Time is greater than the input value
	bool GETEval(double get);

	//Stores a state vector for later user
	void StoreStateVector(VehicleDataBlock sv);
	void StoreStateVector(SV sv);
	void StoreStateVector(EphemerisData sv, double Weight);
	//Restores the state vector that was stored earlier
	void RestoreStateVector(VehicleDataBlock &sv);
	void RestoreStateVector(SV &sv);
};