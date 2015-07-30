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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.1  2009/02/18 23:22:01  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.9  2008/04/11 11:50:26  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.8  2006/11/13 14:47:34  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.7  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.6  2006/06/21 12:52:31  tschachim
  *	Bugfix.
  *	
  *	Revision 1.5  2006/01/08 21:43:34  movieman523
  *	First phase of implementing inverters, and stopped PanelSDK trying to delete objects which weren't allocated with new().
  *	
  *	Revision 1.4  2005/11/18 02:40:54  movieman523
  *	Major revamp of PanelSDK electrical code, and various modifications to run off fuel cells.
  *	
  *	Revision 1.3  2005/09/30 11:18:48  tschachim
  *	New components for the water-glycol cooling.
  *	
  *	Revision 1.2  2005/05/02 13:04:25  tschachim
  *	only code formatting
  *	
  *	Revision 1.1  2005/04/22 14:22:38  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef __THERMAL_H_
#define __THERMAL_H_

#include "../matrix.h"
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"

class therm_obj			//thermal object.an object that can receive thermal energy
{ public:

  therm_obj();
  therm_obj *next_t;			//the objects are linked..
  int external;				//1/0 is this object radiating heat into outerspace
  double energy;			//Q ,or termic energy in Joules ??
  double c;					//c - material constant in J/gr*K
  double isolation;			//0..1 isolation factor
  double Area;
  vector3 pos;				//position in ship (in centimeters.. 0,0,0 is offset from GC
  double  mass;				//total mass , in grams
  double Temp;				//duh!, in K (default constrctor =273+ 12
  virtual void thermic( double _en);//thermic function.. negative values, if this looses energy
  void SetTemp(double _t);	//this is a hack, shouldn't be used. Violates energy conservation
  double GetTemp();			//get temp
};

class Thermal_engine  //main thermal parent. Handles most thermic problems
{
public:
  Thermal_engine();		//basic constructor
  ~Thermal_engine();	//basic destructor

  void InitThermal();	//builds a bunch of tables we'll need at runtime

  void GetSun();
  void Conductive(double dt);	//runs the conductive calculations inbetween the thermal objects
  void Radiative(double dt);	//- II -   radiative   - II -, only for external objects..

  float *distance_matrix;	//table holding distances from item x to item y
  int NumberOfObjects;		//in thr engine
  therm_obj List;
  therm_obj* AddThermalObject(therm_obj *n_obj, bool debug = false);
  void RemoveThermalObject(therm_obj *n_obj);
  therm_obj* GetElement(int i);
  void Save(FILEHANDLE scn);
  void Load(FILEHANDLE scn);

  VESSEL *v;
  OBJHANDLE Planet;
  float pl_radius;
  vector3 myr;
  vector3 sun;
  VECTOR3 ToPlanet;
  VECTOR3 ToSun;
  int InSun;
  double InPlanet;
  double PlanetDistanceFactor;

  therm_obj* ObjToDebug;
};

///
/// \ingroup PanelSDK
/// The generic ship object base class.
///
class ship_object
{ 
public:
	ship_object() { name[0] = '\0'; max_stage = 0; next = NULL; deletable = true; };

	///
	/// \brief object name.
	///
	char name[100];
	int max_stage;

	///
	/// \brief next object in linked list.
	///
	ship_object *next;

	virtual ~ship_object(){};
	virtual void refresh(double dt);

	///
	/// Each object is passed the line loaded from the file. If it's a line holding state for
	/// that object, it should extract the state from the line and update its internal variables.
	///
	/// \brief Load state from file.
	/// \param scn The scenario file to read from.
	///
	virtual void Load(FILEHANDLE scn);

	///
	/// Each class will save its state to the scenario file as a single line, typically starting with
	/// the object name.
	///
	/// \brief Save state to file.
	/// \param scn The scenario file to save state to.
	///
	virtual void Save(FILEHANDLE scn);

	///
	/// Returns a pointer to a component of the system: for example, a valve in a pipe.
	///
	/// \brief Get a system component by name.
	/// \param component_name Name of the component (e.g. "VALVE")
	///
	virtual void* GetComponent(char *component_name);
	virtual void BroadcastDemision(ship_object * gonner){};
	virtual therm_obj* GetThermalInterface(){return NULL;};
	virtual void UpdateFlow(double dt) { };

	///
	/// Specifies whether the object was allocated with new(), in which case it's
	/// deletable, or allocated statically, in which case it's not.
	///
	/// \brief Is the object deletable?
	///
	bool deletable;
};

class ship_system
{ public:
	ship_object List;
	ship_system();
	~ship_system();

	Thermal_engine *P_thermal;
	VESSEL* Vessel;

	ship_object* AddSystem(ship_object *object);
	void DeleteSystem(ship_object *object);
	void BroadcastDemision(ship_object * gonner);
	void SetMaxStage(char *name, int stage);
	void ConfigStage(int stage);
	ship_object* GetSystemByName(char *r_name);
	virtual void* GetPointerByString(char *query);
	virtual void Refresh(double dt);
	virtual void Load (FILEHANDLE scn)=0;
	virtual void Save (FILEHANDLE scn)=0;
	virtual void Build()=0;
};
#endif