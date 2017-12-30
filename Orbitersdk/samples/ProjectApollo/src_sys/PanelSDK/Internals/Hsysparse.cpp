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

#include "hsystems.h"
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "../build.h"

void* ship_system::GetPointerByString(char *query)
{
ship_object* query_object;
int len=0;
for (int i=0; i < (int)strlen(query); i++) {
	if (query[i]==':') {
		len = i;
		break;
	}
}

if (len){
	char buf[1000];
	for (int i=0;i<len;i++) {
		buf[i]=query[i];
	}
	buf[len] = '\0';;
	query_object=GetSystemByName(buf);
	if (query_object)
			return query_object->GetComponent(query+len+1);
	BuildError(1);
	return NULL;//requested a component of a non existent object
}
query_object=GetSystemByName(query);//not a component search, just the object maybe?
if (query_object)
	return query_object;
BuildError(1);
return NULL;
}
void H_system::Create_h_crew(char *line)
{
char name[100];
int nmb;
char source[100];
h_Tank *SRC;
sscanf(line+6,"%s %i %s",name,&nmb,source);
SRC=(h_Tank*)GetPointerByString(source);
AddSystem(new h_crew(name,nmb,SRC));
}

void H_system::Create_h_Radiator(char *line) {

	char name[100];
	h_Radiator *new_one;
	vector3 pos;
	vector3 dir;
	double volume, isol, mass, temp;

	sscanf(line+10, " %s  <%lf %lf %lf> %lf ",
		   name,
		   &pos.x,&pos.y,&pos.z,&temp);

	line = ReadConfigLine();
	sscanf(line," %lf %lf %lf", &volume, &isol, &mass);
	new_one = (h_Radiator*)AddSystem(new h_Radiator(name, pos, volume, isol));

	// Debugging thermal management
	//if (!strcmp(name, "ECSRADIATOR1"))
	//	P_thermal->AddThermalObject(new_one, true);
	//else 

	P_thermal->AddThermalObject(new_one);

	new_one->isolation = 1.0;
	new_one->Area = (1.0 / 4.0 * volume);
	new_one->mass = mass;
	new_one->SetTemp(temp);
	new_one->parent = this;
}

void H_system::Create_h_HeatExchanger(char *line) {

	char name[100], sourceName[100], targetName[100];
	double length, tempMin, tempMax;
	int pump;

	sscanf(line + 15, " %s %i %lf %s %s %lf %lf",
		   name, &pump, &length, sourceName, targetName, &tempMin, &tempMax);

	ship_object* so = (ship_object*) GetPointerByString(targetName) ;
	therm_obj *target = so->GetThermalInterface();

	so = (ship_object*) GetPointerByString(sourceName) ;
	therm_obj *source = so->GetThermalInterface();

	AddSystem(new h_HeatExchanger(name, pump, length, source, target, tempMin, tempMax));
}

void H_system::Create_h_Evaporator(char *line) {

	char name[100], liquidSourceName[100], targetName[100], tempControlName[100];
	int pump;
	double targetTemp, turnOnTemp;
	h_Evaporator *new_one;

	sscanf(line + 12, " %s %i %s %lf %s %lf %s",
		   name, &pump, targetName, &targetTemp, liquidSourceName, &turnOnTemp, tempControlName);

	ship_object* so = (ship_object*) GetPointerByString(targetName) ;
	therm_obj *target = so->GetThermalInterface();

	h_Valve *liquidSource = (h_Valve*)GetPointerByString(liquidSourceName);

	so = (ship_object*) GetPointerByString(tempControlName) ;
	therm_obj *tempControl = so->GetThermalInterface();
	
	new_one = (h_Evaporator*) AddSystem(new h_Evaporator(name, pump, target, targetTemp, liquidSource, turnOnTemp, tempControl));
	new_one->parent = this;
}

void H_system::Create_h_MixingPipe(char *line) {

	char name[100], in1Name[100], in2Name[100], outName[100];
	int pump;
	double targetTemp;

	sscanf(line + 12, " %s %i %s %s %s %lf",
		   name, &pump, in1Name, in2Name, outName, &targetTemp);

	h_Valve *in1 = (h_Valve*)GetPointerByString(in1Name);
	h_Valve *in2 = (h_Valve*)GetPointerByString(in2Name);
	h_Valve *out = (h_Valve*)GetPointerByString(outName);

	AddSystem(new h_MixingPipe(name, pump, in1, in2, out, targetTemp));
}

void H_system::Create_h_Valve(char *line) {

	char name[100], parentName[100];
	int i_open = 0;
	int i_ct = 0;
	float i_size = 0;
	h_Tank *parent;

	if (sscanf(line + 7, " %s", name) <= 0)
		name[0] = '\0';

	line = ReadConfigLine();
	while (strnicmp(line, "</VALVE>", 8)) {

		sscanf(line, "%s %i %i %f",
			parentName, &i_open, &i_ct, &i_size);

		line = ReadConfigLine();
	}

	parent = (h_Tank*)GetPointerByString(parentName);

	AddSystem(new h_Valve(name, i_open, i_ct, i_size, parent));
}

void H_system::Create_h_Vent(char *line) {

	char name[100];
	h_Vent *new_one;
	vector3 pos=_vector3(0.0, 0.0, 0.0);
	vector3 dir;
	double volume,isol = 0;
	int one, two, three, four;
	sscanf (line+6, " %s %i %i %i %i",
		name,
		&one, &two, &three, &four);
	new_one = (h_Vent*)AddSystem(new h_Vent(name, pos));
	new_one->IN_valve.open = one;
	new_one->OUT_valve.open = two;
	new_one->OUT2_valve.open = three;
	new_one->LEAK_valve.open = four;
	new_one->space.Void(); //empty the space

	line = ReadConfigLine();
	while (strnicmp(line,"</VENT>",7)) {
		sscanf (line, "<%lf %lf %lf> <%lf %lf %lf> %lf",
			&pos.x, &pos.y, &pos.z,
			&dir.x, &dir.y, &dir.z,
			&volume);
		new_one->AddVent(pos,dir,volume);

		line=ReadConfigLine();
	}
}

void H_system::Create_h_Tank(char *line) {

	char name[100], valvename[100];
	h_Tank *new_one;
	vector3 pos;
	double volume,isol=0;
	float size = 0;
	int open;
	h_Valve *valve;

	sscanf (line+6, " %s <%lf %lf %lf> %lf %lf",
								name,
								&pos.x,
								&pos.y,
								&pos.z,
								&volume, &isol);

	new_one=(h_Tank*)AddSystem(new h_Tank(name,pos, volume));

	h_substance loaded_sub;
	new_one->space.Void(); //empty the space

	while (strnicmp(line,"</TANK>",7)) {

		if (!strnicmp (line, "CHM",3 )) {
            sscanf (line+3, "   %i %lf %lf %lf",
						&loaded_sub.subst_type,
						&loaded_sub.mass,
						&loaded_sub.vapor_mass,
						&loaded_sub.Q);
			new_one->space+=loaded_sub;

		} else if (!strnicmp (line, "VALVE", 5)) {
            sscanf (line + 5, " %s %i %f",
						valvename, &open, &size);

			valve = 0;
			if (!strnicmp (valvename, "IN", 2)) {
				valve = &(new_one->IN_valve);
			} else if (!strnicmp (valvename, "OUT2", 4)) {
				valve = &(new_one->OUT2_valve);
			} else if (!strnicmp (valvename, "OUT", 3)) {
				valve = &(new_one->OUT_valve);
			} else if (!strnicmp (valvename, "LEAK", 4)) {
				valve = &(new_one->LEAK_valve);
			} else {
				BuildError(2); //no such component
			}

			if (valve) {
				valve->open = open;
				valve->size = size;
			}
		}
		do {
			line = ReadConfigLine();
		} while (line == NULL);
	}
	new_one->mass=new_one->space.GetMass();//get all the mass,etc..
	new_one->space.GetMaxSub();//recompute sub_number;
	new_one->energy=new_one->space.GetQ();//sum up Qs
	new_one->Original_volume=volume;
	P_thermal->AddThermalObject(new_one);
	if (isol)
		new_one->isolation=isol;
	else
		new_one->isolation=1.0;

	new_one->Area=pow(3.0/4.0*PI*volume/1000,0.3333); //radius of tank
	new_one->Area=2*PI*new_one->Area*new_one->Area;//projection circle ois 2*PI*R
	new_one->parent=this;
}

void H_system::Create_h_Pipe(char *line) {

	h_Valve *in;
	h_Valve *out;
	char name[100];
	char in_valve[100];
	char out_valve[100];
	char type[100];
	double max=0;
	double min=0;
	char is_two[100];

	if (sscanf(line + 6, " %s", name) <= 0)
		name[0] = '\0';

	line = ReadConfigLine();
	while (!Compare(line,"</PIPE>")) {type[0]=0;is_two[0]=0;
		sscanf (line, "%s %s %s %lf %lf %s",in_valve,out_valve,type,&max,&min,is_two);

		int two_way=1;
		if (Compare(type,"ONEWAY")) two_way=0;
		if (Compare(is_two,"ONEWAY")) two_way=0;

		in=(h_Valve*)GetPointerByString(in_valve);
		out=(h_Valve*)GetPointerByString(out_valve);

		if (Compare(type,"PREG"))
			AddSystem(new h_Pipe(name,in,out,1,max,min,two_way));
		else if (Compare(type,"BURST"))
			AddSystem(new h_Pipe(name,in,out,2,max,min,two_way));
		else if (Compare(type,"PVALVE"))
			AddSystem(new h_Pipe(name,in,out,3,max,min,two_way));
		else
			AddSystem(new h_Pipe(name,in,out,0,0,0,two_way));
		
		line=ReadConfigLine();
	}
}

void H_system::Create_h_CO2Scrubber(char *line) {

	h_Valve *in;
	h_Valve *out;
	char name[100];
	char in_valve[100];
	char out_valve[100];
	double flowMax = 0;

	sscanf(line + 13, " %s %lf", name, &flowMax);

	line = ReadConfigLine();
	while (!Compare(line, "</CO2SCRUBBER>")) {
		sscanf(line, "%s %s", in_valve, out_valve);

		in = (h_Valve*)GetPointerByString(in_valve);
		out = (h_Valve*)GetPointerByString(out_valve);

		AddSystem(new h_CO2Scrubber(name, flowMax, in, out));

		line = ReadConfigLine();
	}
}

void H_system::Create_h_WaterSeparator(char *line) {

	h_Valve *in;
	h_Valve *out;
	h_Valve *h2o;
	char name[100];
	char in_valve[100];
	char out_valve[100];
	char h2o_valve[100];
	double flowMax = 0;

	sscanf(line + 8, " %s %lf", name, &flowMax);

	line = ReadConfigLine();
	while (!Compare(line, "</H2OSEP>")) {
		sscanf(line, "%s %s %s", in_valve, out_valve, h2o_valve);

		in = (h_Valve*)GetPointerByString(in_valve);
		out = (h_Valve*)GetPointerByString(out_valve);
		h2o = (h_Valve*)GetPointerByString(h2o_valve);

		AddSystem(new h_WaterSeparator(name, flowMax, in, out, h2o));

		line = ReadConfigLine();
	}
}

void H_system::Build() {
	
	char *line;

	line = ReadConfigLine();
	while (!Compare(line,"</HYDRAULIC>")) {
		if(Compare(line,"<TANK>"))
			Create_h_Tank(line);
		else if(Compare(line,"<PIPE>"))
			Create_h_Pipe(line);
		else if(Compare(line,"<VENT>"))
			Create_h_Vent(line);
		else if(Compare(line,"<RADIATOR>"))
			Create_h_Radiator(line);
		else if(Compare(line,"<CREW>"))
			Create_h_crew(line);
		else if(Compare(line,"<HEATEXCHANGER>"))
			Create_h_HeatExchanger(line);
		else if(Compare(line,"<EVAPORATOR>"))
			Create_h_Evaporator(line);
		else if(Compare(line,"<MIXINGPIPE>"))
			Create_h_MixingPipe(line);
		else if (Compare(line, "<VALVE>"))
			Create_h_Valve(line);
		else if (Compare(line, "<CO2SCRUBBER>"))
			Create_h_CO2Scrubber(line);
		else if (Compare(line, "<H2OSEP>"))
			Create_h_WaterSeparator(line);

		line = ReadConfigLine();
	}
}

void* h_Pipe::GetComponent(char *component_name) {

	if (!strnicmp (component_name, "FLOWMAX", 7))
		return (void*)&flowMax;
	if (!strnicmp (component_name, "FLOW", 4))
		return (void*)&flow;
	if (!strnicmp (component_name, "PRESSMAX", 8))
		return (void*)&P_max;
	if (!strnicmp (component_name, "PRESSMIN", 8))
		return (void*)&P_min;

	BuildError(2);	//no such component
	return NULL;
}

void* h_Valve::GetComponent(char *component_name) {

	if (!strnicmp (component_name, "OPEN", 4))//handle to open it
		return (void*)&h_open;
	if (!strnicmp (component_name, "PZ", 2)) //moving flag
		return (void*)&pz;
	if (!strnicmp (component_name, "ISOPEN", 6)) //on/off
		return (void*)&open;
	if (!strnicmp (component_name, "SIZE", 4)) //size
		return (void*)&size;

	BuildError(2); //no such component
	return NULL;
}

void* h_Tank::GetComponent(char *component_name) {

	if (!strnicmp (component_name, "OUT2", 4)) {//talks about the out valve
		if (strlen(component_name) == 4)
				return &OUT2_valve;//the valve is all it wants
		//or maybe something else
		return OUT2_valve.GetComponent(component_name + 5);
		};
	if (!strnicmp (component_name, "OUT", 3)) {//talks about the out valve
		if (strlen(component_name) == 3)
				return &OUT_valve;//the valve is all it wants
		//or maybe something else
		return OUT_valve.GetComponent(component_name + 4);
		};
	if (!strnicmp (component_name, "IN", 2)) {
 		if (strlen(component_name) == 2)
				return &IN_valve;//the valve is all it wants
		//or maybe something else
		return IN_valve.GetComponent(component_name + 3);
		};
	if (!strnicmp (component_name, "LEAK", 4)) {
 		if (strlen(component_name) == 4)
				return &LEAK_valve;//the valve is all it wants
		//or maybe something else
		return LEAK_valve.GetComponent(component_name + 5);
		};
	if (!strnicmp (component_name, "TEMP",4 ))
		return &(space.Temp);
	if (!strnicmp(component_name, "ENERGY", 6))
		return &(space.Q);
	if (!strnicmp (component_name, "MASS",4 ))
		 return &(space.total_mass);
	if (!strnicmp (component_name, "PRESS",5 ))
		 return &(space.Press);
	if (!strnicmp (component_name, "VOLUME",6 ))
		 return &(space.Volume);
	if (!strnicmp (component_name, "O2_PPRESS",9 ))
		 return &(space.composition[0].p_press);
	if (!strnicmp (component_name, "H2_PPRESS",9 ))
		 return &(space.composition[1].p_press);
	if (!strnicmp (component_name, "N2_PPRESS",9 ))
		 return &(space.composition[3].p_press);
	if (!strnicmp (component_name, "CO2_PPRESS",10 ))
		 return &(space.composition[4].p_press);
	if (!strnicmp (component_name, "H2O_PPRESS",10 ))
		 return &(space.composition[2].p_press);
	if (!strnicmp (component_name, "H2_VAPORMASS", 12))
		 return &(space.composition[1].vapor_mass);
	if (!strnicmp (component_name, "O2_VAPORMASS", 12))
		 return &(space.composition[0].vapor_mass);
	if (!strnicmp (component_name, "H2O_MASS", 8))
		 return &(space.composition[2].mass);

	BuildError(2);
	return NULL;
}

void* h_Radiator::GetComponent(char *component_name) {
	
	if (!strnicmp (component_name, "TEMP", 4))
		return (void*)&Temp;
	if (!strnicmp (component_name, "RAD", 3))
		return (void*)&rad;

	BuildError(2);
	return NULL;
}

void* h_HeatExchanger::GetComponent(char *component_name) {
	
	if (Compare(component_name,"PUMP"))
		return (void*)&h_pump;
	if (Compare(component_name,"POWER"))
		return (void*)&power;
	if (Compare(component_name,"LENGTH"))
		return (void*)&length;
	if (Compare(component_name,"TEMPMIN"))
		return (void*)&tempMin;
	if (Compare(component_name,"TEMPMAX"))
		return (void*)&tempMax;

	BuildError(2);
	return NULL;
}

void* h_Evaporator::GetComponent(char *component_name) {
	
	if (Compare(component_name,"PUMP"))
		return (void*)&h_pump;
	if (Compare(component_name,"VALVE"))
		return (void*)&h_valve;
	if (Compare(component_name,"THROTTLE"))
		return (void*)&throttle;
	if (Compare(component_name,"STEAMPRESSURE"))
		return (void*)&steamPressure;

	BuildError(2);
	return NULL;
}

void* h_MixingPipe::GetComponent(char *component_name) {
	
	if (Compare(component_name,"PUMP"))
		return (void*)&h_pump;
	if (Compare(component_name,"RATIO"))
		return (void*)&ratio;

	BuildError(2);
	return NULL;
}

void* h_crew::GetComponent(char *component_name) {

	if (!strnicmp (component_name, "NUMBER", 6))
		return (void*)&number;

	BuildError(2);	//no such component
	return NULL;
}

void* h_CO2Scrubber::GetComponent(char *component_name) {

	if (Compare(component_name, "FLOWMAX"))
		return (void*)&flowMax;
	if (Compare(component_name, "FLOW"))
		return (void*)&flow;
	if (Compare(component_name, "CO2REMOVALRATE"))
		return (void*)&co2removalrate;

	BuildError(2);	//no such component
	return NULL;
}

void* h_WaterSeparator::GetComponent(char *component_name) {

	if (Compare(component_name, "FLOWMAX"))
		return (void*)&flowMax;
	if (Compare(component_name, "FLOW"))
		return (void*)&flow;

	BuildError(2);	//no such component
	return NULL;
}