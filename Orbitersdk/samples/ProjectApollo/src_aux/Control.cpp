/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2002-2005 Chris Knestrick

  Routines to provide spacecraft control

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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip.h>
#include <math.h>
#include "CDK.h"


// CCK
// These are magnitudes, sign can be + or -.
//const double DEADBAND_MID = Radians(2);
//const double DEADBAND_HIGH = Radians(10);
//const double RATE_NULL = Radians(0.05);
//const double RATE_LOW = Radians(0.1);
//const double RATE_MID = Radians(1.0);
//const double RATE_HIGH = Radians(2.0);


const double RATE_MAX = Radians(2.0);
const double DEADBAND_MAX = Radians(10.0);
const double RATE_HIGH = Radians(1.0);
const double DEADBAND_HIGH = Radians(3.0);
const double RATE_MID = Radians(0.5);
const double DEADBAND_MID = Radians(1.0);
const double RATE_LOW = Radians(0.25);
const double DEADBAND_LOW = Radians(0.15);
const double RATE_FINE = Radians(0.01);

const double RATE_NULL = Radians(0.0001);
// CCK End






// Generates rotational commands in order to achieve a target attitude in a given axis.
// Note that the user is responsible for providing an accurate current attitude information.
// This allows for greater flexability but also can be a source of problems if you pass
// incorrect data to the function.  For example, if you invert signs, then the spacecraft will
// fly out of control because as it rotates in the correct direction, the DeltaAngle will
// increase!  Please be careful :-)  The function returns true when the desired attitude
// has been reached (within the given deadband) and the velocity has been nulled; otherwise,
// it returns false.
bool SetAttitude(double TargetAttitude, double CurrentAttitude, double RotRate,
				 AXIS Axis, DEADBAND DeadbandLow)
{
	VESSEL *Vessel;			// Spacecraft interface

	double Rate;			// Depends on magnitude of DeltaAngle
	double RateDeadband;	// Depends on the magnitude of Rate
	double Thrust;			// Thrust required
	double Level;			// Required thrust level [0 - 1]

	VECTOR3 PMI;			// Prinicple moment of inertia
	double MaxThrust;		// Maximum thrust delivered by the engines
	double Size;			// Spacecraft radius
	double Mass;			// Spacecraft mass

	double DeltaRate;		// The difference between the desired and the actual rate
	double DeltaAngle = TargetAttitude - CurrentAttitude;

	// Get State
	Vessel = oapiGetFocusInterface();
	Vessel->GetPMI(PMI);
	MaxThrust = Vessel->GetMaxThrust(ENGINE_ATTITUDE);
	Mass = Vessel->GetMass();
	Size = Vessel->GetSize();

	// Let's take care of the good case first :-)
	if ((fabs(DeltaAngle) < DeadbandLow)) {
		//sprintf(oapiDebugString(), "NULL");
		if (fabs(RotRate) < RATE_NULL) {
			Vessel->SetAttitudeRotLevel(Axis, 0);
			return true;
		}

		return (NullRate(Axis));
	}

	// CCK
	// Now, we actually have to DO something! ;-)  Well divide it up into two cases, once
	// for each direction.  There's probably a better way, but not right now :-)
	//if (fabs(DeltaAngle) < DEADBAND_MID) {
	//	Rate = RATE_LOW;
	//	sprintf(oapiDebugString(), "LOW");
	//} else if(fabs(DeltaAngle) < DEADBAND_HIGH) {
	//	Rate = RATE_MID;
	//	sprintf(oapiDebugString(), "MID");
	//} else {
	//	Rate = RATE_HIGH;
	//	sprintf(oapiDebugString(), "HIGH");
	//}
	if (fabs(DeltaAngle) < DEADBAND_LOW) {
		Rate = RATE_FINE;
		//sprintf(oapiDebugString(), "FINE");
	} else if (fabs(DeltaAngle) < DEADBAND_MID) {
		Rate = RATE_LOW;
		//sprintf(oapiDebugString(), "LOW");
	} else if (fabs(DeltaAngle) < DEADBAND_HIGH) {
		Rate = RATE_MID;
		//sprintf(oapiDebugString(), "MID");
	} else if (fabs(DeltaAngle) < DEADBAND_MAX) {
		Rate = RATE_HIGH;
		//sprintf(oapiDebugString(), "HIGH");
	} else {
		Rate = RATE_MAX;
		//sprintf(oapiDebugString(), "MAX");
	}
	// CCK End

	RateDeadband = min(Rate / 2, Radians(0.01/*2*/));

	if (DeltaAngle < 0 ) {
		Rate = -Rate;
		RateDeadband = -RateDeadband;
	}


	DeltaRate = Rate - RotRate;

	if (DeltaAngle > 0) {
		if (DeltaRate > RateDeadband) {
			Thrust = (Mass * PMI.data[Axis] * DeltaRate) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			Vessel->SetAttitudeRotLevel(Axis, Level);
		} else if (DeltaRate < -RateDeadband) {
			Thrust = (Mass * PMI.data[Axis] * DeltaRate) / (Size);
			Level = max((Thrust/MaxThrust), -1);
			Vessel->SetAttitudeRotLevel(Axis, Level);
		} else {
			Vessel->SetAttitudeRotLevel(Axis, 0);
		}
	} else {
		if (DeltaRate < RateDeadband) {
			Thrust = (Mass * PMI.data[Axis] * DeltaRate) / (Size);
			Level = max((Thrust/MaxThrust), -1);
			Vessel->SetAttitudeRotLevel(Axis, Level);
		} else if (DeltaRate > -RateDeadband) {
			Thrust = (Mass * PMI.data[Axis] * DeltaRate) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			Vessel->SetAttitudeRotLevel(Axis, Level);
		} else {
			Vessel->SetAttitudeRotLevel(Axis, 0);
		}
	}



	return false;
}


// Wrapper for the above function that assumes that the rotation rate is vrot.
// Calling the function directly allows more control but will probably be used less
bool SetAttitude(double TargetAttitude, double CurrentAttitude, AXIS Axis, DEADBAND DeadbandLow)
{
	VESSEL *Vessel;			// Spacecraft interface
	VESSELSTATUS Status;	// Spacecraft status

	Vessel = oapiGetFocusInterface();
	Vessel->GetStatus(Status);

	return SetAttitude(TargetAttitude, CurrentAttitude, Status.vrot.data[Axis], Axis, DeadbandLow);
}


// Basically NAVMODE_KILLROT in a single dimension.  Returns true when the rate has been nulled
// out, otherwise it returns false.
bool NullRate(AXIS Axis)
{
	VESSEL *Vessel = oapiGetFocusInterface();
	VESSELSTATUS Status;
	Vessel->GetStatus(Status);

	double RateDeadband = Radians(0.001), Thrust, Level, Rate = Status.vrot.data[Axis],
			Mass = Vessel->GetMass(),
			MaxThrust = Vessel->GetMaxThrust(ENGINE_ATTITUDE),
			Size = Vessel->GetSize();

	VECTOR3 PMI;
	Vessel->GetPMI(PMI);

	if (fabs(Rate) < RATE_NULL) {
		Vessel->SetAttitudeRotLevel(Axis, 0.0);
		return true;
	}

	Thrust = -(Mass * PMI.data[Axis] * Rate) / (Size);
	Level = min((Thrust/MaxThrust), 1);
	Vessel->SetAttitudeRotLevel(Axis, Level);

	return false;
}
