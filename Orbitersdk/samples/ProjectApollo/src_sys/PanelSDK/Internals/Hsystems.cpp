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
#include "orbitersdk.h"
#include <stdio.h>
#include <math.h>
//const float CONST_R=8.31904f/1000.0f;
//const float TEMP_PRESS_RATIO=0.07;

/// \todo For testing
//extern FILE *PanelsdkLogFile;
//int Compare(char* ln, char* trg);


h_object::h_object()

{
	next=NULL;
}

void ship_object::refresh(double dt)

{
}

void ship_object::Save(FILEHANDLE scn)

{
}

void ship_object::Load(FILEHANDLE scn)

{
}

void* ship_object::GetComponent(char *component_name)

{
	return NULL;
}

ship_system::ship_system()

{
	List.next=NULL;
}

ship_system::~ship_system()
{
	ship_object *runner;
	ship_object *second_runner;

	runner=List.next;
	if (runner){
		second_runner=runner;
		while (runner->next) { 
			second_runner=runner;
			while ((second_runner->next)&&(second_runner->next->next)) 
				second_runner=second_runner->next;
				if (second_runner->next && second_runner->next->deletable) delete second_runner->next;
				second_runner->next=NULL;
			};
		if (second_runner->deletable)
			delete second_runner;
	}
};

ship_object* ship_system::AddSystem(ship_object *object)
{ 
	ship_object *runner;

	runner=&List;
	while (runner->next) runner=runner->next;
	runner->next=object;
	object->next=NULL;
	return object;
}

void ship_system::DeleteSystem(ship_object *object)
{
	ship_object *runner;
	runner=&List;
	while ((object!=runner->next)&&(runner->next)) runner=runner->next;
	if (object==runner->next) {
		runner->next=object->next;
		BroadcastDemision(object);
		if (object->deletable)
			 delete object;
	}
}

void ship_system::BroadcastDemision(ship_object * gonner)
{
 ship_object *runner;
 runner=List.next;
 while (runner){
	 runner->BroadcastDemision(gonner);
	 runner=runner->next;
 					}
};
void ship_system::Refresh(double dt)
{ ship_object *runner;
 runner=List.next;
 while (runner){ runner->refresh(dt);
				 runner=runner->next;}
};

ship_object* ship_system::GetSystemByName(char *r_name)
{ship_object *runner;
 runner=List.next;
 while (runner){ if (!stricmp (runner->name, r_name)) return runner;
				 runner=runner->next;}
return NULL;
};
void ship_system::SetMaxStage(char *name, int stage)
{
ship_object *runner;
 runner=List.next;
 while (runner){ if (!strnicmp (runner->name, name,strlen(name)))runner->max_stage=stage;
				 runner=runner->next;}
}
void ship_system::ConfigStage(int stage)
{ship_object *runner;
 ship_object *gonner;
 runner=List.next;
 while (runner){ gonner=runner;
				runner=runner->next;
				if (gonner->max_stage<=stage) DeleteSystem(gonner);

				};
}

void H_system::Save(FILEHANDLE scn) { 
	
	ship_object *runner;
	runner = List.next;

	oapiWriteScenario_string(scn, "<HYDRAULIC>","");
	while (runner) { 
		runner->Save(scn);
		runner = runner->next;
	}
	oapiWriteScenario_string(scn, "</HYDRAULIC>","");
}

void H_system::Load(FILEHANDLE scn) { 
	
	ship_object *runner;
	runner=List.next;

	char *line;
	char string1[100];
	vector3 pos;
	double volume, temp, pmax, pmin, flowmax, throttle, ratio, length;
	int one, two, three, four;
	float size1, size2, size3, size4;
 
	oapiReadScenario_nextline (scn, line);
	while (strnicmp(line, "</HYDRAULIC>", 12)) {
		if (!strnicmp (line, "<TANK>", 6)) {
			sscanf(line + 6, "%s %lf %i %i %i %i %f %f %f %f", 
				   string1, &volume, 
				   &one, &two, &three, &four,
				   &size1, &size2, &size3, &size4);
			h_Tank *tank = (h_Tank*) GetSystemByName(string1);
			if (tank) {
				tank->Load(scn);
				tank->space.Volume = volume;

				tank->IN_valve.open = one;
				tank->OUT_valve.open = two;
				tank->OUT2_valve.open = three;
				tank->LEAK_valve.open = four;

				tank->IN_valve.size = size1;
				tank->OUT_valve.size = size2;
				tank->OUT2_valve.size = size3;
				tank->LEAK_valve.size = size4;
			}

		} else if (!strnicmp (line, "<RADIATOR>", 10)) {
			sscanf(line + 10, "%s %lf %lf", string1, &temp, &length);
			h_Radiator *rad = (h_Radiator*) GetSystemByName(string1);
			if (rad) {
				rad->SetTemp(temp);
				rad->rad = length;
			}

		} else if (!strnicmp (line, "<CREW>", 6)) {
			sscanf(line + 6, "%s %i", string1, &one);
			h_crew *crew = (h_crew*) GetSystemByName(string1);
			if (crew) {
				crew->number = one;
			}
		
		} else if (!strnicmp (line, "<PIPE>", 6)) {
			sscanf(line + 6, "%s %lf %lf %lf", string1, &pmax, &pmin, &flowmax);
			h_Pipe *pipe = (h_Pipe*) GetSystemByName(string1);
			if (pipe) {
				pipe->P_max = pmax;
				pipe->P_min = pmin;
				pipe->flowMax = flowmax;
			}

		} else if (!strnicmp (line, "<EVAPORATOR>", 12)) {
			sscanf(line + 12, "%s %i %i %lf", string1, &one, &two, &throttle);
			h_Evaporator *evap = (h_Evaporator*) GetSystemByName(string1);
			if (evap) {
				evap->h_pump = one;
				evap->h_valve = two;
				evap->throttle = throttle; 
			}

		} else if (!strnicmp (line, "<HEATEXCHANGER>", 15)) {
			sscanf(line + 15, "%s %i %lf %lf %lf %i", string1, &one, &pmin, &pmax, &length, &two);
			h_HeatExchanger *heatEx = (h_HeatExchanger*) GetSystemByName(string1);
			if (heatEx) {
				heatEx->h_pump = one;
				heatEx->tempMin = pmin;
				heatEx->tempMax = pmax;
				heatEx->length = length;
				heatEx->bypassed = (two != 0);
			}

		} else if (!strnicmp (line, "<MIXINGPIPE>", 12)) {
			sscanf(line + 12, "%s %i %lf", string1, &one, &ratio);
			h_MixingPipe *mixer = (h_MixingPipe*) GetSystemByName(string1);
			if (mixer) {
				mixer->h_pump = one;
				mixer->ratio = ratio;
			}
		} else if (!strnicmp(line, "<VALVE>", 7)) {
			sscanf(line + 7, "%s %i %f", string1, &one, &size1);
			h_Valve *valve = (h_Valve*)GetSystemByName(string1);
			if (valve)
			{
				valve->open = one;
				valve->size = size1;
			}
		}
		oapiReadScenario_nextline (scn, line);
	}
}

void H_system::ProcessShip(VESSEL *vessel,PROPELLANT_HANDLE ph)
{
 ship_object *runner;
 runner=List.next;

 while (runner){ ((h_object*)runner)->ProcessShip(vessel,ph);
				 runner=runner->next;}

}

h_substance _substance(int s_type,double i_mass, double i_Q,float i_vm) { 
	h_substance temp(s_type,i_mass,i_Q,i_vm);
	return temp;
}

h_substance::h_substance(int i_subst_type,double i_mass,double i_Q,float i_vapor_mass) {
	subst_type = i_subst_type;
	mass = i_mass;
	Q = i_Q;
	vapor_mass = i_vapor_mass;
}

void h_substance::operator+= (h_substance add) {
	if (add.subst_type == subst_type){
		mass += add.mass;
		Q += add.Q;
		vapor_mass += add.vapor_mass;
	}
}

h_substance h_substance::operator*(float mult) {
	h_substance temp;
	temp.mass = mass * mult;
	temp.Q = Q * mult;
	temp.vapor_mass = vapor_mass * mult;
	temp.subst_type = subst_type;
	return temp;
}

void h_substance::operator -=(h_substance add) {
	if (add.subst_type == subst_type){
		mass -= add.mass;
		Q -= add.Q;
		vapor_mass -= add.vapor_mass;
	}
}

double h_substance::Condense(double dt) {

	if (vapor_mass < dt)
		dt = vapor_mass;

	vapor_mass -= dt;
	Q += VAPENTH[subst_type] * dt;

	return VAPENTH[subst_type] * dt;
}

double h_substance::Boil(double dt) {

	if (vapor_mass + dt > mass - 1.0)
		dt = mass - 1.0 - vapor_mass;

	if (dt < 0)
		return 0;

	if (Q < VAPENTH[subst_type] * dt)
		dt = Q / VAPENTH[subst_type];

	vapor_mass += dt;
	Q -= VAPENTH[subst_type] * dt;
	return -VAPENTH[subst_type] * dt;
}

double h_substance::BoilAll() {

/*	double dm = mass - vapor_mass;
	if (dm <= 0) return 0;

	if (Q < VAPENTH[subst_type] * dm)
		dm = Q / VAPENTH[subst_type];

	vapor_mass += dm;
	Q -= VAPENTH[subst_type] * dm;
	return -VAPENTH[subst_type] * dm;
*/

	vapor_mass = 0.999 * mass;
	return 0;
}

void h_substance::SetTemp(double _temp)
{
	Temp = _temp;
	Q = Temp * mass * SPECIFICC[subst_type];
}


//------------------------------- VOLUME CLASS ------------------------------------

h_volume::h_volume() {
	for (int i = 0; i < MAX_SUB; i++) {
		composition[i].subst_type = i;
		composition[i].mass = 0;
		composition[i].Q = 0;
		composition[i].vapor_mass = 0;
	}
	max_sub = 0;
	Temp = 273;
	Press = 0;
	Volume = 0;
	Q = 0;	//no energy
}

void h_volume::GetMaxSub() {
	max_sub=0;
	for (int i = 0; i < MAX_SUB; i++)
		if (composition[i].mass) max_sub++;
}

void h_volume::operator +=(h_substance add) {
	composition[add.subst_type] += add;
	Q += add.Q;
	GetMaxSub();
}

void h_volume::operator +=(h_volume add) {

	for (int i = 0; i < MAX_SUB; i++)
		composition[i] += add.composition[i];

	Q += add.Q;
	GetMaxSub();
}

h_volume h_volume::Break(double vol,int * mask, double maxMass) {

	h_volume temp;

	double ratio = vol / Volume;
	// TSCH
	//if (ratio > 1) ratio = 1.;
	if (ratio > .5) ratio = .5;
	
	if (maxMass) {
		if (ratio * GetMass() > maxMass) {
			ratio = maxMass / total_mass;
		}
	}

	for (int i=0; i<MAX_SUB; i++) {
		temp += composition[i] * (float) ratio * (float) mask[i];
		composition[i] -= temp.composition[i];
		Q -= temp.composition[i].Q;
	}
	return temp;   //then feed it to the requester
}

double h_volume::GetMass() {

	double mass = 0;
	for (int i = 0; i < MAX_SUB; i++)
			mass += composition[i].mass;

	total_mass = mass;
	return mass;
}

double h_volume::GetQ() {

	double q = 0;
	for (int i = 0; i < MAX_SUB; i++)
		q += composition[i].Q;

	Q = q;
	return q;
}

void h_volume::ThermalComps(double dt) {
	//1. averaging temperature, based on Q
	//2. computing vapor pressure based on new temp, for each subst present
	//3. boil / condense if needed, affecting Q
	//4. redo this every second or so..

	int i;

	//1. compute average temp
	double AvgC = 0;
	double vap_press;
	for (i = 0; i < MAX_SUB; i++)
		AvgC += composition[i].mass * SPECIFICC[composition[i].subst_type];

	if (GetMass()) {
		AvgC = AvgC / total_mass;	//weighted average heat capacity.. gives us averaged temp (ideal case)
		Temp = Q / AvgC / total_mass; //average Temp of substances
		for (i = 0; i < MAX_SUB; i++)
			composition[i].SetTemp(Temp);	//redistribute the temps,re-computing the Qs... mathwise we are OK
	} else
		Temp = 0;

	//2. Compute average Press
	double m_i=0;
	double NV=0;
	double PNV=0;
	double tNV;
	//some sums we need
	for (i = 0; i < MAX_SUB; i++) {
		m_i += composition[i].vapor_mass / MMASS[composition[i].subst_type];

		// temperature dependency of the density is assumed 1 to 2 g/l
		double density = L_DENSITY[composition[i].subst_type];
		if (composition[i].subst_type == SUBSTANCE_O2) {
			// Liquid density is temperature dependent because of cryo tank pressurization with a heater
			// Correction term is 0 at O2 initial tank temperature (75K), the other factors are "empirical"
			density += 0.56 * Temp * Temp - 134.0 * Temp + 6900.0;

		} else if (composition[i].subst_type == SUBSTANCE_H2) {
			// Liquid density is temperature dependent because of cryo tank pressurization with a heater
			// Correction term is 0 at H2 boiling point (20K), the other factors are "empirical"
			density += 0.03333 * Temp * Temp - 4.3333 * Temp + 73.3333;
		}
		tNV = (composition[i].mass - composition[i].vapor_mass) / density;
		NV += tNV;

		PNV += tNV / BULK_MOD[composition[i].subst_type];;
	}

	m_i = -m_i * R_CONST * Temp;
	NV = Volume - NV;

	double delta = NV * NV - 4.0 * m_i * PNV; //delta of quadric eq. P^2*PNV+ P*NV + m_i = 0
	if (PNV)
		Press = (-NV + sqrt(delta)) / 2.0 / PNV;	//only first solution is always valid. why is there a second?
	else
		Press = 0;

	NV = Volume - NV;
	double air_volume = Volume - NV + Press * PNV;
	for (i = 0; i < MAX_SUB; i++) {
		//recompute the vapor press
		vap_press = VAPPRESS[composition[i].subst_type] - (273.0 - Temp) * VAPGRAD[composition[i].subst_type];//this is vapor pressure of current substance
		if (vap_press > Press)	//need to boil material if any
			Q += composition[i].Boil(dt);
		else
		    Q += composition[i].Condense(dt);

		composition[i].p_press = R_CONST * Temp * composition[i].vapor_mass / MMASS[composition[i].subst_type] / air_volume;
	}
}

void h_volume::Void()
{
	for (int i = 0; i < MAX_SUB; i++) {
		composition[i].mass = 0;
		composition[i].p_press = 0;
		composition[i].Q = 0;
		composition[i].Temp = 0;
		composition[i].vapor_mass = 0;
	}
	Q = 0;
	Temp = 0;
	Press = 0;
	total_mass = 0;
}


//------------------------------------ VALVE CLASS------------------

h_Valve::h_Valve() {
	open = 0;
	closing_time = 2;
	parent = NULL;
	h_open = 0;
	pz = 0;
}

h_Valve::h_Valve(char *i_name, int i_open,int i_ct,float i_size,h_Tank *i_parent) {
	strcpy(name, i_name);
	open = i_open;
	closing_time = i_ct;
	parent = i_parent;
	size = i_size;
	h_open = 0;
	pz = 0;
}

void h_Valve::Set(int i_open,int i_ct,float i_size,h_Tank *i_parent) {
	open = i_open;
	closing_time = i_ct;
	parent = i_parent;
	size = i_size;
	h_open = 0;
}

void h_Valve::Close() { 
	if (open) h_open = -1;
}

void h_Valve::Open() {
	if (!open) h_open = 1;
}

double h_Valve::GetPress() {
	if (open)
		return parent->space.Press;
	else
		return 0;//closed, no press whatsoever
}

double h_Valve::GetTemp() { 
	return parent->Temp;
}

void h_Valve::thermic(double _en) {
	parent->thermic(_en);
}

int h_Valve::Flow(h_volume block) { //valves are simply sockets, forward this to parent

	if (open)
		return parent->Flow(block);
	else
		return 0;//unable to put block
}

h_volume h_Valve::GetFlow(double dPdT, double maxMass) {

	double vol = dPdT * size / 1000.0;		//size= Liters/Pa/second

	if (!open) vol = 0; //no flow obviously
	return parent->GetFlow(vol, maxMass);
}

void h_Valve::refresh(double dt) {

	if (h_open)	{	
		pz = (float) (h_open * 0.1);
		h_open = 0; //not moving??
	}
	//start the open/close procedure,
	if (pz < 0)	{
		pz += (float) dt;
	    if (pz >= 0) {
			pz = 0;
			open = 0;
		}
	} else if (pz > 0) {
		pz -= (float) dt;
		if (pz <= 0) { 
			pz = 0;
			open = 1;
		}
	}
}

void h_Valve::Save(FILEHANDLE scn) {

	char text[100];

	if (*name != '\0') {
		sprintf(text, " %s %i %f", name, open, size);
		oapiWriteScenario_string(scn, "   <VALVE>", text);
	}
}


//------------------------------- TANK CLASS ------------------------------------

h_Tank::h_Tank(char *i_name,vector3 i_p,double i_vol) {

	strcpy(name, i_name);
	max_stage = 99;
	pos = i_p;
	space.Volume = i_vol;
	IN_valve.Set(0, 2, (float) 0.001, this);			//connect the two valves.. (old size 0.001)
	OUT_valve.Set(0, 2, (float) 0.001, this);
	OUT2_valve.Set(0, 2, (float) 0.001, this);
	LEAK_valve.Set(0, 2, (float) 0.001, this);
	energy = 0;
	mass = 0;
	Temp = 290;	//290K is the default temp ...just so we don't get a /div0
	for (int i = 0; i < MAX_SUB; i++) {
		OUT_FLOW_MASK[i] = 1;
		IN_FLOW_MASK[i] = 1;
	}
}

h_Tank::~h_Tank() {
	if (parent)
		parent->P_thermal->RemoveThermalObject(this);
}

h_volume h_Tank::GetFlow(double m, double maxMass) {

	h_volume temp = space.Break(m, OUT_FLOW_MASK, maxMass);
	mass -= temp.GetMass();  //might not be as much as requested
	return temp;
}

int h_Tank::Flow(h_volume block) {	//add the block to the tank

	space += block;
	mass += block.GetMass();
	Temp = space.Temp;//backpropagate temp??
	energy = space.Q; //and energy
	return 1;
}

void h_Tank::thermic(double _en) {

	if (-_en > space.Q)
		_en = -space.Q / 10.0; //not really possible,
						  	   //but happens on small tanks combined with large
							   //time acceleration
	space.Q +=_en;
	energy += _en;
}

void h_Tank::refresh(double dt) {

	/*if (Compare("ACCU", name) || Compare("EVAPOUTLET", name)) {	// TSCH Test
		fprintf(PanelsdkLogFile, "%s.refresh1 Q %f Temp %f Mass %f\n", name, space.Q, space.Temp, space.total_mass);
		for (int i = 0; i < MAX_SUB; i++)
			fprintf(PanelsdkLogFile, "\t%i Q %f\n", i, space.composition[i].Q);
	}*/

	space.ThermalComps(dt);	

	Temp = space.Temp;
	energy = space.Q;

	IN_valve.refresh(dt);
	OUT_valve.refresh(dt);
	OUT2_valve.refresh(dt);
	LEAK_valve.refresh(dt);
}

void h_Tank::operator +=(h_substance add) { 
	space += add;

	mass = space.GetMass();	//get all the mass,etc..	
	energy = space.GetQ();	//sum up Qs
}

void h_Tank::Load(FILEHANDLE scn) {
	
	char *line;
	h_substance loaded_sub;

	space.Void(); //empty the space
	oapiReadScenario_nextline (scn, line);
	while (strnicmp(line,"</TANK>",7)) {
		if (!strnicmp (line, "CHM",3 )) {
            sscanf (line+3, "   %i %lf %lf %lf",
						&loaded_sub.subst_type,
						&loaded_sub.mass,
						&loaded_sub.vapor_mass,
						&loaded_sub.Q);
			space+=loaded_sub;
		}
		oapiReadScenario_nextline (scn, line);
	}
	mass = space.GetMass();//get all the mass,etc..
	space.GetMaxSub();//recompute sub_number;
	energy = space.GetQ();//sum up Qs
}

void h_Tank::Save(FILEHANDLE scn) {

	char text[100];

	/*int Compare(char* ln, char* trg);
	if (Compare(name, "TESTPIPE")) {	// TSCH Test
		int test = 0;
	}*/

	sprintf(text," %s %lf %i %i %i %i %.8f %.8f %.8f %.8f", 
		    name, space.Volume, 
		    IN_valve.open, OUT_valve.open, OUT2_valve.open, LEAK_valve.open, 
			IN_valve.size, OUT_valve.size, OUT2_valve.size, LEAK_valve.size);
	oapiWriteScenario_string(scn, "   <TANK>", text);

	for (int i=0;i<MAX_SUB;i++)
		if (space.composition[i].mass) {
			sprintf(text,"   %i %.12lf %.12lf %.12lf",
				space.composition[i].subst_type,
				space.composition[i].mass,
				space.composition[i].vapor_mass,
				space.composition[i].Q);
			oapiWriteScenario_string(scn, "      CHM", text);
		}
	oapiWriteScenario_string(scn, "   </TANK>","");
}

// These are hacks and should be used only in special cases. Violates energy conservation

void h_Tank::BoilAllAndSetTemp(double _t) {

	for (int i=0; i < MAX_SUB; i++) {
		if (space.composition[i].mass) {
			space.composition[i].BoilAll(); 
			space.composition[i].SetTemp(_t);
			
		}
	}
	energy = space.GetQ(); //recalc and sum up Qs
}


//------------------------------- PIPE CLASS ------------------------------------

h_Pipe::h_Pipe(char *i_name, h_Valve *i_IN, h_Valve *i_OUT, int i_type, double max, double min, int is_two) { 

	strcpy(name, i_name);
	max_stage = 99;
	type = i_type;
	two_ways = is_two;
	P_max = max;
	P_min = min;
	in = i_IN;
	out = i_OUT;
	open = 0;
	flow = 0;
	flowMax = 0;
}

void h_Pipe::BroadcastDemision(ship_object * gonner) {
	if ((in)&&(in->parent==gonner)) in=NULL;
	if ((out)&&(out->parent==gonner)) out=NULL;
}

void h_Pipe::refresh(double dt) {

	/*	int Compare(char* ln, char* trg);
	if (Compare(name, "SUITCIRCUITRETURNINLET")) {	// TSCH Test
		int test = 0;
	}
	*/

	//volume flow bases on press difference
	flow = 0;
	if ((!in) || (!out)) return;

	if (out->open && in->open) {

		double in_p = in->GetPress();
		double out_p = out->GetPress();

		if (type == 1) {	  //PREG
			in_p = (in_p > P_max ? P_max : in_p);

		} else if (type == 2) { //BURST
			if (in_p - out_p > P_max) open = 1;
			if (in_p - out_p < P_min) open = 0;
			if (open == 0) return;

		} else if (type == 3) {	//PVALVE
			if (in_p - P_max > out_p) { //one way flow;
				double vol = (in_p - P_max - out_p) * dt * in->size / 1000.0;		//size= Liters/Pa/second
				if (out->parent->space.Volume > vol) {
					out->parent->space.Volume -= vol;
					in->parent->space.Volume += vol;
				}
			}
			if ((two_ways) && (out_p - P_max > in_p) &&
				(out->parent->space.Volume > out->parent->Original_volume)) {
				double vol = (out_p - P_max - in_p) * dt * in->size / 1000.0;		//size= Liters/Pa/second
				if (in->parent->space.Volume > vol) {
					out->parent->space.Volume += vol;
					in->parent->space.Volume -= vol;
				}
			}
			return;
		}
		if (in_p > out_p) {
			h_volume v = in->GetFlow(dt * (in_p - out_p), flowMax * dt);
			flow = v.GetMass() / dt; 
			out->Flow(v);
		}

		if ((two_ways) && (out_p > in->GetPress())) {
			h_volume v = out->GetFlow(dt * (out_p - in_p));
			flow -= v.GetMass() / dt; 
			in->Flow(v);
		}

		//heat transfer is directly prop with deltaT.
		//all other conductive heat props are ignored.. ie. time for actual
		//"heating" is not accurate

		double in_t = in->GetTemp();
		double out_t = out->GetTemp();
		double trQ = (in_t - out_t) * dt;

		// conductive heat transfer should depend on valve-size
		// as a "quick hack" it's proportional to the minimum size,
		// but this has to be improved
		double minSize = __min(in->size, out->size);
		trQ = trQ * minSize;

		if (in->parent->space.Q < trQ)
			trQ = in->parent->space.Q / 10.0;
		if (out->parent->space.Q < -trQ)
			trQ = -out->parent->space.Q / 10.0;

		in->thermic(-trQ);
		out->thermic(trQ);
	}
}

void h_Pipe::Save(FILEHANDLE scn) {

	char text[100];

	if (*name != '\0') {
		sprintf(text," %s %lf %lf %lf", name, P_max, P_min, flowMax);
		oapiWriteScenario_string(scn, "   <PIPE>", text);
	}
}

h_Vent::h_Vent(char *i_name, vector3 i_p) : h_Tank(i_name, i_p, 0.0) {
	
	space.Void();
	Num_Vents = 0;
	parent = NULL;
}

h_Vent::~h_Vent() {
	//nothing !!!!
}

void h_Vent::AddVent(vector3 i_pos,vector3 i_dir,double i_size)
{
pos[Num_Vents]=i_pos;
dir[Num_Vents]=i_dir;
size[Num_Vents]=i_size;
Num_Vents++;
};
void h_Vent::ProcessShip(VESSEL *vessel,PROPELLANT_HANDLE ph)
{ph_vent=ph;
v=vessel;
for (int i=0;i<Num_Vents;i++)
{  thg[i]=v->CreateThruster (_V(pos[i].x,pos[i].y,pos[i].z), _V(dir[i].x,dir[i].y,dir[i].z), 50, ph,50);
	v->AddExhaust (thg[i], size[i]*1000,size[i]*50);
v->SetThrusterLevel(thg[i],1.0);};

};

int h_Vent::Flow(h_volume block) {

	// just venting...
	space.Press = 0;

/*	double mass=block.GetMass()/1000.0;//this we need to flow out in kg
	v->SetEmptyMass(v->GetEmptyMass()-mass);
	v->SetPropellantMass(ph_vent,v->GetPropellantMass(ph_vent)+mass);
	for (int i=0;i<Num_Vents;i++)
		v->SetThrusterLevel(thg[i],1.0);
*/	
	return 1;
}


h_Radiator::h_Radiator(char *i_name,vector3 i_pi,double i_size,double i_rad) {

	strcpy(name, i_name);
	max_stage = 99;
	c = 0.15;
	pos = i_pi;
	size = i_size;
	rad = i_rad;
}

h_Radiator::~h_Radiator() {
	parent->P_thermal->RemoveThermalObject(this);
}

void h_Radiator::refresh(double dt) {

	double Q = rad * size * 5.67e-8 * dt * pow(Temp - 3.0, 4);	//aditional cooling from the radiator??

	// if (!strcmp(name, "SPSPROPELLANTLINE")) 
	//	sprintf(oapiDebugString(), "Radiator %.3f Temp %.1f", Q / dt, GetTemp());

	thermic(-Q);
}

void h_Radiator::Save(FILEHANDLE scn) {

	char text[100];

	sprintf(text," %s %f %f",name, Temp, rad);
	oapiWriteScenario_string(scn, "   <RADIATOR>", text);
}


h_HeatExchanger::h_HeatExchanger(char *i_name, int i_pump, double i_length, therm_obj *i_source, therm_obj *i_target, double i_tempMin, double i_tempMax) {

	strcpy(name, i_name);
	max_stage = 99;
	h_pump = i_pump;
	length = i_length;
	source = i_source;
	target = i_target;
	tempMin = i_tempMin;
	tempMax = i_tempMax; 
	power = 0;
	bypassed = false;
}

void h_HeatExchanger::refresh(double dt) {

	power = 0;
	bool pump = false;

	if (bypassed) {
		pump = false;

	} else if (h_pump == -1) {
		pump = true;

	} else if (h_pump == 1) {
		
		// Only cooling at the moment, heating causes bugs during high time accelerations
		//if (target->GetTemp() < tempMin && source->GetTemp() > target->GetTemp()) {
		//	pump = true;
		//}
		if (target->GetTemp() > tempMax && source->GetTemp() < target->GetTemp()) {
			pump = true;
		}
	}

	if (pump) {	
		double Q = (source->GetTemp() - target->GetTemp()) * length * dt;
	
		source->thermic(-Q);
		target->thermic(Q);
		power = Q / dt;
	}
}

void h_HeatExchanger::Save(FILEHANDLE scn) {

	char text[100];

	sprintf(text," %s %i %f %f %f %i",name, h_pump, tempMin, tempMax, length, bypassed);
	oapiWriteScenario_string(scn, "   <HEATEXCHANGER>", text);
}


h_Evaporator::h_Evaporator(char *i_name, int i_pump, therm_obj *i_target, double i_targetTemp, h_Valve *i_liquidSource, double i_tempTurnOn, therm_obj *i_tempControl) {

	strcpy(name, i_name);
	max_stage = 99;
	h_pump = i_pump;
	h_valve = 0;
	throttle = 0;
	steamPressure = 0;
	target = i_target;
	targetTemp = i_targetTemp; 
	liquidSource = i_liquidSource;
	tempTurnOn = i_tempTurnOn;
	tempControl = i_tempControl;
}

void h_Evaporator::refresh(double dt) {

	double steamUnderPressure = 0;

	if (!h_pump) {
		steamUnderPressure = -0.11;
	}
	if (h_pump == -1) {
		if (h_valve ==  1) throttle += dt / 58.0;
		if (h_valve == -1) throttle -= dt / 58.0;
	}
	if (h_pump == 1) {
		if (tempControl->GetTemp() >= tempTurnOn) {
			if (target->GetTemp() < targetTemp) {
				throttle -= dt / 58.0; 
			} else {
				throttle += dt / 58.0; 
			}
		} else {
			throttle = 0;
		}
	}
	if (throttle < 0) throttle = 0;
	if (throttle > 1) throttle = 1;

	// Throttle simulates the valve which remains unchanged when the evaporator is turned off
	double throttle_temp = throttle;
	if (!h_pump) throttle_temp = 0;

	// The evaporators don't work inside the atmosphere, they stop working shortly before apex cover jettison
	if (parent->Vessel->GetAtmPressure() > 30000.0) {
		throttle_temp = 0;
		steamUnderPressure = -0.11;
	}

	if (throttle_temp) {
		double targetFlow = 1.1 * dt * throttle_temp; //???
	
		/// get liquid from sources
		/// \todo Only H2O at the moment!
		h_substance *h2o = &liquidSource->parent->space.composition[SUBSTANCE_H2O];
		double flow = h2o->mass;
	
		// max. consumption
		if (flow > targetFlow) 
			flow = targetFlow;
		else 
			steamUnderPressure = (1 - flow / targetFlow) * 0.09;
	
		// flow from sources
		if (flow > 0.0) { 
			// remove substance
			h2o->Q -= h2o->Q * flow / h2o->mass;
			double vapor_flow = h2o->vapor_mass * flow / h2o->mass; 
			h2o->vapor_mass -= vapor_flow;
			h2o->mass -= flow;
			
			// recalc source
			liquidSource->parent->space.GetQ();
			liquidSource->parent->space.GetMass();
			liquidSource->parent->mass -= flow;

			// evaporate liquid
			if (flow - vapor_flow > 0)
				target->thermic(-VAPENTH[SUBSTANCE_H2O] * (flow - vapor_flow));
		}
	}
	//steam pressure is not simulated physically at the moment
	steamPressure = (-0.05 * throttle_temp + 0.14 - steamUnderPressure) / 0.000145038;
}

void h_Evaporator::Save(FILEHANDLE scn) {

	char text[100];

	sprintf(text," %s %i %i %lf",name, h_pump, h_valve, throttle);
	oapiWriteScenario_string(scn, "   <EVAPORATOR>", text);
}


h_MixingPipe::h_MixingPipe(char *i_name, int i_pump, h_Valve *i_in1, h_Valve *i_in2, h_Valve *i_out, double i_targetTemp) {

	strcpy(name, i_name);
	max_stage = 99;
	h_pump = i_pump;
	in1 = i_in1;
	in2 = i_in2;
	out = i_out;
	targetTemp = i_targetTemp;
	ratio = 0;
}

void h_MixingPipe::refresh(double dt) {

	// See also h_Pipe::refresh

	if (!h_pump) return;
	if ((!in1) || (!in2) || (!out)) return;
	if (out->open && in1->open && in2->open) {

		if (h_pump == 1) {
			if (out->GetTemp() > targetTemp) {
				if (in1->GetTemp() > in2->GetTemp()) {
					ratio -= .01 * dt;
				} else {
					ratio += .01 * dt;
				}
			} else {
				if (in1->GetTemp() < in2->GetTemp()) {
					ratio -= .01 * dt;
				} else {
					ratio += .01 * dt;
				}
			}
		}
		if (ratio < 0) ratio = 0;
		if (ratio > 1) ratio = 1;

		double in1_p = in1->GetPress();
		double in2_p = in2->GetPress();
		double out_p = out->GetPress();

		if (in1_p > out_p) {
			h_volume v = in1->GetFlow(ratio * dt * (in1_p - out_p));
			out->Flow(v);
		}

		if (in2_p > out_p) {
			h_volume v = in2->GetFlow((1.0 - ratio) * dt * (in2_p - out_p));
			out->Flow(v);
		}
	}
}

void h_MixingPipe::Save(FILEHANDLE scn) {

	char text[100];

	sprintf(text," %s %i %lf",name, h_pump, ratio);
	oapiWriteScenario_string(scn, "   <MIXINGPIPE>", text);
}


h_crew::h_crew(char *i_name, int nr, h_Tank *i_src) {
	
	strcpy(name, i_name);
	max_stage = 99;
	number = nr;
	SRC = i_src;
}

void h_crew::refresh(double dt) {

	double oxygen = 0.00949 * number * dt; //grams of O2	
	if (SRC) {
		double srcTemp = SRC->GetTemp();
		therm_obj *t = SRC->GetThermalInterface();

		if (SRC->space.composition[SUBSTANCE_O2].vapor_mass < oxygen)
			oxygen = SRC->space.composition[SUBSTANCE_O2].vapor_mass;
		SRC->space.composition[SUBSTANCE_O2].mass -= oxygen;
		SRC->space.composition[SUBSTANCE_O2].vapor_mass -= oxygen;
		SRC->space.composition[SUBSTANCE_O2].SetTemp(srcTemp);

		double co2 = 0.01013 * number * dt; //grams of CO2
		SRC->space.composition[SUBSTANCE_CO2].mass += co2;
		SRC->space.composition[SUBSTANCE_CO2].vapor_mass += co2;
		SRC->space.composition[SUBSTANCE_CO2].SetTemp(srcTemp);

		double h2o = 0.0264 * number * dt;  // grams of H2O water vapor
		SRC->space.composition[SUBSTANCE_H2O].mass += h2o;	
		SRC->space.composition[SUBSTANCE_H2O].vapor_mass += h2o;	
		SRC->space.composition[SUBSTANCE_H2O].SetTemp(srcTemp);

		SRC->space.GetQ();
		SRC->space.GetMass();
			
		double heat = 10.0 * number * dt;  //heat
		t->thermic(heat);
	}
}

void h_crew::Save(FILEHANDLE scn) {

	char text[100];

	sprintf(text," %s %i", name, number);
	oapiWriteScenario_string(scn, "   <CREW>", text);
}

h_CO2Scrubber::h_CO2Scrubber(char *i_name, double i_flowmax, h_Valve* in_v, h_Valve* out_v)

{
	strcpy(name, i_name);
	max_stage = 99;
	flowMax = i_flowmax;
	in = in_v;
	out = out_v;

	co2removalrate = 0;
	flow = 0;
}

void h_CO2Scrubber::refresh(double dt) {

	co2removalrate = 0;

	flow = 0;
	if ((!in) || (!out)) return;

	if (out->open && in->open) {

		double delta_p = in->GetPress() - out->GetPress();
		if (delta_p < 0)
			delta_p = 0;

		h_volume fanned = in->GetFlow(dt * delta_p, flowMax * dt);
		co2removalrate = fanned.composition[SUBSTANCE_CO2].mass / dt;

		if (co2removalrate <= 0.0356) {
			fanned.composition[SUBSTANCE_CO2].mass =
				fanned.composition[SUBSTANCE_CO2].vapor_mass =
				fanned.composition[SUBSTANCE_CO2].Q = 0;
		}
		else {
			double removedmass = 0.0356 * dt;
			double factor = (fanned.composition[SUBSTANCE_CO2].mass - removedmass) / fanned.composition[SUBSTANCE_CO2].mass;
			fanned.composition[SUBSTANCE_CO2].mass -= removedmass;
			fanned.composition[SUBSTANCE_CO2].vapor_mass -= removedmass;
			fanned.composition[SUBSTANCE_CO2].Q = fanned.composition[SUBSTANCE_CO2].Q * factor;

			co2removalrate = removedmass / dt;
		}

		// flow to output
		flow = fanned.GetMass() / dt;
		fanned.GetQ();
		out->Flow(fanned);
	}
}

h_WaterSeparator::h_WaterSeparator(char *i_name, double i_flowmax, h_Valve* in_v, h_Valve* out_v, h_Valve *i_H2Owaste)

{
	strcpy(name, i_name);
	max_stage = 99;
	in = in_v;
	out = out_v;
	H20waste = i_H2Owaste;
	flowMax = i_flowmax;

	flow = 0;
}

void h_WaterSeparator::refresh(double dt) {

	flow = 0;
	if ((!in) || (!out)) return;

	if (out->open && in->open) {

		double delta_p = in->GetPress() - out->GetPress();
		if (delta_p < 0)
			delta_p = 0;

		h_volume fanned = in->GetFlow(dt * delta_p, flowMax * dt);

		// separate water
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

		// flow to output
		flow = fanned.GetMass() / dt;
		fanned.GetQ();
		out->Flow(fanned);
	}
}