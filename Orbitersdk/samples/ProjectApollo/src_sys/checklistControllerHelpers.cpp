/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008 

  Checklist controller

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


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "checklistController.h"

// Code to make the compiler shut up.
#pragma warning ( push )
#pragma warning ( disable:4018 )
#pragma warning ( disable:4996 )

using namespace std;
// Event enum methods.
// Todo: Verify
RelativeEvent checkEvent(const char* input, bool Group)
{	
	if (!Group && !strnicmp(input,"HIDDEN_DELAY",12))
		return HIDDEN_DELAY;
	if (!Group && !strnicmp(input,"LAST_ITEM_RELATIVE",18))
		return LAST_ITEM_RELATIVE;
	if (!Group && !strnicmp(input,"CHECKLIST_RELATIVE",18))
		return CHECKLIST_RELATIVE;
	if (!strnicmp(input,"EARTH_ORBIT_INSERTION",21))
		return EARTH_ORBIT_INSERTION;
	if (!strnicmp(input,"PAYLOAD_EXTRACTION",18))
		return PAYLOAD_EXTRACTION;
	if (!strnicmp(input,"MISSION_TIME",12))
		return MISSION_TIME;
	if (!strnicmp(input,"SECOND_STAGE_STAGING",20))
		return SECOND_STAGE_STAGING;
	if (!strnicmp(input,"CSM_LV_SEPARATION_DONE",22))
		return CSM_LV_SEPARATION_DONE;
	if (!strnicmp(input,"CSM_LV_SEPARATION",17))
		return CSM_LV_SEPARATION;
	if (!strnicmp(input,"TOWER_JETTISON",14))
		return TOWER_JETTISON;
	if (!strnicmp(input,"SPLASHDOWN",10))
		return SPLASHDOWN;
	if (!strnicmp(input,"BACKUP_CREW_PRELAUNCH",21))
		return BACKUP_CREW_PRELAUNCH;
	if (!strnicmp(input,"PRIME_CREW_PRELAUNCH",20))
		return PRIME_CREW_PRELAUNCH;
	if (!strnicmp(input,"SIVB_STAGE_STAGING",18))
		return SIVB_STAGE_STAGING;
	if (!strnicmp(input,"CM_SM_SEPARATION_DONE",21))
		return CM_SM_SEPARATION_DONE;
	if (!strnicmp(input,"CM_SM_SEPARATION",16))
		return CM_SM_SEPARATION;
	if (!strnicmp(input,"TLI",3))
		return TLI;
	return NO_TIME_DEF;
}
//Checklist Item methods.

ChecklistItem::ChecklistItem() 
{
	group = -1;
	index = -1;
	time = 0;
	relativeEvent = NO_TIME_DEF;
	failGroup = -1;
	callGroup = -1;
	text[0] = 0;
	panel[0] = 0;
	heading1[0] = 0;
	heading2[0] = 0;
	info[0] = 0;
	automatic = false;
	item[0] = 0;
	position = 0;
	guard = false;
	hold = false;
	lineFeed = false;
	status = PENDING;
	dskyIndex = 0;
	dskyNo = 0;
	dskyPressed = false;
}

// Todo: Verify
bool ChecklistItem::operator==(ChecklistItem input)
{
	if (input.group == this->group)
	{
		if (input.index == this->index)
		{
			return true;
		}
	}
	return false;
}
// Todo: Verify
void ChecklistItem::init(vector<BasicExcelCell> &cells, const vector<ChecklistGroup> &groups)
{
	if (cells[0].GetString())
		strncpy(text,cells[0].GetString(),100);
	else
		return;

	time = cells[1].GetDouble();
	
	if (cells[2].GetString())
		relativeEvent = checkEvent(cells[2].GetString());
	else
		relativeEvent = NO_TIME_DEF;
	
	if (cells[3].GetString())
		strncpy(panel, cells[3].GetString(),100);
	else if (cells[3].GetInteger() != 0) 
		_snprintf(panel, 100, "%d", cells[3].GetInteger()); 
	
	if (cells[4].GetString())
		strncpy(heading1, cells[4].GetString(),100);
	if (cells[5].GetString())
		strncpy(heading2, cells[5].GetString(),100);

	if (cells[6].GetString())
		lineFeed = true;

	if (cells[7].GetString())
		strncpy(info, cells[7].GetString(),300);
	if (cells[8].GetString())
		strncpy(item, cells[8].GetString(),100);

	const char *c9 = cells[9].GetString(); 
	if (c9) {
		sscanf(c9, "%d", &position);
		if (!strnicmp(c9 + strlen(c9) - 1, "G", 1))
			guard = true;
		if (!strnicmp(c9 + strlen(c9) - 1, "H", 1))
			hold = true;
	} else 
		position = cells[9].GetInteger();
	
	automatic = (cells[10].GetInteger() != 0);

	callGroup = -1;
	if (cells[11].GetString()) {
		for (int i = 0; i < groups.size(); i ++) {
			if (!stricmp(cells[11].GetString(), groups[i].name))
				callGroup = groups[i].group;
		}
	}

	failGroup = -1;
	if (cells[12].GetString()) {
		for (int i = 0; i < groups.size(); i ++) {
			if (!stricmp(cells[12].GetString(), groups[i].name))
				failGroup = groups[i].group;
		}
	}

	if (strnicmp(item, "DSKY", 4) == 0) {
		char seps[] = " \t\n";
		char *token;
		DSKYChecklistItem temp;

		token = strtok(item, seps);
		if (token != NULL ) {
			if (stricmp(token, "DSKY1") == 0) 
				dskyNo = 1;
			else if (stricmp(token, "DSKY2") == 0) 
				dskyNo = 2;
			else
				dskyNo = 3;

			token = strtok(NULL, seps );
			while( token != NULL ) {
				temp.init(token);
				dskyItemsSet.push_back(temp);
				temp = DSKYChecklistItem();
				token = strtok(NULL, seps );
			}
			strcpy(item, "DSKY");
		}
	}
}

void ChecklistItem::setFlashing(MFDConnector *conn, bool flashing) {

	if (!stricmp(item, "DSKY")) {
		if (dskyItemsSet.size() > 0) {
			for (int i = 0; i < dskyItemsSet.size(); i++) {
				conn->SetFlashing(dskyItemsSet[i].item, false);
				conn->SetFlashing(dskyItemsSet[i].item2, false);
			}
			if (dskyNo & 1)
				conn->SetFlashing(dskyItemsSet[dskyIndex].item, flashing);
			if (dskyNo & 2)
				conn->SetFlashing(dskyItemsSet[dskyIndex].item2, flashing);
		}
	} else {
		conn->SetFlashing(item, flashing);
	}
}

bool ChecklistItem::iterate(MFDConnector *conn, bool autoexec) {

	if (!autoexec) {
		if (!stricmp(item, "DSKY")) {
			if (dskyItemsSet.size() > 0) {
				if (!dskyPressed) {
					if ((((dskyNo & 1) != 0) && conn->GetState(dskyItemsSet[dskyIndex].item) == position) ||
						(((dskyNo & 2) != 0) && conn->GetState(dskyItemsSet[dskyIndex].item2) == position)) {
						dskyPressed = true;
					} 
				} else {
					if (conn->GetState(dskyItemsSet[dskyIndex].item) != position && 
						conn->GetState(dskyItemsSet[dskyIndex].item2) != position) {
						dskyPressed = false;
						dskyIndex++;
					}
				}
			}
			if (dskyIndex >= dskyItemsSet.size()) {
				dskyIndex = 0;
				return true;
			}
		} else {
			if (conn->GetState(item) == position) {
				return true;
			}
		}
	} else {
		if (!stricmp(item, "DSKY")) {
			if (dskyItemsSet.size() > 0) {
				if (dskyNo & 1) {
					if (conn->SetState(dskyItemsSet[dskyIndex].item, position)) {
						dskyIndex++;
					} 
				} else {
					if (conn->SetState(dskyItemsSet[dskyIndex].item2, position)) {
						dskyIndex++;
					} 
				}
			}
			if (dskyIndex >= dskyItemsSet.size()) {
				dskyIndex = 0;
				return true;
			}
		} else {
			if (position == -1) {
				return true;
			} else if (conn->SetState(item, position, guard, hold)) {
				return true;
			}
		}
	}
	return false;
}

double ChecklistItem::checkIterate(MFDConnector *conn) {

	if (!stricmp(item, "DSKY")) {
		return false;
	}
	if (position == -1) {
		return true;
	}
	if (conn->GetState(item) == position) {
		return true;
	}
	return false;
}


double ChecklistItem::getAutoexecuteSlowDelay(MFDConnector *conn) {

	if (!stricmp(item, "DSKY")) {
		if (dskyIndex == 0) {
			return 4;
		} else {
			return 1;
		}
	}
	if (position == -1) {
		return 2;
	}
	if (conn->GetState(item) == position) {
		return 2;
	}
	return 4;
}

// Todo: Verify
void ChecklistItem::load(FILEHANDLE scn)
{
	char *line,buffer[100];
	buffer[0] = 0;
	line = "";
	int i;
	oapiReadScenario_nextline(scn, line);
	while(strnicmp(line,ChecklistItemEndString,strlen(ChecklistItemEndString)))
	{
		bool found = false;
		if (!found && !strnicmp(line,"STATUS",6))
		{
			sscanf(line+6,"%d",&i);
			status = static_cast<Status> (i);
			found = true;
		}
		if (!found && !strnicmp(line,"DSKYINDEX",9))
		{
			sscanf(line+9,"%d",&dskyIndex);
			found = true;
		}
		oapiReadScenario_nextline(scn, line);
	}
}
// Todo: Verify
void ChecklistItem::save(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn,ChecklistItemStartString,"");
	oapiWriteScenario_int(scn,"INDEX",index);
	oapiWriteScenario_int(scn,"STATUS",status);
	oapiWriteScenario_int(scn,"DSKYINDEX",dskyIndex);

	oapiWriteScenario_string(scn,ChecklistItemEndString,"");
}
// Todo: Verify
bool ChecklistItem::checkExec(double lastMissionTime, double checklistStart, double lastItemTime, SaturnEvents &eventController, bool autoexecuteAllItemsAutomatic)
{
	if (!automatic && !autoexecuteAllItemsAutomatic)
		return false;
	double t = 0;
	switch(relativeEvent)
	{
	case LAST_ITEM_RELATIVE:
	case HIDDEN_DELAY:
		t = lastMissionTime - lastItemTime;
		if (time > t)
			return false;
		break;
	case CHECKLIST_RELATIVE:
		t = lastMissionTime - checklistStart;
		if (time > t)
			return false;
		break;
	case MISSION_TIME:
		if (time > lastMissionTime)
			return false;
		break;
	case EARTH_ORBIT_INSERTION:
		if (eventController.EARTH_ORBIT_INSERTION == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.EARTH_ORBIT_INSERTION;
		if (time > t)
			return false;
		break;
	case SPLASHDOWN:
		if (eventController.SPLASHDOWN == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.SPLASHDOWN;
		if (time > t)
			return false;
		break;
	case BACKUP_CREW_PRELAUNCH:
		if (eventController.BACKUP_CREW_PRELAUNCH == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.BACKUP_CREW_PRELAUNCH;
		if (time > t)
			return false;
		break;
	case PRIME_CREW_PRELAUNCH:
		if (eventController.PRIME_CREW_PRELAUNCH == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.PRIME_CREW_PRELAUNCH;
		if (time > t)
			return false;
		break;
	case SECOND_STAGE_STAGING:
		if (eventController.SECOND_STAGE_STAGING == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.SECOND_STAGE_STAGING;
		if (time > t)
			return false;
		break;
	case SIVB_STAGE_STAGING:
		if (eventController.SIVB_STAGE_STAGING == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.SIVB_STAGE_STAGING;
		if (time > t)
			return false;
		break;
	case TOWER_JETTISON:
		if (eventController.TOWER_JETTISON == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.TOWER_JETTISON;
		if (time > t)
			return false;
		break;
	case CSM_LV_SEPARATION_DONE:
		if (eventController.CSM_LV_SEPARATION_DONE == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.CSM_LV_SEPARATION_DONE;
		if (time > t)
			return false;
		break;
	case CSM_LV_SEPARATION:
		if (eventController.CSM_LV_SEPARATION == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.CSM_LV_SEPARATION;
		if (time > t)
			return false;
		break;
	case CM_SM_SEPARATION:
		if (eventController.CM_SM_SEPARATION == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.CM_SM_SEPARATION;
		if (time > t)
			return false;
		break;
	case CM_SM_SEPARATION_DONE:
		if (eventController.CM_SM_SEPARATION_DONE == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.CM_SM_SEPARATION_DONE;
		if (time > t)
			return false;
		break;
	case TLI:
		if (eventController.TLI == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.TLI;
		if (time > t)
			return false;
		break;
	case PAYLOAD_EXTRACTION:
		if (eventController.PAYLOAD_EXTRACTION == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.PAYLOAD_EXTRACTION;
		if (time > t)
			return false;
		break;
	}
	return true;
}

void DSKYChecklistItem::init(char *k) {

	strncpy(key, k, 10);
	if (!stricmp(key, "V")) {
		strcpy(item, "DskySwitchVerb");
		strcpy(item2, "Dsky2SwitchVerb");
	} else if (!stricmp(key, "N")) {
		strcpy(item, "DskySwitchNoun");
		strcpy(item2, "Dsky2SwitchNoun");
	} else if (!stricmp(key, "+")) {
		strcpy(item, "DskySwitchPlus");
		strcpy(item2, "Dsky2SwitchPlus");	
	} else if (!stricmp(key, "-")) {
		strcpy(item, "DskySwitchMinus");
		strcpy(item2, "Dsky2SwitchMinus");
	} else if (!stricmp(key, "0")) {
		strcpy(item, "DskySwitchZero");
		strcpy(item2, "Dsky2SwitchZero");
	} else if (!stricmp(key, "1")) {
		strcpy(item, "DskySwitchOne");
		strcpy(item2, "Dsky2SwitchOne");
	} else if (!stricmp(key, "2")) {
		strcpy(item, "DskySwitchTwo");
		strcpy(item2, "Dsky2SwitchTwo");
	} else if (!stricmp(key, "3")) {
		strcpy(item, "DskySwitchThree");
		strcpy(item2, "Dsky2SwitchThree");
	} else if (!stricmp(key, "4")) {
		strcpy(item, "DskySwitchFour");
		strcpy(item2, "Dsky2SwitchFour");
	} else if (!stricmp(key, "5")) {
		strcpy(item, "DskySwitchFive");
		strcpy(item2, "Dsky2SwitchFive");
	} else if (!stricmp(key, "6")) {
		strcpy(item, "DskySwitchSix");
		strcpy(item2, "Dsky2SwitchSix");
	} else if (!stricmp(key, "7")) {
		strcpy(item, "DskySwitchSeven");
		strcpy(item2, "Dsky2SwitchSeven");
	} else if (!stricmp(key, "8")) {
		strcpy(item, "DskySwitchEight");
		strcpy(item2, "Dsky2SwitchEight");
	} else if (!stricmp(key, "9")) {
		strcpy(item, "DskySwitchNine");
		strcpy(item2, "Dsky2SwitchNine");
	} else if (!stricmp(key, "C")) {
		strcpy(item, "DskySwitchClear");
		strcpy(item2, "Dsky2SwitchClear");
	} else if (!stricmp(key, "P")) {
		strcpy(item, "DskySwitchProg");
		strcpy(item2, "Dsky2SwitchProg");
	} else if (!stricmp(key, "K")) {
		strcpy(item, "DskySwitchKeyRel");
		strcpy(item2, "Dsky2SwitchKeyRel");
	} else if (!stricmp(key, "E")) {
		strcpy(item, "DskySwitchEnter");
		strcpy(item2, "Dsky2SwitchEnter");
	} else if (!stricmp(key, "R")) {
		strcpy(item, "DskySwitchReset");
		strcpy(item2, "Dsky2SwitchReset");
	}
}

//ChecklistGroup methods.

void ChecklistGroup::init(std::vector<BasicExcelCell> &cells)
{
	if (cells[0].GetString())
		strncpy(name,cells[0].GetString(),100);
	time = cells[1].GetDouble();
	deadline = cells[2].GetDouble();
	if (cells[3].GetString())
		relativeEvent = checkEvent(cells[3].GetString(),true);
	if (cells[4].GetString())
		strncpy(heading, cells[4].GetString(), 100);
	autoSelect = (cells[5].GetInteger() != 0);
	manualSelect = (cells[6].GetInteger() != 0);
	essential = (cells[7].GetInteger() != 0);
	if (cells[8].GetString())
		strncpy(soundFile,cells[8].GetString(),100);
	autoSlow = (cells[9].GetInteger() != 0);
}
// Todo: Verify
void ChecklistGroup::load(FILEHANDLE scn)
{
	char *line,buffer[100];
	buffer[0] = 0;
	line = "";
	while (strnicmp(line,ChecklistGroupEndString,strlen(ChecklistGroupEndString)))
	{
		bool found = false;
		if (!found && !strnicmp(line,"CALLED",6))
		{
			int i;
			sscanf(line+6,"%d",&i);
			called = (i != 0);
			found = true;
		}
		oapiReadScenario_nextline(scn,line);
	}
}
// Todo: Verify
void ChecklistGroup::save(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn,ChecklistGroupStartString,"");
	oapiWriteScenario_int(scn,"INDEX",group);
	oapiWriteScenario_int(scn,"CALLED",called);
	oapiWriteScenario_string(scn,ChecklistGroupEndString,"");
}
// Todo: Verify
bool ChecklistGroup::checkExec(double lastMissionTime,SaturnEvents &eventController)
{
	switch (relativeEvent)
	{	
	case MISSION_TIME:
		if (time <= lastMissionTime && lastMissionTime - time <= deadline)
			return true;
		break;
	case EARTH_ORBIT_INSERTION:
		if (eventController.EARTH_ORBIT_INSERTION != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.EARTH_ORBIT_INSERTION;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case SPLASHDOWN:
		if (eventController.SPLASHDOWN != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.SPLASHDOWN;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case BACKUP_CREW_PRELAUNCH:
		if (eventController.BACKUP_CREW_PRELAUNCH != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.BACKUP_CREW_PRELAUNCH;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case PRIME_CREW_PRELAUNCH:
		if (eventController.PRIME_CREW_PRELAUNCH != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.PRIME_CREW_PRELAUNCH;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case SECOND_STAGE_STAGING:
		if (eventController.SECOND_STAGE_STAGING != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.SECOND_STAGE_STAGING;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case SIVB_STAGE_STAGING:
		if (eventController.SIVB_STAGE_STAGING != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.SIVB_STAGE_STAGING;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case TOWER_JETTISON:
		if (eventController.TOWER_JETTISON != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.TOWER_JETTISON;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case CSM_LV_SEPARATION_DONE:
		if (eventController.CSM_LV_SEPARATION_DONE != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CSM_LV_SEPARATION_DONE;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case CSM_LV_SEPARATION:
		if (eventController.CSM_LV_SEPARATION != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CSM_LV_SEPARATION;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case CM_SM_SEPARATION:
		if (eventController.CM_SM_SEPARATION != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CM_SM_SEPARATION;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case CM_SM_SEPARATION_DONE:
		if (eventController.CM_SM_SEPARATION_DONE != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CM_SM_SEPARATION_DONE;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case TLI:
		if (eventController.TLI != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.TLI;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case PAYLOAD_EXTRACTION:
		if (eventController.PAYLOAD_EXTRACTION != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.PAYLOAD_EXTRACTION;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	}
	return false;
}
//ChecklistContainer methods.

// Todo: Verify
ChecklistContainer::ChecklistContainer()
{
	sequence = set.begin();
	startTime = 0;
}
// Todo: Verify
ChecklistContainer::ChecklistContainer(const ChecklistGroup &inprogram, ChecklistController &controller, double start, bool dontspawn)
{
	program = inprogram;
	startTime = start;
	initSet(inprogram,set,controller);
	sequence = set.begin();
	if (inprogram.soundFile[0])
	{
		controller.soundLib.LoadSound(controller.checkSound,program.soundFile);
		controller.playSound = true;
	}
	if (!dontspawn)
		controller.groups[program.group].called = true;
}
// Todo: Verify
ChecklistContainer::ChecklistContainer(const ChecklistContainer & input)
{
	program = input.program;
	set = input.set;
	sequence = set.begin();
	if (input.set.size())
		sequence += input.sequence->index;
	startTime = input.startTime;
}
// Todo: Verify
void ChecklistContainer::operator=(const ChecklistContainer &input)
{
	program = input.program;
	set = input.set;
	sequence = set.begin();
	if (input.set.size())
		sequence += input.sequence->index;
	startTime = input.startTime;
}
// Todo: Verify
void ChecklistContainer::initSet(const ChecklistGroup &program,vector<ChecklistItem> &set,ChecklistController &controller)
{
	BasicExcelWorksheet* sheet;
	vector<BasicExcelCell> vec_temp;
	sheet = controller.file.GetWorksheet(program.name);
	int rows = sheet->GetTotalRows();
	ChecklistItem temp;
	for (int i = 1; i < rows; i++)
	{
		// Ignore empty texts
		if (sheet->Cell(i,0)->GetString() != 0) {
			for (int ii = 0; ii < 13; ii++)
			{
				vec_temp.push_back(*(sheet->Cell(i,ii)));
			}
			temp.init(vec_temp,controller.groups);
			temp.group = program.group;
			temp.index = set.size();
			set.push_back(temp);
			vec_temp = vector<BasicExcelCell>();
			temp = ChecklistItem();		
		}
	}
}
// Todo: Verify
void ChecklistContainer::save(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn,ChecklistContainerStartString,"");
	oapiWriteScenario_int(scn,"INDEX",program.group);
	for (int i = 0; i < set.size(); i++)
	{
		set[i].save(scn);
	}
	oapiWriteScenario_int(scn,"SEQUENCE",sequence->index);
	oapiWriteScenario_float(scn,"TIME",startTime);

	oapiWriteScenario_string(scn,ChecklistContainerEndString,"");
}
// Todo: Verify
void ChecklistContainer::load(FILEHANDLE scn, ChecklistController &controller)
{
	char *line,buffer[100];
	line = "";
	buffer[0] = 0;
	int integer;
	float fcpt;

	oapiReadScenario_nextline(scn,line);
	if (!strnicmp(line,"INDEX",5))
	{
		sscanf (line+5,"%d",&integer);
		if (integer < controller.groups.size()) { 
			*this = ChecklistContainer(controller.groups[integer],controller,0);
		} else
			return;
	}
	else
		return;
	while (strnicmp(line,ChecklistContainerEndString,strlen(ChecklistContainerEndString)))
	{
		bool found = false;
		if (!found && !strnicmp(line,"SEQUENCE",8))
		{
			sscanf(line+8,"%d",&integer);
			if (integer < set.size())			
				sequence+= integer;
			found = true;
		}
		if (!found && !strnicmp(line,"TIME",4))
		{
			sscanf(line+4,"%f",&fcpt);
			startTime = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,ChecklistItemStartString,strlen(ChecklistItemStartString)))
		{
			oapiReadScenario_nextline(scn, line);
			sscanf(line+5,"%d", &integer);
			if (integer < set.size())
				set[integer].load(scn);
		}
		oapiReadScenario_nextline(scn,line);
	}
}
//Saturn Events methods

// Todo: Verify
SaturnEvents::SaturnEvents()
{
	PRIME_CREW_PRELAUNCH = SPLASHDOWN = EARTH_ORBIT_INSERTION = BACKUP_CREW_PRELAUNCH = SECOND_STAGE_STAGING = SIVB_STAGE_STAGING = TOWER_JETTISON = 
		CSM_LV_SEPARATION_DONE = CSM_LV_SEPARATION = CM_SM_SEPARATION_DONE = CM_SM_SEPARATION = TLI = PAYLOAD_EXTRACTION = MINUS_INFINITY;
}
// Todo: Verify
void SaturnEvents::save(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn, SaturnEventStartString, "");

	oapiWriteScenario_float(scn, "BACKUP_CREW_PRELAUNCH", BACKUP_CREW_PRELAUNCH);
	oapiWriteScenario_float(scn, "PRIME_CREW_PRELAUNCH", PRIME_CREW_PRELAUNCH);
	oapiWriteScenario_float(scn, "SECOND_STAGE_STAGING", SECOND_STAGE_STAGING);
	oapiWriteScenario_float(scn, "TOWER_JETTISON", TOWER_JETTISON);
	oapiWriteScenario_float(scn, "SIVB_STAGE_STAGING", SIVB_STAGE_STAGING);
	oapiWriteScenario_float(scn, "EARTH_ORBIT_INSERTION", EARTH_ORBIT_INSERTION);
	oapiWriteScenario_float(scn, "TLI", TLI);
	oapiWriteScenario_float(scn, "CSM_LV_SEPARATION", CSM_LV_SEPARATION);
	oapiWriteScenario_float(scn, "CSM_LV_SEPARATION_DONE", CSM_LV_SEPARATION_DONE);
	oapiWriteScenario_float(scn, "PAYLOAD_EXTRACTION", PAYLOAD_EXTRACTION);
	oapiWriteScenario_float(scn, "CM_SM_SEPARATION", CM_SM_SEPARATION);
	oapiWriteScenario_float(scn, "CM_SM_SEPARATION_DONE", CM_SM_SEPARATION_DONE);
	oapiWriteScenario_float(scn, "SPLASHDOWN", SPLASHDOWN);

	oapiWriteScenario_string(scn, SaturnEventEndString, "");
}
// Todo: Verify
void SaturnEvents::load(FILEHANDLE scn)
{
	float fcpt = 0;
	char *line,buffer[100];
	line = "";
	buffer[0] = 0;
	while (strnicmp(line,SaturnEventEndString,strlen(SaturnEventEndString)))
	{
		bool found = false;
		if (!found && !strnicmp(line,"PAYLOAD_EXTRACTION",18))
		{
			sscanf(line+18,"%f",&fcpt);
			PAYLOAD_EXTRACTION = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"TOWER_JETTISON",14))
		{
			sscanf(line+14,"%f",&fcpt);
			TOWER_JETTISON = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"SECOND_STAGE_STAGING",20))
		{
			sscanf(line+20,"%f",&fcpt);
			SECOND_STAGE_STAGING = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"SPLASHDOWN",10))
		{
			sscanf(line + 10,"%f",&fcpt);
			SPLASHDOWN = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"BACKUP_CREW_PRELAUNCH",21))
		{
			sscanf(line+21,"%f",&fcpt);
			BACKUP_CREW_PRELAUNCH = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"PRIME_CREW_PRELAUNCH",20))
		{
			sscanf(line+20,"%f",&fcpt);
			PRIME_CREW_PRELAUNCH = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"CSM_LV_SEPARATION_DONE",22))
		{
			sscanf(line+22,"%f",&fcpt);
			CSM_LV_SEPARATION_DONE = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"CSM_LV_SEPARATION",17))
		{
			sscanf(line+17,"%f",&fcpt);
			CSM_LV_SEPARATION = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"SIVB_STAGE_STAGING",18))
		{
			sscanf(line+18,"%f",&fcpt);
			SIVB_STAGE_STAGING = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"CM_SM_SEPARATION_DONE",21))
		{
			sscanf(line+21,"%f",&fcpt);
			CM_SM_SEPARATION_DONE = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"CM_SM_SEPARATION",16))
		{
			sscanf(line+16,"%f",&fcpt);
			CM_SM_SEPARATION = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"EARTH_ORBIT_INSERTION",21))
		{
			sscanf(line+21,"%f",&fcpt);
			EARTH_ORBIT_INSERTION = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"TLI",3))
		{
			sscanf(line+3,"%f",&fcpt);
			TLI = fcpt;
			found = true;
		}

		oapiReadScenario_nextline(scn,line);
	}
}

#pragma warning ( pop )
