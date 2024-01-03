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

#include <stdio.h>
#include "Esystems.h"
#include "../BUILD.H"

void E_system::Create_Boiler(char *line) {

	char name[100], source[100], targetName[100], typeName[100];
	int pump, type, ramp;
	double watts, ewatts, valueMin, valueMax, rampRate;

	ramp = 0;
	rampRate = 0.0;

	sscanf(line + 8,"%s %i %s %lf %lf %s %lf %lf %s %i %lf",
		name, &pump, source, &watts, &ewatts, typeName, &valueMin, &valueMax, targetName, &ramp, &rampRate);

	ship_object* so = (ship_object*) GetPointerByString(targetName) ;
	therm_obj *t = so->GetThermalInterface();
	e_object *src = (e_object*) GetPointerByString(source);
	if (Compare(typeName, "TEMP"))
		type = 0;
	else if (Compare(typeName, "PRESS"))
		type = 1;
	else if (Compare(typeName, "CHILLER"))
		type = 2;
	else if (Compare(typeName, "PWM"))
		type = 3;

	AddSystem(new Boiler(name, pump, src, watts, ewatts, type, valueMin, valueMax, t, (bool)ramp, rampRate));
}

void E_system::Create_AtmRegen(char *line) {

	char name[100], source[100], in_v[100], out_v[100], h2owaste_v[100];
	int pump, pumpH2o;
	double size;
	h_Valve *in, *out, *h2owaste;
	e_object *SRC;

	sscanf(line + 10, "%s %i %i %s %lf %s %s %s", name, &pump, &pumpH2o, source, &size, in_v, out_v, h2owaste_v);
	SRC = (e_object*)GetPointerByString(source);
	in = (h_Valve*)P_hydraulics->GetPointerByString(in_v);
	out = (h_Valve*)P_hydraulics->GetPointerByString(out_v);
	h2owaste = (h_Valve*)P_hydraulics->GetPointerByString(h2owaste_v);

	AddSystem(new AtmRegen(name, pump, pumpH2o, SRC, size, in, out, h2owaste));
}

void E_system::Create_Pump(char *line) {

	char name[100], source[100], in_v[100], out_v[100];
	int pump;
	double size, power;
	h_Valve *in, *out;
	e_object *SRC;

	sscanf(line + 6, "%s %i %s %lf %lf %s %s", name, &pump, source, &size, &power, in_v, out_v);
	SRC = (e_object*)GetPointerByString(source);
	in = (h_Valve*)P_hydraulics->GetPointerByString(in_v);
	out = (h_Valve*)P_hydraulics->GetPointerByString(out_v);

	AddSystem(new Pump(name, pump, SRC, size, power, in, out));
}

void E_system::Create_Cooling(char* line)
{
	char name[100];
	char source[100];
	double lngt;
	//int num;
	int pump;
	double term, max, min;
	therm_obj* TR1;
	e_object* P_SRC;
	ship_object* NON_t;
	sscanf(line + 9, "%s %i %s %lf %lf %lf", name, &pump, source, &term, &min, &max);
	P_SRC = (e_object*)GetPointerByString(source);
	Cooling* new_c = (Cooling*)AddSystem(new Cooling(name, pump, P_SRC, term, min, max));
	line = ReadConfigLine();
	while (!Compare(line, "</COOLING>")) {
		sscanf(line, "%s %lf", source, &lngt);
		NON_t = (ship_object*)GetPointerByString(source);
		if (NON_t) TR1 = NON_t->GetThermalInterface();
		if (TR1)
			new_c->AddObject(TR1, lngt);
		line = ReadConfigLine();
	}

}
void E_system::Create_Socket(char* line)
{
	char name[100];
	char source[100];
	int ip;
	char tr1[100], tr2[100], tr3[100];

	sscanf(line + 8, "%s %i %s %s %s %s", name, &ip, source, tr1, tr2, tr3);
	e_object* SRC = (e_object*)GetPointerByString(source);
	e_object* TR1 = (e_object*)GetPointerByString(tr1);
	e_object* TR2 = (e_object*)GetPointerByString(tr2);
	e_object* TR3 = (e_object*)GetPointerByString(tr3);
	AddSystem(new Socket(name, SRC, ip, TR1, TR2, TR3));
}

void E_system::Create_DCbus(char* line)
{
	char name[100];
	char source[100];
	sscanf(line + 7, "%s %s", name, source);
	e_object* SRC = (e_object*)GetPointerByString(source);
	DCbus* new_dc = (DCbus*)AddSystem(new DCbus(name, SRC));
};

void E_system::Create_ACbus(char *line)
{
	char name[100];
	char source[100];
	double voltage;

	sscanf(line+7,"%s %lf %s", name, &voltage, source);
	e_object *SRC=(e_object*)GetPointerByString(source);
	ACbus *new_dc = (ACbus*)AddSystem(new ACbus(name, voltage, SRC));
}

void E_system::Create_Inverter(char *line)
{
	char name[100];
	char source[100];
	double voltage;

	sscanf(line + 10,"%s %lf %s", name, &voltage, source);
	e_object *SRC=(e_object*)GetPointerByString(source);
	ACInverter *new_dc = (ACInverter*) AddSystem(new ACInverter(name, voltage, SRC));
}

void E_system::Create_Diode(char *line)
{
	char name[100];
	char source[100];
	double NominalTemperature;
	double SaturationCurrent;
	sscanf(line + 7, "%s %s %lf %lf", name, source, &NominalTemperature, &SaturationCurrent);
	e_object *Source = (e_object*)GetPointerByString(source);
	AddSystem(new Diode(name, Source, NominalTemperature, SaturationCurrent));
}


/// 
/// Create a new PanelSDK-based electric light, consisting of a OAPI lightemitter and beacon
/// 
///	Config file creation string is as follows
/// 
///	|Field					|Unit						|Type						|
///	|-----------------------|---------------------------|---------------------------|
///	|name					|n / a						|string						|
///	|sourceName				|n / a						|string						|
///	|flashing				|n / a						|string						|
///	|OnTime					|seconds					|double						|
///	|OffTime				|seconds					|double						|
///	|<position>				|meters						|VECTOR3					|
///	|<direction>			|meters						|VECTOR3					|
///	|CutoffRange			|meters						|double						|
///	|ConstantAttenuation	|meters						|double						|
///	|LinearAttenuation		|meters^-1					|double						|
///	|QuadraticAttenuation	|meters^-2					|double						|
///	|Umbra					|radians					|double						|
///	|Penumbra				|radians					|double						|
///	|< % f % f % f % f>		|n / a						|COLOUR4					|
///	|<% f% f% f% f>			|n / a						|COLOUR4					|
///	|<% f% f% f% f>			|n / a						|COLOUR4					|
///	|% lf					|Watts						|double						|
///	|% lf					|Volts						|double						|
/// 
/// Example:
/// <LIGHT> SPOTLIGHT DC_DUMMY 0 0.0 0.0 <-1.439 1.390 0.920> <0.0045 -0.0046 0.9999> 5000.0 0.0 0.0 0.0015 0.174533 0.5 <1.0 0.945 0.878 0.0> <1.0 0.945 0.878 0.0> <0.0 0.0 0.0 0.0> 100.0 115.0
/// 
/// \ingroup PanelSDK
/// \brief Create a new electric light
/// <param name="line">A single line begining with "<LIGHT>" followed by the fields shown in the table above</param>
///
void E_system::Create_ElectricLight(char* line)
{
	char name[100];
	char sourceName[100];
	e_object* powerSource = nullptr;
	int flashing = 0;
	double onTime = 0;
	double offTime = 0;
	VECTOR3 pos = _V(0, 0, 0);
	VECTOR3 dir = _V(1, 0, 0);
	double range = 0;
	double att0 = 0;
	double att1 = 0;
	double att2 = 0;
	double umbra = 0;
	double penumbra = 0;
	COLOUR4 diffuse = { 0, 0, 0, 0};
	COLOUR4 specular = { 0, 0, 0, 0 };
	COLOUR4 ambient = { 0, 0, 0, 0 };
	double powerDraw = 0;
	double nomVoltage = 0;

	sscanf(line + 7, "%s %s %d %lf %lf <%lf %lf %lf> <%lf %lf %lf> %lf %lf %lf %lf %lf %lf <%f %f %f %f> <%f %f %f %f> <%f %f %f %f> %lf %lf",
		&name, &sourceName, &flashing, &onTime, &offTime,
		&pos.x, &pos.y, &pos.z,
		&dir.x, &dir.y, &dir.z,
		&range, &att0, &att1, &att2, &umbra, &penumbra, 
		&diffuse.r, &diffuse.g, &diffuse.b, &diffuse.a,
		&specular.r, &specular.g, &specular.b, &specular.a, 
		&ambient.r, &ambient.g, &ambient.b, &ambient.a,
		&powerDraw, &nomVoltage);


	powerSource = (e_object*)GetPointerByString(sourceName);
	ElectricLight* newLight = new ElectricLight(name, powerSource, (bool)flashing, onTime, offTime, this->Vessel, pos, dir, range, att0, att1, att2, umbra, penumbra, diffuse, specular, ambient, powerDraw, nomVoltage);
	AddSystem(newLight);
}

void E_system::Create_Battery(char *line)
{
	char name[100];
	double power, operating_voltage, resistance, volume, isolation, mass, temp = 0;
	vector3 pos;
	char source[100];
	char targetName[100];

	sscanf(line + 10, "%s %lf %lf %lf %s %lf <%lf %lf %lf> %lf %lf %lf %s", name, &power, &operating_voltage, &resistance, source, &temp, &pos.x, &pos.y, &pos.z, &volume, &isolation, &mass, targetName);
	e_object* SRC=(e_object*)GetPointerByString(source);
	h_Tank* batcase = (h_Tank*)GetPointerByString(targetName);
	Battery *new_b=(Battery*)AddSystem(new Battery(name, SRC, power, operating_voltage, resistance, batcase));

	new_b->parent = this;

	if (temp > 0)
	{
		P_thermal->AddThermalObject(new_b);
		new_b->isolation = isolation;
		new_b->mass = mass;
		new_b->Area = (1.0 / 4.0 * volume);
		new_b->pos = pos;
		new_b->SetTemp(temp);
	}
} 

void E_system::Create_FCell(char *line) {

	char name[100];
	double power;
	char source1[100];
	char source2[100];
	char source3[100];
	char source4[100];
	vector3 pos;
	int status;

	sscanf(line+7, "%s %i <%lf %lf %lf> %lf %s %s %s %s", name, &status, &pos.x, &pos.y, &pos.z,
		&power, source1, source2, source3, source4);

	h_Valve* O_SRC = (h_Valve*)P_hydraulics->GetPointerByString(source1);
	h_Valve* H_SRC = (h_Valve*)P_hydraulics->GetPointerByString(source2);
	h_Valve* WATER = (h_Valve*)P_hydraulics->GetPointerByString(source3);
	h_Tank* N2 = (h_Tank*)P_hydraulics->GetPointerByString(source4);

	FCell *new_fc = (FCell*)AddSystem(new FCell(name, status, pos, O_SRC, H_SRC, WATER, (float)power, N2));
}

void E_system::Build() {

	char *line;

	line = ReadConfigLine();
	while (!Compare(line,"</ELECTRIC>")) {
		if (Compare(line,"<BATTERY>"))
			Create_Battery(line);
		else if (Compare(line,"<FCELL>"))
			Create_FCell(line);
		else if (Compare(line,"<DCBUS>"))
			Create_DCbus(line);
		else if (Compare(line,"<ACBUS>"))
			Create_ACbus(line);
		else if (Compare(line,"<INVERTER>"))
			Create_Inverter(line);
		else if (Compare(line,"<SOCKET>"))
			Create_Socket(line);
		else if (Compare(line,"<COOLING>"))
			Create_Cooling(line);
		else if (Compare(line,"<ATMREGEN>"))
			Create_AtmRegen(line);
		else if (Compare(line,"<BOILER>"))
			Create_Boiler(line);
		else if (Compare(line,"<PUMP>"))
			Create_Pump(line);
		else if (Compare(line, "<DIODE>"))
			Create_Diode(line);
		else if (Compare(line, "<LIGHT>"))
			Create_ElectricLight(line);

		line =ReadConfigLine();
	}
}

void* E_system::GetPointerByString(char *query)
{
if (Compare(query,"NOPOWER")) return NULL;
if (Compare(query,"ELECTRIC")) query=query+9;
if (Compare(query,"HYDRAULIC"))
		return P_hydraulics->GetPointerByString(query+10);
return ship_system::GetPointerByString(query);
};

void* e_object::GetComponent(char *name) {
	
	if (Compare(name,"VOLTS"))
		return (void*)&Volts;
	if (Compare(name,"AMPS"))
		return (void*)&Amperes;
	if (Compare(name,"SRC"))
		return (void*)&SRC;

	return NULL;
}

void* Battery::GetComponent(char *component_name) {
	
	void *norm=e_object::GetComponent(component_name);
	if (norm) return norm;

	if (Compare(component_name, "TEMP"))
		return (void*)&Temp;
	if (Compare(component_name, "HEAT"))
		return (void*)&batheat;

	BuildError(2);
	return NULL;
}

void* FCell::GetComponent(char *component_name) {

	void *norm=e_object::GetComponent(component_name);
	if (norm) return norm;

	if (Compare(component_name,"TEMP"))
		return (void*)&Temp;
	if (Compare(component_name,"CONDENSERTEMP"))
		return (void*)&condenserTemp;
	if (Compare(component_name,"START"))
		return (void*)&start_handle;
	if (Compare(component_name,"PURGE"))
		return (void*)&purge_handle;
	if (Compare(component_name,"DPH"))
		return (void*)&cloggVoltageDrop;
	if (Compare(component_name,"H2FLOW"))
		return (void*)&H2_flowPerSecond;
	if (Compare(component_name,"O2FLOW"))
		return (void*)&O2_flowPerSecond;

	BuildError(2);
	return NULL;
}

void* Socket::GetComponent(char *component_name)
{
if (Compare(component_name,"CONNECT"))
	return (void*)&socket_handle;
BuildError(2);
return NULL;
};

void* Cooling::GetComponent(char *component_name) {

	int item;
	char itemcomp[50];

	if (Compare(component_name,"PUMP"))
		return (void*)&h_pump;
	if (Compare(component_name,"HMAX"))
		return (void*)&handle_max;
	if (Compare(component_name,"HMIN"))
		return (void*)&handle_min;
	if (Compare(component_name,"ISON"))
		return (void*)&pumping;
	if (Compare(component_name,"MAXT"))
		return (void*)&max;
	if (Compare(component_name,"MINT"))
		return (void*)&min;
	if (Compare(component_name,"TEMP"))
		return (void*)&coolant_temp;
	if (Compare(component_name,"ISOLATION"))
		return (void*)&isolation;

	sscanf (component_name, "%i:%s", &item, itemcomp);
	if (Compare(itemcomp, "BYPASSED"))
		return (void*)&bypassed[item];
		
	BuildError(2);
	return NULL;
}

void* AtmRegen::GetComponent(char *component_name) {

	if (Compare(component_name,"PUMPH2O"))
		return (void*)&h_pumpH2o;
	if (Compare(component_name,"PUMP"))
		return (void*)&h_pump;
	if (Compare(component_name,"ISON"))
		return (void*)&pumping;
	if (Compare(component_name,"CO2REMOVALRATE"))
		return (void*)&co2removalrate;
	if (Compare(component_name,"FANCAP"))
		return (void*)&fan_cap;

	BuildError(2);
	return NULL;
}

void* Pump::GetComponent(char *component_name) {

	if (Compare(component_name,"PUMP"))
		return (void*)&h_pump;
	if (Compare(component_name,"ISON"))
		return (void*)&pumping;
	if (Compare(component_name,"FANCAP"))
		return (void*)&fan_cap;
	if (Compare(component_name, "FLOW"))
		return (void*)&flow;

	BuildError(2);
	return NULL;
}

void* Boiler::GetComponent(char *component_name) {

	if (Compare(component_name,"PUMP"))
		return (void*)&h_pump;
	if (Compare(component_name,"HMAX"))
		return (void*)&handleMax;
	if (Compare(component_name,"HMIN"))
		return (void*)&handleMin;
	if (Compare(component_name,"MAXV"))
		return (void*)&valueMax;
	if (Compare(component_name,"MINV"))
		return (void*)&valueMin;
	if (Compare(component_name,"ISON"))
		return (void*)&pumping;
	if (Compare(component_name, "PWMCYC"))
		return (void*)&pwmThrotle;

	BuildError(2);
	return NULL;
}
