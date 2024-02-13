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

#include "Thermal.h"
#include <math.h>
#include <stdio.h>

/// \todo For testing
//extern FILE *PanelsdkLogFile;

therm_obj::therm_obj()
{
	next_t = NULL;
	c = 0.15;
	mass = 1.0;
	SetTemp(273.15);
	external = 0;
	isolation = 0.0;
	Area = 0.0;
	pos = _vector3(0, 0, 0);
	polar = directional;
}

void therm_obj::thermic(double _en) {

	if (-_en > energy)
		_en = -energy / 10.0;

	energy += _en; //total energy, in joules to add or substract

	Temp = energy / (c * mass);
}

void therm_obj::SetTemp(double _t) { 
	Temp = _t;
	energy = Temp * c * mass;
}

double therm_obj::GetTemp() {
	return Temp;
}

Thermal_engine::Thermal_engine() {
	v = nullptr;

	List.next_t = nullptr;
	NumberOfObjects = 0;
	distance_matrix = nullptr;
	InSun = 0;
	InPlanet = 0;

	PlanetRelPos = _V(1.0, 0.0, 0.0);
	PlanetRelPosNorm = _V(1.0, 0.0, 0.0);
	SunRelPos = _V(1.0, 0.0, 0.0); 
	SunRelPosNorm = _V(1.0, 0.0, 0.0);

	ObjToDebug = nullptr;

	PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoMercury] = 0.088;
	PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoVenus] = 0.76;
	PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoEarth] = 0.306;
	PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoMoon] = 0.11;
	PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoMars] = 0.25;
	PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoJupiter] = 0.503;
	PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoSaturn] = 0.342;

}

void Thermal_engine::Save(FILEHANDLE scn)
{};
void Thermal_engine::Load(FILEHANDLE scn)
{};

Thermal_engine::~Thermal_engine() {
	 
	if (distance_matrix) 
		delete[] distance_matrix;
}

therm_obj* Thermal_engine::AddThermalObject(therm_obj *n_obj, bool debug, therm_obj::thermalPolar ThermPolar) {
	
	therm_obj *runner;
	runner = &List;
	NumberOfObjects++;

	while (runner->next_t) {
		runner = runner->next_t;
	}

	runner->next_t = n_obj;
	n_obj->next_t = NULL;
	n_obj->polar = ThermPolar;

	if (debug) { 
		ObjToDebug = n_obj; 
	}

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


void Thermal_engine::Radiative(double dt){
	
	Planet = v->GetGravityRef();
	PlanetRadius = oapiGetSize(Planet);

	v->GetRotationMatrix(VesselRotationMatrix);
	v->GetGlobalPos(VesselPosition);
	oapiGetGlobalPos(Planet, &PlanetGlobalPos);
	SunRelPos = -VesselPosition;

	PlanetRelPos = PlanetGlobalPos - VesselPosition;
	PlanetRelPosNorm = unit(tmul(VesselRotationMatrix, PlanetRelPos));

	SunRelPosNorm = unit(tmul(VesselRotationMatrix, SunRelPos));


	PlanetDistanceFactor = (PlanetRadius * PlanetRadius) / length2(PlanetRelPos);
	double acosArg = dotp(PlanetRelPosNorm, SunRelPosNorm);
	if (acosArg > 1.0) { acosArg = 1.0; }
	if (acosArg < -1.0) { acosArg = -1.0; }
	double angle = acos(acosArg);
	double EclipseAngle;
	double PlanetDistance = length(PlanetRelPos);

	//VERY HELPFUL DEBUGGING CODE...
	//if (!strcmp(v->GetName(), "Eagle")) {
	//	sprintf(oapiDebugString(), "Sun <%lf %lf %lf> Planet <%lf %lf %lf>", SunRelPosNorm.x, SunRelPosNorm.y, SunRelPosNorm.z, PlanetRelPosNorm.x, PlanetRelPosNorm.y, PlanetRelPosNorm.z);
	//}

	if (PlanetRadius > PlanetDistance) {
		EclipseAngle = 90 * RAD;
	}
	else {
		EclipseAngle = asin(PlanetRadius / length(PlanetRelPos));
	}

	if (angle > EclipseAngle) {
		InSun = true;
	}
	else {
		InSun = false;
	}

	if (angle > PI / 2.0) {
		InPlanet = sin(angle - PI / 2.0) * PlanetDistanceFactor; // percentage of lighted from planet
	} 
	else {
		InPlanet = 0.0;
	}


	char planetName[16];
	bool planetIsSun = false;
	double PlanetaryBondAlbedo = 0.0;

	oapiGetObjectName(Planet, planetName, 16);

	double SolarFlux = 3.014607552E+25 / (length2(SunRelPos)); // W/m^2
	double PlanetIRFlux = 0.0;
	double DifferentialIR = 0.0;

	if (!strcmp(planetName, "Sun")) { 
		planetIsSun = true; 
	}
	else if (!strcmp(planetName, "Earth")) {
		PlanetaryBondAlbedo = PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoEarth];
		DifferentialIR = 0.15;
	}
	else if (!strcmp(planetName, "Moon")) {
		PlanetaryBondAlbedo = PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoMoon];
		DifferentialIR = 0.996830182;
	}
	else if (!strcmp(planetName, "Venus")) {
		PlanetaryBondAlbedo = PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoVenus];
		DifferentialIR = 0.15;
	}
	else if (!strcmp(planetName, "Mars")) {
		PlanetaryBondAlbedo = PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoMars];
		DifferentialIR = 0.15;
	}
	else if (!strcmp(planetName, "Jupiter")) {
		PlanetaryBondAlbedo = PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoJupiter];
		DifferentialIR = 0.15;
	}
	else if (!strcmp(planetName, "Saturn")) {
		PlanetaryBondAlbedo = PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoSaturn];
		DifferentialIR = 0.15;
	}
	else if (!strcmp(planetName, "Mercury")) {
		PlanetaryBondAlbedo = PlanetBondAlbedo[PlanetBondAlbedoIndex::rhoMercury];
		DifferentialIR = 0.15;
	}

	if (!planetIsSun) {
		PlanetIRFlux = (3.014607552E+25 / (length2(PlanetGlobalPos))) * (1 - PlanetaryBondAlbedo) / 4.0; // W/m^2
	}

	//https://tfaws.nasa.gov/wp-content/uploads/On-Orbit_Thermal_Environments_TFAWS_2014.pdf

	const double q = 5.67e-8;//Stefan-Boltzmann

	therm_obj *runner;
	runner=List.next_t;

	while (runner) {
		double Q = 0; //Flux=q*T^4*Area;
		double PlanetAlbedoRadiation = 0.0;
		double PlanetInfaredRadiation = 0.0;
		double SolarRadiation = 0.0;
		double SelfRadiation = 0.0;
		double AtmosphericConvection = 0.0;
		double SunIncidence, PlanetIncidence = 0.0;

		VECTOR3 SystemPosition = _V(runner->pos.x, runner->pos.y, runner->pos.z); // therm_obj::pos should eventually get converted over to a VECTOR3 and this line will not be necessary

		// SunIncidence and PlanetIncidence are scaled in the code below
		// because objects with a larger [than a flat plate] radiative coverage,
		// have an larger surface area comparted with the projected area of their shadow.

		if (runner->polar == therm_obj::directional) {
			SunIncidence = dotp(SystemPosition,SunRelPosNorm);
			PlanetIncidence = dotp(SystemPosition, PlanetRelPosNorm);

			if (SunIncidence < 0.0) { SunIncidence = 0.0; }
			if (PlanetIncidence < 0.0) { PlanetIncidence = 0.0; }
		}
		else if (runner->polar == therm_obj::cardioid) {
			double dotpSun = dotp(SystemPosition, SunRelPosNorm) + 1.0;
			if (dotpSun < 0.0) { dotpSun = 0.0; }
			SunIncidence = sqrt(dotpSun) / PI;
			
			double dotpPlanet = dotp(SystemPosition, PlanetRelPosNorm) + 1.0;
			if (dotpPlanet < 0.0) { dotpPlanet = 0.0; }
			PlanetIncidence = sqrt(dotpPlanet) / PI;
		}
		else if (runner->polar == therm_obj::subcardioid) {
			double dotpSun = dotp(SystemPosition, SunRelPosNorm) + 2.0;
			if (dotpSun < 0.0) { dotpSun = 0.0; }
			SunIncidence = sqrt(dotp(SystemPosition, SunRelPosNorm) + 2.0) / (2 + PI / 2.0);

			double dotpPlanet = dotp(SystemPosition, PlanetRelPosNorm) + 2.0;
			if (dotpPlanet < 0.0) { dotpPlanet = 0.0; }
			PlanetIncidence = sqrt(dotp(SystemPosition, PlanetRelPosNorm) + 2.0) / (2 + PI / 2.0);
		}
		else { //omni
			SunIncidence = 0.25;
			PlanetIncidence = 0.25;
		}
		
		if (InSun || planetIsSun) {
			SolarRadiation = SolarFlux * SunIncidence; //we are not behind planet,
		}	

		if (!planetIsSun) {
			PlanetInfaredRadiation = PlanetIRFlux * PlanetIncidence * PlanetDistanceFactor * (2 * InPlanet * DifferentialIR + (1 - DifferentialIR)); //infared radiation from the planet
			PlanetAlbedoRadiation = PlanetaryBondAlbedo * SolarFlux * PlanetIncidence * InPlanet;  //300W from planet's albedo
		}
		
		SelfRadiation = q * pow(runner->Temp - 2.7, 4);

		AtmosphericConvection = 100. * runner->Area * (runner->Temp - v->GetAtmTemperature())*v->GetAtmDensity() / 1.225;
		
		Q = SolarRadiation + PlanetAlbedoRadiation + PlanetInfaredRadiation - SelfRadiation - AtmosphericConvection;

		//if (ObjToDebug && (runner == ObjToDebug) && (!strcmp(v->GetName(), "Eagle"))) {
		//	sprintf(oapiDebugString(), "SolarRadiation %fW/m², PlanetAlbedoRadiation %fW/m², PlanetInfaredRadiation %fW/m², SelfRadiation %fW/m², AtmosphericConvection %fW/m², Temp %lfK, Angle %lf° %lf %lf",
		//		SolarRadiation, PlanetAlbedoRadiation, PlanetInfaredRadiation, - SelfRadiation, - AtmosphericConvection, runner->GetTemp(), angle, PlanetRadius, length(PlanetRelPos));
		//}

		runner->thermic(Q * runner->Area * dt * runner->isolation);
		runner = runner->next_t;
	}
}

void Thermal_engine::Conductive(double dt) {
}
