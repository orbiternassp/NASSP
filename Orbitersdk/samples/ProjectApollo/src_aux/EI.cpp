/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Chris Knestrick

  Code to calculate the entry interface paramters

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

#include <stdio.h>
#include "CDK.h"

// Calculate the entry interface parameters
void CalcEntryInterface(ENTRY_INTERFACE &Interface)
{
	VESSEL *Spacecraft = oapiGetFocusInterface();
	VECTOR3 AngularMomentum;
	double MagRadius, MagVelocity, MagEntryInterfaceVelocity, MagAngularMomentum;
	VESSELSTATUS Status;
	double EntryInterfaceTrueAnomaly, SpacecraftEccAnomaly, EntryInterfaceEccAnomaly;
	ELEMENTS_VEC Elements;
	GRAVBODY Body;
	double ENTRY_INTERFACE_RADIUS, MU;

	// We'll assume the worst
	Interface.InterfaceDefined = false;

	Spacecraft->GetStatus(Status);

	Body = GetGravBody(Status.rbody);
	ENTRY_INTERFACE_RADIUS = GravBodyData[Body].EIRadius;


	// Make sure that the reference body actually has an entry interface radius defined
	if (ENTRY_INTERFACE_RADIUS <= 0) {
		return;
	}

	MU = GravBodyData[Body].Mu;

	MagRadius = Mag(Status.rpos);
	MagVelocity = Mag(Status.rvel);

	// Calculate the angular momentum of the orbit
	AngularMomentum = CrossProduct(Status.rpos, Status.rvel);
	MagAngularMomentum = Mag(AngularMomentum);

	// Calculate the velocity at the entry interface altitude
	double Temp = (pow(MagVelocity, 2) - (2 * MU) / MagRadius +
					(2 * MU) / ENTRY_INTERFACE_RADIUS);

	if (Temp < 0) {
		return;
	}

	MagEntryInterfaceVelocity = sqrt(Temp);

	Temp = MagAngularMomentum / (MagEntryInterfaceVelocity * ENTRY_INTERFACE_RADIUS);

	if (Temp >= 1.0 || Temp <= -1.0) {
		return;
	}

	// This "returns" the angle
	Interface.Angle = acos(Temp);


	// Calculate the time until entry interface
	GetElements(Elements);
	EntryInterfaceTrueAnomaly = (2 * PI) - acos((Elements.p - ENTRY_INTERFACE_RADIUS) /
									(ENTRY_INTERFACE_RADIUS * Elements.e));


	EntryInterfaceEccAnomaly = CalcEccAnomaly(EntryInterfaceTrueAnomaly, Elements.e);
	SpacecraftEccAnomaly = CalcEccAnomaly(Elements.TrueAnomaly, Elements.e);

	// This "returns" the time
	Interface.TimeToGo = (sqrt(pow(Elements.a, 3) / MU) *
						  ((EntryInterfaceEccAnomaly - Elements.e * sin(EntryInterfaceEccAnomaly)) -
						  (SpacecraftEccAnomaly - Elements.e * sin(SpacecraftEccAnomaly))));

	if (Interface.TimeToGo < 0) {
		return;
	}

	// Calculate the entry interface position and velocity vectors
	PredictPosVelVectors(Elements.R, Elements.V, Elements.a, MU,
							Interface.TimeToGo, Interface.Pos, Interface.Vel);

	Interface.InterfaceDefined = true;

}
