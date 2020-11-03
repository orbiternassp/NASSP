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

#include <fstream>
#include <stdio.h>
#include <string.h>
#include "instruments.h"
#include "vsmgmt.h"
#include "Internals/Hsystems.h"
#include "Internals/Esystems.h"
#include "oapichar.cpp"

int Line_Number;
char I_line[275];
double buffer_space[50];
FILE *config_file;
FILE *resources;
FILE *debug;
char* ReadConfigLine()
{if (!feof(config_file))
{ 	fgets(I_line,255,config_file);
    I_line[strlen(I_line)-1]=0; //drop the CR?
	Line_Number++;	//counter for the line we are reading
	int i;
	for (i=0; i < (int) strlen(I_line);i++)
			if (I_line[i]==9) I_line[i]=' '; //remove the tabs
	for (i=0; i < (int) strlen(I_line);i++)
			if (I_line[i]=='#') I_line[i]=0; //block the comments out!
	for (i=0; i < (int) strlen(I_line);i++)
			if (I_line[i]!=' ')
					return &I_line[i];//then return the first non-space caracter
}
return NULL;
}
char *ReadResourceLine()
{if (!feof(resources))
{
	fgets(I_line,255,resources);
	I_line[strlen(I_line)-1]=0; //drop the CR?
	Line_Number++;	//counter for the line we are reading
	int i;
	for (i=0; i < (int) strlen(I_line);i++)
			if (I_line[i]==9) I_line[i]=' '; //remove the tabs
	for (i=0; i < (int) strlen(I_line);i++)
			if (I_line[i]=='#') I_line[i]=0; //block the comments out!
	for (i=0; i < (int) strlen(I_line);i++)
			if (I_line[i]!=' ')
					return &I_line[i];//then return the first non-space caracter
}
return NULL;
};

void BuildError(int err)
{

#ifdef _DEBUG

	char ErrorTable[3][50]={" 01:Object name not found\n",
							" 02:Invalid component\n",
							" 03:Invalid number of parameters\n"
							};
	char ert[30];

	sprintf(ert,"ERROR AT LINE: %i",Line_Number);
	fputs(ert,debug);
	fputs(I_line,debug);
	fputs(ErrorTable[err],debug);

	// debug<<"ERROR AT LINE "<<Line_Number<<": \n";
	// debug<<I_line<<"\n\t"<<ErrorTable[err]<<'\n';
	// debug.flush();//make sure this gets to disk .. we might have to crash
#endif
}


//*******************************************************
// PanelResouces
// Loads up a bunch of instrument descriptors from a prd file
//
//*******************************************************
void PanelSDK::PanelResources(char *FileName)
{
char name[150];
char string1[25];
int int1;
char *line;
sprintf(name,"config\\%s.prd",FileName);
resources=fopen(name,"rt");
while (!feof(resources)){
line=ReadResourceLine();
if (Compare(line,"<")){	//new instrument descriptor
	InstrumentDescriptor *new_ins= new InstrumentDescriptor;
	//sscanf(line+1,"%s>",new_ins->name);// the name of the new instrument
	strcpy(new_ins->name,line);
	while (!Compare(line,"</")) {
		line=ReadResourceLine();
		if (Compare(line,"CLASS"))
						sscanf(line+5,"%s",new_ins->class_name);
		else if (Compare(line,"SIZE"))
						sscanf(line+4,"%i %i",&(new_ins->p_int1),&(new_ins->p_int2));
		else if (Compare(line,"BORDER"))
						sscanf(line+6,"%i %i",&(new_ins->p_int3),&(new_ins->p_int4));
		else if (Compare(line,"BUTTONSIZE"))
						sscanf(line+10,"%i %i",&(new_ins->p_int7),&(new_ins->p_int8));
		else if (Compare(line,"BUTTONS"))
						sscanf(line+7,"%i %i",&(new_ins->p_int5),&(new_ins->p_int6));
		else if (Compare(line,"BUGSIZE"))
						sscanf(line+7,"%i %i",&(new_ins->p_int3),&(new_ins->p_int4));
		else if (Compare(line,"TRACE"))
				    sscanf(line+5,"%i",&(new_ins->p_int5));
		else if (Compare(line,"BITMAP"))
						new_ins->SurfaceIndex[0]=AddBitmapResource(line+7);
		else if (Compare(line,"MASK1"))
						new_ins->SurfaceIndex[1]=AddBitmapResource(line+6);
		else if (Compare(line,"MASK2"))
						new_ins->SurfaceIndex[2]=AddBitmapResource(line+6);
		else if (Compare(line,"CENTER"))
					sscanf(line+6,"%i %i",&(new_ins->p_int3),&(new_ins->p_int4));
		else if (Compare(line,"MAX"))
					sscanf(line+3,"%lf %lf",&(new_ins->p_double1) ,&(new_ins->p_double2));
		else if (Compare(line,"MIN"))
				    sscanf(line+3,"%lf %lf",&(new_ins->p_double3 ),&(new_ins->p_double4));
		else if (Compare(line,"NEEDLE"))
				    sscanf(line+6,"%i %i",&(new_ins->p_int5) ,&(new_ins->p_int6));
		else if (Compare(line,"FONT")){
					sscanf(line+4,"%s %i",string1,&int1);
					new_ins->FontIndex=AddFontResource(line+4,int1);
					new_ins->p_double1=int1;}
		else if (Compare(line,"COLOR"))
					{sscanf(line+5,"%i %i %i",&(new_ins->p_int1),&(new_ins->p_int2),&(new_ins->p_int3));
					new_ins->SurfaceIndex[1]=AddBrushResource(new_ins->p_int1,new_ins->p_int2,new_ins->p_int3);}
		else if (Compare(line,"BUTTONCOL"))
					{sscanf(line+9,"%lf %lf %lf",&(new_ins->p_double1),&(new_ins->p_double2),&(new_ins->p_double3));
					}
		else if (Compare(line,"NOCOL"))
					{sscanf(line+5,"%i %i %i",&(new_ins->p_int4),&(new_ins->p_int5),&(new_ins->p_int6));
					new_ins->SurfaceIndex[2]=AddBrushResource(new_ins->p_int4,new_ins->p_int5,new_ins->p_int6);}
		else if (Compare(line,"BALTEX"))
					strcpy(new_ins->param_string,line+6);
		else if (Compare(line,"ZOOM"))
					sscanf(line+4,"%lf",&(new_ins->p_double1));
		else if (Compare(line,"LIGHT"))
					sscanf(line+5,"%lf %lf %lf",&(new_ins->p_double2),&(new_ins->p_double3),&(new_ins->p_double4));
		else if (Compare(line,"AMBIENT"))
					sscanf(line+8,"%i %i %i",&(new_ins->p_int3),&(new_ins->p_int4),&(new_ins->p_int5));
        else if (Compare(line,"LGHTCOL"))
					sscanf(line+8,"%i %i %i",&(new_ins->p_int6),&(new_ins->p_int7),&(new_ins->p_int8));
		else if (Compare(line,"ROTATE"))
				    sscanf(line+6,"%lf",&(new_ins->p_double5));
		else if (Compare(line,"BLINK"))
				    sscanf(line+5,"%lf",&(new_ins->p_double1));

	};

	AddDescriptor(new_ins);

};//end  of new descriptor
}
fclose(resources);
}

//*******************************************************
// InitFromFile
// Loads everything needed from the .cfg file of the module
//
//*******************************************************
void PanelSDK::InitFromFile(char *FileName)

{char *line;
char name[40];
char type[40];
int stage;
sprintf(name,"Config\\\\%s.cfg",FileName);

config_file=fopen(name,"rt");

#ifdef _DEBUG
debug=fopen("ProjectApollo PanelSDK.log","wt");
#endif

while (!feof(config_file))
{
	line=ReadConfigLine();
	if (Compare(line,"Panel"))	//pointer to the PRD
		{sscanf(line+7,"%s",name);
				PanelResources(name);
		}
	else if (Compare(line,"<VESSEL>"))
		VESSELMGMT->Build();
	else if (Compare(line,"<HYDRAULIC>"))
		HYDRAULIC->Build();
	else if (Compare(line,"<ELECTRIC>"))
		ELECTRIC->Build();
	else if (Compare(line,"<PANEL>"))
		{sscanf(line+7,"%s",name);
				DoPanel(name);
		}
	else if (Compare(line,"<KEYBOARD>"))
	{	line=ReadConfigLine();
		while (!Compare(line,"</KEYBOARD>")){

			sscanf(line,"%s %s %i",name, type, &stage);
			KeyPress *new_k=new KeyPress;
			new_k->index=stage;
			if (Compare(type,"SEPARATION")) new_k->trigger_type=1;
			else new_k->trigger_type=2;
			new_k->key=GetDWORDfromChar(&name[0]);
			if (VESSELMGMT->DefinedKeys)
					VESSELMGMT->DefinedKeys->AddToList(new_k);
			else VESSELMGMT->DefinedKeys=new_k;
			line=ReadConfigLine();
											}
	}
    else if (Compare(line,"<SEPARATION>"))
		while (!Compare(line,"</SEPARATION>")){
				line=ReadConfigLine();
				if (Compare(line,"<CONFIG>")){
					sscanf(line+8,"%i",&stage);
					while (!Compare(line,"</CONFIG>")){
						if (Compare(line,"<DELETE>"))
							if (Compare(line+9,"HYDRAULIC:"))
								HYDRAULIC->SetMaxStage(line+19,stage);
							else if (Compare(line+9,"ELECTRIC:"))
								ELECTRIC->SetMaxStage(line+18,stage);
						if (Compare(line,"<CGOFFSET>"))
							sscanf(line+10," <%lf %lf %lf>",
							&VESSELMGMT->Stages[stage].cgoffset.x,
							&VESSELMGMT->Stages[stage].cgoffset.y,
							&VESSELMGMT->Stages[stage].cgoffset.z);
						if (Compare(line,"<EJECT>"))
						{ Ejector *new_e=new Ejector;
						  sscanf(line+7," %s <%lf %lf %lf> <%lf %lf %lf>",
								new_e->class_name,
								&new_e->offset.x,
								&new_e->offset.y,
								&new_e->offset.z,
								&new_e->rotation.x,
								&new_e->rotation.y,
								&new_e->rotation.z);
						  if (VESSELMGMT->Stages[stage].EJ_list)
						  VESSELMGMT->Stages[stage].EJ_list->AddToList(new_e);
						  else VESSELMGMT->Stages[stage].EJ_list=new_e;



								}
						line=ReadConfigLine();
														}
												}

		}
	//do something
	//do something
};
fclose(config_file);

#ifdef _DEBUG
fclose(debug);
#endif

if (NumPanels) panels[0]->GDI_Init_Resources();
THERMAL->InitThermal();
THERMAL->v=v;
}

void PanelSDK::DoPanel(char *name)
{
panels[NumPanels]=new Panel;
panels[NumPanels]->GDI_res=GDI_res; //same as before
panels[NumPanels]->idx=NumPanels;
panels[NumPanels]->v=v;
panels[NumPanels]->parent=this;
strcpy(panels[NumPanels]->name,name);
char *line;
char string1[25];
char string2[25];
char Pointer[25];
char up_text[25];
char down_text[25];
char iname[25];
int x,y,i;
int int1,int2,int3;
double dbl1,dbl2,dbl3,dbl4,dbl5;
InstrumentDescriptor *descriptor;
line=ReadConfigLine();
while (!Compare(line,"</PANEL>"))
{   iname[0]=0;
	if (Compare(line,"SIZE"))
		sscanf(line+4,"%i %i",&(panels[NumPanels]->Wdth),&(panels[NumPanels]->Hght));
	else if (Compare(line,"BITMAP"))
		strcpy(panels[NumPanels]->BackgroundName,line+7);
	else if (Compare(line,"NEIGHBOURS"))
		sscanf(line+10,"%i %i %i %i",&(panels[NumPanels]->neighbours[0]),
									&(panels[NumPanels]->neighbours[1]),
									&(panels[NumPanels]->neighbours[2]),
									&(panels[NumPanels]->neighbours[3]));
	else if (Compare(line,"TRANSPARENT")){
		sscanf(line +11,"%i %i %i",&int1,&int2,&int3);
		panels[NumPanels]->transparent_color=(int1*256+int2)*256+int3;
	}
	else if (Compare(line,"<SELECTEL>")){	//RotEl class needs no descriptor
			sscanf(line,"%s %s %s %i %i %i",string1,up_text,down_text,&int1,&int2,&int3);
			double **SRC=(double **)GetPointerByString(up_text); //this is what we will change
			int *SEL=(int *)GetPointerByString(down_text);// based on this value
			double **TRG=new double*[int2-int1];
			double *trgi;
			RotEl *new_re=new RotEl(SRC,TRG,SEL,int3,int1,int2,panels[NumPanels]);

			for (int i=int1;i<=int2;i++)
				{line=ReadConfigLine();
				sscanf(line,"%s",string1);
				trgi=(double*)GetPointerByString(string1);// ??this safe
				TRG[i-int1]=trgi;
			}



	}
	else // not a predefined instruction, must be a
	if (descriptor=GetInstrumentDescriptor(line))	//instrument of some sort
	{
		if (Compare(descriptor->class_name,"SWITCH")){
			sscanf(line,"%s %i %i %i %i %s %s",string1,&x,&y,&int1,&int2,Pointer,iname);
			int *SRC=(int*)GetPointerByString(Pointer);
			Switch* new_s=new Switch(x,y,descriptor->p_int1,descriptor->p_int2,int1,int2,SRC,
					panels[NumPanels]);
			strcpy(new_s->name,iname);
			new_s->SRF_index=descriptor->SurfaceIndex[0];//the bitmap assigned
		}else if (Compare(descriptor->class_name,"CSWITCH")){
			sscanf(line,"%s %i %i %i %i %i %s",string1,&x,&y,&int1,&int2,&int3,Pointer);
			int *SRC=(int*)GetPointerByString(Pointer);
			CodeSwitch* new_s=new CodeSwitch(x,y,descriptor->p_int1,descriptor->p_int2,int1,int2,int3,SRC,
					panels[NumPanels]);
			new_s->SRF_index=descriptor->SurfaceIndex[0];//the bitmap assigned
			if (int3==2) //a keyboard press
			{	int2=GetDWORDfromChar(Pointer);
			new_s->code_index=int2;}

		}else if (Compare(descriptor->class_name,"ROTATIONAL")){
			sscanf(line,"%s %i %i %s %s",string1,&x,&y,Pointer,iname);
			double *rot_SRC=(double*)GetPointerByString(Pointer);
			Rotational *new_rot=new Rotational(x,y,	panels[NumPanels]);
			strcpy(new_rot->name,iname);
			new_rot->G=rot_SRC;
			new_rot->SRF_index=descriptor->SurfaceIndex[0];
			new_rot->SetSize(descriptor->p_int1,descriptor->p_int2);
			new_rot->SetRange(descriptor->p_double2,descriptor->p_double1,
							descriptor->p_double4,descriptor->p_double3);
			new_rot->SetCenter(descriptor->p_int3,descriptor->p_int4);
			new_rot->SetNeedleSize(descriptor->p_int5,descriptor->p_int6);
		}else if (Compare(descriptor->class_name,"TEXT")){
			sscanf(line,"%s %i %i %i %i",string1,&x,&y,&int1,&int2);

			int3=0;
			for (i=7;i < (int) strlen(line);i++)
				if (line[i]=='<') {int3=i;break;};
			if (int3)
			  for (i=int3;i < (int) strlen(line);i++)
				if (line[i]=='>') {line[i]=0;};
			CText *new_c=new CText(x,y,int1,int2,&line[int3+1],	panels[NumPanels]);
			new_c->FontIndex=descriptor->FontIndex;
				new_c->SetColor(descriptor->p_int1,descriptor->p_int2,descriptor->p_int3);
			new_c->SetNoCol(descriptor->p_int4,descriptor->p_int5,descriptor->p_int6);
			new_c->ColPenIndex=descriptor->SurfaceIndex[1];
			new_c->NoColPenIndex=descriptor->SurfaceIndex[2];
			new_c->height = (int) descriptor->p_double1;
		}else if(Compare(descriptor->class_name,"LINEAR")){
			sscanf(line,"%s %i %i %lf %lf %lf %lf %s %s",string1,&x,&y,
				&dbl1,&dbl2,&dbl3,&dbl4,Pointer,iname);
			double *SRC=(double*)GetPointerByString(Pointer);
			Linear* new_line=new Linear(x,y,dbl1,dbl2,dbl3,dbl4,SRC,panels[NumPanels]);
			strcpy(new_line->name,iname);
			new_line->size_x=descriptor->p_int1;
			new_line->size_y=descriptor->p_int2;
			new_line->SetMax(descriptor->p_double1,descriptor->p_double2);
			new_line->SetMin(descriptor->p_double3,descriptor->p_double4);
			new_line->SetNeedleSize(descriptor->p_int5,descriptor->p_int6);
			new_line->SetAngle(descriptor->p_double5/180.0*PI);
			new_line->SRF_index=descriptor->SurfaceIndex[0];
		}else if(Compare(descriptor->class_name,"TB")){
			sscanf(line,"%s %i %i %s %s %s",string1,&x,&y,&Pointer,string2,iname);
			int type;
			if (!strnicmp(string2,"ZERO",4)) type=4;
			if (!strnicmp(string2,"NON_ZERO",8)) type=3;
			if (!strnicmp(string2,"GR_ZERO",7)) type=1;
			if (!strnicmp(string2,"SM_ZERO",7)) type=2;
			double *SRC=(double*)GetPointerByString(Pointer);
			TB * new_tb=new TB(x,y,SRC,type,panels[NumPanels]);
			new_tb->size_x=descriptor->p_int1;
			new_tb->size_y=descriptor->p_int2;
			new_tb->SRF_index=descriptor->SurfaceIndex[0];
			new_tb->blink=descriptor->p_double1;
			strcpy(new_tb->name,iname);
		}else if(Compare(descriptor->class_name,"SELECT")){
			sscanf(line,"%s %i %i %i %s",string1,&x,&y, &int1,Pointer);
			Rot *new_rot=new Rot(x,y,int1, (int) descriptor->p_double1, (int) descriptor->p_double3,panels[NumPanels]);
			new_rot->size_x=descriptor->p_int1;
			new_rot->size_y=descriptor->p_int2;
			new_rot->SRF_index=descriptor->SurfaceIndex[0];
			strcpy(new_rot->name,Pointer);
		} else if (Compare(descriptor->class_name,"DIGIT")){
			sscanf (line,"%s %i %i %i %s %s",string1,&x,&y, &int1,Pointer,iname);
			double *SRC=(double*)GetPointerByString(Pointer);
			small_clock* new_sc=new small_clock(x,y,int1,SRC,panels[NumPanels]);
			strcpy(new_sc->name,iname);
			new_sc->size_x=descriptor->p_int1;
			new_sc->size_y=descriptor->p_int2;
			new_sc->SRF_index=descriptor->SurfaceIndex[0];
		}else if (Compare(descriptor->class_name,"MFD")){
			sscanf(line,"%s %i %i %i",string1,&x,&y,&int1);
			inst_MFD *new_mfd=new inst_MFD(x,y,descriptor->p_int1,
											descriptor->p_int2,int1,panels[NumPanels]);
			new_mfd->SetColor((int) descriptor->p_double1,
							  (int) descriptor->p_double2,
							  (int) descriptor->p_double3);
			new_mfd->SetButtons(descriptor->p_int3,
								descriptor->p_int4,
								descriptor->p_int5,
								descriptor->p_int6,
								descriptor->p_int7,

								descriptor->p_int8);
			strcpy(new_mfd->name,"MFD");
			new_mfd->FontIndex=descriptor->FontIndex;
		} else if (Compare(descriptor->class_name,"PLOT")){
			sscanf(line,"%s %i %i %s %lf %lf %s %lf %lf %lf %s",string1, &x, &y,
				up_text,&dbl1,&dbl2,down_text,&dbl3,&dbl4,&dbl5,string2);
			double *SRC1=(double*)GetPointerByString(up_text);
			double *SRC2=(double*)GetPointerByString(down_text);
			Plot * new_plot=new Plot(x,y,SRC2,SRC1,panels[NumPanels]);
			strcpy(new_plot->name,string2);
			new_plot->size_x=descriptor->p_int1;
			new_plot->size_y=descriptor->p_int2;
			new_plot->SRF_index=descriptor->SurfaceIndex[0];
			new_plot->SRF_mask_index=descriptor->SurfaceIndex[1];
			new_plot->SRF_bug_index=descriptor->SurfaceIndex[2];
			new_plot->trace=descriptor->p_int5;
			new_plot->bug_x=descriptor->p_int3;
			new_plot->bug_y=descriptor->p_int4;
			new_plot->x_max = (int) descriptor->p_double1;
			new_plot->y_max = (int) descriptor->p_double2;
			new_plot->x_min = (int) descriptor->p_double3;
			new_plot->y_min = (int) descriptor->p_double4;
			//end of descriptor data
			new_plot->y_scale_min = (int) dbl1;
			new_plot->y_scale_max = (int) dbl2;
			new_plot->x_scale_min = (int) dbl3;
			new_plot->x_scale_max = (int) dbl4;
			new_plot->x_resolution=dbl5;
			//end !!!

		};
	};
	line=ReadConfigLine();
}



NumPanels++;
}

void *PanelSDK::GetPointerByString(char *query)
{
//check the names of all the panels against the query:
if (!strnicmp (query, "HYDRAULIC",9 )) //wants a hydraulic thinige
	return HYDRAULIC->GetPointerByString(query+10);
if (!strnicmp (query, "ELECTRIC",8 )) //wants a electric thinige
	return ELECTRIC->GetPointerByString(query+9);

if (!strnicmp(query,"SHIP:",5))	//it requested a custom pointer
{
CustomVariable *runner=CustomVarList;
while (runner)
		{if (!strnicmp(query+5,runner->name,sizeof(runner->name)))
					return runner->item;
		 runner=runner->next;
		}
BuildError(1);
return NULL;
}
int i;
for (i=0;i<NumPanels+1;i++)
	if (!strnicmp(panels[i]->name,query,strlen(panels[i]->name)))//same name,
				return panels[i]->GetPointerByString(query+strlen(panels[i]->name)+1);
BuildError(1);
return NULL;
};

void* Panel::GetPointerByString(char *query)//panel query is the biggest one.. because panel queries can go lcx,hydraulic and electric
{

if (!strnicmp (query, "LIGHT",5 ))
		return (void*)&pl_handle;
if (!strnicmp (query, "TEXTLIGHT",9 ))
		return (void*)&tl_handle;
if (!strnicmp (query, "MFDMODE",7 ))
		return (void*)&MFD_mode;
if (!strnicmp (query, "HUDMODE",7 ))
		return (void*)&HUD_mode;

//if all is not a panel component, must be an instrument one ?

int len=0;
int i;
for (i=0;i < (int) strlen(query);i++)
	if (query[i]==':') {len=i;break;};
//query[len]=0;
instrument *inst=GetInstrumentByName(query);
query+=len+1;//move over the instrument,
len=0;
for (i=0;i < (int) strlen(query);i++)
	if (query[i]==':') {len=i;break;};
if (inst)
return inst->GetComponent(query);

BuildError(1);
return NULL;
  }

