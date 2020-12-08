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

#ifndef __ESYSTEMS_H_
#define __ESYSTEMS_H_

#include "thermal.h"
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "hsystems.h"

///
/// \ingroup PanelSDK
/// The generic electrical object class.
///
class e_object:public ship_object
{ 
public:
    e_object *SRC; //for loading

	e_object();

	///
	/// Each timestep the object should call this function to tell the Panel SDK
	/// how much power it's drawing. This power drain is fed back through the
	/// electrical systems to the original power sources.
	///
	/// \brief Draw power from electrical supply.
	/// \param watts Amount of power to draw in watts.
	///
	virtual void DrawPower(double watts);

	///
	/// Thifs function is now obsolete: it was used in the old Panel SDK code to reduce
	/// the power load on an electrical system. It should be removed.
	///
	/// \deprecated
	/// \brief Old function to reduce power load (OBSOLETE).
	///
	virtual void PUNLOAD(double watts);

	///
	/// Obsolete function to wire this object to another electrical source. Use WireTo()
	/// instead.
	///
	/// \deprecated
	/// \brief Wire this object to another electrical source (OBSOLETE).
	/// \param new_src Electrical source to wire us to.
	///
	virtual void connect(e_object *new_src);

	///
	/// Called each timestep to allow the object to refresh its state and decide how
	/// much power to draw in the next timestep.
	///
	/// \brief Refresh power draw.
	/// \param dt Time in seconds since last timestep.
	///
	virtual void refresh(double dt);

	///
	/// Each object is passed the line loaded from the file. If it's a line holding state for
	/// that object, it should extract the state from the line and update its internal variables.
	///
	/// \brief Load state from file.
	/// \param line The line read from the scenario file.
	///
	virtual void Load(char *line);

	///
	/// Each class will save its state to the scenario file as a single line, typically starting with
	/// the object name.
	///
	/// \brief Save state to file.
	/// \param scn The scenario file to save state to.
	///
	virtual void Save(FILEHANDLE scn);

	virtual void* GetComponent(char *component_name);

	///
	/// This is called each timestep to allow the object to update state based on power flow over
	/// time, for example a battery to reduce the stored energy by the amount that is being drawn
	/// from it, and the internal resistance.
	///
	/// \brief Update power flow.
	/// \param dt Timestep duration in seconds.
	///
	virtual void UpdateFlow(double dt);

	///
	/// Electrical objects are arranged in a chain from a true electrical source (e.g.
	/// fuel cell or battery) to the electrical power users. Each one is wired to the
	/// next until it eventually gets to NULL (in which case the entire chain is unpowered)
	/// or a fuel cell or battery, in which case the chain pulls power from that source.
	///
	/// \brief Wire this object to another electrical source.
	/// \param p Electrical source to wire us to.
	///
	virtual void WireTo(e_object *p) { SRC = p; };

	///
	/// \brief Get the voltage.
	/// \return Voltage in volts.
	///
	virtual double Voltage();

	///
	/// \brief Get the current flow.
	/// \return Current flow in Amps.
	///
	virtual double Current();

	///
	/// \brief Get the frequency.
	/// \return Frequency in Hertz.
	///
	virtual double Frequency();

	///
	/// \brief Get the current power load.
	/// \return Power load in Watts.
	///
	virtual double PowerLoad();

	///
	/// \brief Are we wired to a power source?
	/// \return True if we're wired to another source, false if we're not.
	///
	virtual bool IsWired() { return (SRC != 0); };

	///
	/// \brief Disable this object.
	///
	void Disable() { enabled = false; };

	///
	/// \brief Enable this object.
	///
	void Enable() { enabled = true; };

	///
	/// \brief Is this object enabled?
	/// \return True if enabled.
	///
	bool IsEnabled() { return enabled; };

protected:
	///
	/// \brief Is this object enabled?
	///
	bool enabled;

    double Amperes; //status
	double Volts;   //
	double Hertz;
	double power_load;	//how much do we need to produce
};

class E_system : public ship_system {

	void Create_Battery(char *line);
	void Create_FCell(char *line);
	void Create_DCbus(char *line);
	void Create_ACbus(char *line);
	void Create_Socket(char *line);
	void Create_Cooling(char *line);
	void Create_AtmRegen(char *line);
	void Create_Boiler(char *line);
	void Create_Pump(char *line);
	void Create_Inverter(char *line);

public:
	E_system();
	~E_system();
	H_system *P_hydraulics;
    void* GetPointerByString(char *query);
	void Load(FILEHANDLE scn);
	void Save(FILEHANDLE scn);
	void Build();
	void Refresh(double dt);
};

class Socket:public e_object
{public:
  e_object* TRG[4];
  e_object* SRC;
  int socket_handle;
  int curent;
  Socket(char *i_name,e_object *i_src,int i_p,e_object *tg1,e_object *tg2,e_object *tg3);
  void refresh(double dt);
  virtual void Load (char *line);
  virtual void Save (FILEHANDLE scn);
  virtual void* GetComponent(char *component_name);
  virtual void BroadcastDemision(ship_object * gonner);

};

///
/// \ingroup PanelSDK
/// The fuel cell simulation class.
///
class FCell : public e_object, public therm_obj {

public:
	h_Valve *O2_SRC;	//source for O2
	h_Valve *H2_SRC;	//source for H2
	h_Valve *H20_waste;	//pointer to a waste tank

	unsigned int numCells = 31;

	double H2_flow, O2_flow, H2_flowPerSecond, O2_flowPerSecond;
	h_volume h2o_volume;

    float max_power;	//in watts;
	double clogg;		//not used yet? need to purge the fcell
	double reaction;		//is it taking place?
	double reactant;
	int start_handle;	//start, stop
	int purge_handle;	// -1-no purging 1-H2 purge 2-O2 purge
	int status;			//what are we doing? 0-running, 1-starting, 2-stopped, 3-H2 purging , 4-O2 purging
	double running;		//for tb indicators only;they need a float
	double condenserTemp;	// condenser exhaust temp, only for display
	int tempTooLowCount; 
	double outputImpedance;

	double H2_clogging;
	double O2_clogging;

	double H2_max_impurities;
	double O2_max_impurities;

	double H2_purity;
	double O2_purity;

	const double hydrogenHHV = 1.482; //volts (per cell), 286kJ/mol /(2 mols * Faraday's constant)
	const double hydrogenLHV = 1.254; //volts (per cell), 241kJ/mol /(2 mols * Faraday's constant)

	FCell(char *i_name, int i_status, vector3 i_pos, h_Valve *o2, h_Valve *h2, h_Valve* waste, float r_watts);
	void DrawPower(double watts);
	void PUNLOAD(double watts);
	void refresh(double dt);
	void Reaction(double dt, double thrust);
	void Clogging(double dt);
	void Load(char *line);
	void Save(FILEHANDLE scn);
	void* GetComponent(char *component_name);
	therm_obj* GetThermalInterface() { return (therm_obj*)this; };
	void UpdateFlow(double dt);
};

///
/// \ingroup PanelSDK
/// The battery simulation class.
///
class Battery:public e_object,public therm_obj
{  //battery is a producer / consumer
public:
	Battery(char *i_name,e_object *i_src, double i_power, double i_voltage, double i_resistance);

	void UpdateFlow(double dt);
	virtual void DrawPower(double watts);
	virtual void PUNLOAD(double watts);
	virtual void refresh(double dt);
    virtual void Load(char *line);
	virtual void Save(FILEHANDLE scn);
	void* GetComponent(char *component_name);
	double Voltage();
	double Current();
	double Capacity() { return power; };
	virtual therm_obj* GetThermalInterface(){return (therm_obj*)this;};

    double max_power; // in Watt * second
	double power;   //in Watt * second
	double max_voltage; // at zero current
	double internal_resistance; // in Ohm
};

///
/// \ingroup PanelSDK
/// The DC bus simulation class.
///
class DCbus: public e_object
{  

public:
	// We'll assume the bus is about 95% efficient.
	DCbus(char *name, e_object *i_SRC, double lossFact = 1.05);

	void DrawPower(double watts);
	void PUNLOAD(double watts);
	void Disconnect();
	void refresh(double dt);
	void Load(char *line);
	void Save(FILEHANDLE scn);
	void BroadcastDemision(ship_object * gonner){if (SRC==gonner) {SRC=NULL;};};
	double Current();

protected:
	double lossFactor;
};

///
/// \ingroup PanelSDK
/// The single-phase AC output simulation class.
///
class ACPhaseOutput: public e_object {
public:
	ACPhaseOutput();
   	void DrawPower(double watts);
	double Current();
};

///
/// \ingroup PanelSDK
/// The AC bus simulation class.
///
class ACbus: public e_object
{ 
public:
	ACbus(char *name, double i_voltage, e_object *i_SRC);

   	void DrawPower(double watts);
	void PUNLOAD(double watts);
	void connect(e_object *new_src);
	void refresh(double dt);
	void Load(char *line);
	void Save(FILEHANDLE scn);
	void BroadcastDemision(ship_object * gonner){if (SRC==gonner) {SRC=NULL;};};
	double Current();
	double Voltage();

protected:
	double ac_voltage;
};

///
/// \ingroup PanelSDK
/// The AC inverter simulation class.
///
class ACInverter: public e_object
{ 
public:
	ACInverter(char *name, double i_voltage, e_object *i_SRC);

   	void DrawPower(double watts);
	void connect(e_object *new_src);
	void refresh(double dt);
	void Load(char *line);
	void Save(FILEHANDLE scn);
	void UpdateFlow(double dt);
	double Current();
	double Voltage();
	void ResetOverload() { overload_tripped = false; overload_check_time = 0.0; };
	bool Overloaded() { return overload_tripped; };
	double get_epw(double base_epw_factor, double SourceVoltage);     // Get an entry from the EPW table
	double calc_epw_util(double maxw,int index,double SourceVoltage); // Utility function used to simplify the above

	double BASE_EPW[30]; // Base efficiency per watt factors, calculated from NASA docs
	double EPW_PV[30];   // Additional inefficiency per volt, calculated from NASA docs

	ACPhaseOutput PhaseA;
	ACPhaseOutput PhaseB;
	ACPhaseOutput PhaseC;

protected:
	double ac_voltage;
	bool overload_tripped;

	bool last_overload_check;
	double overload_check_time;
};

///
/// \ingroup PanelSDK
/// The cooling system simulation class.
///
class Cooling: public e_object {

public:
	Cooling(char *i_name,int i_pump,e_object *i_SRC,double thermal_prop,double min_t,double max_t);
	~Cooling();
	therm_obj* list[16];	//the list of objects
	double length[16];	//and their pipe length
	bool bypassed[16];	// and are they bypassed 
	int nr_list;
	double coolant_temp[16];
	double isolation;
	void AddObject(therm_obj* new_t,double lght);
	virtual void refresh(double dt);
	int h_pump;
	int handle_min;
	int handle_max;
	double min;
	double max;
	double pumping;
	int loaded;
	void* GetComponent(char *component_name);
	virtual void Load(char *line, FILEHANDLE scn);
	virtual void Save(FILEHANDLE scn);
	virtual void BroadcastDemision(ship_object * gonner){if (SRC==gonner) {SRC=NULL;loaded=0;};};
};

///
/// \ingroup PanelSDK
/// The generic electrical object class.
///
/// For specific transducers, derive a class with an appropriate GetValue() call to get the
/// input value. Note that by default, the transducer must be powered in order to function.
///
/// \brief Transducer: convert an arbitrary value into an appropriate output voltage.
///
class Transducer : public e_object {
public:
	///
	/// \brief Constructor.
	/// \param i_name PanelSDK name for the transducer.
	/// \param minIn Minimum input value for the linear range.
	/// \param maxIn Maximum input value for the linear range.
	/// \param minOut Output voltage level for minimum input value.
	/// \param maxOut Output voltage level for maximum input value.
	///
	/// Note that the transducer will output appropriate voltage levels for values outside the minimum and
	/// maximum specified, down to 0V and up to the power level voltage.
	///
	Transducer(char *i_name, double minIn, double maxIn, double minOut, double maxOut);

	///
	/// \brief Get the output voltage from the transducer.
	///
	double Voltage();

	///
	/// \brief Get the value to convert to a voltage.
	///
	virtual double GetValue() = 0;

	///
	/// \brief Check whether the device is powered. Output is 0V with no power.
	/// \return True if it has power, false if not.
	///
	virtual bool IsPowered();

protected:
	double minInputValue;			///< Minimum input value for linear range.
	double maxInputValue;			///< Maximum input value for linear range.
	double minOutputVolts;			///< Output voltage corresponding to minimum input value.
	double maxOutputVolts;			///< Output voltage corresponding to maximum input value.
	double scaleFactor;				///< Scale factor from input value to voltage.
};

///
/// \ingroup PanelSDK
/// \brief Voltage attenuator.
///
/// Basically just a transducer that converts an input voltage to an output voltage. Wire it
/// to your input source and set it to convert as appropriate.
///
class VoltageAttenuator : public Transducer
{
public:
	VoltageAttenuator(char *i_name, double minIn, double maxIn, double minOut, double maxOut);
	double GetValue();
	bool IsPowered();
};

class AtmRegen : public e_object {

public:
	AtmRegen(char *i_name, int i_pump, int i_pumpH2o, e_object *i_SRC, double i_fan_cap, h_Valve *in_v, h_Valve *out_v, h_Valve *i_H2Owaste);

	int h_pump;
	int h_pumpH2o;
	double pumping;
	int loaded;
	double fan_cap;
	h_Valve* in;
	h_Valve* out;
	h_Valve *H20waste;

	bool IsOn() { return (pumping != 0); }
	virtual void refresh(double dt);
	virtual void Load(char *line);
	virtual void Save(FILEHANDLE scn);
	void *GetComponent(char *component_name);
	virtual void BroadcastDemision(ship_object * gonner){if (SRC==gonner) {SRC=NULL;loaded=0;};};

	double co2removalrate;
	//double fanrate;
};

///
/// \ingroup PanelSDK
/// The pump simulation class.
///
class Pump : public e_object {

public:
	Pump(char *i_name, int i_pump, e_object *i_SRC, double i_fan_cap, double i_power, h_Valve* in_v, h_Valve* out_v);

	int h_pump;
	double pumping;
	int loaded;
	double fan_cap;
	double power;
	double flow;	// in g/s
	h_Valve* in;
	h_Valve* out;
	virtual void refresh(double dt);
	virtual void Load(char *line);
	virtual void Save(FILEHANDLE scn);
	void *GetComponent(char *component_name);
	virtual void BroadcastDemision(ship_object * gonner){if (SRC == gonner) {SRC = NULL; loaded = 0;};};

	void SetPumpOn() { h_pump = 1; };
	void SetPumpOff() { h_pump = 0; };
};

///
/// \ingroup PanelSDK
/// The boiler simulation class.
///
class Boiler : public e_object {

public:
    Boiler(char *i_name, int i_pump, e_object *i_src, double heat_watts, double electric_watts,
		   int i_type, double i_valueMin, double i_valueMax, therm_obj *i_target);

	int h_pump;
	double pumping;
	int loaded;
	int type;		// 0: TEMP, 1: PRESS
	therm_obj *target;
	double valueMin, valueMax;
	int handleMin, handleMax;

	double boiler_power;
	double boiler_electrical_power;

	virtual void refresh(double dt);
	virtual void Load(char *line);
	virtual void Save(FILEHANDLE scn);
	virtual void *GetComponent(char *component_name);
	virtual void BroadcastDemision(ship_object * gonner)
		{if (SRC == gonner) {SRC = NULL; loaded = 0;};};
	
	double Current();
	void SetPumpOn()   {h_pump = -1; };
	void SetPumpOff()  {h_pump =  0; };
	void SetPumpAuto() {h_pump =  1; };
};

#endif
