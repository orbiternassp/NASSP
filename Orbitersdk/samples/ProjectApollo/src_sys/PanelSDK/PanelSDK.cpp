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

#include "PanelSDK.h"
#include "Internals/Thermal.h"
#include "Internals/Hsystems.h"
#include "Internals/Esystems.h"
#include "VSMGMT.H"

PanelSDK::PanelSDK() {

	ELECTRIC = new E_system;
	HYDRAULIC = new H_system;
	THERMAL = new Thermal_engine;
	VESSELMGMT = new VesselMgmt;
	HYDRAULIC->P_thermal = THERMAL;
	HYDRAULIC->P_electric = ELECTRIC;
	ELECTRIC->P_thermal = THERMAL;
	ELECTRIC->P_hydraulics = HYDRAULIC;

	lastTime = 0;
	firstTimestepDone = false;
}

PanelSDK::~PanelSDK(){
	
	delete ELECTRIC;
	delete HYDRAULIC;
	delete THERMAL;
	delete VESSELMGMT;
}

void PanelSDK::RegisterVessel(VESSEL* vessel){
	
	v = vessel;
	VESSELMGMT->vs = vessel;
	ELECTRIC->Vessel = vessel;
	HYDRAULIC->Vessel = vessel;
	THERMAL->v = vessel;
}

void PanelSDK::Load(FILEHANDLE scn){

	char* line, buffer[100];
	bool dontLoad;

	// check version 
	dontLoad = true;
	oapiReadScenario_nextline(scn, line);
	if (!strnicmp(line, "<VERSION>", 9)) {
		sscanf(line + 9, "%s", buffer);
		if (!strnicmp(buffer, PANELSDK_VERSION, strlen(PANELSDK_VERSION)))
			dontLoad = false;
	}

	oapiReadScenario_nextline(scn, line);
	if (dontLoad) {
		while (strnicmp(line, "</INTERNALS>", 12)) {
			oapiReadScenario_nextline(scn, line);
		}
		return;
	}

	while (strnicmp(line, "</INTERNALS>", 12)) {
		if (!strnicmp(line, "<HYDRAULIC>", 11))
			HYDRAULIC->Load(scn);
		else if (!strnicmp(line, "<ELECTRIC>", 10))
			ELECTRIC->Load(scn);

		oapiReadScenario_nextline(scn, line);
	}
}

void PanelSDK::Save(FILEHANDLE scn) {

	oapiWriteScenario_string(scn, "<INTERNALS>", "");
	oapiWriteScenario_string(scn, "<VERSION>", PANELSDK_VERSION);

	HYDRAULIC->Save(scn);
	ELECTRIC->Save(scn);

	oapiWriteScenario_string(scn, "</INTERNALS>", "");
}

void PanelSDK::Timestep(double time){
	
	if (!firstTimestepDone) {
		lastTime = time;
		firstTimestepDone = true;
		return;
	}

	double dt = time - lastTime;
	lastTime = time;

	double mintFactor = __max(dt / 100.0, 0.5);
	double tFactor = __min(mintFactor, dt);
	while (dt > 0) {
		THERMAL->Radiative(tFactor);
		HYDRAULIC->Refresh(tFactor);
		ELECTRIC->Refresh(tFactor);

		dt -= tFactor;
		tFactor = __min(mintFactor, dt);
	}
}

void PanelSDK::SimpleTimestep(double simdt){
	THERMAL->Radiative(simdt);
	HYDRAULIC->Refresh(simdt);
	ELECTRIC->Refresh(simdt);
}


void PanelSDK::AddElectrical(e_object* e, bool can_delete){
	ELECTRIC->AddSystem(e);
	e->deletable = can_delete;
}

void PanelSDK::AddHydraulic(h_object* h){
	HYDRAULIC->AddSystem(h);
}

void PanelSDK::AddThermal(therm_obj* t){
	THERMAL->AddThermalObject(t, false);
}
