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

#ifndef __PANELSDK_H_
#define __PANELSDK_H_

#include <stdio.h>
#include "Orbitersdk.h"


#define PANELSDK_VERSION	 "1.9.10"

#define SP_VALVE_NONE		 0
#define SP_VALVE_OPEN		 1
#define SP_VALVE_CLOSE		-1

#define SP_PUMP_OFF			 0
#define SP_PUMP_ON			-1
#define SP_PUMP_AUTO		 1

#define SP_FUELCELL_NONE	 0
#define SP_FUELCELL_START	 1
#define SP_FUELCELL_STOP	-1
#define SP_FUELCELL_NOPURGE	-1
#define SP_FUELCELL_H2PURGE	 1
#define SP_FUELCELL_O2PURGE	 2

#define SP_MIN_DCVOLTAGE	20.0
#define SP_MIN_ACVOLTAGE	100.0

class VESSEL;
class E_system;
class H_system;
class Thermal_engine;
class VesselMgmt;
class e_object;
class h_object;
class therm_obj;

///
/// \ingroup PanelSDK
/// The main Panel SDK class.
///
class PanelSDK {

public:

	PanelSDK();
	~PanelSDK();
	void RegisterVessel(VESSEL *vessel);
    void *GetPointerByString(char *query);

	void InitFromFile(char *FileName);

	void Timestep(double time);
	void SimpleTimestep(double simdt);

	void AddElectrical(e_object *e, bool can_delete);
	void AddHydraulic(h_object *h);
	void AddThermal(therm_obj *t);

	void Load(FILEHANDLE scn);
	void Save(FILEHANDLE scn);


private:
	VESSEL* v;

	E_system *ELECTRIC;
	H_system *HYDRAULIC;
	Thermal_engine *THERMAL;
    VesselMgmt *VESSELMGMT;

	double lastTime;
	bool firstTimestepDone;
};

#endif
