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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "CDK.h"

// Calculate the eccentric anomaly for a given true anomaly and orbit eccentricity
double CalcEccAnomaly(double TrueAnomaly, double Ecc)
{
	if (TrueAnomaly <= PI) {
		return acos((Ecc + cos(TrueAnomaly)) / (1 + Ecc * cos(TrueAnomaly)));
	} else {
		return ((2 * PI) - acos((Ecc + cos(TrueAnomaly)) / (1 + Ecc * cos(TrueAnomaly))));
	}
}


// Return the time of flight between the two eccentric anomalies for a given (eliptical) orbit,
// where K is the number of times thru periapsis
double CalcTimeOfFlightEcc(double EccAnomaly1, double EccAnomaly2, int K,
						   const ELEMENTS_VEC &Elements)
{
	return 	(sqrt(pow(Elements.a, 3) / GravBodyData[Elements.Body].Mu) *  ((2 * K * PI) +
						(EccAnomaly2 - Elements.e * sin(EccAnomaly2)) -
						(EccAnomaly1 - Elements.e * sin(EccAnomaly1))));
}

// Return the time of flight between the two true anomalies for a given (eliptical) orbit,
// where K is the number of times thru periapsis
double CalcTimeOfFlight(double TrueAnomaly1, double TrueAnomaly2, int K,
						const ELEMENTS_VEC &Elements)
{
	double EA1 = CalcEccAnomaly(TrueAnomaly1, Elements.e),
		   EA2 = CalcEccAnomaly(TrueAnomaly2, Elements.e);

//	sprintf(oapiDebugString(), "%.2f  %.2f", Degree(EA1), Degree(EA2));

	return CalcTimeOfFlightEcc(EA1, EA2, K, Elements);
}


void GetElements(ELEMENTS_VEC &Elements)
{
	VESSEL *Spacecraft;
	VESSELSTATUS Status;

	Spacecraft = oapiGetFocusInterface();
	Spacecraft->GetStatus(Status);
	Elements.Body = GetGravBody(Status.rbody);


	// This should never happen
	if (Elements.Body == GRAVBODY_UNKNOWN) {
		PrintString("UpdateElements(): Unknown grav body");
		return;
	}

	double Mu = GravBodyData[Elements.Body].Mu;


	Elements.R = Status.rpos;							// Position
	Elements.r = Mag(Elements.R);
	Elements.V = Status.rvel;							// Velocity
	Elements.v = Mag(Elements.V);
	Elements.H = CrossProduct(Elements.R, Elements.V);	// Angular momentum
	Elements.h = Mag(Elements.H);
	Elements.N = CrossProduct(UNIT_K, Elements.H);		// Node vector
	Elements.n = Mag(Elements.N);
	Elements.Ecc = (Elements.R * (pow(Elements.v, 2) - Mu / Elements.r)  - Elements.V * (Elements.R * Elements.V)) / Mu;
	Elements.e = Mag(Elements.Ecc);
	Elements.Energy = (pow(Elements.v, 2) / 2) - (Mu / Elements.r);
	Elements.p = pow(Elements.h, 2) / Mu;
	Elements.a = -Mu / (2 * Elements.Energy);

	Elements.Inc = acos(Elements.H.z / Elements.h);

	// Longitude of Ascending node
	Elements.LAN = acos(Elements.N.x / Elements.n);

	if (Elements.N.y <= 0) {
		Elements.LAN = 2 * PI - Elements.LAN;
	}


	Elements.AOP = acos((Elements.N * Elements.Ecc) / (Elements.n * Elements.e));

	if (Elements.Ecc.z >= 0) {
		Elements.AOP = 2 * PI - Elements.AOP;
	}

	// True Anomaly
	Elements.TrueAnomaly = acos((Elements.Ecc * Elements.R) / (Elements.e * Elements.r));

	if ((Elements.R * Elements.V) < 0) {
		Elements.TrueAnomaly = 2 * PI - Elements.TrueAnomaly;
	}

}
