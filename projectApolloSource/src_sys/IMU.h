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
  *	Revision 1.2  2010/10/04 13:36:55  vrouleau
  *	PIPA rate for the LM is 1 cm/s. Not the same as CSM ( 5.85 cm/s)
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.13  2009/01/17 23:58:11  tschachim
  *	Bugfix for docked vessels, GetForceVector isn't working in this case.
  *	
  *	Revision 1.12  2007/04/25 18:48:58  tschachim
  *	New PIPA implementation.
  *	
  *	Revision 1.11  2006/12/19 15:55:54  tschachim
  *	ECS test stuff, bugfixes.
  *	
  *	Revision 1.10  2006/08/20 08:28:06  dseagrav
  *	LM Stage Switch actually causes staging (VERY INCOMPLETE), Incorrect "Ascent RCS" removed, ECA outputs forced to 24V during initialization to prevent IMU/LGC failure on scenario load, Valves closed by default, EDS saves RCS valve states, would you like fries with that?
  *	
  *	Revision 1.9  2006/06/29 22:38:44  tschachim
  *	Bugfix saving/loading
  *	
  *	Revision 1.8  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.7  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
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

class IMU {

public:
	IMU(ApolloGuidance &comp, PanelSDK &p);
	~IMU();

	void Init();
	void ChannelOutput(int address, int value);
	void Timestep(double simt);
	void SystemTimestep(double simdt); 
	void TurnOn();
	void TurnOff();
	void DriveGimbals(double x, double y, double z);
	void SetVessel(VESSEL *v, bool LEMFlag);
	VECTOR3 GetTotalAttitude();

	void WireToBuses(e_object *a, e_object *b, GuardedToggleSwitch *s);
	void WireHeaterToBuses(Boiler *heater, e_object *a, e_object *b);

	bool IsCaged();
	bool IsPowered();
	void SetCaged(bool val);
	Boiler *GetHeater() { return IMUHeater; };

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

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
	void ZeroIMUCDUs();

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

	MATRIX3 getNavigationBaseToOrbiterLocalTransformation();
	MATRIX3 getOrbiterLocalToNavigationBaseTransformation();

	ApolloGuidance &agc;
	VESSEL *OurVessel;
	bool LEM; // Flag to indicate LEM mode

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

		MATRIX3 AttitudeReference;
	} Orbiter;

	VECTOR3 LastWeightAcceleration;
	VECTOR3 LastGlobalVel;

	PowerMerge DCPower;
	PowerMerge DCHeaterPower;
	Boiler *IMUHeater;
	GuardedToggleSwitch *PowerSwitch;

	double pipaRate;	// PIPA pulse representation of speed change
	double LastTime;	// in seconds

	// Allow the MFD to touch our privates
	friend class ProjectApolloMFD;
};

//
// Strings for state saving.
//

#define IMU_START_STRING	"IMU_BEGIN"
#define IMU_END_STRING		"IMU_END"

#endif