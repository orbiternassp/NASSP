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

  **************************** Revision History ****************************
  *	$Log$
  *	
  **************************************************************************/

// This is an abbreviated ST-124 IMU for the launch vehicle's use.

#if !defined(_PA_LVIMU_H)
#define _PA_LVIMU_H

typedef union {      // 3 vector
	double data[3];
	struct { double x, y, z; };
} LVIMU_Vector3;

typedef union {      // 3x3 matrix
	double data[9];
	struct { double m11, m12, m13, m21, m22, m23, m31, m32, m33; };
} LVIMU_Matrix3;

// Indexes into CDURegisters
#define LVRegCDUX  000
#define LVRegCDUY  001
#define LVRegCDUZ  002
#define LVRegPIPAX 003
#define LVRegPIPAY 004
#define LVRegPIPAZ 005

class LVIMU {

public:
	LVIMU();
	~LVIMU();

	void Init();
	void Timestep(double simt);
	void TurnOn();
	void TurnOff();
	void DriveGimbals(double x, double y, double z);
	void SetVessel(VESSEL *v, bool LEMFlag) { OurVessel = v; LEM = LEMFlag; };
	VECTOR3 GetTotalAttitude();

	bool IsCaged();
	bool IsPowered();
	void SetCaged(bool val);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	int CDURegisters[6]; // CDU output registers

protected:
	
	bool LEM; // Flag to indicate LEM mode
	VECTOR3 CalculateAccelerations(double deltaT);
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
	void ZeroIMUCDUs();

	// Flags to replace IO channel discretes
	bool ZeroIMUCDUFlag;
	bool CoarseAlignEnableFlag;

	//
	// Maths.
	//

	LVIMU_Matrix3 getRotationMatrixX(double angle);
	LVIMU_Matrix3 getRotationMatrixY(double angle);
	LVIMU_Matrix3 getRotationMatrixZ(double angle);
	LVIMU_Matrix3 multiplyMatrix(LVIMU_Matrix3 a, LVIMU_Matrix3 b);
	LVIMU_Vector3 multiplyMatrixByVector(LVIMU_Matrix3 m, LVIMU_Vector3 v);
	LVIMU_Vector3 getRotationAnglesXZY(LVIMU_Matrix3 m);
	LVIMU_Vector3 getRotationAnglesZYX(LVIMU_Matrix3 m);
	LVIMU_Vector3 VECTOR3ToLVIMU_Vector3(VECTOR3 v);
	VECTOR3 LVIMU_Vector3ToVECTOR3(LVIMU_Vector3 iv);
	LVIMU_Matrix3 MATRIX3ToLVIMU_Matrix3(MATRIX3 m);

	double degToRad(double angle);
	double radToDeg(double angle);
	double gyroPulsesToRad(int pulses);
	int radToGyroPulses(double angle);

	LVIMU_Matrix3 getNavigationBaseToOrbiterLocalTransformation();
	LVIMU_Matrix3 getOrbiterLocalToNavigationBaseTransformation();

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

		LVIMU_Matrix3 AttitudeReference;
	} Orbiter;

	VECTOR3 Velocity;

	double LastTime;	// in seconds
};

//
// Strings for state saving.
//

#define LVIMU_START_STRING	"LVIMU_BEGIN"
#define LVIMU_END_STRING	"LVIMU_END"

#endif

