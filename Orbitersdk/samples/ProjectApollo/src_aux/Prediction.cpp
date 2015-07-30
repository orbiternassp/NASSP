/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Chris Knestrick

  Code to solve the prediction problem - given the position and velocity
  vectors at some initial time, what will the position and velocity vectors
  be at specified time in the future.  The method implemented here is
  described in Chapter 4 of "Fundementals of Astrodynamics" by Bate, Mueller,
  and White.

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
  *	Revision 1.2  2008/04/11 11:49:24  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include <stdlib.h>
#include <math.h>

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "OrbiterSDK.h"

static inline double CalcZ(double X, double a)
{
	return (pow(X, 2) / a);
}

static inline double CalcC(double Z)
{
	if (Z > 0) {
		return ((1 - cos(sqrt(Z))) / Z);
	} else if (Z < 0) {
		return ((1 - cosh(sqrt(-Z))) / Z);
	} else {
		return 0;
		// Z == 0  This needs to be added
	}
}

static inline double CalcS(double Z)
{
	if (Z > 0) {
		return ((sqrt(Z) - sin(sqrt(Z))) / sqrt(pow(Z, 3)));
	} else if (Z < 0) {
		return ((sinh(sqrt(-Z)) - sqrt(-Z)) / sqrt(pow(-Z, 3)));
	} else {
		return 0;
		// Z == 0 This needs to be added
	}
}

static inline double CalcF(double X, double C, double r)
{
	return (1 - ((pow(X, 2) / r) * C));
}

static inline double CalcG(double Time, double X, double S, double SqrtMu)
{
	return (Time - ((pow(X, 3) / SqrtMu) * S) );
}

static inline double CalcFDot(double SqrtMu, double r_0, double r, double X, double Z, double S)
{
	return ((SqrtMu / (r * r_0)) * X * (Z * S - 1));
}

static inline double CalcGDot(double X, double C, double r)
{
	return (1 - ((pow(X, 2) / r) * C));
}


// Magnitude of a vector
inline double Mag(const VECTOR3 &a)
{
	return (sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2)));
}

// This is the dot product
inline double operator* (const VECTOR3 &a, const VECTOR3 &b)
{
	VECTOR3 c;

	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;

	return (c.x + c.y + c.z);
}


// Required accuracy for the iterative computations
const double EPSILON = 0.0001;

// Iterative method to calculate new X and Z values for the specified time of flight
static inline void CalcXandZ(double &X, double &Z, const VECTOR3 Pos, const VECTOR3 Vel,
							 double a, const double Time, const double SqrtMu)
{
	const double MAX_ITERS = 10;
	double C, S, T, dTdX, DeltaTime, r = Mag(Pos), IterNum = 0;

	// These don't change over the iterations
	double RVMu = (Pos * Vel) / SqrtMu;		// Dot product of position and velocity divided
											// by the squareroot of Mu
	double OneRA = (1 - (r / a));			// One minus Pos over the semi-major axis

	C = CalcC(Z);
	S = CalcS(Z);
	T = ((RVMu * pow(X, 2) * C) +  (OneRA * pow(X, 3) * S) + (r * X)) / SqrtMu;

	DeltaTime = Time - T;

	// Iterate while the result isn't within tolerances
	while (fabs(DeltaTime) > EPSILON && IterNum++ < MAX_ITERS) {
		dTdX = ((pow(X, 2) * C) + (RVMu * X * (1 - Z * S)) + (r * (1 - Z * C))) / SqrtMu;

		X = X + (DeltaTime / dTdX);
		Z = CalcZ(X, a);

		C = CalcC(Z);
		S = CalcS(Z);
		T = ((RVMu * pow(X, 2) * C) +  (OneRA * pow(X, 3) * S) + (r * X)) / SqrtMu;

		DeltaTime = Time - T;

	}


}

// Given the specified position and velocity vectors for a given orbit, retuns the position
// and velocity vectors after a specified time
void PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel, double a, double Mu,
						  double Time, VECTOR3 &NewPos, VECTOR3 &NewVel, double &NewVelMag)
{
	double SqrtMu = sqrt(Mu);

	// Variables for computation
	double X = (SqrtMu * Time) / a;					// Initial guesses for X
	double Z = CalcZ(X, a);							// and Z
	double C, S;									// C(Z) and S(Z)
	double F, FDot, G, GDot;

	// Calculate the X and Z for the specified time of flight
	CalcXandZ(X, Z, Pos, Vel, a, Time, SqrtMu);

	// Calculate C(Z) and S(Z)
	C = CalcC(Z);
	S = CalcS(Z);

	// Calculate the new position and velocity vectors
	F = CalcF(X, C, Mag(Pos));
	G = CalcG(Time, X, S, SqrtMu);
	NewPos = (Pos * F) + (Vel * G);

	FDot = CalcFDot(SqrtMu, Mag(Pos), Mag(NewPos), X, Z, S);
	GDot = CalcGDot(X, C, Mag(NewPos));

	NewVel = (Pos * FDot) + (Vel * GDot);

	NewVelMag = Mag(NewVel);
}