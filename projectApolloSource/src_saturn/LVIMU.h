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
  *	Revision 1.2  2012/11/04 13:33:13  meik84
  *	LVDC++
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.4  2007/10/18 00:23:17  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.3  2007/09/04 13:51:55  dseagrav
  *	IGM first stage. It doesn't work yet.
  *	
  *	Revision 1.2  2007/06/23 21:20:36  dseagrav
  *	LVDC++ Update: Now with Pre-IGM guidance
  *	
  *	Revision 1.1  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	
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
	VECTOR3 Velocity;

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

