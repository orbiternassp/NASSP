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

#include "instruments.h"
#include "Internals/Thermal.h"
#include "Internals/Hsystems.h"
#include "Internals/Esystems.h"
#include "vsmgmt.h"

PanelSDK::PanelSDK() {

	for (int i = 0; i < 10; i++) 
		panels[i]=NULL;
	NumPanels = 0;
	InstDescriptor = NULL;
	CustomVarList = NULL;
	GDI_res = NULL;

    ELECTRIC = new E_system;
	HYDRAULIC = new H_system;
    THERMAL = new Thermal_engine;
	VESSELMGMT = new VesselMgmt;
	HYDRAULIC->P_thermal = THERMAL;
	ELECTRIC->P_thermal = THERMAL;
	ELECTRIC->P_hydraulics = HYDRAULIC;

	CurentStage = 1;
	lastTime = 0;
	firstTimestepDone = false;
}

PanelSDK::~PanelSDK()
{
	if (GDI_res) delete GDI_res;
	if (InstDescriptor)	{
		InstrumentDescriptor *runner;
		runner = InstDescriptor;
		while (runner) {
			InstDescriptor=runner;
			runner = runner->next;
			delete InstDescriptor;
		}
	}
	if (NumPanels)
		for (int i=0; i < NumPanels; i++)
			delete panels[i];
	
	delete ELECTRIC;
	delete HYDRAULIC;
	delete THERMAL;
	delete VESSELMGMT;
}

void PanelSDK::RegisterVessel(VESSEL *vessel)
{
	v = vessel;
	VESSELMGMT->vs = vessel;
	ELECTRIC->Vessel = vessel;
	HYDRAULIC->Vessel = vessel;
}

void PanelSDK::RegisterCustomPointer(char *PointerName, void *point)
{
CustomVariable *runner= new CustomVariable();
runner->next=CustomVarList;
runner->item=point;
strcpy(runner->name,PointerName);
CustomVarList=runner;
}
void PanelSDK::AddDescriptor(InstrumentDescriptor *new_desc)
{
new_desc->next=InstDescriptor;
InstDescriptor=new_desc;
};

int PanelSDK::AddBitmapResource(char* BitmapName)
{
if (!GDI_res) GDI_res=new GDI_resources;
GDI_res->num_surfaces++;
HBITMAP new_b=(HBITMAP)LoadImage(NULL,BitmapName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
GDI_res->h_Surface[GDI_res->num_surfaces]=oapiCreateSurface (new_b);
if (new_b)
return GDI_res->num_surfaces;
else
return 0;
}
int PanelSDK::AddFontResource(char *FontName,int size)
{
if (!GDI_res) GDI_res=new GDI_resources;
GDI_res->num_fonts++;
GDI_res->hFNT_Panel[GDI_res->num_fonts]=CreateFont(size,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_RASTER_PRECIS,
			 CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,FontName);
if (GDI_res->hFNT_Panel[GDI_res->num_fonts])
return GDI_res->num_fonts;
else
return 0;
}
int PanelSDK::AddBrushResource(int red,int green,int blue)
{
if (!GDI_res) GDI_res=new GDI_resources;
GDI_res->num_brush++;
GDI_res->hPEN[GDI_res->num_brush]=CreatePen(PS_SOLID,1,RGB(red,green,blue));
return GDI_res->num_brush;
}
bool  PanelSDK::LoadPanel(int id)
{
if (id<NumPanels) {
Current_Panel=id;
oapiRegisterPanelBackground (panels[id]->MakeYourBackground(), PANEL_ATTACH_BOTTOM|PANEL_MOVEOUT_BOTTOM, (unsigned int)panels[id]->transparent_color);
oapiSetPanelNeighbours (panels[id]->neighbours[0],
						panels[id]->neighbours[1],
						panels[id]->neighbours[2],
						panels[id]->neighbours[3]);
panels[id]->RegisterYourInstruments();
return TRUE;
}
return FALSE;
};

void PanelSDK::PanelEvent(int id,int event,SURFHANDLE surf)
{

panels[Current_Panel]->surf=surf;	//get the surface handle, since intruments will be using this
switch (event) {
	case PANEL_REDRAW_INIT:
		panels[Current_Panel]->Paint(id);
		break;
	case PANEL_REDRAW_ALWAYS:
		panels[Current_Panel]->Refresh(id);
		break;
	case PANEL_REDRAW_USER:
		panels[Current_Panel]->Paint(id);
		break;
	case PANEL_REDRAW_MOUSE:
		panels[Current_Panel]->Paint(id);
		break;

}
};

void PanelSDK::MouseEvent(int id,int event,int mx,int my)
{
	switch (event)
	{
	  case PANEL_MOUSE_LBDOWN:
		panels[Current_Panel]->LBD(id,mx,my);
		break;
	  case PANEL_MOUSE_RBDOWN:
			panels[Current_Panel]->RBD(id,mx,my);
		break;
	  case PANEL_MOUSE_LBUP:
	  case PANEL_MOUSE_RBUP:
			panels[Current_Panel]->BU(id);
		break;
	}
}



InstrumentDescriptor* PanelSDK::GetInstrumentDescriptor(char *line)
{
InstrumentDescriptor* runner=InstDescriptor;
if (!line) return NULL;
while ((runner)&&(strnicmp(runner->name,line,strlen(runner->name))))
	   runner=runner->next;
return runner;
};


void PanelSDK::Load(FILEHANDLE scn) {
	
	int id;
	char *line, buffer[100];
	bool dontLoad;

	// check version 
	dontLoad = true;
	oapiReadScenario_nextline (scn, line);
    if (!strnicmp (line, "<VERSION>", 9)) {
		sscanf(line + 9, "%s", buffer);
	    if (!strnicmp (buffer, PANELSDK_VERSION, strlen(PANELSDK_VERSION))) 
			dontLoad = false;
	}

	oapiReadScenario_nextline (scn, line);
	if (dontLoad) {
		while (strnicmp(line,"</INTERNALS>",12)) {
			oapiReadScenario_nextline (scn, line);
		}
		return;
	}

	while (strnicmp(line,"</INTERNALS>",12)) {
        if (!strnicmp (line, "<HYDRAULIC>", 11))
			HYDRAULIC->Load(scn);
        else if (!strnicmp (line, "<ELECTRIC>", 10))
			ELECTRIC->Load(scn);
        else if (!strnicmp (line, "<PANEL>", 7)) {
			sscanf(line + 7, "%i", &id);
			panels[id]->Load(scn);
		}
		oapiReadScenario_nextline (scn, line);
	}
}

void PanelSDK::Save(FILEHANDLE scn) {

	oapiWriteScenario_string(scn, "<INTERNALS>","");
	oapiWriteScenario_string(scn, "<VERSION>", PANELSDK_VERSION);
	
	HYDRAULIC->Save(scn);
	ELECTRIC->Save(scn);
	for (int i=0;i<NumPanels;i++)
		panels[i]->Save(scn);
	
	oapiWriteScenario_string (scn, "</INTERNALS>","");
}

void PanelSDK::Timestep(double time) 

{
	if (!firstTimestepDone) {
		lastTime = time;
		firstTimestepDone = true;
		return;
	}

	double dt = time - lastTime;
	lastTime = time;

	double mintFactor = __max(dt / 100.0, 0.5);
	double tFactor = __min(mintFactor, dt);
	while (dt > 0) {
		THERMAL->Radiative(tFactor);
		HYDRAULIC->Refresh(tFactor);
		ELECTRIC->Refresh(tFactor);

		dt -= tFactor;
		tFactor = __min(mintFactor, dt);
	}
}

void PanelSDK::SimpleTimestep(double simdt) 

{
	THERMAL->Radiative(simdt);
	HYDRAULIC->Refresh(simdt);
	ELECTRIC->Refresh(simdt);
}

void PanelSDK::SetStage(int stage,int load)
{
if ((!load)&&(stage-1!=CurentStage)) return; //only process succesive separations
if (!load) //then we are staging
		VESSELMGMT->Separation(stage); //do the staging stuff
VESSELMGMT->SetConfig(stage,load);
PROPELLANT_HANDLE ph_vent=v->CreatePropellantResource(0);
HYDRAULIC->ConfigStage(stage);
ELECTRIC->ConfigStage(stage);
HYDRAULIC->ProcessShip(v,ph_vent);
CurentStage=stage;

}

void PanelSDK::MFDEvent(int mfd)
{
	if (Current_Panel<0) Current_Panel=0;
	if (Current_Panel>NumPanels) Current_Panel=NumPanels;
oapiTriggerPanelRedrawArea(Current_Panel,
						   panels[Current_Panel]->mfd_idx[mfd]);
};

int PanelSDK::KeybEvent(DWORD key,char *kstate)
{
	KeyPress *runner;
	runner=VESSELMGMT->DefinedKeys;

	while ((runner)&&(runner->key!=key))
			runner=runner->next;
	if (runner)
			if (runner->trigger_type==1)
					SetStage(runner->index,0); //separate

	return 0;
}

void PanelSDK::AddElectrical(e_object *e, bool can_delete)

{
	ELECTRIC->AddSystem(e);
	e->deletable = can_delete;
}

void PanelSDK::AddHydraulic(h_object *h)

{
	HYDRAULIC->AddSystem(h);
}

void PanelSDK::AddThermal(therm_obj *t)

{
	THERMAL->AddThermalObject(t,false);
}
