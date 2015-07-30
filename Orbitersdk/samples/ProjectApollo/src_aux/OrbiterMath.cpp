/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2002-2005 Chris Knestrick

  General math operation that I found useful for Orbiter programming

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

#if defined(_MSC_VER) && (_MSC_VER >= 1300 ) // Microsoft Visual Studio Version 2003 and higher
#define _CRT_SECURE_NO_DEPRECATE 
#endif

#include "CDK.h"

void MultiplyByMatrix(const VECTOR3 &Initial, const MATRIX3 &RotMatrix, VECTOR3 &Result)
{

	Result.x =	(Initial.x * RotMatrix.m11)
				+ (Initial.y * RotMatrix.m12)
				+ (Initial.z * RotMatrix.m13);

	Result.y =	(Initial.x * RotMatrix.m21)
				+ (Initial.y * RotMatrix.m22)
				+ (Initial.z * RotMatrix.m23);

	Result.z =	(Initial.x * RotMatrix.m31)
				+ (Initial.y * RotMatrix.m32)
				+ (Initial.z * RotMatrix.m33);

}

void RotateVector(const VECTOR3 &Initial, const VECTOR3 &Angles, VECTOR3 &Result)
{
	MATRIX3 RotMatrixX, RotMatrixY, RotMatrixZ;
	VECTOR3 AfterZ, AfterZY;					// Temporary variables


	GetRotMatrixX(Angles.x, RotMatrixX);
	GetRotMatrixY(Angles.y, RotMatrixY);
	GetRotMatrixZ(Angles.z, RotMatrixZ);

	MultiplyByMatrix(Initial, RotMatrixZ, AfterZ);
	MultiplyByMatrix(AfterZ, RotMatrixY, AfterZY);
	MultiplyByMatrix(AfterZY, RotMatrixX, Result);
}

// Return a vector containing the pitch, yaw, and roll angles between the current spacecraft
// attitude and the attitude defined by the two vectors; the first defines the pitch and yaw,
// and the second defines the roll.  If the second vector is the null vector, the function
// returns only the pitch and yaw (roll is assigned 0.0).  The input vectors should be with
// respect to the local frame
VECTOR3 GetPitchYawRoll(VECTOR3 PitchYaw, VECTOR3 Roll)
{
	double Distance;
	VECTOR3 Res;

	Distance = sqrt(pow(PitchYaw.x, 2) + pow(PitchYaw.y, 2) + pow(PitchYaw.z, 2));
	Res.data[PITCH] = asin(PitchYaw.y/Distance);

	if (PitchYaw.z < 0) {
		//Res.data[PITCH] = -Res.data[PITCH];

		//Res.data[PITCH] = PI - Res.data[PITCH];
	}


	// Yaw
	Res.data[YAW] = acos(PitchYaw.z/(Distance * sin(acos(PitchYaw.y/Distance))));

	//if (Res.data[PITCH] > PI/2) {
	//	Res.data[YAW] = PI - Res.data[YAW];
	//}

	if (PitchYaw.x < 0) {
		Res.data[YAW] = -Res.data[YAW];
	}

	// Roll, if the second vector is not null
	if (Roll == NULL_VECTOR) {
		Res.data[ROLL] = 0.0;
		return Res;
	}

	Distance = sqrt(pow(Roll.x, 2) + pow(Roll.y, 2) + pow(Roll.z, 2));


	Res.data[ROLL] = asin(Roll.y / Distance);

	//if (Roll.z < 0) {
	//	Res.data[ROLL] = PI-Res.data[ROLL];
	//}

	return Res;
}

// Prints the pitch, yaw, and roll for a vector
void PrintPitchYawRoll(VECTOR3 PitchYaw, VECTOR3 Roll)
{
	PrintAngleVector(GetPitchYawRoll(PitchYaw, Roll));
}


#define LAT x
#define LONG y

// Returns the distance between two Lat/Long positions over the surface of a sphere of a given
// radius.
double CalcSphericalDistance(VECTOR3 Pos1, VECTOR3 Pos2, double Radius)
{
	double DeltaLat = Pos2.LAT - Pos1.LAT;
	double DeltaLong = Pos2.LONG - Pos1.LONG;

	double A = pow(sin(DeltaLat/2), 2) + cos(Pos1.LAT) * cos (Pos2.LAT) * pow(sin(DeltaLong/2), 2);
	double C = 2 * atan2(sqrt(A), sqrt(1 - A));

	return (Radius * C);
}


// Returns the bearing between two Lat/Long points on a sphere
double CalcSphericalBearing(VECTOR3 Pos1, VECTOR3 Pos2)
{

#define LAT1 Pos1.LAT
#define LAT2 Pos2.LAT
#define LONG1 Pos1.LONG
#define LONG2 Pos2.LONG

	// Since we can't do mod with doubles...
	double Result = atan2(sin(LONG1 - LONG2) * cos(LAT2),
					cos(LAT1) * sin(LAT2) - sin(LAT1) * cos(LAT2) * cos(LONG1 - LONG2));

	if (Result > (2*PI)) {
		Result -= 2*PI;
	}

	return Result;

#undef LAT1
#undef LAT2
#undef LONG1
#undef LONG2

}





void CalcOffsetLatLong(VECTOR3 Pos, VECTOR3& Result, double Distance, double Angle)
{
	double sinDeltaLong, cosDeltaLong, DeltaLong;

	Result.LAT = asin(sin(Pos.LAT) * cos(Distance) + cos(Pos.LAT) * sin(Distance) * cos(Angle));
	sinDeltaLong = (sin(Angle) * sin(Distance)) / cos(Result.LAT);
	cosDeltaLong = (cos(Distance) - sin(Pos.LAT) * sin(Result.LAT)) /
					(cos(Pos.LAT) * cos(Result.LAT));

	DeltaLong = atan2(sinDeltaLong, cosDeltaLong);
	Result.LONG = Pos.LONG - DeltaLong + PI;

	// Mod
	if (Result.LONG > (2*PI)) {
		Result.LONG -= 2*PI;
	}

	Result.LONG -=PI;

}

#undef LAT
#undef LONG

// Add two angles  - the result is between 0 and 2*PI
double AddAngles(double Initial, double Angle)
{
	double Result;

	Result = Initial + Angle;

	if (Result > (2*PI)) {
		Result -= 2*PI;
	}

	return Result;
}

