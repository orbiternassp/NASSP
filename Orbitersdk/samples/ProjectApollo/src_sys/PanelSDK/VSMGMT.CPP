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

#include "vsmgmt.h"
#include <stdio.h>
#include "BUILD.H"


PARTICLESTREAMSPEC panelsdk_srb_contrail = {
	0, 8.0, 5, 350.0, 0.3, 8.0, 4, 3.0, PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};
PARTICLESTREAMSPEC panelsdk_srb_exhaust = {
	0, 4.0, 20, 350.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};

Mesh::Mesh()
{next=NULL;
 name[0]=0;
 mesh_name[0]=0;
 h_mesh=NULL;
 dry_weight=0;
}
Mesh::~Mesh()
{if (next) delete next;
};
void Mesh::Init()
{
h_mesh=oapiLoadMeshGlobal(mesh_name);
if (next) next->Init();
}
void Mesh::AddToList(Mesh *new_mesh)
{
new_mesh->next=next;
next=new_mesh;
}
void Mesh::Show(VESSEL *vs, int stage)
{ if ((stage>=min_stage)&&(stage<max_stage)) {
	vs->AddMesh(h_mesh,&offset[stage]);
	double w=vs->GetEmptyMass();
	vs->SetEmptyMass(w+dry_weight);
						}
 if (next) next->Show(vs,stage);	//pass it on
};

Propellant::Propellant()
{next=NULL;
 name[0]=0;
 h_prop=NULL;
 total_weight=0;
}
Propellant::~Propellant()
{if (next) delete next;
};
void Propellant::Define(VESSEL *vs, int stage, int load)
{
if (stage>=max_stage){ //we delete the resource
		vs->DelPropellantResource(h_prop);
}
else	//we are still within the resource's lifetime
{
if ((!load)&&(stage==min_stage)){ //we add the resource if staging now
	  h_prop=vs->CreatePropellantResource(total_weight);
	  double w=vs->GetEmptyMass();
	  vs->SetEmptyMass(w-total_weight);
								}
//or if we have just loaded this
if ((load) &&(stage>=min_stage)){
		h_prop=vs->CreatePropellantResource(total_weight);
	  double w=vs->GetEmptyMass();
        vs->SetEmptyMass(w-total_weight);
								}

if ((!load)&&(stage>min_stage)){		//even for staging, we still have to deduct mass
		double w=vs->GetEmptyMass();
        vs->SetEmptyMass(w-total_weight);
			}
}

 if (next) next->Define(vs,stage,load);
}

void Propellant::AddToList(Propellant *new_p)
{ new_p->next=next;
  next=new_p;
};
PROPELLANT_HANDLE Propellant::GetPhByName(char *p_name)
{
	if (!strnicmp(name,p_name,strlen(p_name)))
			return h_prop;
	if (next)
			return next->GetPhByName(p_name);
	return NULL;
};
Thruster::Thruster()
{propellant[0]=0;
 next=NULL;
 th=NULL;
};
Thruster::~Thruster()
{ if (next) delete next;
};

void Thruster::Define(VESSEL *vs,Propellant *PH_list)
{
	th = vs->CreateThruster (pos, dir, thr, PH_list->GetPhByName(propellant), isp1,isp2);
	double w=vs->GetPropellantMass(PH_list->GetPhByName(propellant));
	vs->AddExhaust (th, length, width, pos,_V(-dir.x,-dir.y,-dir.z));
	if (next) next->Define(vs,PH_list);

}
void Thruster::AddToList(Thruster *new_th)
{ new_th->next=next;
  next=new_th;
};

Ejector::Ejector()
{next=NULL;
class_name[0]=0;
}
Ejector::~Ejector()
{if (next) delete next;
}
void Ejector::AddToList(Ejector *new_e)
{new_e->next=next;
 next=new_e;
}
void Ejector::Eject(VESSEL *vessel)
{


	VESSELSTATUS vs;vessel->GetStatus (vs);
	vs.flag[0] = 3;vs.fuel=1.0; //fully fueled sep. jetts ??
	vessel->Local2Rel (offset, vs.rpos);vs.status = 0;
	vs.arot.x+=rotation.x;
	vs.arot.y+=rotation.y;
	vs.arot.z+=rotation.z;
	char name[256];
	sprintf (name,"%s_%s", vessel->GetName(), class_name);
	oapiCreateVessel (name, class_name, vs);


if (next) next->Eject(vessel);
};
KeyPress::KeyPress()
{next=NULL;
 key=0;
}
KeyPress::~KeyPress()
{ if (next) delete next;
}

void KeyPress::AddToList(KeyPress *new_k)
{ new_k->next=next;
  next=new_k;
};

DockPort::DockPort()
{next=NULL;
};
DockPort::~DockPort()
{if (next) delete next;
};
void DockPort::AddToList(DockPort *new_dp)
{new_dp->next=next;
 next=new_dp;
};

void DockPort::Define(VESSEL *vs)
{vs->CreateDock(pos,dir,up);
if (next) next->Define(vs);
};

Stage::Stage()
{ size=0;
  cw=_V(0.0,0.0,0.0);
crosss=_V(0.0,0.0,0.0);
rotdrag=_V(0.0,0.0,0.0);
pmi=_V(0.0,0.0,0.0);
camera_off=_V(0.0,0.0,0.0);
touchd[0]=_V(0.0,0.0,0.0);
touchd[1]=_V(0.0,0.0,0.0);
touchd[2]=_V(0.0,0.0,0.0);
cgoffset=_V(0.0,0.0,0.0);
wingname[0]=0;
TH_list=NULL;
EJ_list=NULL;
DK_list=NULL;
}
Stage::~Stage()
{if (TH_list) delete TH_list;
 if (EJ_list) delete EJ_list;
 if (DK_list) delete DK_list;
};
void Stage::SetStage(VESSEL *vs)
{
vs->SetSize(size);
vs->SetCW(cw.x,cw.x,cw.y,cw.z);
vs->SetCrossSections(crosss);
vs->SetRotDrag(rotdrag);
vs->SetPMI(pmi);
vs->SetCameraOffset(camera_off);
vs->SetTouchdownPoints(touchd[0],touchd[1],touchd[2]);
if (DK_list) DK_list->Define(vs);
//then define the various
}


VesselMgmt::VesselMgmt()
{Meshes=NULL;
 Propellants=NULL;
 DefinedKeys=NULL;
  max_stages=1;
}
VesselMgmt::~VesselMgmt()
{
if (Meshes) delete Meshes;
if (Propellants) delete Propellants;
if (DefinedKeys) delete DefinedKeys;
};
void VesselMgmt::SetConfig(int config, int load)
{
	vs->ClearMeshes();
	//vs->ClearAttExhaustRefs();
	//vs->ClearExhaustRefs();
	vs->ClearThrusterDefinitions();
	vs->SetEmptyMass(0);

	Stages[config].SetStage(vs);
	if (Meshes) Meshes->Show(vs,config);
	if (Propellants) Propellants->Define(vs,config,load);
	if (Stages[config].TH_list) Stages[config].TH_list->Define(vs,Propellants);
	GroupTHR(Stages[config].TH_list);
};

void VesselMgmt::GroupTHR(Thruster *TH_list)
{ int k=0;
  THRUSTER_HANDLE ths[15];	//max 15 thr/ group
  	panelsdk_srb_exhaust.tex = oapiRegisterParticleTexture ("Contrail2");
  Thruster *runner;
	for (int i=0;i<15;i++) //for all 15 types of thrusters
	{   runner=TH_list;k=0;
	while (runner) {
	  	if (runner)
				if (runner->group==i)	//one of ours
				{ths[k++]=runner->th;
				if (i==0)// main th's get a contrail
						{
					    vs->AddExhaustStream (runner->th, &panelsdk_srb_contrail);
						vs->AddExhaustStream (runner->th,  &panelsdk_srb_exhaust);
						}
						};
		runner=runner->next;
	} if (k)
	switch (i){
	case 0:
		vs->CreateThrusterGroup(ths,k,THGROUP_MAIN);
		break;
	case 1:
		vs->CreateThrusterGroup(ths,k,THGROUP_RETRO);
		break;
	case 2:
		vs->CreateThrusterGroup(ths,k,THGROUP_HOVER);
		break;
	case 3:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_PITCHUP);
		break;
	case 4:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_PITCHDOWN);
		break;
	case 5:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_YAWLEFT);
		break;
	case 6:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_YAWRIGHT);
		break;
	case 7:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_BANKLEFT);
		break;
	case 8:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_BANKRIGHT);
		break;
	case 9:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_RIGHT);
		break;
	case 10:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_LEFT);
		break;
	case 11:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_UP);
		break;
	case 12:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_DOWN);
		break;
	case 13:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_FORWARD);
		break;
	case 14:
		vs->CreateThrusterGroup(ths,k,THGROUP_ATT_BACK);
		break;
	case 15:
		vs->CreateThrusterGroup(ths,k,THGROUP_USER);
		break;
	};
	}
};

void VesselMgmt::Build()
{

char *line;
int stage;
line=ReadConfigLine();

while (!Compare(line,"</VESSEL>")) {
	if (Compare(line,"<ELEMENTS>"))
		BuildMeshElements();
	else if (Compare(line,"<PROPELLANTS>"))
		BuildPropellants();
	else if (Compare(line,"<CONFIG"))
			{sscanf(line,"<CONFIG %i>",&stage);
			BuildStage(stage);}
	line=ReadConfigLine();
}
Meshes->Init(); //globally load the meshes
}

void VesselMgmt::BuildMeshElements()
{
char *line;
line=ReadConfigLine();
char name[25];
char mesh[25];
double w;
int min,max;
while (!Compare(line,"</ELEMENTS>")){
	if (Compare(line,"<"))//beginning of a new element
	{
		sscanf(line+1,"%s %s %lf %i %i",name, mesh,&w,&min,&max);
		name[strlen(name)-1]=0;
		Mesh *new_m=new Mesh;
		strcpy(new_m->name,name);
		strcpy(new_m->mesh_name,mesh);
		new_m->dry_weight=w;
		new_m->max_stage=max;
		new_m->min_stage=min;

		if (Meshes) Meshes->AddToList(new_m);
		else Meshes=new_m;
		for (int i=min;i<max;i++)
		{
			line=ReadConfigLine();
			sscanf(line,"<%lf %lf %lf>",&new_m->offset[i].x,
										&new_m->offset[i].y,
										&new_m->offset[i].z);
		}
		line=ReadConfigLine();//pass over the </>
	}

	line=ReadConfigLine();
}

};
void VesselMgmt::BuildPropellants()
{
char *line;
line=ReadConfigLine();
char name[25];
double w;
int min,max;
while (!Compare(line,"</PROPELLANTS>")){
	sscanf(line,"%s %lf %i %i",name,&w,&min,&max);
	Propellant *new_p=new Propellant;
	strcpy(new_p->name,name);
	new_p->total_weight=w;
	new_p->max_stage=max;
	new_p->min_stage=min;
	if (Propellants) Propellants->AddToList(new_p);
	else Propellants=new_p;
	line=ReadConfigLine();

}

};
void VesselMgmt::BuildStage(int stage)
{
char *line;
line=ReadConfigLine();
char name[25];
//double w;
//int min,max;
while (!Compare(line,"</CONFIG")){
	if (Compare(line,"<SIZE>"))
			sscanf(line+6,"%lf",&Stages[stage].size);
	else if (Compare(line,"<CW>"))
			sscanf(line+4," <%lf %lf %lf>",
					&Stages[stage].cw.x,
					&Stages[stage].cw.y,
					&Stages[stage].cw.z);
	else if (Compare(line,"<CROSSSECTION>"))
				sscanf(line+14," <%lf %lf %lf>",
					&Stages[stage].crosss.x,
					&Stages[stage].crosss.y,
					&Stages[stage].crosss.z);
	else if (Compare(line,"<ROTDRAG>"))
		         sscanf(line+9," <%lf %lf %lf>",
					&Stages[stage].rotdrag.x,
					&Stages[stage].rotdrag.y,
					&Stages[stage].rotdrag.z);
	else if (Compare(line,"<PMI>"))
		         sscanf(line+5," <%lf %lf %lf>",
					&Stages[stage].pmi.x,
					&Stages[stage].pmi.y,
					&Stages[stage].pmi.z);
	else if (Compare(line,"<CAMERAOFF>"))
		         sscanf(line+11," <%lf %lf %lf>",
					&Stages[stage].camera_off.x,
					&Stages[stage].camera_off.y,
					&Stages[stage].camera_off.z);
	else if (Compare(line,"<TOUCHDOWN>"))
		         sscanf(line+11," <%lf %lf %lf> <%lf %lf %lf> <%lf %lf %lf>",
					&Stages[stage].touchd[0].x,
					&Stages[stage].touchd[0].y,
					&Stages[stage].touchd[0].z,
					&Stages[stage].touchd[1].x,
					&Stages[stage].touchd[1].y,
					&Stages[stage].touchd[1].z,
					&Stages[stage].touchd[2].x,
					&Stages[stage].touchd[2].y,
					&Stages[stage].touchd[2].z);
	else if (Compare(line,"<DOCK>"))
	{		DockPort *new_dp=new DockPort;
			 sscanf(line+6," <%lf %lf %lf> <%lf %lf %lf> <%lf %lf %lf>",
					&new_dp->pos.x,
					&new_dp->pos.y,
					&new_dp->pos.z,
					&new_dp->dir.x,
					&new_dp->dir.y,
					&new_dp->dir.z,
					&new_dp->up.x,
					&new_dp->up.y,
					&new_dp->up.z);
			 if (Stages[stage].DK_list)
					Stages[stage].DK_list->AddToList(new_dp);
			 else Stages[stage].DK_list=new_dp;
			 }

	else if (Compare(line,"<THRUSTERS>")) {
			line= ReadConfigLine();
		while (!Compare(line,"</THRUSTERS>")){
					Thruster *new_t=new Thruster;
					sscanf(line,"<%lf %lf %lf> <%lf %lf %lf> %lf %s %lf %lf %s <%lf %lf>",
							&new_t->pos.x,&new_t->pos.y,&new_t->pos.z,
							&new_t->dir.x,&new_t->dir.y,&new_t->dir.z,
							&new_t->thr,
							new_t->propellant,
							&new_t->isp1,
							&new_t->isp2,
							name,
							&new_t->length,
							&new_t->width);
					if (Compare(name,"THGROUP_MAIN")) new_t->group=0;
					else
					if (Compare(name,"THGROUP_RETRO")) new_t->group=1;
					else
					if (Compare(name,"THGROUP_HOVER")) new_t->group=2;
					else
					if (Compare(name,"THGROUP_ATT_PITCHUP")) new_t->group=3;
					else
					if (Compare(name,"THGROUP_ATT_PITCHDOWN")) new_t->group=4;
					else
					if (Compare(name,"THGROUP_ATT_YAWLEFT")) new_t->group=5;
					else
					if (Compare(name,"THGROUP_ATT_YAWRIGHT")) new_t->group=6;
					else
					if (Compare(name,"THGROUP_ATT_BANKLEFT")) new_t->group=7;
					else
					if (Compare(name,"THGROUP_ATT_BANKRIGHT")) new_t->group=8;
					else
					if (Compare(name,"THGROUP_ATT_RIGHT")) new_t->group=9;
					else
					if (Compare(name,"THGROUP_ATT_LEFT")) new_t->group=10;
					else
					if (Compare(name,"THGROUP_ATT_UP")) new_t->group=11;
					else
					if (Compare(name,"THGROUP_ATT_DOWN")) new_t->group=12;
					else
					if (Compare(name,"THGROUP_ATT_FORWARD")) new_t->group=13;
					else
					if (Compare(name,"THGROUP_ATT_BACK")) new_t->group=14;
					else
					if (Compare(name,"THGROUP_USER")) new_t->group=15;

					if (Stages[stage].TH_list)  Stages[stage].TH_list->AddToList(new_t);
					else Stages[stage].TH_list=new_t;
					line=ReadConfigLine();
				}; //</THRUSTERS>


	} // end of th definitions

	line=ReadConfigLine();
}//</STAGE>



}

void VesselMgmt::Separation(int config)	// ie .. go to config

{
//jettison any necessary ships..
	if (Stages[config].EJ_list)Stages[config].EJ_list->Eject(vs);
	vs->ShiftCentreOfMass(Stages[config].cgoffset);
//offset the CG

}
