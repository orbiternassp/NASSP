/****************************************************************************
This file is part of Project Apollo - NASSP

Apollo Generalized Optics Program (Header)

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

#include "OrbMech.h"
#include "RTCCTables.h"
#include "RTCCModule.h"

struct AGOPInputs
{
	//GENERAL INPUTS
	//	1 = Cislunar navigation, 2 = Reference body computation, 3 = Star catalog, 4 = Antenna pointing, 5 = PTC Attitude, 6 = Terminator/horizon angles, 7 = OST, 8 = SST
	int Option;
	//	Option 1: 1 = Star/Earth horizon, 2 = Star/Moon horizon, 3 = Star/Earth landmark, 4 = Star/Moon landmark
	//	Option 2: 1 = Calculate RA and declination of SC wrt the Earth, calculate RA and declination of Earth, Moon, Sun wrt the SC,
	//			  2 = Compute RA, declination, unit vector from spaceraft to landmark or center of Earth, Moon, Sun
	// Option 4: 1 = S-Band HGA (movable), 2 = S-Band Steerable (movable), 3 = RR (movable), 4 = HGA (fixed), 5 = Steerable (fixed), 6 = RR (fixed)
	// Option 6: 1 = Fwd horizon, 2 = Aft horizon
	int Mode;
	//Option 7, Mode 2: 0 = star search, 1 = stars input
	int AdditionalOption = 0;

	//Pointer to star catalog
	VECTOR3 *startable;

	//Array of state vectors
	std::vector<EphemerisData> sv_arr;
	//CSM REFSMMAT (option 1, 4, 5 and 6)
	MATRIX3 CSM_REFSMMAT;
	//LM REFSMMAT (option 4)
	MATRIX3 LM_REFSMMAT;
	//For option 4, 7. Second set used for REFSMMAT calculation (option 7, mode 3) and docked alignment (option 7, mode 4)
	VECTOR3 IMUAttitude[2];
	//Vehicle to which the IMU attitude belongs. True = CSM, false = LM
	bool AttIsCSM;
	//To convert between CSM and LM attitudes
	double DockingAngle = 0.0;
	//Number of stars. Card G2000
	unsigned int NumStars = 1;
	//Desired stars (option 1, 3 and 7). Card G2001-2010
	//1 star only for options 1 and 3
	unsigned int StarIDs[10] = { 1,1,1,1,1,1,1,1,1,1 };
	//If manual star input
	double StarRightAscension = 0.0;
	double StarDeclination = 0.0;
	//Landmark coordinates (option 1 and 4)
	double lmk_lat, lmk_lng, lmk_alt;
	//ID of ground station. If no ID is input, the manual coordinates above are used (option 4)
	std::string GroundStationID;
	//Heads up/down for fixed antenna angles (option 4)
	bool HeadsUp;
	//Fixed antenna angles (option 4, mode 4-6)
	double AntennaPitch, AntennaYaw;
	//Optical Instrument for option 7-9 (0 = sextant/telescope, 1 = LM COAS, 2 = AOT)
	int Instrument;

	//Star number to begin search
	unsigned int StartingStar = 1;

	//OPTICAL DATA

	//SEXTANT
	//Shaft angles for two stars (Cards G411-412)
	double SextantShaftAngles[2];
	//Trunnion angles for two stars (Cards G413-414)
	double TrunnionShaftAngles[2];

	//COAS
	//false = x-axis, true = z-axis
	bool LMCOASAxis;
	//EL (COAS elevation angle) for two stars
	double COASElevationAngle[2];
	//SXP (position on COAS reticle line) for two stars
	double COASPositionAngle[2];

	//AOT
	//1 = +Y-axis, 2 = +X-axis, 3 = -Y-axis, 4 = -X-axis
	int AOTLineID[2];
	//
	int AOTDetent;
	//A1 (angle to place star ion line ID) for two stars
	double AOTReticleAngle[2];
	//A2 (angle to place star on spirale) for two stars
	double AOTSpiraleAngle[2];
};

struct AGOPOutputs
{
	//Option 1 and 4
	VECTOR3 IMUAttitude;
	//For option 1 (true = near horizon, false = far horizon)
	bool bIsNearHorizon;

	//Option 4 - antenna pointing
	double pitch, yaw;

	//Option 5
	MATRIX3 REFSMMAT;

	//For display
	std::vector<std::string> output_text;
	std::string errormessage;
};

class AGOP : public RTCCModule
{
public:
	AGOP(RTCC *r);
	void Calc(const AGOPInputs &in, AGOPOutputs &out);
protected:
	//Option 1
	void CislunarNavigation(const AGOPInputs &in, AGOPOutputs &out);
	//Option 2
	void ReferenceBodyComputation(const AGOPInputs &in, AGOPOutputs &out);
	//Option 3
	void StarCatalog(const AGOPInputs &in, AGOPOutputs &out);
	//Option 4
	void AntennaPointing(const AGOPInputs &in, AGOPOutputs &out);
	//Option 5
	void PassiveThermalControl(const AGOPInputs &in, AGOPOutputs &out);
	//Option 6
	void HorizonAngles(const AGOPInputs &in, AGOPOutputs &out);
	//Option 7
	void OpticalSupportTable(const AGOPInputs &in, AGOPOutputs &out);
	//Option 7, Mode 1
	void LMHorizonCheck(const AGOPInputs &in, AGOPOutputs &out);
	//Option 7, Mode 2
	void OSTAlignmentManeuverCheck(const AGOPInputs &in, AGOPOutputs &out);
	//Option 7, Mode 3
	void OSTComputeREFSMMAT(const AGOPInputs &in, AGOPOutputs &out);
	//Option 7, Mode 4
	void DockingAlignment(const AGOPInputs &in, AGOPOutputs &out);
	//Option 7, Mode 5
	void PointAOTWithCSM(const AGOPInputs &in, AGOPOutputs &out);
	//Option 8
	void StarSightingTable(const AGOPInputs &in, AGOPOutputs &out);

	void WriteError(AGOPOutputs &out, int err);
	void RightAscension_Display(char *Buff, double angle);
	void Declination_Display(char *Buff, double angle);
	bool GetInertialLandmarkVector(double lat, double lng, double alt, double GMT, bool isEarth, VECTOR3 &R_LMK);
	void CSMHGAngles(VECTOR3 R, MATRIX3 SMNB, MATRIX3 REFSMMAT, double &pitch, double &yaw);
	void LMSteerableAngles(VECTOR3 R, MATRIX3 SMNB, MATRIX3 REFSMMAT, double &pitch, double &yaw);
	void RRAngles(VECTOR3 R, MATRIX3 SMNB, MATRIX3 REFSMMAT, double &trunnion, double &shaft);
	VECTOR3 CSMIMUtoLMIMUAngles(MATRIX3 CSM_REFSMMAT, MATRIX3 LM_REFSMMAT, VECTOR3 CSMIMUAngles, double DockingAngle);
	VECTOR3 LMIMUtoCMIMUAngles(MATRIX3 CSM_REFSMMAT, MATRIX3 LM_REFSMMAT, VECTOR3 LMIMUAngles, double DockingAngle);
	MATRIX3 LVLHAttitude(VECTOR3 LVLHAtt, VECTOR3 R, VECTOR3 V);
	MATRIX3 ThreeAxisPointing(VECTOR3 SCAXIS, VECTOR3 U_LOS, VECTOR3 R, VECTOR3 V, double OMICRON);
	VECTOR3 GetBodyFixedHGAVector(double pitch, double yaw) const;
	VECTOR3 GetBodyFixedSteerableAntennaVector(double pitch, double yaw) const;
	VECTOR3 GetBodyFixedRRVector(double trunnion, double shaft) const;
	VECTOR3 GetStarUnitVector(const AGOPInputs &in, unsigned num);
	VECTOR3 GetCSMCOASVector(double SPA, double SXP);
	VECTOR3 GetLMCOASVector(double EL, double SXP, bool IsZAxis);
	VECTOR3 GetAOTNBVector(double EL, double AZ, double YROT, double SROT, int axis);
};