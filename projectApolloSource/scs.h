/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Stability & Control Systems definitions.
  The implementation is in satsystems.cpp

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

// DS20060304 SCS objects
class Saturn;

class BMAG {
	// Body-Mounted Attitude Gyro
public: // We use these inside a timestep, so everything is public to make data access as fast as possible.
	BMAG();                                                                  // Cons
	void Init(Saturn *vessel, e_object *dcbus, ThreeWayPowerMerge *acbus);   // Initialization
	void TimeStep();                                                         // Update function
	e_object *dc_source;                                                     // DC source for gyro heater
	ThreeWayPowerMerge *ac_source;                                           // 3-Phase AC source for gyro
	e_object *dc_bus;					  	          					     // DC source to use when powered
	ThreeWayPowerMerge *ac_bus;                                              // 3-Phase AC source to use when powered
	int temperature;                                                         // Temperature
	VECTOR3 rates;                                                           // Detected rotation acceleration
	Saturn *sat;                                                             // Pointer to ship we're attached to
	bool powered;                                                            // Data valid flag.
};

// Confusing mathematics blatantly copipe from IMU, and renamed to avoid conflict
typedef union {      // 3x3 matrix
	double data[9];
	struct { double m11, m12, m13, m21, m22, m23, m31, m32, m33; };
} GDC_Matrix3;

class GDC {
	// Gyro Display Coupler
public: // We use these inside a timestep, so everything is public to make data access as fast as possible.
	GDC();                      //  Cons
	void Init(Saturn *vessel);	// Initialization
	void TimeStep(double simt); // TimeStep
	bool AlignGDC();            // Alignment Switch Pressed
	GDC_Matrix3 multiplyMatrix(GDC_Matrix3 a, GDC_Matrix3 b);
	GDC_Matrix3 getRotationMatrixX(double angle);
	GDC_Matrix3 getRotationMatrixY(double angle);
	GDC_Matrix3 getRotationMatrixZ(double angle);
	VECTOR3 getRotationAnglesXZY(GDC_Matrix3 m);
	void SetOrbiterAttitudeReference();
	GDC_Matrix3 getNavigationBaseToOrbiterLocalTransformation();	
	GDC_Matrix3 getOrbiterLocalToNavigationBaseTransformation();
	VECTOR3 rates;              // Integrated Euler rotation rates
	VECTOR3 attitude;           // Calculated Attitude
	double LastTime;			// in seconds
	bool Initialized;			// Was it?
	int roll_bmag_failed;       // Fault-finding
	int pitch_bmag_failed;
	int yaw_bmag_failed;
	struct {					// Orbiter's state
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
		GDC_Matrix3 AttitudeReference;
	} Orbiter;
	// GDC is supplied from AC1 and AC2 phase A, and from MNA and MNB.
	// All of these are needed for it to operate!
	Saturn *sat;
	// FDAI error needle data from CMC
	int fdai_err_ena;
	int fdai_err_x;
	int fdai_err_y;
	int fdai_err_z;
	friend class CSMcomputer; // Needs to write FDAI error indications, which are really not on the GDC, but meh.
};

class ASCP {
	// Attitude Set Control Panel
public: // We use these inside a timestep, so everything is public to make data access as fast as possible.
	ASCP();														   // Cons
	void Init(Saturn *vessel);									   // Initialization
	void TimeStep();									           // Timestep
	bool PaintRollDisplay(SURFHANDLE surf, SURFHANDLE digits);     // Update panel image
	bool PaintPitchDisplay(SURFHANDLE surf, SURFHANDLE digits);    // Update panel image
	bool PaintYawDisplay(SURFHANDLE surf, SURFHANDLE digits);      // Update panel image
	bool RollDisplayClicked();									   // Clicked
	bool PitchDisplayClicked();									   // Clicked
	bool YawDisplayClicked();									   // Clicked
	bool RollUpClick(int Event);								   // Clicked
	bool RollDnClick(int Event);
	bool PitchUpClick(int Event);								   // Clicked
	bool PitchDnClick(int Event);
	bool YawUpClick(int Event);									   // Clicked
	bool YawDnClick(int Event);

	VECTOR3 input;												   // Input attitude
	VECTOR3 output;												   // Output attitude
	Saturn *sat;												   // The spacecraft
	int msgcounter;
};

class EDA {
	// Electronic Display Assembly
	// This really just serves as a placeholder right now, and does some of the FDAI sourcing
	// so the FDAI redraw is less messy.
public: // Same stuff about speed and I'm lazy too.
	EDA();															// Cons
	void Init(Saturn *vessel);										// Initialization
	VECTOR3 ReturnCMCErrorNeedles();								// Return said data.
	VECTOR3 ReturnASCPError(VECTOR3 attitude);						// Return said data.
	VECTOR3 ReturnBMAG1Error();										// See the general theme here?
	VECTOR3 AdjustErrorsForRoll(VECTOR3 attitude, VECTOR3 errors);  // Adjust errors for roll so as to be FLY-TO

	Saturn *sat;
};

class RJEC {
	// Reaction Jet Engine Control
public: // Same stuff about speed and I'm lazy too.
	RJEC();															// Cons
	void Init(Saturn *vessel);										// Initialization
	void SetThruster(int thruster,bool Active);                     // Set Thruster Level
	void TimeStep();                                                // Timestep
	bool ThrusterDemand[20];                                        // Set when this thruster is requested to fire
	bool SPSActive;                                                 // SPS Active notification
	bool DirectPitchActive,DirectYawActive,DirectRollActive;        // Direct axis fire notification
	Saturn *sat;
};