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
  *	Revision 1.2  2008/04/11 11:50:16  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.1  2005/04/22 14:22:37  tschachim
  *	Initial version
  *	
  **************************************************************************/

#if defined(_MSC_VER) && (_MSC_VER >= 1300 ) // Microsoft Visual Studio Version 2003 and higher
#define _CRT_SECURE_NO_DEPRECATE 
#endif

#include "instruments.h"
#include <stdio.h>

GDI_resources::~GDI_resources()
{

if (Panel_Resources_Loaded){
  if (hPEN_NULL) {	DeleteObject(hPEN_NULL);hPEN_NULL=NULL;};
  if (hPEN_Cyan) {	DeleteObject(hPEN_Cyan);hPEN_Cyan=NULL;};
  if (hPEN_Green){	DeleteObject(hPEN_Green);hPEN_Green=NULL;};
  if (hBRUSH_TotalBlack){ DeleteObject(hBRUSH_TotalBlack);hBRUSH_TotalBlack=NULL;};
  if (hBRUSH_TotalWhite)   DeleteObject(hBRUSH_TotalWhite);
  if (hBRUSH_Gray)			DeleteObject(hBRUSH_Gray);
//  if (hFNT_Panel)			DeleteObject(hFNT_Panel);
  if (hPEN_Brown)			DeleteObject(hPEN_Brown);
}
int i;
for (i=1;i<num_surfaces;i++)
	if (h_Surface[i]) oapiDestroySurface(h_Surface[i]);
for (i=1;i<num_fonts;i++)
	if (hFNT_Panel[i]) DeleteObject(hFNT_Panel[i]);
for (i=1;i<num_brush;i++)
	if (hPEN[i]) DeleteObject(hPEN[i]);
};

Panel::Panel()
{instruments=NULL;//no instruments
GDI_res=NULL;
panel_logic=0;    //no lights.
text_logic=0;
pl_handle=-1;
tl_handle=-1;
HUD_mode=0;
MFD_mode=-1;
};

Panel::~Panel()
{
  instrument_list *runner=instruments;
  instrument_list *gone;
  while (runner){ gone=runner;
				  runner=runner->next;
				  delete gone->instance;
				  delete gone;
  };
};
HBITMAP Panel::MakeYourBackground()
{
	return (HBITMAP)LoadImage(NULL,BackgroundName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

}
void Panel::SetNeighbours()
{
oapiSetPanelNeighbours (neighbours[0],neighbours[1],neighbours[2],neighbours[3]);
};

void Panel::CodeCallFromSwitch(int index)//only first 3 are handled by panel
{
instrument_list *runner=instruments;
switch (index)
{
case 1:	//panel logic & lights switch
	panel_logic=(pl_handle+1)/2;
	text_logic=(tl_handle+1)/2;
	while (runner)
		{ if (runner->instance)
				runner->instance->SetLight();
				runner=runner->next;
		};
	break;

//case 2:	//HUD mode
	//oapiSetHUDMode(HUD_mode+1);
	//break;
//case 3: //MFD mode
	//if (MFD_mode<1)	oapiOpenMFD(0,0);//turn it off
	//break;
//case 4: //some keyboard trigger
	//we had some sort of a keyboard event maybe?
}
if (index>1) //anything BUT escape
	parent->KeybEvent((DWORD)index,0);

};

void Panel::AddInstrument(instrument *new_inst)
{ instrument_list *new_item=new instrument_list;
  new_item->instance=new_inst;
  new_item->next=instruments;
  instruments=new_item;
};
void Panel::RegisterYourInstruments()
{  instrument_list *runner=instruments;
  int index=0;
  while (runner)
  { if (runner->instance)
				{runner->instance->RegisterMe(index++); //now register all the inst. in the list
				if (!strnicmp(runner->instance->name,"MFD",3))
					{inst_MFD *mfd=(inst_MFD*)runner->instance;
						mfd_idx[mfd->mfd_type]=mfd->idx;
				};
					//mfd_idx[runner->instance->type
  }
    runner=runner->next;
  }
};
void Panel::Paint(int index)
{ instrument_list *runner=instruments;
   for (int i=0;i<index;i++) runner=runner->next;
   runner->instance->PaintMe();

}
void Panel::Refresh(int index)
{ instrument_list *runner=instruments;
   for (int i=0;i<index;i++) runner=runner->next;
   runner->instance->RefreshMe();

}
void Panel::LBD(int index,int x,int y)
{ instrument_list *runner=instruments;
   for (int i=0;i<index;i++) runner=runner->next;
   runner->instance->LBD( x, y);

}
void Panel::RBD(int index,int x,int y)
{ instrument_list *runner=instruments;
   for (int i=0;i<index;i++) runner=runner->next;
   runner->instance->RBD( x, y);

}
void Panel::BU(int index)
{ instrument_list *runner=instruments;
   for (int i=0;i<index;i++) runner=runner->next;
   runner->instance->BU();

}

void Panel::Save(FILEHANDLE scn)
{ instrument_list *runner=instruments;
  char cbuf[80];
//  int int_sv[10];
  int index=0;
sprintf(cbuf,"%i",idx);

  oapiWriteScenario_string (scn, "<PANEL>", cbuf);
  oapiWriteScenario_int(scn,"<TEXT>",text_logic);
  oapiWriteScenario_int(scn,"<LIGHT>",panel_logic);
  oapiWriteScenario_string (scn, "</PANEL>", cbuf);
}
void Panel::Load(FILEHANDLE scn)
{ instrument_list *runner=instruments;
// char *line;
// int int_sv[10];
};
instrument* Panel::GetInstrumentByName(char *inst_name)
{
instrument_list *runner=instruments;
while (runner){
	  	if ((!strnicmp (runner->instance->name, inst_name,strlen(runner->instance->name)))&&(strlen(runner->instance->name)>0))
				return runner->instance;
		runner=runner->next;
}
return NULL;//problem, yes

}

void Panel::GDI_Init_Resources()
{
  if (GDI_res==NULL) return; //nothing to load
 if (GDI_res->Panel_Resources_Loaded) return; //already loaded
 //load the default resources first
  //GDI_res->hFNT_MFD=CreateFont(12,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_RASTER_PRECIS,
	//		 CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"Arial");
  GDI_res->hPEN_NULL=CreatePen(PS_NULL,1,RGB(0,0,0));
  GDI_res->hPEN_Cyan=CreatePen(PS_SOLID,1,RGB(0,95,235));
  GDI_res->hPEN_Green=CreatePen(PS_SOLID,1,RGB(0,255,0));
  GDI_res->hBRUSH_TotalBlack=CreateSolidBrush(RGB(0,0,0));
  GDI_res->hBRUSH_TotalWhite=CreateSolidBrush(RGB(255,255,255));
  GDI_res->hBRUSH_Gray=CreateSolidBrush(RGB(200,200,200));
  GDI_res->hPEN_Brown=CreatePen(PS_SOLID,1,RGB(90,90,90));

  GDI_res->Panel_Resources_Loaded=1;
 }


void Panel::SetGDI(GDI_resources *GDI)
{GDI_res=GDI;};

