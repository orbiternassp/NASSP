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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.6  2006/01/14 20:58:15  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.5  2005/12/02 19:47:19  movieman523
  *	Replaced most PowerSource code with e_object.
  *	
  *	Revision 1.4  2005/11/17 21:04:52  movieman523
  *	IMU and AGC now start powered-down. Revised battery code, and wired up all batteries in CSM.
  *	
  *	Revision 1.3  2005/08/19 13:44:24  tschachim
  *	Fixes because of new Virtual AGC version.
  *	
  *	Revision 1.2  2005/08/13 00:09:43  movieman523
  *	Added IMU Cage switch
  *	
  *	Revision 1.1  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  **************************************************************************/

// IMU

#if !defined(_PA_IMU_H)
#define _PA_IMU_H

class ApolloGuidance;

#include "powersource.h"

typedef union {      // 3 vector
	double data[3];
	struct { double x, y, z; };
} IMU_Vector3;

typedef union {      // 3x3 matrix
	double data[9];
	struct { double m11, m12, m13, m21, m22, m23, m31, m32, m33; };
} IMU_Matrix3;

class IMU {

public:
	IMU(ApolloGuidance &comp, PanelSDK &p);
	~IMU();

	void Init();
	void ChannelOutput(int address, int value);
	void Timestep(double simt);
	void TurnOn();
	void TurnOff();
	void DriveGimbals(double x, double y, double z);
	void SetVessel(VESSEL *v, bool LEMFlag) { OurVessel = v; LEM = LEMFlag; };
	VECTOR3 GetTotalAttitude();

	void WireToBuses(e_object *a, e_object *b) { DCPower.WireToBuses(a, b); };

	bool IsCaged();
	bool IsPowered();
	void SetCaged(bool val);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

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
	void DriveGimbal(int index, int RegCDU, double angle, int changeCDU);
	void PulsePIPA(int RegPIPA, int pulses);
	void SetOrbiterAttitudeReference();
	void ZeroIMUCDUs();

	//
	// Logging.
	//

	void LogInit();
	void LogState(int channel, char *device, int value);
	void LogTimeStep(long time);
	void LogVector(char* message, IMU_Vector3 v);
	void LogMessage(char* s);

	//
	// Maths.
	//

	IMU_Matrix3 getRotationMatrixX(double angle);
	IMU_Matrix3 getRotationMatrixY(double angle);
	IMU_Matrix3 getRotationMatrixZ(double angle);
	IMU_Matrix3 multiplyMatrix(IMU_Matrix3 a, IMU_Matrix3 b);
	IMU_Vector3 multiplyMatrixByVector(IMU_Matrix3 m, IMU_Vector3 v);
	IMU_Vector3 getRotationAnglesXZY(IMU_Matrix3 m);
	IMU_Vector3 getRotationAnglesZYX(IMU_Matrix3 m);
	IMU_Vector3 VECTOR3ToIMU_Vector3(VECTOR3 v);
	VECTOR3 IMU_Vector3ToVECTOR3(IMU_Vector3 iv);
	IMU_Matrix3 MATRIX3ToIMU_Matrix3(MATRIX3 m);

	double degToRad(double angle);
	double radToDeg(double angle);
	double gyroPulsesToRad(int pulses);
	int radToGyroPulses(double angle);

	IMU_Matrix3 getNavigationBaseToOrbiterLocalTransformation();
	IMU_Matrix3 getOrbiterLocalToNavigationBaseTransformation();

	ApolloGuidance &agc;
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

		IMU_Matrix3 AttitudeReference;
	} Orbiter;

	VECTOR3 Velocity;

	PowerMerge DCPower;

	double LastTime;	// in seconds
};

//
// Strings for state saving.
//

#define IMU_START_STRING	"IMU_BEGIN"
#define IMU_END_STRING		"IMU_END"

#endif