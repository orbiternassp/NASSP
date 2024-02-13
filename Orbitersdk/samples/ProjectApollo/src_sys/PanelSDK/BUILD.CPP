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
#include "VSMGMT.H"
#include "PanelSDK.h"
#include "Internals/Hsystems.h"
#include "Internals/Esystems.h"
#include "OAPICHAR.CPP"

int Line_Number;
char I_line[275];
double buffer_space[50];
FILE* config_file;
FILE* resources;
FILE* debug;
char* ReadConfigLine()
{
	if (!feof(config_file))
	{
		fgets(I_line, 255, config_file);
		I_line[strlen(I_line) - 1] = 0; //drop the CR?
		Line_Number++;	//counter for the line we are reading
		int i;
		for (i = 0; i < (int)strlen(I_line); i++)
			if (I_line[i] == 9) I_line[i] = ' '; //remove the tabs
		for (i = 0; i < (int)strlen(I_line); i++)
			if (I_line[i] == '#') I_line[i] = 0; //block the comments out!
		for (i = 0; i < (int)strlen(I_line); i++)
			if (I_line[i] != ' ')
				return &I_line[i];//then return the first non-space caracter
	}
	return NULL;
}
char* ReadResourceLine()
{
	if (!feof(resources))
	{
		fgets(I_line, 255, resources);
		I_line[strlen(I_line) - 1] = 0; //drop the CR?
		Line_Number++;	//counter for the line we are reading
		int i;
		for (i = 0; i < (int)strlen(I_line); i++)
			if (I_line[i] == 9) I_line[i] = ' '; //remove the tabs
		for (i = 0; i < (int)strlen(I_line); i++)
			if (I_line[i] == '#') I_line[i] = 0; //block the comments out!
		for (i = 0; i < (int)strlen(I_line); i++)
			if (I_line[i] != ' ')
				return &I_line[i];//then return the first non-space caracter
	}
	return NULL;
};

void BuildError(int err)
{

#ifdef _DEBUG

	char ErrorTable[3][50] = { " 01:Object name not found\n",
							" 02:Invalid component\n",
							" 03:Invalid number of parameters\n"
	};
	char ert[30];

	sprintf(ert, "ERROR AT LINE: %i", Line_Number);
	fputs(ert, debug);
	fputs(I_line, debug);
	fputs(ErrorTable[err], debug);

	// debug<<"ERROR AT LINE "<<Line_Number<<": \n";
	// debug<<I_line<<"\n\t"<<ErrorTable[err]<<'\n';
	// debug.flush();//make sure this gets to disk .. we might have to crash
#endif
}


//*******************************************************
// InitFromFile
// Loads everything needed from the .cfg file of the module
//
//*******************************************************
void PanelSDK::InitFromFile(char* FileName)

{
	char* line;
	char name[40];
	char type[40];
	int stage;
	sprintf(name, "Config\\\\%s.cfg", FileName);

	config_file = fopen(name, "rt");

#ifdef _DEBUG
	debug = fopen("ProjectApollo PanelSDK.log", "wt");
#endif

	while (!feof(config_file))
	{
		line = ReadConfigLine();
		if (Compare(line, "<VESSEL>"))
			VESSELMGMT->Build();
		else if (Compare(line, "<HYDRAULIC>"))
			HYDRAULIC->Build();
		else if (Compare(line, "<ELECTRIC>"))
			ELECTRIC->Build();
		else if (Compare(line, "<KEYBOARD>"))
		{
			line = ReadConfigLine();
			while (!Compare(line, "</KEYBOARD>")) {

				sscanf(line, "%s %s %i", name, type, &stage);
				KeyPress* new_k = new KeyPress;
				new_k->index = stage;
				if (Compare(type, "SEPARATION")) new_k->trigger_type = 1;
				else new_k->trigger_type = 2;
				new_k->key = GetDWORDfromChar(&name[0]);
				if (VESSELMGMT->DefinedKeys)
					VESSELMGMT->DefinedKeys->AddToList(new_k);
				else VESSELMGMT->DefinedKeys = new_k;
				line = ReadConfigLine();
			}
		}
		else if (Compare(line, "<SEPARATION>"))
			while (!Compare(line, "</SEPARATION>")) {
				line = ReadConfigLine();
				if (Compare(line, "<CONFIG>")) {
					sscanf(line + 8, "%i", &stage);
					while (!Compare(line, "</CONFIG>")) {
						if (Compare(line, "<DELETE>"))
							if (Compare(line + 9, "HYDRAULIC:"))
								HYDRAULIC->SetMaxStage(line + 19, stage);
							else if (Compare(line + 9, "ELECTRIC:"))
								ELECTRIC->SetMaxStage(line + 18, stage);
						if (Compare(line, "<CGOFFSET>"))
							sscanf(line + 10, " <%lf %lf %lf>",
								&VESSELMGMT->Stages[stage].cgoffset.x,
								&VESSELMGMT->Stages[stage].cgoffset.y,
								&VESSELMGMT->Stages[stage].cgoffset.z);
						if (Compare(line, "<EJECT>"))
						{
							Ejector* new_e = new Ejector;
							sscanf(line + 7, " %s <%lf %lf %lf> <%lf %lf %lf>",
								new_e->class_name,
								&new_e->offset.x,
								&new_e->offset.y,
								&new_e->offset.z,
								&new_e->rotation.x,
								&new_e->rotation.y,
								&new_e->rotation.z);
							if (VESSELMGMT->Stages[stage].EJ_list)
								VESSELMGMT->Stages[stage].EJ_list->AddToList(new_e);
							else VESSELMGMT->Stages[stage].EJ_list = new_e;



						}
						line = ReadConfigLine();
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

	THERMAL->InitThermal();
	THERMAL->v = v;
}

void* PanelSDK::GetPointerByString(char* query)
{
	//check the names of all the panels against the query:
	if (!strnicmp(query, "HYDRAULIC", 9)) //wants a hydraulic thinige
		return HYDRAULIC->GetPointerByString(query + 10);
	if (!strnicmp(query, "ELECTRIC", 8)) //wants a electric thinige
		return ELECTRIC->GetPointerByString(query + 9);
	
	BuildError(1);
	return NULL;
};
