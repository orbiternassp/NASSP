/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2002-2005 Chris Knestrick

  Data for the different graviational bodies in Orbiter.  Values were
  taken from the Orbiter manual as well as several config files.

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
  *	Revision 1.2  2008/04/11 11:49:10  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.1  2005/02/11 12:17:54  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef GRAV_BODY_DATA_H
#define GRAV_BODY_DATA_H

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

// All the gravitational bodies - GRAVBODY_UNKNOWN must always be last, so NUM_GRAV_BODIES (below)
// is correct
enum GRAVBODY {SUN, MERCURY, VENUS, EARTH, MOON, MARS, PHOBOS,
			DEIMOS, JUPITER, CALLISTO, EUROPA, GANYMEDE, IO,
			SATURN, DIONE, ENCELADUS, MIMAS, RHEA, TITAN,
			URANUS, ARIEL, MIRANDA, OBERON, TITANIA,
			NEPTUNE, NERIED, TRITON, UMBRIEL, PLUTO, CHARON, GRAVBODY_UNKNOWN};

const int NUM_GRAV_BODIES = GRAVBODY_UNKNOWN + 1;
const int GRAVBODY_NAME_LEN = 25;

// The data stored for each gravitational body
struct GravBody {
	char Name[GRAVBODY_NAME_LEN];		// Name of body - used to search to find the object handle
	double Radius;						// Radius in m
	double Mass;						// Mass in kg
	double Mu;							// Mass x G
	double GravAccel;					// Gravitation acceleration in m/s^2
	double EscapeVel;					// Escape velocity in m/s
	double Tilt;						// Tilt from eliptic, in radians
	double EIRadius;					// Radius at which entry interface occurs
};

// These values were taken from the Orbiter documentation, as well as config files.  This list
// is currently incomplete - values of 0 indicate an unknown value.  The data for each planet
// should be complete - it is the moons/sun that are missing data.
#define SUN_DATA		{"Sun",		6.96e8,	1.99e30,	1.328e20,	0,		0,		Radians(0), 0}
#define MERCURY_DATA	{"Mercury", 2440000,	3.302e23,	2.203e13,	3.701,	4435,	Radians(7.01), 0}
#define VENUS_DATA		{"Venus",	6051840,	48.685e23,	3.249e14,	8.87,	10361,	Radians(1.27), 0}
#define EARTH_DATA		{"Earth",	6371010,	59.736e23,	3.986e14,	9.780327,	11186,	Radians(23.44), 6470000}
#define MOON_DATA		{"Moon",	1738000,	7.350e22,	4.904e12,	1.62,		2380,		Radians(0), 0}
#define MARS_DATA		{"Mars",	3389920,	6.4185e23,	4.283e13,	3.69,	5027,	Radians(26.72), 0}
#define PHOBOS_DATA		{"Phobos",	11000,		10.8e15,	7.206e5,	0,		0,		Radians(0), 0}
#define DEIMOS_DATA		{"Deimos",	65000,		1.8e15,		1.201e5,	0,		0,		Radians(0), 0}
#define JUPITER_DATA	{"Jupiter",	69911000,	18986e23,	1.267e17,	23.12,	59500,	Radians(2.22), 0}
#define CALLISTO_DATA	{"Callisto",	2.403e6,	1.08e23,	7.207e12,	0,	0,		Radians(0), 0}
#define EUROPA_DATA		{"Europa",	1.565e6,	4.80e22,	3.20e12,	0,		0,		Radians(0), 0}
#define	GANYMEDE_DATA	{"Ganymede", 2.634e6,	1.48e23,	9.88e12,	0,		0,		Radians(0), 0}
#define	IO_DATA			{"Io",		1.821e6,		8.93e22,	5.96e12,	0,		0,		Radians(0), 0}
#define SATURN_DATA		{"Saturn",	58232000,	5684.6e23,	3.793e16,	8.96,	35500,	Radians(28.05), 0}
#define DIONE_DATA		{"Dione",	5.60e5,		1.05e21,	7.01e10,	0,		0,		Radians(0), 0}
#define	ENCELADUS_DATA	{"Enceladus",	2.49e5,	7.30e19,	4.78e9,		0,		0,		Radians(0), 0}
#define MIMAS_DATA		{"Mimas",	1.96e5,		3.80e19,	2.54e9,		0,		0,		Radians(0), 0}
#define	RHEA_DATA		{"Rhea",	7.64e5,		2.49e21,	1.66e11,	0,		0,		Radians(0), 0}
#define	TITAN_DATA		{"Titan",	2.575e6,	1.35e23,	9.01e12,		0,		0,		Radians(0), 0}
#define URANUS_DATA		{"Uranus",	25362000,	868.32e23,	5.794e15,	8.69,	21300,	Radians(82.05), 0}
#define ARIEL_DATA		{"Ariel",	581000,		1.35e21,	9.008e10,	0,		0,		Radians(0), 0}
#define MIRANDA_DATA	{"Miranda",	235800,		6.3e19,		4.2e9,		0,		0,		Radians(0), 0}
#define	OBERON_DATA		{"Oberon",	761400,		3.03e21,	2.02e11,	0,		0,		Radians(0), 0}
#define TITANIA_DATA	{"Titania",	7.889e5,	3.49e21,	2.33e11,	0,		0,		Radians(0), 0}
#define NEPTUNE_DATA	{"Neptune",	24624000,	1024.3e23,	6.835e15,	11.00,	23500,	Radians(29.48), 0}
#define NEREID_DATA		{"Nereid",	170000,		3.087e19,	2.060e9,	0,		0,		Radians(0), 0}
#define TRITON_DATA		{"Triton",	1353400,	2.14e22,	1.43e12,	0,		0,		Radians(0), 0}
#define UMBRIEL_DATA	{"Umbriel",	584700,		1.27e21,	8.47e10,	0,		0,		Radians(0), 0}
#define PLUTO_DATA		{"Pluto",	1151000,	0.15e23,	1.001e12,	0.655,	1300,	Radians(68.69), 0}
#define CHARON_DATA		{"Charon",	5.93e5,		3.3e21,		2.2e11,		0,		0,		Radians(0), 0}

const GravBody GravBodyData[NUM_GRAV_BODIES] = {SUN_DATA,
												MERCURY_DATA,
												VENUS_DATA,
												EARTH_DATA,
												MOON_DATA,
												MARS_DATA,
												PHOBOS_DATA,
												DEIMOS_DATA,
												JUPITER_DATA,
												CALLISTO_DATA,
												EUROPA_DATA,
												GANYMEDE_DATA,
												IO_DATA,
												SATURN_DATA,
												DIONE_DATA,
												ENCELADUS_DATA,
												MIMAS_DATA,
												RHEA_DATA,
												TITAN_DATA,
												URANUS_DATA,
												ARIEL_DATA,
												MIRANDA_DATA,
												OBERON_DATA,
												TITANIA_DATA,
												NEPTUNE_DATA,
												NEREID_DATA,
												TRITON_DATA,
												UMBRIEL_DATA,
												PLUTO_DATA,
												CHARON_DATA};

// Returns the GRAVBODY for a given object reference
static inline GRAVBODY GetGravBody(OBJHANDLE bodyref)
{
	char BodyName[GRAVBODY_NAME_LEN];

	for (int body = 0; body < NUM_GRAV_BODIES; body++) {
		oapiGetObjectName(bodyref, BodyName, GRAVBODY_NAME_LEN);

		if (strcmp(BodyName, GravBodyData[body].Name) == 0) {
			return (GRAVBODY) body;
		}
	}

	return GRAVBODY_UNKNOWN;
}

// Return the velocity required (m/s) to maintain a circular orbit of a given altitude (m)
// around a body.  Note that altitude is in reference to the surface, not the center of
// mass
static inline double CircularOrbitVel(double Altitude, GRAVBODY Body)
{
	return (sqrt(GravBodyData[Body].Mu / (GravBodyData[Body].Radius + Altitude)));
}


// Return the perigee velocity required (m/s) to maintain an orbit with a given perigee and apogee (m)
// around a body.  Note that altitude is in reference to the surface, not the center of
// mass
static inline double HohmannPerigeeVel(double Perigee, double Apogee, GRAVBODY Body)
{
	Perigee += GravBodyData[Body].Radius;
	Apogee += GravBodyData[Body].Radius;

	double Energy = -(GravBodyData[Body].Mu) / (Perigee + Apogee);

	return (sqrt(2 * ((GravBodyData[Body].Mu / Perigee) + Energy)));
}

// Return the apogee velocity required (m/s) to maintain an orbit with a given perigee and apogee (m)
// around a body.  Note that altitude is in reference to the surface, not the center of
// mass
static inline double HohmannApogeeVel(double Perigee, double Apogee, GRAVBODY Body)
{
	Perigee += GravBodyData[Body].Radius;
	Apogee += GravBodyData[Body].Radius;

	double Energy = -(GravBodyData[Body].Mu) / (Perigee + Apogee);

	return (sqrt(2 * ((GravBodyData[Body].Mu / Apogee) + Energy)));
}

#endif