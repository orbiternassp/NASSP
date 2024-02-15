/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  IMU header

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

// IMU

#if !defined(_PA_IMU_H)
#define _PA_IMU_H

#include "apolloguidance.h"
#include "inertial.h"

#include "powersource.h"

class IMU {

public:
	IMU(ApolloGuidance& comp, PanelSDK& p, InertialData& inertialData);
	~IMU();

	const double MERU = 7.29211586E-8; //rad/sec 

	void Init();
	void ChannelOutput(int address, ChannelValue value);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	void TurnOn();
	void TurnOff();
	void DriveGimbals(double x, double y, double z);
	void SetVessel(VESSEL* v, bool LEMFlag);
	void SetVesselFlag(bool LEMFlag);
	void SetDriftRates(const MATRIX3 DriftRateMatrix);
	void SetPIPABias(const VECTOR3 PIPABias);
	void SetPIPAScale(const VECTOR3 PIPAScale);
	VECTOR3 GetNBDriftRates();
	inline VECTOR3 getResolverPhaseError() { return ResolverPhaseError; };
	inline VECTOR3 getResolverSineGimbal() { return SineGimbal; };
	inline VECTOR3 getResolverCosineGimbal() { return CosineGimbal; };
	VECTOR3 getPlatformEulerAnglesZYX();
	virtual VECTOR3 GetTotalAttitude();

	void WireToBuses(e_object *a, e_object *b, GuardedToggleSwitch *s);
	void WireHeaterToBuses(Boiler *heater, e_object *a, e_object *b);
	void InitThermals(h_HeatLoad *imuht, h_Radiator *cas, h_HeatLoad *ptaht, h_HeatLoad *psaht, h_HeatLoad *cduht);

	bool IsCaged();
	bool IsPowered();
	void SetCaged(bool val);
	Boiler *GetHeater() { return IMUHeater; };
	double GetPIPATempF();

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);


	typedef struct {
		double NBD_X;
		double NBD_Y;
		double NBD_Z;
		double ADSRA_X;
		double ADSRA_Y;
		double ADSRA_Z;
		double ADIA_X;
		double ADIA_Y;
		double ADIA_Z;
	} IMU_DriftRates;

	typedef struct {
		double PIPA_BiasX;
		double PIPA_BiasY;
		double PIPA_BiasZ;
		double PIPA_ScalePPM_X;
		double PIPA_ScalePPM_Y;
		double PIPA_ScalePPM_Z;
	} PIPA_BiasScale;

protected:
	
	void DriveCDUX(int cducmd);
	void DriveCDUY(int cducmd);
	void DriveCDUZ(int cducmd);
	void DriveCDU(int index, int RegCDU, int cducmd);
	void DriveGimbalX(double angle);
	void DriveGimbalY(double angle);
	void DriveGimbalZ(double angle);
	void DriveGimbal(int index, int RegCDU, double angle);
	void PulsePIPA(int RegPIPA, int pulses);
	void SetOrbiterAttitudeReference();
	void DoZeroIMUCDUs();
	void DoZeroIMUGimbals();
	void calculatePhase(const VECTOR3 NewAngles);

	//
	// Logging.
	//

	void LogInit();
	void LogState(int channel, char *device, int value);
	void LogTimeStep(long time);
	void LogVector(char* message, VECTOR3 v);
	void LogMessage(char* s);

	//
	// Maths.
	//

	MATRIX3 getRotationMatrixX(double angle);
	MATRIX3 getRotationMatrixY(double angle);
	MATRIX3 getRotationMatrixZ(double angle);
	VECTOR3 getRotationAnglesXZY(MATRIX3 m);
	VECTOR3 getRotationAnglesZYX(MATRIX3 m);

	double degToRad(double angle);
	double radToDeg(double angle);
	double gyroPulsesToRad(int pulses);
	int radToGyroPulses(double angle);
	void clampTo2pi(double angle);

	MATRIX3 getNavigationBaseToOrbiterLocalTransformation();
	MATRIX3 getOrbiterLocalToNavigationBaseTransformation();

	ApolloGuidance &agc;
	VESSEL *OurVessel;
	bool LEM; // Flag to indicate LEM mode

	bool Operate;
	bool TurnedOn;
	bool Initialized;
	bool Caged;
	bool Powered;

	union {
		struct {
			double X;	//outer gimbal angle in rad
			double Y;	//inner gimbal angle in rad
			double Z;	//middle gimbal angle in rad
		} Gimbal;
		double Gimbals[3];
	};

	VECTOR3 SineGimbal;
	VECTOR3 CosineGimbal;
	VECTOR3 ResolverPhaseError;

	struct {
		double X;
		double Y;
		double Z;
	} RemainingPIPA;

	IMU_DriftRates imuDriftRates;
	PIPA_BiasScale pipaBiasScale;

	struct {
		MATRIX3 Attitude_v2g;
		MATRIX3 Attitude_g2v;
		MATRIX3 AttitudeReference;
	} Orbiter;

	InertialData &inertialData;

	PowerMerge DCPower;
	PowerMerge DCHeaterPower;
	h_Radiator *IMUCase;
	Boiler *IMUHeater;
	h_HeatLoad *IMUHeat;
	h_HeatLoad *PTAHeat;
	h_HeatLoad *PSAHeat;
	h_HeatLoad* CDUHeat;

	GuardedToggleSwitch *PowerSwitch;

	double pipaRate;	// PIPA pulse representation of speed change
	double LastSimDT;	// in seconds

	double IMUTempF;

	// Allow the MFD to touch our privates
	friend class ProjectApolloMFD;
	// And the RTCC as well
	friend class RTCC;
};

//
// Strings for state saving.
//

#define IMU_START_STRING	"IMU_BEGIN"
#define IMU_END_STRING		"IMU_END"

#endif
