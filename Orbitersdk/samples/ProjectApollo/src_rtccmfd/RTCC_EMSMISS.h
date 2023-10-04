/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2021

  RTCC Numerical Integration Control Element - EMSMISS (Header)

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
#include "RTCCModule.h"
#include "RTCCTables.h"
#include "EnckeIntegrator.h"

struct MissionPlanTable;

struct EMSMISSState
{
	EphemerisData StateVector;
	PLAWDTOutput WeightsTable;
	bool isLanded;
};

class RTCC_EMSMISS : public RTCCModule
{
public:
	RTCC_EMSMISS(RTCC *r);

	void Call(EMSMISSInputTable *in);
protected:

	void CutoffModeLogic();
	void EphemerisModeLogic();
	void CallCoastIntegrator();
	void CallManeuverIntegrator();
	void CallCSMLMIntegrator();
	void CallSIVBIntegrator();
	void CallDescentIntegrator();
	void CallAscentIntegrator();
	void LunarStayPhase();

	bool NextManeuverTime(double &gmt);
	double NextEventTime(double gmt);

	void UpdateWeightsTableAndSVAfterCoast();
	void UpdateWeightsTableAndSVAfterManeuver();
	void WriteNIAuxOutputTable();
	void WeightsAtManeuverBegin();
	void WriteEphemerisHeaders();

	bool InitEphemTables();
	void AddCoastOrSurfaceEphemeris();
	void AddManeuverEphemeris();

	MissionPlanTable *mpt;
	int nierror;

	//Temporary variables and tables
	EphemerisData svtemp;
	EphemerisDataTable2 tempephemtable, tempcoastephemtable[4];
	RTCCNIAuxOutputTable AuxTableIndicator;
	ManeuverTimesTable ManTimesTable;

	//Inputs
	bool EphemerisIndicatorList[4];
	EphemerisDataTable2 * EphemerisTableIndicatorList[4];
	double DensityMultiplier;

	//Tables
	EMMENIInputTable emmeniin;
	PLAWDTOutput CurrentWeightsTable, InitialWeightsTable;

	//Current maneuver
	unsigned int i;	
	int TerminationCode;
	bool manflag;
	//Ephemeris building switched on
	bool EphemerisBuildOn;

	//Absolute limit for GMT
	double gmt_lim;
	//Maximum time for coast integrator
	double gmt_coast;
	//Minimum interval between two state vectors being stored on the ephemeris
	double min_ephem_dt;

	//Start and end time for venting (TLI cutoff plus X)
	double T_NV[2];

	//Last processed maneuver
	unsigned int LastManeuver;

	EMSMISSState state;
	EphemerisData sv0;
	int ErrorCode;
	double LunarStayBeginGMT, LunarStayEndGMT;

	EMSMISSInputTable *intab;
};