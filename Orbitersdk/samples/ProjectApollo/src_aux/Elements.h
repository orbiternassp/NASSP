/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Chris Knestrick

  Routines to provide orbital elements in vector form

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

#ifndef ELEMENTS_H
#define ELEMENTS_H

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "GravBodyData.h"

// Orbital elements in vector for, since Vessel->GetElements() only provides the scalars. Also
// includes several scaler elements, including the magnitude of each vector.  The general
// convention is that if R is the vector, then r is the magnitude of that vector
struct ELEMENTS_VEC {
	GRAVBODY Body;		// The reference body of the orbit
	double Energy;		// The specific mechanic energy of the spacecraft

	// Vectors
	VECTOR3 R;			// Position
	VECTOR3 V;			// Velocity
	VECTOR3 H;			// Angular Momentum
	VECTOR3 N;			// Node Vector
	VECTOR3 Ecc;		// Eccentricity

	// Vector magnitude
	double r;			// Position
	double v;			// Velocity
	double h;			// Angular Momentum
	double n;			// Node
	double e;			// Eccentricity
	double p;			// Semi-latus rectum
	double a;			// Semi-major axis

	// Angles
	double Inc;			// Inclination
	double LAN;			// Longitude of Ascending Node
	double AOP;			// Arguement of Periapsis
	double TrueAnomaly;	// True Anonaly (Duh!)
};

const VECTOR3 UNIT_I = { 1, 0, 0 };
const VECTOR3 UNIT_J = { 0, 1, 0 };
const VECTOR3 UNIT_K = { 0, 0, 1 };

// Prototypes
double CalcEccAnomaly(double TrueAnomaly, double Ecc);
double CalcTimeOfFlightEcc(double EccAnomaly1, double EccAnomaly2, int K,
						   const ELEMENTS_VEC &Elements);
double CalcTimeOfFlight(double TrueAnomaly1, double TrueAnomaly2, int K,
						const ELEMENTS_VEC &Elements);
void GetElements(ELEMENTS_VEC &Elements);

#endif
