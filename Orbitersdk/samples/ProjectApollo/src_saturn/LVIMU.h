/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LV IMU header

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

// This is an abbreviated ST-124 IMU for the launch vehicle's use.

#if !defined(_PA_LVIMU_H)
#define _PA_LVIMU_H

// Indexes into CDURegisters
#define LVRegCDUX  000
#define LVRegCDUY  001
#define LVRegCDUZ  002
#define LVRegPIPAX 003
#define LVRegPIPAY 004
#define LVRegPIPAZ 005

///
/// \brief Saturn IMU simulation.
/// \ingroup LVSystems
///
class LVIMU {

public:
	LVIMU();
	~LVIMU();

	void Init();
	void Timestep(double simt);
	void TurnOn();
	void TurnOff();
	void DriveGimbals(double x, double y, double z);
	void SetVessel(VESSEL *v) { OurVessel = v; };
	VECTOR3 GetTotalAttitude();

	bool IsCaged();
	bool IsPowered();
	void SetCaged(bool val);
	void ZeroPIPACounters();

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	double CDURegisters[6]; // CDU output registers

protected:
	
	void DriveCDUX(int cducmd);
	void DriveCDUY(int cducmd);
	void DriveCDUZ(int cducmd);
	void DriveCDU(int index, int RegCDU, int cducmd);
	void DriveGimbalX(double angle);
	void DriveGimbalY(double angle);
	void DriveGimbalZ(double angle);
	void DriveGimbal(int index, int RegCDU, double angle);
	void PulsePIPA(int RegPIPA, double pulses);
	void SetOrbiterAttitudeReference();
	void ZeroIMUCDUs();

	// Flags to replace IO channel discretes
public: bool ZeroIMUCDUFlag;
public: bool CoarseAlignEnableFlag;

	//
	// Maths.
	//

public: MATRIX3 getRotationMatrixX(double angle);
	MATRIX3 getRotationMatrixY(double angle);
	MATRIX3 getRotationMatrixZ(double angle);
	VECTOR3 getRotationAnglesXZY(MATRIX3 m);
	VECTOR3 getRotationAnglesZYX(MATRIX3 m);

	double degToRad(double angle);
	double radToDeg(double angle);
	double gyroPulsesToRad(int pulses);
	int radToGyroPulses(double angle);

	MATRIX3 getNavigationBaseToOrbiterLocalTransformation();
	MATRIX3 getOrbiterLocalToNavigationBaseTransformation();

	VESSEL *OurVessel;

	bool Operate;
	bool TurnedOn;
	bool Initialized;
	bool Caged;

	union {
		struct {
			double X;	//outer gimbal angle in rad
			double Y;	//inner gimbal angle in rad
			double Z;	//middle gimbal angle in rad
		} Gimbal;
		double Gimbals[3];
	};

	struct {
		double X;
		double Y;
		double Z;
	} RemainingPIPA;

	struct {
		struct {
			double X;
			double Y;
			double Z;
		} Attitude;
		struct {
			double X;
			double Y;
			double Z;
		} LastAttitude;

		MATRIX3 AttitudeReference;
	} Orbiter;

	VECTOR3 LastWeightAcceleration;
	VECTOR3 LastGlobalVel;

	double LastTime;	// in seconds
};

//
// Strings for state saving.
//

#define LVIMU_START_STRING	"LVIMU_BEGIN"
#define LVIMU_END_STRING	"LVIMU_END"

// Now the LV rate gyros

class LVRG {
public: 
	LVRG();                                                                  // Cons
	void Init(VESSEL *v);													 // Initialization
	void Timestep(double simdt);                                             // Update function
	VECTOR3 GetRates() { return rates; };

protected:
	VECTOR3 rates;                                                           // Detected rotation acceleration
	VESSEL *sat;                                                             // Pointer to ship we're attached to
};

#endif

