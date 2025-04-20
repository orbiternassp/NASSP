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

	bool CreateEphemeris(EphemerisData sv, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2 &ephem);
	double EnvironmentChange(EphemerisDataTable2 &ephem, double gmt_estimate, int option, bool present, bool terminator);
	double Sunrise(EphemerisDataTable2 &ephem, double gmt_estimate);
	double TerminatorRise(EphemerisDataTable2 &ephem, double gmt_estimate);
	bool LongitudeCrossing(EphemerisDataTable2 &ephem, double lng, double gmt_estimate, double &gmt_cross);
	double FindOrbitalSunrise(SV sv, double t_sunrise_guess);
	double FindOrbitalMidnight(SV sv, double t_TPI_guess);
	void FindRadarAOSLOS(SV sv, double lat, double lng, double &GET_AOS, double &GET_LOS);
	int SPSRCSDecision(double a, VECTOR3 dV_LVLH);	//0 = SPS, 1 = RCS
	bool REFSMMATDecision(VECTOR3 Att); //true = everything ok, false = Preferred REFSMMAT necessary
	void PrelaunchMissionInitialization();

	//Mission specific rendezvous plans
	void DMissionRendezvousPlan(SV sv_A0, double &t_TPI0);
	void FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double t_TIG, double t_TPI, double &t_Ins, double &CSI);

	//Returns true if the current Ground Elapsed Time is greater than the input value
	bool GETEval(double get);
};