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

#ifndef __HSYSTEMS_H
#define __HSYSTEMS_H

#include "DataTables.h"
#include "Thermal.h"
// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

//base class for hydraulic objects
class h_substance
{ public:
    h_substance() {subst_type = 0; mass = 0; vapor_mass = 0; Q = 0;};
	h_substance(int i_subst_type, double i_mass, double i_Q, float i_vapor_mass);
	int subst_type;					//index of substance type
	double mass;					// (gr)
	double Q;						// (J) total enthalpy (NOT thermal energy)
	double vapor_mass;				// mass(gr) of this block that exists in vapour state

	//these two are not propagated through block maneuvers..
	//instead they are re-computed by generating parent
	double p_press;					// partial pressure (Pa), computed using current Q ..
	double Temp;					// temp of this substance, again, based on Q

	void operator+= (h_substance);	//add some block to this..
	h_substance operator* (float);	//returns a subst block that is "Ratio" part of the main (ie. 0.5 will generate half of the block)
	void operator-= (h_substance);  //substact this block from itself
	double VAPENTH() const;
	double GET_LIQUID_DENSITY(const int SUBSTANCE_TYPE, const double temperature) const;
	double GET_BULK_MOD(const int SUBSTANCE_TYPE, const double temperature, const double pressure) const;
	double Condense(double dt);
	double Boil(double dt);
	double BoilAll();
	void SetTemp(double _temp);
};

class h_volume
{public:
	h_volume();

	h_substance composition[MAX_SUB]; //all the substances can co-exist :)
	int max_sub;						//number of substance present in the volume

	void operator+=(h_volume);		//add two volumes together
	void operator+=(h_substance);	//or simply add some sub. to the volume
	h_volume Break(double vol, int* mask, double maxMass = 0);		//break 'vol' liters from the volume ..into another volume
	void GetMaxSub();				//re-computes number of substances present in the volume
	double GetMass();				//total mass inside the volume
	double GetQ();
	double Q;
	double total_mass;
	double Press;					//total press.. sum of all p_press (Pa)
	double Temp;					//averaged temp of volume.. (K)
	double Volume;					//liters
	void ThermalComps(double dt);	//levels temp throughout all subst...much like stirring a tank,only instanaeously
	void Void();					//empty all inside the volume
};
h_substance _substance(int s_type,double i_mass, double i_Q,float i_vm);
class H_system;
class h_object:public ship_object				//:public therm_obj
{ 
public:
	h_object();
	H_system *parent;
	virtual void ProcessShip(VESSEL *vessel,PROPELLANT_HANDLE ph){ };
};

class E_system;

//all the objects form a system, basically a chained list
class H_system:public ship_system
{
	void Create_h_Tank(char *line);
	void Create_h_Pipe(char *line);
	void Create_h_Vent(char *line);
	void Create_h_Radiator(char *line);
	void Create_h_crew(char *line);
	void Create_h_HeatExchanger(char *line);
	void Create_h_Evaporator(char *line);
	void Create_h_MixingPipe(char *line);
	void Create_h_Valve(char *line);
	void Create_h_CO2Scrubber(char *line);
	void Create_h_WaterSeparator(char *line);
	void Create_h_HeatLoad(char *line);
	void Create_h_Accumulator(char* line);
	void Create_h_ExteriorEnvironment();
	void Create_h_ExteriorVentPipe(char* line);

public:

	E_system* P_electric;
	void* GetPointerByString(char *query);
	void Load (FILEHANDLE scn);
	void Save (FILEHANDLE scn);
	void Build();
	void ProcessShip(VESSEL *vessel, PROPELLANT_HANDLE ph);
private:
	bool ExteriorEnvironmentCreated = false;
};

class h_Tank;

class h_Valve : public ship_object
{
public:
	h_Valve();
	h_Valve(char *i_name, int i_open,int i_ct,float i_size, h_Tank* i_parent);
	void Set(int i_open,int i_ct,float i_size,h_Tank* i_parent);
	h_Tank* parent;		//pointer to the tank to which they belong
	int open;
	int closing_time;	//time in sec to close the valve
	float pz;			//pz is used as "pozition" of the closing valve,0 is stationary
	int h_open;			//handle for switches
	float size;			//grams / second at full open..
	void Close();		//for autmation,
	void Open();
	double GetPress();	//press is used by Pipe to compute flow
	double GetTemp();
	void thermic(double _en);
	int Flow(h_volume block);//block of substance flowing INTO  the valve
	h_volume GetFlow(double dPdT, double maxMass = 0);//deltaP * deltaT gives us flow rate OUTOF(in volume)
	virtual void refresh(double dt);	//for open/close updating
	virtual void Save(FILEHANDLE scn);
	virtual void* GetComponent(char *component_name);
};

class h_Tank : public h_object, public therm_obj {	//tanks is just a basic receptacle of liquid or gas..

public:
	int IN_FLOW_MASK[MAX_SUB];			//what is allowed in and out..
	int OUT_FLOW_MASK[MAX_SUB];			//kinda of a hack, I know, but very customizable this way

	h_volume space;						//tank actual space
	h_Valve IN_valve;
	h_Valve OUT_valve;
	h_Valve OUT2_valve;
	h_Valve LEAK_valve;
	double Original_volume;

	h_Tank(char *i_name,vector3 i_p,double i_vol);	//create a room of i_vol liters at i_p position (assume sphere )
	virtual ~h_Tank();
	virtual	void refresh(double dt);	//this called at each timestep
	virtual int Flow(h_volume block);
	h_volume GetFlow(double volume, double maxMass = 0);	//flow from a tank is defined in volume
	virtual void thermic( double _en);  //tank has it's own thermic function, to account for the h_volume
	virtual void Load(FILEHANDLE scn);
	virtual void Save(FILEHANDLE scn);
	virtual void* GetComponent(char *component_name);
	virtual therm_obj* GetThermalInterface(){return (therm_obj*)this;};

	void BoilAllAndSetTemp(double _t);	//This is a hack and should be used only in special cases. Violates energy conservation	

	void operator +=(h_substance);
};


class h_Pipe : public h_object {	//pipes are the connections between valves!!

public:
	int type;
	int two_ways;
	double P_max;
	double P_min;
	int open;
	h_Valve *in;
	h_Valve *out;
	double flow;	// in g/s
	double flowMax;

	h_Pipe(char *i_name, h_Valve *i_IN, h_Valve *i_OUT, int i_type, double max, double min, int is_two, double maxFlow);
	virtual	void refresh(double dt);	//this called at each timestep
	virtual void* GetComponent(char *component_name);
	void BroadcastDemision(ship_object * gonner);
	virtual void Save(FILEHANDLE scn);
};

class h_Vent: public h_Tank
{public:
	h_Vent(char *i_name,vector3 i_p);
	virtual ~h_Vent();
	void AddVent(vector3 i_pos,vector3 i_dir,double i_size);
	void ProcessShip(VESSEL *vessel,PROPELLANT_HANDLE ph);
	virtual int Flow(h_volume block);
	vector3 pos[4];
	vector3 dir[4];
	double size[4];
	PROPELLANT_HANDLE ph_vent;
	THRUSTER_HANDLE thg[4];
	VESSEL *v;
	int Num_Vents;
};

class h_Radiator : public h_object, public therm_obj {

public:
    h_Radiator(char *i_name,vector3 i_p,double i_size,double i_rad);
	~h_Radiator();
	double size;
	double rad;
	virtual	void refresh(double dt);	//this called at each timestep
	virtual void* GetComponent(char *component_name);
	virtual therm_obj* GetThermalInterface(){ return (therm_obj*)this; };
	virtual void Save(FILEHANDLE scn);

	double AirHeatTransferCoefficient;

protected:
	double Qc, Qr;
	double AirTemp;
};

class h_HeatExchanger : public h_object {

public:
    h_HeatExchanger(char *i_name, int i_pump, double i_length, therm_obj *i_source, therm_obj *i_target, double i_tempMin, double i_tempMax);
	int h_pump;
	double length;
	double tempMin;
	double tempMax;
	therm_obj *source;
	therm_obj *target;
	double power;
	bool bypassed;

	void SetPumpOn() {h_pump = -1; };
	void SetPumpOff() {h_pump = 0; };
	void SetPumpAuto() {h_pump = 1; };
	void SetLength(double l) {length = l; };
	void SetBypassed(bool b) {bypassed = b; };

	virtual	void refresh(double dt);	//this called at each timestep
	virtual void* GetComponent(char *component_name);
	virtual void Save(FILEHANDLE scn);
};

class h_Evaporator : public h_object {

public:
    h_Evaporator(char *i_name, int i_pump, therm_obj *i_target, double i_targetTemp, h_Valve *i_liquidSource, double i_tempTurnOn, therm_obj *i_tempControl);
	int h_pump;
	int h_valve;
	double throttle;
	double steamPressure;
	therm_obj *target;			// target to be cooled
	double targetTemp;
	h_Valve *liquidSource;	// source for liquid to evaporate

	therm_obj *tempControl;		// Turn on evaporator if temperature of tempControl
	double tempTurnOn;			// greater than tempTurnOn

	void SetPumpOn() {h_pump = -1; };
	void SetPumpOff() {h_pump = 0; };
	void SetPumpAuto() {h_pump = 1; };

	virtual	void refresh(double dt);	// this called at each timestep
	virtual void* GetComponent(char *component_name);
	virtual void Save(FILEHANDLE scn);
};

class h_MixingPipe : public h_object {

public:
    h_MixingPipe(char *i_name, int i_pump, h_Valve *i_in1, h_Valve *i_in2, h_Valve *i_out, double i_targetTemp);
	int		h_pump;
	double	targetTemp;
	h_Valve *in1;	
	h_Valve *in2;	
	h_Valve *out;	
	double ratio;

	void SetPumpOn() {h_pump = -1; };
	void SetPumpOff() {h_pump = 0; };
	void SetPumpAuto() {h_pump = 1; };

	virtual	void refresh(double dt);	//this called at each timestep
	virtual void* GetComponent(char *component_name);
	virtual void Save(FILEHANDLE scn);
};

class h_crew : public h_object {

public:
	h_crew(char *i_name, int nr, h_Tank *i_src, h_Pipe *i_pipe, h_Tank *i_urine);
	h_Tank *SRC, *UCD;
	h_Pipe *drinkpipe;
	int number;
	virtual	void refresh(double dt);	//this called at each timestep
	virtual void* GetComponent(char *component_name);
	virtual void Save(FILEHANDLE scn);
};

class h_CO2Scrubber : public h_object {

public:
	h_CO2Scrubber(char *i_name, double i_flowmax, h_Valve *in_v, h_Valve *out_v);

	h_Valve* in;
	h_Valve* out;

	virtual void refresh(double dt);
	virtual void* GetComponent(char *component_name);

	double co2removalrate;
	double flow;	// in g/s
	double flowMax;
};

class h_WaterSeparator : public h_object {

public:
	h_WaterSeparator(char *i_name, double i_flowmax, h_Valve *in_v, h_Valve *out_v, h_Valve *i_H2Owaste);

	h_Valve* in;
	h_Valve* out;
	h_Valve* H20waste;

	virtual void refresh(double dt);
	virtual void* GetComponent(char *component_name);
	virtual void Save(FILEHANDLE scn);

	double h2oremovalrate;
	double h2oremovalratio;
	double flow;	// in g/s
	double flowMax;
	double RPM;
	double rpmcmd;
};

class h_HeatLoad : public h_object {

public:
	h_HeatLoad(char *i_name, therm_obj *i_target);

	therm_obj *target;

	double heat_load;

	void GenerateHeat(double watts);
	virtual void refresh(double dt);
	virtual void* GetComponent(char *component_name);
};

class h_Accumulator : public h_Tank {
public:

	h_Accumulator(char* i_name, vector3 i_p, double i_vol);
	void refresh(double dt);

};

///
/// \ingroup PanelSDK
/// The purpose of this object is to simulate the exterior environment surrounding the vessel
/// so that internal systems objects can realistically simulate fluid interactions through exterior connections.
/// Exactly one instance of this class should get created per vessel. This is done by PanelSDK before the systems
/// config files are parsed so that this object is avaliable to other h_Objects at the time of parsing.
/// 
/// Connections than be do the exterior environment like any other tank, by means of a pipe connecting to
/// EXTERIOR:IN, EXTERIOR:OUT etc. The name of this object will always be "EXTERIOR". Connections to this object are also
/// avaliable through the "Vent" class.
/// 
/// Principal of Operation.
/// The internal state of the ExteriorEnvironment is simulated exactly as in the h_Tank class (h_ExteriorEnvironment derives
/// from h_Tank). Once per systems timestep, h_ExteriorEnvironment calls GetAtmDensity() from the vessel to which the h_ExteriorEnvironment
/// instance is attached.
///
class h_ExteriorEnvironment : public h_Tank
{
public:
	h_ExteriorEnvironment(char* i_name, vector3 i_p, double i_vol) : h_Tank(i_name, i_p, i_vol) {};
	virtual ~h_ExteriorEnvironment();
	virtual void refresh(double dt);
private:
	enum body
	{
		None,
		Earth,
		Mars,
		num_bodies
	};

	const double compositionRatio[num_bodies][MAX_SUB] = {

		//None
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},

		//Earth
		{
			0.20947,		//O2
			0.0,			//H2
			0.00934,		//H2O
			0.78084,		//N2
			0.00035,		//CO2
			0.0,			//Glycol
			0.0,			//Aerozine
			0.0,			//N204
			0.0				//He
		},

		//Mars
		{
			0.001,			//O2
			0.0,			//H2
			0.0,			//H2O
			0.019,			//N2
			0.98,			//CO2
			0.0,			//Glycol
			0.0,			//Aerozine
			0.0,			//N204
			0.0				//He
		}
	};
};


///
/// \ingroup PanelSDK
/// This object is a replacement for the obsolete h_Vent class. It is used to create a fluid
/// connection between an h_Tank, and h_ExteriorEnvironment
///
class h_ExteriorVentPipe : public h_Pipe
{
public:
	h_ExteriorVentPipe(char* i_name, h_Valve* i_IN, h_Valve* i_OUT, int i_type, double max, double min, int is_two);
	virtual ~h_ExteriorVentPipe();
	void AddVent(VECTOR3 i_pos, VECTOR3 i_dir, double i_size);
	void ProcessShip(VESSEL* vessel, PROPELLANT_HANDLE ph);
	virtual void* GetComponent(char* component_name);
private:
	virtual int Flow(h_volume block);
	VECTOR3 pos[4];
	VECTOR3 dir[4];
	double size[4];
	PROPELLANT_HANDLE ph_vent;
	THRUSTER_HANDLE thg[4];
	VESSEL* v;
	int Num_Vents;
};

#endif
