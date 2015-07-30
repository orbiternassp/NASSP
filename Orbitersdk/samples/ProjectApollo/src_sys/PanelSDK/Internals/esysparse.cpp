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

#include <stdio.h>
#include "esystems.h"
#include "../BUILD.H"

void E_system::Create_Boiler(char *line) {

	char name[100], source[100], targetName[100], typeName[100];
	int pump, type;
	double watts, ewatts, valueMin, valueMax;

	sscanf(line + 8,"%s %i %s %lf %lf %s %lf %lf %s",
		name, &pump, source, &watts, &ewatts, typeName, &valueMin, &valueMax, targetName);

	ship_object* so = (ship_object*) GetPointerByString(targetName) ;
	therm_obj *t = so->GetThermalInterface();
	e_object *src = (e_object*) GetPointerByString(source);
	if (Compare(typeName, "TEMP"))
		type = 0;
	else if (Compare(typeName, "PRESS"))
		type = 1;

	AddSystem(new Boiler(name, pump, src, watts, ewatts, type, valueMin, valueMax, t));
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

void E_system::Create_Cooling(char *line)
{
char name[100];
char source[100];
double lngt;
//int num;
int pump;
double term,max,min;
therm_obj *TR1;
e_object *P_SRC;
ship_object* NON_t;
sscanf(line+9,"%s %i %s %lf %lf %lf",name,&pump,source,&term,&min,&max);
P_SRC=(e_object*)GetPointerByString(source);
Cooling *new_c=(Cooling*)AddSystem(new Cooling(name,pump,P_SRC,term,min,max));
line=ReadConfigLine();
while (!Compare(line,"</COOLING>")){
		sscanf(line,"%s %lf",source,&lngt);
		NON_t=(ship_object*)GetPointerByString(source);
		if (NON_t) TR1=NON_t->GetThermalInterface();
		if (TR1)
			new_c->AddObject(TR1,lngt);
		line=ReadConfigLine();
		}

}
void E_system::Create_Socket(char *line)
{
char name[100];
char source[100];
int ip;
char tr1[100],tr2[100],tr3[100];

sscanf(line+8,"%s %i %s %s %s %s",name,&ip,source,tr1,tr2,tr3);
e_object *SRC=(e_object*)GetPointerByString(source);
e_object *TR1=(e_object*)GetPointerByString(tr1);
e_object *TR2=(e_object*)GetPointerByString(tr2);
e_object *TR3=(e_object*)GetPointerByString(tr3);
AddSystem(new Socket(name,SRC,ip,TR1,TR2,TR3));

}
void E_system::Create_DCbus(char *line)
{
char name[100];
char source[100];
sscanf(line+7,"%s %s",name,source);
e_object *SRC=(e_object*)GetPointerByString(source);
DCbus *new_dc=(DCbus*)AddSystem(new DCbus(name,SRC));
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

void E_system::Create_Battery(char *line)
{
	char name[100];
	double power, operating_voltage, resistance;
	char source[100];
	sscanf(line+9,"%s %lf %lf %lf %s",name, &power, &operating_voltage, &resistance, source);
	e_object* SRC=(e_object*)GetPointerByString(source);
	Battery *new_b=(Battery*)AddSystem(new Battery(name, SRC, power, operating_voltage, resistance));
} 

void E_system::Create_FCell(char *line) {

	char name[100];
	double power;
	char source1[100];
	char source2[100];
	char source3[100];
	vector3 pos;
	int status;

	sscanf(line+7, "%s %i <%lf %lf %lf> %lf %s %s %s", name, &status, &pos.x, &pos.y, &pos.z,
		&power, source1, source2, source3);

	h_Valve* O_SRC = (h_Valve*)P_hydraulics->GetPointerByString(source1);
	h_Valve* H_SRC = (h_Valve*)P_hydraulics->GetPointerByString(source2);
	h_Valve* WATER = (h_Valve*)P_hydraulics->GetPointerByString(source3);

	FCell *new_fc = (FCell*)AddSystem(new FCell(name, status, pos, O_SRC, H_SRC, WATER, (float)power));
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
	if (Compare(component_name,"RUNNING"))
		return (void*)&running;
	if (Compare(component_name,"DPH"))
		return (void*)&clogg;
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

	BuildError(2);
	return NULL;
}
