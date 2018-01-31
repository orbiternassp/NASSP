/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Radu Poenaru

  System & Panel SDK (SPSDK)

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

#include "thermal.h"
#include <math.h>
#include <stdio.h>

/// \todo For testing
//extern FILE *PanelsdkLogFile;

therm_obj::therm_obj() {

	next_t = NULL;
}

void therm_obj::thermic(double _en) {

	if (-_en > energy)
		_en = -energy / 10.0;

	energy += _en; //total energy, in joules to add or substract

	Temp = energy / c / mass;
}

void therm_obj::SetTemp(double _t) { 
	Temp = _t;
	energy = Temp * c * mass;
}

double therm_obj::GetTemp() {
	return Temp;
}

Thermal_engine::Thermal_engine() {

	List.next_t = NULL;
	NumberOfObjects = 0;
	distance_matrix = NULL;
	InSun = 0;
	InPlanet = 0;

	ObjToDebug = NULL;
}

void Thermal_engine::Save(FILEHANDLE scn)
{};
void Thermal_engine::Load(FILEHANDLE scn)
{};

Thermal_engine::~Thermal_engine() {

	if (distance_matrix) 
		delete[] distance_matrix;
}

therm_obj* Thermal_engine::AddThermalObject(therm_obj *n_obj, bool debug) { 
	
	therm_obj *runner;
	runner = &List;
	NumberOfObjects++;

	while (runner->next_t) 
		runner = runner->next_t;

	runner->next_t = n_obj;
	n_obj->next_t = NULL;

	if (debug) ObjToDebug = n_obj;
	return n_obj;
};

void Thermal_engine::RemoveThermalObject(therm_obj *n_obj) {

	therm_obj *runner; //,*gonner;
	runner = &List;
	while ((runner)&&(runner->next_t)) { 
		if (runner->next_t==n_obj)
			runner->next_t=n_obj->next_t;
		runner=runner->next_t;
	}
}

therm_obj* Thermal_engine::GetElement(int i) {

	therm_obj *runner;

	runner = &List;
	while ((i-->-1) && (runner->next_t)) 
		runner = runner->next_t;
	return runner;
}

void Thermal_engine::InitThermal()
{

	//builds the distance matrix for one thing
	distance_matrix = new (float[NumberOfObjects*NumberOfObjects]);
	therm_obj *elm_i,*elm_j;
	for (int i=0;i<NumberOfObjects;i++)
		//get the dists between i and j
	{	elm_i=GetElement(i);
		for (int j=i+1;j<NumberOfObjects;j++)
		{
			elm_j=GetElement(j);
			distance_matrix[i*NumberOfObjects+i]=0;
			distance_matrix[i*NumberOfObjects+j] = (float) ((elm_i->pos-elm_j->pos).mod());
			distance_matrix[j*NumberOfObjects+i]=distance_matrix[i*NumberOfObjects+j];
		}
		elm_i->pos.selfnormalize();
	}
}

void Thermal_engine::GetSun() {

	Planet = v->GetGravityRef();
	pl_radius = (float) oapiGetSize(Planet);
	v->GetRelativePos(Planet, ToPlanet);
	v->GetGlobalPos(ToSun);

	myr = _vector3(ToPlanet.x, ToPlanet.y, ToPlanet.z);
	sun = _vector3(ToSun.x, ToSun.y, ToSun.z);
	PlanetDistanceFactor = (pow(pl_radius, 2)) / myr.sqmod();
	
	float angle = (float) myr.angle(sun);	
	if (angle > asin(pl_radius / myr.mod())) 
		InSun = true;
	else 
		InSun = false;

	if (angle > PI / 2.0) 
		InPlanet = sin(angle - PI / 2.0) * PlanetDistanceFactor; // percentage of lighted from planet
	else 
		InPlanet = 0.0;
}

void Thermal_engine::Radiative(double dt) {

	GetSun();// need to convert the myr and sun vectors to local coordinates

	VECTOR3 LocalS;
	v->Global2Local(_V(ToSun.x / 2.0, ToSun.y / 2.0, ToSun.z / 2.0), LocalS);
	sun = _vector3(LocalS.x, LocalS.y, LocalS.z);
	sun.selfnormalize();

	char planetName[1000];
	bool planetIsSun = false;
	bool planetIsEarth = false;

	oapiGetObjectName(Planet, planetName, 255);

	if (!strcmp(planetName, "Sun")) planetIsSun = true;
	if (!strcmp(planetName, "Earth")) planetIsEarth = true;

	if (!planetIsSun) {
		VECTOR3 LocalR;
		v->Global2Local(_V(ToSun.x - ToPlanet.x,
  	  					   ToSun.y - ToPlanet.y,
						   ToSun.z - ToPlanet.z), LocalR);
		myr = _vector3(LocalR.x, LocalR.y, LocalR.z);
		myr.selfnormalize();
	}

	//Flux=q*T^4*Area;

	float q = (float) 5.67e-8;//Stefan-Boltzmann
	float Q = 0, Q0 = 0, Q1 = 0, Q2 = 0, Q3 = 0;
	therm_obj *runner;
	runner=List.next_t;

	while (runner) {
		if (planetIsEarth) {
			Q = (float) (190.0 * (runner->pos % myr) * PlanetDistanceFactor); //blank radiation from Earth
		} else {
			Q = 0;
		}
		Q0 = Q;
		if (Q < 0.0) Q = 0.0;
		
		Q1 = 0.0;
		if (InSun || planetIsSun) Q1 = (float) (1372.0 * (runner->pos % sun));	//we are not behind planet,
		if (Q1 > 0)	Q += Q1;

		Q2 = 0.0;
		if (!planetIsSun && InPlanet > 0) Q2 = (float) (300.0 * (runner->pos % myr) * InPlanet);  //300W from planet's albedo
		if (Q2 > 0) Q += Q2;

		Q3 = (float) (q * pow(runner->Temp - 3.0, 4));
		Q -= Q3;

		if (ObjToDebug && runner == ObjToDebug) 
			sprintf(oapiDebugString(), "Earth %.1f Sun %.1f Albedo %.1f Space %.1f Ges %.1f Temp %.1f", (Q0>0?Q0:0) * runner->Area * runner->isolation, (Q1>0?Q1:0) * runner->Area * runner->isolation, (Q2>0?Q2:0) * runner->Area * runner->isolation, -Q3 * runner->Area * runner->isolation, Q * runner->Area * runner->isolation, runner->GetTemp());

		runner->thermic(Q * runner->Area * dt * runner->isolation);
		runner=runner->next_t;
	}
}

void Thermal_engine::Conductive(double dt) {
}
