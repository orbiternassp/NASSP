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

#include "esystems.h"
#include <math.h>
#include <stdio.h>

#define SP_MIN_DCVOLTAGE	20.0
#define SP_MIN_ACVOLTAGE	100.0

e_object::e_object()

{
	next = NULL;
	power_load = 0.0;
	SRC = 0;
	enabled = true;

	Volts = 0.0;
	Amperes = 0.0;
	Hertz = 0.0;
}

void e_object::refresh(double dt)

{
}

void e_object::DrawPower(double watts)

{
	if (SRC)
		SRC->DrawPower(watts);
}

void e_object::PUNLOAD(double watts)

{
}

void e_object::connect(e_object *new_src)

{
	SRC=new_src;
}

void e_object::Save(FILEHANDLE scn)

{
}

void e_object::Load(char *line)

{
}

double e_object::Voltage() 

{
	if (IsEnabled()) {
		if (SRC)
			return SRC->Voltage();
		return Volts;
	}

	return 0.0;
}

double e_object::Current() 

{
	if (IsEnabled()) {
		if (SRC)
			return SRC->Current();
		return Amperes;
	}

	return 0.0;
}

double e_object::Frequency()

{
	if (IsEnabled()) {
		if (SRC)
			return SRC->Frequency();
		return Hertz;
	}

	return 0.0;
}

double e_object::PowerLoad()

{
	if (IsEnabled())
		return power_load;

	return 0.0;
}

//
// Update volts and amps before clearing the old power
// drain state.
//

void e_object::UpdateFlow(double dt)

{
	if (SRC)
		Volts = SRC->Voltage();

	if (Volts > 0.0) {
		Amperes = (power_load / Volts);
	}

	power_load = 0.0;
}

E_system::E_system()
{
	List.next=NULL;
}

E_system::~E_system()

{
}

void E_system::Refresh(double dt)

{
	ship_object *runner;

	//
	// First we go through all the systems zeroing their power-drain and updating
	// voltage and current.
	//
	runner=List.next;
	while (runner){ 
		runner->UpdateFlow(dt);
		runner=runner->next;
	}

	//
	// Then refresh them to allow the power drain to update.
	//
	runner=List.next;
	while (runner){ 
		runner->refresh(dt);
		runner=runner->next;
	}
}


void E_system::Save(FILEHANDLE scn)
{ ship_object *runner;
 runner=List.next;
  oapiWriteScenario_string(scn, "<ELECTRIC>","");
 while (runner){ runner->Save(scn);
				 runner=runner->next;}
  oapiWriteScenario_string(scn, "</ELECTRIC>","");
};

void E_system::Load(FILEHANDLE scn) {
	
	ship_object *runner;
	runner = List.next;
 
	char *line;
	char name[100];
	char object[100];
 
	oapiReadScenario_nextline (scn, line);
	while (strnicmp(line,"</ELECTRIC>", 11)) {
		sscanf(line,"%s %s",object, name);		
		runner = GetSystemByName(name);
		if (runner != 0) {
			if (!strnicmp (object, "<COOLING>", 9)) {
				((Cooling*) runner)->Load(line, scn);
			} else {
				((e_object*) runner)->Load(line);
			}
		}
		oapiReadScenario_nextline (scn, line);
	}
}

//------------------------ SOCKET CONNECTOR ---------------------------------------

Socket::Socket(char *i_name,e_object *i_src,int i_p,e_object *tg1,e_object *tg2,e_object *tg3)
{
strcpy(name,i_name);max_stage=99;
curent=-1; SRC=i_src; TRG[0]=tg1;TRG[1]=tg2;TRG[2]=tg3;
socket_handle=i_p-2;
curent=-2;
}
void Socket::refresh(double dt)
{
if (socket_handle!=curent)
			{curent=socket_handle;
             if (SRC) SRC->connect(TRG[curent+1]);
			};
};
void Socket::Load(char *line) //maybe not using anymore ?

{
   sscanf (line,"    <SOCKET> %s %i",name,&socket_handle);
   curent=socket_handle; //make sure we re-conect on load
   if (SRC) SRC->SRC=TRG[curent+1];
}

void Socket::Save(FILEHANDLE scn)
{   char cbuf[1000];
	sprintf (cbuf, "%s %i",name,curent);
	oapiWriteScenario_string (scn, "    <SOCKET>", cbuf);
};
void Socket::BroadcastDemision(ship_object * gonner)
{

for (int i=0;i<3;i++)
	if (TRG[i]==gonner) {TRG[i]=NULL;
	if ((i==curent+1)&&(SRC)) SRC->connect(TRG[curent+1]);}
};
//----------------------------------- FUEL CELL --------------------------------------

FCell::FCell(char *i_name, int i_status, vector3 i_pos, h_Valve *o2, h_Valve *h2, h_Valve* waste, float r_watts) 
{
	strcpy(name, i_name);
	max_stage = 99;
	pos = i_pos;
	Area = 0.35; //size of fuel cell
	mass = 4000; 
	c = 0.5;
	isolation = 0.0; 

	O2_SRC = o2;
	H2_SRC = h2;
	H20_waste = waste;

	outputImpedance = 0.0346667; //ohms

	H2_clogging = 0.0;
	O2_clogging = 0.0;

	H2_purity = 0.9994; //set me somewhere else
	O2_purity = 0.9999; //set me somewhere else

	H2_max_impurities = 0.0504; //nominal impurities after 24hrs //set me somewhere else
	O2_max_impurities = 0.106; //nominal impurities after 24hrs set me somewhere else

	SetTemp(475.0); 
	condenserTemp = 345.0;
	tempTooLowCount = 0;
	Volts = 31.0;
	power_load = 600.0; //2 amps is internal impedance		//TSCH Test 
	max_power = r_watts; //max watts
	clogg = 0.0; //no clog
	start_handle = 0; //stopped
	purge_handle = -1; //no purging
	status = i_status;		//2; //stopped
	reaction = 0; //no chemical react.
	SRC = NULL; //for now a FCell cannot have a source
	running = 1; //ie. not running

	H2_flow = 0;
	O2_flow = 0;
	H2_flowPerSecond = 0;
	O2_flowPerSecond = 0;
	h2o_volume.Void();
}

void FCell::DrawPower(double watts) 
{
	power_load += watts;
}

void FCell::PUNLOAD(double watts) {
	power_load -= watts;
}

void FCell::Reaction(double dt, double thrust) 
{
	//this function needs to be called *after* the power/current/voltage estimation code runs

	#define H2RATIO 0.1119
	#define O2RATIO 0.8881

	//reactant = dt * max_power * thrust / 2880.0 * 0.2894; //grams /second/ 100 amps
		
	// get fuel from sources, maximum flow: grams/timestep from each valve
	double O2_maxflow = O2_SRC->parent->space.composition[SUBSTANCE_O2].mass;
	double H2_maxflow = H2_SRC->parent->space.composition[SUBSTANCE_H2].mass;

	// Reactant consumption
	H2_flow = ((Amperes * MMASS[SUBSTANCE_H2]) / (2*FaradaysConstant)) * numCells * dt; //Faraday's 2nd law electrolysis. confirmed against CSM databook equation
	O2_flow = H2_flow / H2RATIO * O2RATIO; //consume a stoichometeric amount of oxygen

	reactant = H2_flow + O2_flow;

	// max. consumption
	if (H2_flow > H2_maxflow) H2_flow = H2_maxflow;
	if (O2_flow > O2_maxflow) O2_flow = O2_maxflow;
	
	// results of reaction
	double H2O_flow = O2_flow + H2_flow;

	//efficiency and heat generation
	double efficiency = Volts / (hydrogenLHV*numCells);
	double heat = ((power_load / efficiency) - power_load)*dt;

	//heat *= 1.5; ///TODO: FIX WHAT EVER IS COOLING TOO MUCH AND REMOVE THIS LINE

	// purging
	if (status == 3)
	{
		H2_flow += __min(0.67 / 7.93665 * dt, H2_maxflow - H2_flow);
		H2_clogging -= H2_clogging * 0.05 * dt; //take approximately 2 minutes to purge
	}

	if (status == 4)
	{
		O2_flow += __min(0.67 / 7.93665 * dt, O2_maxflow - O2_flow);
		O2_clogging -= O2_clogging * 0.05 * dt; //take approximately 2 minutes to purge
	}

	H2_flowPerSecond = H2_flow / dt;
	O2_flowPerSecond = O2_flow / dt;
	reaction = (H2_flow + O2_flow) / reactant; // % of reaction
	
	// flow from sources
	if (H2_SRC->parent->space.composition[SUBSTANCE_H2].mass > 0.0) 
	{ 
		//reduce enthalpy in the hydrogen source by the amount of enthalpy removed by flow
		H2_SRC->parent->space.composition[SUBSTANCE_H2].Q -= H2_SRC->parent->space.composition[SUBSTANCE_H2].Q * H2_flow / H2_SRC->parent->space.composition[SUBSTANCE_H2].mass;

		//recalculate total enthalpy
		H2_SRC->parent->space.GetQ();

		//remove gaseous hydrogen from tank 
		H2_SRC->parent->space.composition[SUBSTANCE_H2].vapor_mass -= H2_SRC->parent->space.composition[SUBSTANCE_H2].vapor_mass * H2_flow / H2_SRC->parent->space.composition[SUBSTANCE_H2].mass;
	}
	if (O2_SRC->parent->space.composition[SUBSTANCE_O2].mass > 0.0)
	{
		//reduce enthalpy in the oxygen source by the amount of enthalpy removed by flow
		O2_SRC->parent->space.composition[SUBSTANCE_O2].Q -= O2_SRC->parent->space.composition[SUBSTANCE_O2].Q * O2_flow / O2_SRC->parent->space.composition[SUBSTANCE_O2].mass;
		
		//recalculate total enthalpy
		O2_SRC->parent->space.GetQ();

		//remove gaseous oxygen from tank 
		O2_SRC->parent->space.composition[SUBSTANCE_O2].vapor_mass -= O2_SRC->parent->space.composition[SUBSTANCE_O2].vapor_mass * O2_flow / O2_SRC->parent->space.composition[SUBSTANCE_O2].mass;
	}

	//take reactants from source
	H2_SRC->parent->space.composition[SUBSTANCE_H2].mass -= H2_flow;
	O2_SRC->parent->space.composition[SUBSTANCE_O2].mass -= O2_flow;

	// flow to output
	h2o_volume.Void();
	h2o_volume.composition[SUBSTANCE_H2O].mass += H2O_flow;
	h2o_volume.composition[SUBSTANCE_H2O].SetTemp(300.0);
	h2o_volume.GetQ(); 
	
	thermic(heat); //heat from the reaction
	H20_waste->Flow(h2o_volume);

	Clogging(dt); //simulate reactant impurity accumulation

	//if (!strcmp(name, "FUELCELL2"))
	//{
	//	sprintf(oapiDebugString(), "%0.10f, %0.10f", H2_SRC->parent->space.composition[SUBSTANCE_H2].mass, H2_SRC->parent->space.composition[SUBSTANCE_H2].vapor_mass);
	//}

	// TSCH
	/* sprintf(oapiDebugString(), "m %f Q %f Q/m %f", H2_SRC->parent->space.composition[SUBSTANCE_H2].mass,
												   H2_SRC->parent->space.composition[SUBSTANCE_H2].Q,
												   H2_SRC->parent->space.composition[SUBSTANCE_H2].Q / H2_SRC->parent->space.composition[SUBSTANCE_H2].mass);
	*/
}

void FCell::UpdateFlow(double dt) 
{

	//
	// For simplicity, we disable the fuel cell when we seperate the CM from the SM. So act as though it
	// doesn't even exist anymore.
	//
	// Note that we're not expecting to be able to re-enable the fuel cell once it's disabled.
	//

	if (!IsEnabled()) {
		H2_flow = O2_flow = 0.0;
		H2_flowPerSecond = O2_flowPerSecond = 0.0;
		Temp = 0.0;
		condenserTemp = 0.0;
		Volts = 0;
		Amperes = 0;
		running = 1; //ie. not running

		e_object::UpdateFlow(dt);
		return;
	}

	//first we check the start_handle;
	double thrust = 0.0;
	double loadResistance = 0.0;
	if (start_handle == -1) status = 2; //stopped
	if (start_handle == 1)	status = 1; //starting
	if ((purge_handle == 1) && (status == 0 || status == 4)) status = 3; //H2 purging;
	if ((purge_handle == 2) && (status == 0 || status == 3)) status = 4; //O2 purging;
	if ((purge_handle == -1) && (status == 3 || status == 4)) status = 0; //no purging;

	// stopping if colder than the critical temperature (300 °F)
	// counter is because of temperature fluctuation at high time accelerations
	if (Temp < 422.0) {
		tempTooLowCount++;
		if (tempTooLowCount > 100 || status == 1) {
			status = 2;	
			tempTooLowCount = 0;
		}
	} 
	else
	{
		tempTooLowCount = 0;

		if(status == 2) //this will allow us to start if heating up for the first time from ambient temperature
		{
			status = 0;
		}
	}

	switch (status) {

	case 2: //stopped, not much to do
		Volts = 0; Amperes = 0;
		reaction *= 0.2;
		H2_flow = 0;
		O2_flow = 0;
		H2_flowPerSecond = 0;
		O2_flowPerSecond = 0;

		running = 1; //ie. not running
		break;

	case 1:// starting; 
		Reaction(dt, 1.0);
		//status = 2;
		if (reaction > 0.96) {
			status = 0; //started
			start_handle = 2;
		}

		if (reaction > 0.3) {
			Volts = 31.0 * reaction;
			Amperes = (power_load / Volts);
		} else {
			Volts = 0;
			Amperes = 0;
		}
		running = 1;
		break;

	case 3: // O2 purging
	case 4: // H2 purging
	case 0: // normal running

		running = 0; //0 = running
		loadResistance = (829.44) / (power_load); //829.44 = 28.8V^2 which is the voltage that DrawPower() expects. use this calculate the resistive load on the fuel cell
		Volts = 31.0; //inital estimate for voltage

		//coefficients for 5th order approximation of fuel cell performance, taken from:
		//CSM/LM Spacecraft Operational Data Book, Volume I CSM Data Book, Part I Constraints and Performance. Figure 4.1-10
		double A = 0.023951368224792 * Temp + 23.9241562583015;
		double B = 0.003480859912024 * Temp - 2.19986938582928;
		double C = -0.0001779207513 * Temp + 0.104916556604259;
		double D = 5.0656524872309E-06 * Temp - 0.002885372247954;
		double E = -6.42229870072935E-08 * Temp + 3.58599071612147E-05;
		double F = 3.02098031429142E-10 * Temp - 1.66275376548748E-07;

		for (int ii = 0; ii < 5; ++ii) //use an iterative procedure to solve for voltage and current. with our guess of 31 volts these should converge in 3~5 steps
		{
			Amperes = (power_load / Volts);
			Volts = A + B * Amperes + C * Amperes*Amperes + D * Amperes*Amperes*Amperes + E * Amperes*Amperes*Amperes*Amperes + F * Amperes*Amperes*Amperes*Amperes*Amperes;
			power_load = Amperes * Volts; //recalculate power_load
		}

		//"clogg" is used to make voltage (and current) drop by 5.2V over 1 day of normal impurity accumulation
		Amperes -= (2.25*clogg);
		Volts -= -(5.2*clogg);
		power_load = Amperes * Volts; //recalculate power_load again after clogging

		//---- throttle of the fuel cell [0..1]
		thrust = power_load / max_power;

		Reaction(dt, thrust);

		break;
	}

	//condenser exhaust temperature is not simulated realistically at the moment
	condenserTemp = (0.29 * Temp) + 209.0;

	//Conductive heat transfer
	const double ConductiveHeatTransferCoefficient = 0.54758; // w/K, calculated from CSM/LM Spacecraft Operational Data Book, Volume I CSM Data Book, Part I Constraints and Performance. Figure 4.1-21
	if (Temp > 300.0) //assume that the ambient internal temperature of the spacecraft is 300K, ~80°F, eventually we need to simulate this too
	{ 
		thermic((300.0 - Temp) * ConductiveHeatTransferCoefficient * dt);
	}	
	//*********************

	e_object::UpdateFlow(dt);
}

void FCell::refresh(double dt)

{
	//
	// Nothing for now.
	//
}

void FCell::Clogging(double dt)
{
	H2_clogging += (1 - H2_purity) * H2_flow *dt;
	O2_clogging += (1 - O2_purity) * O2_flow *dt;

	if (H2_clogging < 0)
	{
		H2_clogging = 0.0; //cannot be negative	
	}

	if (O2_clogging < 0)
	{
		O2_clogging = 0.0; //cannot be negative	
	}

	//O2 impurities effect voltage drop substantially more than H2(not detectable according to AOH)
	//here we're simulating the effect by making the O2 clogging effect the voltage drop 25x as much as the H2
	clogg = (25 * (O2_clogging / O2_max_impurities) + (H2_clogging / H2_max_impurities)) / 26.0;
}

void FCell::Load(char *line)
{
	double temp;
	sscanf(line, "    <FCELL> %s %i %lf %lf %lf %lf", name, &status, &H2_clogging, &temp, &power_load, &O2_clogging);
	SetTemp(temp);
	if (status == 0 || status >=3) running = 0;
}

void FCell::Save(FILEHANDLE scn)
{
	char cbuf[1000];
	sprintf(cbuf, "%s %i %0.10f %0.4f %0.4f %0.10f", name, status, H2_clogging, Temp, power_load, O2_clogging);
	oapiWriteScenario_string (scn, "    <FCELL> ", cbuf);
}

//
//-------------------------------------- BATTERY ---------------------------------
//

Battery::Battery(char *i_name, e_object *i_src, double i_power, double i_voltage, double i_resistance)
{
	 strcpy(name,i_name);
	 max_stage=99;
	 SRC = i_src;

	 internal_resistance = i_resistance;
	 max_voltage = i_voltage;
	 power_load = 0.0;
	 max_power = power = i_power;
	 Volts = max_voltage;
}

void Battery::DrawPower(double watts)
{ 
	power_load += watts;
}

void Battery::PUNLOAD(double watts)
{
	power_load -= watts;
}

double Battery::Voltage()
{
	if (IsEnabled())
	{
		return Volts;
	}

	return 0.0;
}

double Battery::Current()
{
//	sprintf(oapiDebugString(), "%s: Current = %gA (%g watts/%gV)", name, Amperes, power_load, Volts);

	if (IsEnabled())
	{
		return Amperes;
	}

	return 0.0;
}

void Battery::UpdateFlow(double dt)
{
	power -= power_load * dt;

	if (Volts > 0.0) 
		Amperes = (power_load / Volts);
	else
		Amperes = 0;

	// Reset power load
	power_load = 0.0;

	// Simple appoximation for voltage: 90% voltage at 20% capacity
	if (power > 0.2 * max_power)
		Volts = max_voltage * (0.875 + 0.125 * power / max_power);
	else
		Volts = max_voltage * 4.5 * power / max_power;

	// Voltage drop because of load
	Volts = (Volts - (Amperes * internal_resistance));

	if (power < 0) { 
		power = 0;
		Amperes = 0;
		Volts = 0 ;
	}
}

void Battery::refresh(double dt)
{
	// charging 
	
	double p;

	if (SRC && SRC->Voltage()) {
		if (Volts < 0.99 * max_voltage) {
			p = Volts * (2.2 + (0.99 * max_voltage - Volts) / (0.99 * max_voltage));
		} else {
			p = Volts * 2.2 / 0.01 * (max_voltage - Volts) / max_voltage;
		}
		SRC->DrawPower(p);
		power += p * dt;
	}
}

void Battery::Load(char *line)
{
	sscanf(line,"    <BATTERY> %s %lf", name, &power);
}

void Battery::Save(FILEHANDLE scn)
{
	char cbuf[1000];
	sprintf (cbuf, "%s %0.4f",name, power);
	oapiWriteScenario_string (scn, "    <BATTERY> ", cbuf);
}

//-------------------------- TRANSDUCER BASE CLASS -------------------------------
Transducer::Transducer(char *i_name, double minIn, double maxIn, double minOut, double maxOut)

{
	strcpy(name, i_name);
	
	minInputValue = minIn;
	minOutputVolts = minOut;
	maxInputValue = maxIn;
	maxOutputVolts = maxOut;

	scaleFactor = (maxOut - minOut) / (maxIn - minIn);
}

double Transducer::Voltage()

{
	if (!IsPowered())
		return 0.0;

	double v = ((GetValue() - minInputValue) * scaleFactor) + minOutputVolts;

	if (v < 0.0)
		return 0.0;

	double inVolts = SRC ? SRC->Voltage() : 0.0;

	if (v > inVolts)
		return inVolts;

	return v;
}

bool Transducer::IsPowered()

{
	if (!SRC)
		return 0.0;

	return (SRC->Voltage() > 25.0);
}

//-------------------------- VOLTAGE ATTENUATOR -------------------------------
VoltageAttenuator::VoltageAttenuator(char *i_name, double minIn, double maxIn, double minOut, double maxOut) :
	Transducer(i_name, minIn, maxIn, minOut, maxOut)
{
}

double VoltageAttenuator::GetValue()

{
	if (SRC)
	{
		return SRC->Voltage();
	}

	return 0.0;
}

//
// Voltage attenuator is effectively powered by its input,
// so always return true.
//
bool VoltageAttenuator::IsPowered()

{
	return true;
}

//-------------------------- DIRECT CURRENT BUS -------------------------------
DCbus::DCbus(char* i_name, e_object *i_SRC, double lossFact) 

{
	SRC=i_SRC;
	strcpy(name,i_name);
	lossFactor = lossFact;
	max_stage=99;
	Volts = 0.0;
	Amperes = 0.0;	
	power_load = 0.0;
}

void DCbus::DrawPower(double watts)

{
	power_load += watts;
	if (SRC)
		SRC->DrawPower(watts * lossFactor);
}

void DCbus::PUNLOAD(double watts)
{
	power_load -= watts;
	if (SRC)
		SRC->PUNLOAD(watts * lossFactor);
}


void DCbus::Disconnect()
{
	SRC = NULL;
	Amperes = 0;
	Volts = 0;
}


void DCbus::refresh(double dt) 

{
	// Nothing to do.
}

double DCbus::Current()

{
	if (SRC && SRC->IsEnabled()) {
		Volts = SRC->Voltage();
		if (Volts > 0.0)
			Amperes = (power_load / Volts);
		else 
			Amperes = 0.0; 
	}
	return Amperes;
}

void DCbus::Load(char *line)
{
	sscanf (line,"    <DC> %s", name);
}

void DCbus::Save(FILEHANDLE scn)
{   
	char cbuf[1000];
	sprintf (cbuf, "%s", name);
	oapiWriteScenario_string (scn, "    <DC> ", cbuf);
}

//------------------------ AC BUS -------------------------------------------------

//
// Note that this really isn't a 'bus' class. It's more a simulation of an invertor, and
// should be changed appropriately in the near future.
//

ACbus::ACbus(char *i_name, double i_voltage, e_object *i_SRC)

{
	ac_voltage = i_voltage;
	SRC=i_SRC;
	strcpy(name,i_name);
	max_stage=99;
	Volts = ac_voltage;
	Amperes=0;
	power_load = 0.0;
}

//
// We'll assume the bus/inverter is about 90% efficient.
//

void ACbus::DrawPower(double watts)

{
	power_load += watts;
	if (SRC)
		SRC->DrawPower(watts * 1.10);
}

void ACbus::PUNLOAD(double watts)
{
	power_load -= watts;
	if (SRC)
		SRC->PUNLOAD(watts * 1.10);
}

void ACbus::connect(e_object *new_src)
{ 
	SRC=new_src;
}

void ACbus::refresh(double dt)
{
	//
	// Nothing to do for now.
	//
}

double ACbus::Current()

{
	if (SRC && SRC->IsEnabled() && SRC->Voltage() > 23.0) {
		Amperes = (power_load / ac_voltage);
		return Amperes;
	}

	return 0.0;
}

double ACbus::Voltage()

{
	if (SRC && SRC->IsEnabled() && SRC->Voltage() > 23.0)
		return ac_voltage;

	return 0.0;
}

void ACbus::Load(char *line)
{

   sscanf (line,"    <AC> %s", name);
}

void ACbus::Save(FILEHANDLE scn)
{    char cbuf[1000];
	sprintf (cbuf, "%s",name);
	oapiWriteScenario_string (scn, "    <AC> ", cbuf);
}

ACInverter::ACInverter(char *i_name, double i_voltage, e_object *i_SRC)

{
	ac_voltage = i_voltage;
	SRC=i_SRC;strcpy(name,i_name);max_stage=99;
	Volts = ac_voltage;
	Amperes=0;
	power_load = 0.0;
	
	// Base	efficiency per watt, calculated	against	a 65% watt-to-VoltAmp ratio
	// from	load data in MSC 67-FM-200
	BASE_EPW[0]	= 0.972307692;
	BASE_EPW[1]	= 0.519230769;
	BASE_EPW[2]	= 0.424615385;
	BASE_EPW[3]	= 0.360256410;
	BASE_EPW[4]	= 0.314725275;
	BASE_EPW[5]	= 0.278846154;
	BASE_EPW[6]	= 0.250598291;
	BASE_EPW[7]	= 0.227692308;
	BASE_EPW[8]	= 0.209230769;
	BASE_EPW[9]	= 0.193589744;
	BASE_EPW[10] = 0.179881657;
	BASE_EPW[11] = 0.168131868;
	BASE_EPW[12] = 0.157948718;
	BASE_EPW[13] = 0.148557692;
	BASE_EPW[14] = 0.140271493;
	BASE_EPW[15] = 0.132871795;
	BASE_EPW[16] = 0.125943320;
	BASE_EPW[17] = 0.119769231;
	BASE_EPW[18] = 0.114139194;
	BASE_EPW[19] = 0.108881119;
	BASE_EPW[20] = 0.104040134;
	BASE_EPW[21] = 0.099653846;
	BASE_EPW[22] = 0.095384615;
	BASE_EPW[23] = 0.091597633;
	BASE_EPW[24] = 0.088034188;
	BASE_EPW[25] = 0.084615385;
	BASE_EPW[26] = 0.058380567;

	// Additional inefficiency per volt, from MSC 67-FM-200
	EPW_PV[0] = 0.011538462;
	EPW_PV[1] = 0.008307692;
	EPW_PV[2] = 0.007487179;
	EPW_PV[3] = 0.005362637;
	EPW_PV[4] = 0.004423077;
	EPW_PV[5] = 0.003452991;
	EPW_PV[6] = 0.002800000;
	EPW_PV[7] = 0.002349650;
	EPW_PV[8] = 0.001948718;
	EPW_PV[9] = 0.001751479;
	EPW_PV[10] = 0.001406593;
	EPW_PV[11] = 0.001230769;
	EPW_PV[12] = 0.000961538;
	EPW_PV[13] = 0.000904977;
	EPW_PV[14] = 0.000813675;
	EPW_PV[15] = 0.000735223;
	EPW_PV[16] = 0.000692308;
	EPW_PV[17] = 0.000673993;
	EPW_PV[18] = 0.000657343;
	EPW_PV[19] = 0.000634114;
	EPW_PV[20] = 0.000648718;
	EPW_PV[21] = 0.000590769;
	EPW_PV[22] = 0.000579882;
	EPW_PV[23] = 0.000592593;
	EPW_PV[24] = 0.000560440;
	EPW_PV[25] = 0.000161943;

	overload_tripped = false;
	last_overload_check = false;
	overload_check_time = 0.0;

	PhaseA.WireTo(this);
	PhaseB.WireTo(this);
	PhaseC.WireTo(this);
}

// I have actual efficiency numbers from MSC 67-FM-200 -- DS20070113
// Inverter efficiency varies with both the input voltage and AC load.

void ACInverter::DrawPower(double watts)

{
	// Add load
	power_load += watts;

	// Don't draw power here. We need to do this later based on total draw and input voltage.

//	if (SRC)
//		SRC->DrawPower(watts * 1.10);
}

void ACInverter::connect(e_object *new_src)
{ 
	SRC = new_src;
}

void ACInverter::refresh(double dt)
{
	//
	// Nothing for now.
	//
}

// According to AOH 2.6-9 the inverter can operate down to 19VDC by internally boosting the DC up to 25.

double ACInverter::Current()

{
	if (SRC && SRC->IsEnabled() && SRC->Voltage() > 19) {
		Amperes = (power_load / ac_voltage);
		return Amperes;
	}

	return 0.0;
}

double ACInverter::Voltage()

{
	if (SRC && SRC->IsEnabled() && SRC->Voltage() > 19)
		return ac_voltage;

	return 0.0;
}

void ACInverter::Load(char *line)
{
	int trip;
	sscanf (line,"    <INVERTER> %s %d", name, &trip);

	overload_tripped = (trip != 0);
}

void ACInverter::Save(FILEHANDLE scn)
{    
	char cbuf[1000];
	sprintf (cbuf, "%s %d",name, overload_tripped ? 1 : 0);
	oapiWriteScenario_string (scn, "    <INVERTER> ", cbuf);
}

double ACInverter::calc_epw_util(double maxw,int index,double SourceVoltage){
	double epw_factor,watt_scale;
	watt_scale = power_load/maxw;
	epw_factor = (BASE_EPW[index]*watt_scale)-(EPW_PV[index]*(SourceVoltage-25));
	return(epw_factor*power_load);
}

double ACInverter::get_epw(double base_epw_factor, double SourceVoltage){
		
	if(power_load < 65){
		// Power load 65, uses base direct
		return(base_epw_factor*SourceVoltage); // This is constant at this low load.		
	}	
	if(power_load < 130){   return(calc_epw_util(130   ,0,SourceVoltage)); }
	if(power_load < 162.5){	return(calc_epw_util(162.5 ,1,SourceVoltage)); }
	if(power_load < 195){   return(calc_epw_util(195   ,2,SourceVoltage)); }
	if(power_load < 227.5){	return(calc_epw_util(227.5 ,3,SourceVoltage)); }
	if(power_load < 260){   return(calc_epw_util(260   ,4,SourceVoltage)); }
	if(power_load < 292.5){	return(calc_epw_util(292.5 ,5,SourceVoltage)); }
	if(power_load < 325){   return(calc_epw_util(325   ,6,SourceVoltage)); }
	if(power_load < 357.5){	return(calc_epw_util(357.5 ,7,SourceVoltage)); }
	if(power_load < 390){	return(calc_epw_util(390   ,8,SourceVoltage)); }
	if(power_load < 422.5){	return(calc_epw_util(422.5 ,9,SourceVoltage)); }
	if(power_load < 455){	return(calc_epw_util(455,  10,SourceVoltage)); }
	if(power_load < 487.5){	return(calc_epw_util(487.5,11,SourceVoltage)); }
	if(power_load < 520  ){	return(calc_epw_util(520  ,12,SourceVoltage)); }
	if(power_load < 552.5){	return(calc_epw_util(552.5,13,SourceVoltage)); }
	if(power_load < 585  ){	return(calc_epw_util(585  ,14,SourceVoltage)); }
	if(power_load < 617.5){	return(calc_epw_util(617.5,15,SourceVoltage)); }
	if(power_load < 650  ){	return(calc_epw_util(650  ,16,SourceVoltage)); }
	if(power_load < 682.5){	return(calc_epw_util(682.5,17,SourceVoltage)); }
	if(power_load < 715  ){	return(calc_epw_util(715  ,18,SourceVoltage)); }
	if(power_load < 747.5){	return(calc_epw_util(747.5,19,SourceVoltage)); }
	if(power_load < 780  ){	return(calc_epw_util(780  ,20,SourceVoltage)); }
	if(power_load < 812.5){	return(calc_epw_util(812.5,21,SourceVoltage)); }
	if(power_load < 845  ){	return(calc_epw_util(845  ,22,SourceVoltage)); }
	if(power_load < 877.5){	return(calc_epw_util(877.5,23,SourceVoltage)); }
	if(power_load < 910  ){	return(calc_epw_util(910  ,24,SourceVoltage)); }
	if(power_load < 1235 ){	return(calc_epw_util(1235 ,25,SourceVoltage)); }
	// SHOULD NEVER GET HERE
	return(70); // 70% eff above 1235 watts
}

void ACInverter::UpdateFlow(double dt)

{
	// EFFICIENCY CALCULATION
	if (SRC){
		// Determine base EPW 
		double SourceVoltage = SRC->Voltage();

		if(SourceVoltage < 19){ // We can't run in this case.
			// Do output updates
			PhaseA.UpdateFlow(dt);
			PhaseB.UpdateFlow(dt);
			PhaseC.UpdateFlow(dt);
			power_load = 0.0;
			return;
		}
		if(SourceVoltage < 25){ SourceVoltage = 25; } // Regulate this back up to 25
		if(SourceVoltage > 30){ SourceVoltage = 30; } // Regulate this back down to 30
		
		double base_epw_factor = (2.528-(0.144266667*(SourceVoltage - 25)));
		// Calculate
		double efactor = (get_epw(base_epw_factor,SourceVoltage)/100);
		efactor = (1 - efactor)+1;

		//sprintf(oapiDebugString(),"INV: LOAD %f WATT, IV %f VDC, EFFX %f",power_load,SourceVoltage,efactor);

		SRC->DrawPower(power_load * efactor);
	}else{
		// Cannot operate without a source of power.
		PhaseA.UpdateFlow(dt);
		PhaseB.UpdateFlow(dt);
		PhaseC.UpdateFlow(dt);
		power_load = 0.0;
		return;
	}
	
	//
	// Check for overload every 5 seconds. Must be overloaded at
	// start and end to qualify.
	//

	overload_check_time += dt;

	if (overload_check_time > 5) {
		bool overloaded = false; // Not overloaded by default

		// If the total AC ouput exceeds 250% of rated current (27.7 A),
		// or any single phase exceeds 300% of rated current (11 A), we have an overload.
		// Overload does *NOT* disconnect the inverter from the bus, but does light the C/W lamp

		if(PhaseA.Current() > 11){ overloaded = true; }
		if(PhaseB.Current() > 11){ overloaded = true; }
		if(PhaseB.Current() > 11){ overloaded = true; }
		if((PhaseA.Current()+PhaseB.Current()+PhaseC.Current()) > 27.7){ overloaded = true; }

		if (overloaded && last_overload_check){
			overload_tripped = true;
		}else{
			overload_tripped = false;
		}

		last_overload_check = overloaded;
		overload_check_time = 0.0;

	}

	// Do output updates
	PhaseA.UpdateFlow(dt);
	PhaseB.UpdateFlow(dt);
	PhaseC.UpdateFlow(dt);

	power_load = 0.0;

}

ACPhaseOutput::ACPhaseOutput()

{
	//
	// Nothing for now.
	//
}

void ACPhaseOutput::DrawPower(double watts)

{
	power_load += watts;
	if (SRC)
		SRC->DrawPower(watts);
}

double ACPhaseOutput::Current()

{
	if (SRC && SRC->IsEnabled()) {
		Volts = SRC->Voltage();
		return power_load / Volts;
	}

	return 0.0;
}

Cooling::Cooling(char *i_name,int i_pump,e_object *i_SRC,double thermal_prop,double min_t,double max_t)
{
	strcpy(name,i_name);
	max_stage=99;
	pumping=h_pump=i_pump;
	max=max_t;
	min=min_t;
	handle_min=0;
	handle_max=0;
	nr_list=0;
	coolant_temp=300.0; // reasonable ambient temperature
	isolation=thermal_prop;
	SRC=i_SRC;
	loaded=0; //ie. not PLOADed
};

void Cooling::AddObject(therm_obj *new_t, double lght) 

{
	if (nr_list < 6) {
		list[nr_list] = new_t;
		length[nr_list] = lght;
		bypassed[nr_list] = false;
		nr_list++;
	}
}

void Cooling::refresh(double dt) 

{
	double throttle, heat_ex;
	therm_obj* activelist[6];	//the list of not bypassed objects
	double activelength[6];		//and their pipe length
	int nr_activelist = 0;

	if (handle_min)
		min += handle_min / 10.0 * dt;
	if (handle_max)
		max += handle_max / 10.0 * dt;
	
	int i;

	// build active list
	for (i = 0; i < nr_list; i++) {
		if (!bypassed[i]) {
			activelist[nr_activelist] = list[i];
			activelength[nr_activelist] = length[i];
			nr_activelist++;
		}
	}

	// everthing is bypassed
	if (nr_activelist == 0)	{
		pumping = 0;
	} else {
		if (h_pump == 0) pumping = 0; //off
		if (h_pump == 1) {
			if (activelist[0]->Temp < min) //turn the cooling off
					pumping = 0;
			if (activelist[0]->Temp > max) { //turn the cooling on
					pumping = 1;
					throttle = 1.0;
			} else if (activelist[0]->Temp > min) {
					pumping = 1;
					throttle = (activelist[0]->Temp - min) / (max - min);
			}
		}
		if (h_pump == -1) {
			pumping = 1;	//manual on
			throttle = 1.0;
		}
	}

	if (pumping) //time to do the rumba
	{ 
		if (SRC && SRC->Voltage() > SP_MIN_DCVOLTAGE)
		{
		  SRC->DrawPower(65.0);

		}
		else //no power
		{ 
		  pumping = 0;
		  return;
		} 

		for (i = 0; i < nr_activelist - 1; i++)
		{
			heat_ex = (activelist[i+1]->Temp - activelist[i]->Temp) * activelength[i] * dt * isolation * throttle;
			activelist[i]->thermic(heat_ex);
			activelist[i+1]->thermic(-heat_ex);
		}

		heat_ex = (activelist[0]->Temp - activelist[nr_activelist-1]->Temp) * activelength[0] * dt * isolation * throttle;
		activelist[0]->thermic(-heat_ex);
	}

	// average temp except the first
	coolant_temp = 0.0;
	for (i = 1; i < nr_activelist; i++) {
		coolant_temp += activelist[i]->Temp;
	}
	coolant_temp = coolant_temp / (nr_activelist - 1.0);
}

void Cooling::Load(char *line, FILEHANDLE scn) {

	char *nextline;
	int i;
	int bp;

	sscanf (line,"    <COOLING> %s %i %i %lf %lf %lf", name, &h_pump, &loaded, &max, &min, &isolation);

	oapiReadScenario_nextline (scn, nextline);
	while (strnicmp(nextline,"</COOLING>", 10)) {
		sscanf (nextline, "%i %i", &i, &bp);

		bypassed[i] = (bp != 0);
		oapiReadScenario_nextline (scn, nextline);
	}
}

Cooling::~Cooling() 

{
}

void Cooling::Save(FILEHANDLE scn) 

{    
	char cbuf[1000];

	sprintf (cbuf, "%s %i %i %0.4f %0.4f %0.4f", name, h_pump, loaded, max, min, isolation);
	oapiWriteScenario_string (scn, "    <COOLING> ", cbuf);

	for (int i = 0; i < nr_list; i++) {
		sprintf (cbuf, "%i %i", i, bypassed[i]);
		oapiWriteScenario_string (scn, "        ", cbuf);
	}
	oapiWriteScenario_string (scn, "    </COOLING> ", "");
}

AtmRegen::AtmRegen(char *i_name, int i_pump, int i_pumpH2o, e_object *i_SRC, double i_fan_cap, h_Valve* in_v, h_Valve* out_v, h_Valve *i_H2Owaste) 

{
	strcpy(name, i_name);
	max_stage = 99;
	pumping = h_pump = i_pump;
	h_pumpH2o = i_pumpH2o;
	SRC = i_SRC;
	fan_cap = i_fan_cap;
	in = in_v;
	out = out_v;
	H20waste = i_H2Owaste;
	loaded = 0;

	co2removalrate = 0;
	//fanrate = 0;
}

void AtmRegen::refresh(double dt) {

	co2removalrate = 0;
	pumping = 0;

	if (h_pump == 0) {
		return;
	} 

	if (SRC) {
		if (SRC->Voltage() < SP_MIN_DCVOLTAGE) 
			return;
		SRC->DrawPower(241.5);
	} else {
		return;
	}

	pumping = 1;

	double delta_p = in->GetPress() - out->GetPress() + fan_cap;
	if (delta_p < 0)
		delta_p = 0;

	h_volume fanned = in->GetFlow(dt * delta_p);
	co2removalrate = fanned.composition[SUBSTANCE_CO2].mass / dt;

	if (co2removalrate <= 0.0356) {
		fanned.composition[SUBSTANCE_CO2].mass =
		fanned.composition[SUBSTANCE_CO2].vapor_mass =
		fanned.composition[SUBSTANCE_CO2].Q = 0;
	} else {
		double removedmass = 0.0356 * dt;
		double factor = (fanned.composition[SUBSTANCE_CO2].mass - removedmass) / fanned.composition[SUBSTANCE_CO2].mass;
		fanned.composition[SUBSTANCE_CO2].mass -= removedmass;
		fanned.composition[SUBSTANCE_CO2].vapor_mass -= removedmass;
		fanned.composition[SUBSTANCE_CO2].Q = fanned.composition[SUBSTANCE_CO2].Q * factor;

		co2removalrate = removedmass / dt;
	}

	// separate water
	if (h_pumpH2o) {
		h_volume h2o_volume;
		h2o_volume.Void();
		h2o_volume.composition[SUBSTANCE_H2O].mass = fanned.composition[SUBSTANCE_H2O].mass;		
		h2o_volume.composition[SUBSTANCE_H2O].SetTemp(300.0);
		h2o_volume.GetQ(); 
		// ... and pump it to waste valve	
		H20waste->Flow(h2o_volume);

		fanned.composition[SUBSTANCE_H2O].mass =
		fanned.composition[SUBSTANCE_H2O].vapor_mass =
		fanned.composition[SUBSTANCE_H2O].Q = 0; 
	}
	
	// flow to output
	fanned.GetMass();
	fanned.GetQ();
	out->Flow(fanned);
}

void AtmRegen::Load(char *line) {

  sscanf (line,"    <ATMREGEN> %s %i %i %i %lf", name, &h_pump, &h_pumpH2o, &loaded, &fan_cap);
}

void AtmRegen::Save(FILEHANDLE scn) {
    
	char cbuf[1000];

	sprintf (cbuf, "%s %i %i %i %lf", name, h_pump, h_pumpH2o, loaded, fan_cap);
	oapiWriteScenario_string (scn, "    <ATMREGEN> ", cbuf);
}

Boiler::Boiler(char *i_name, int i_pump, e_object *i_src, double heat_watts, double electric_watts,
		   int i_type, double i_valueMin, double i_valueMax, therm_obj *i_target) {

	strcpy(name, i_name);
	max_stage = 99;
	SRC = i_src;
	loaded = 0;

	if(SRC != NULL){
		Amperes = electric_watts / (SRC->Voltage());
	}else{
		Amperes = 0;
	}

	boiler_power = heat_watts;
	boiler_electrical_power = electric_watts;
	type = i_type;
	target = i_target;
	pumping = h_pump = i_pump;
	valueMin = i_valueMin;
	valueMax = i_valueMax;
	handleMin = 0;
	handleMax = 0;
}

double Boiler::Current()

{
	if (pumping && SRC) {
		double v = SRC->Voltage();
		if (v) {
			return boiler_power / v;
		}
	}

	return 0.0;
}

void Boiler::refresh(double dt) 

{
	if (h_pump > 0) { //on auto
		if (type == 0) { // TEMP
			if ((target->Temp < valueMin) && (!pumping))
				pumping = 1;
			if ((target->Temp > valueMax) && (pumping))
				pumping = 0;
		} else if (type == 1) { // PRESS
			// in this case the target has to be a tank
			h_Tank *tank = (h_Tank *) target;
			if ((tank->space.Press < valueMin) && (!pumping))
				pumping = 1;
			if ((tank->space.Press > valueMax) && (pumping))
				pumping = 0;
		}
	} else if (h_pump < 0)
		pumping = 1; //force manual on
	else
		pumping = 0;

	if (pumping && SRC) {
		if (SRC->Voltage() < SP_MIN_DCVOLTAGE) { //or unload if no power.
			pumping = 0;
			return;
		}
		SRC->DrawPower(boiler_electrical_power);
		target->thermic(boiler_power * dt); //1 joule = 1 watt * dt
	} else {
		pumping = 0;
	}

	if (handleMax)
		valueMax += handleMax / 10.0 * dt; //adjusting max. target value
	if (handleMin)
		valueMin += handleMin / 10.0 * dt; //adjusting min. target value
}

void Boiler::Load(char *line) 

{
	sscanf(line,"    <BOILER> %s %i %i %lf %lf %lf %lf %lf", name, &h_pump, &loaded, &pumping, &valueMin, &valueMax, &boiler_power, &boiler_electrical_power);
}

void Boiler::Save(FILEHANDLE scn) 

{
    char cbuf[1000];
	sprintf (cbuf, "%s %i %i %lf %lf %lf %lf %lf", name, h_pump, loaded, pumping, valueMin, valueMax, boiler_power, boiler_electrical_power);
	oapiWriteScenario_string (scn, "    <BOILER> ", cbuf);
}


Pump::Pump(char *i_name, int i_pump, e_object *i_SRC, double i_fan_cap, double i_power, h_Valve* in_v, h_Valve* out_v) 

{
	strcpy(name, i_name);
	max_stage = 99;
	pumping = h_pump = i_pump; 
	SRC = i_SRC;
	fan_cap = i_fan_cap;
	power = i_power;
	in = in_v;
	out = out_v;
	loaded = 0;
	flow = 0;
}

void Pump::refresh(double dt) 
{
	flow = 0;

	if (h_pump == 0) {
		pumping = 0;
	} //off
	else
	{
		pumping = 1;
		if (SRC) {
			if (SRC->Voltage() < SP_MIN_DCVOLTAGE)
				pumping = 0;
			else
				SRC->DrawPower(power);
		}
		else
			pumping = 0;
	}

	double delta_p = in->GetPress() - out->GetPress() + (pumping > 0 ? fan_cap : 0.0);
	if (delta_p < 0)
		delta_p = 0;

	h_volume fanned = in->GetFlow(dt * delta_p);
	flow = fanned.GetMass() / dt;
	out->Flow(fanned);
}

void Pump::Load(char *line) 

{
	sscanf (line,"    <PUMP> %s %i %i %lf", name, &h_pump, &loaded, &fan_cap);
}

void Pump::Save(FILEHANDLE scn) {
    
	char cbuf[1000];

	sprintf (cbuf, "%s %i %i %lf", name, h_pump, loaded, fan_cap);
	oapiWriteScenario_string (scn, "    <PUMP> ", cbuf);
}
