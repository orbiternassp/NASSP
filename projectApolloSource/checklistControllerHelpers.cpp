#include "checklistController.h"

// Code to make the compiler shut up.
#pragma warning ( push )
#pragma warning ( disable:4018 )

using namespace std;
// Event enum methods.
// Todo: Verify
RelativeEvent checkEvent(const char* input, bool Group)
{	
	if (!Group && !strnicmp(input,"CHECKLIST_RELATIVE",18))
		return CHECKLIST_RELATIVE;
	if (!strnicmp(input,"EARTH_ORBIT_INSERT",18))
		return EARTH_ORBIT_INSERT;
	if (!strnicmp(input,"PAYLOAD_EXTRACTION",18))
		return PAYLOAD_EXTRACTION;
	if (!strnicmp(input,"MISSION_TIME",12))
		return MISSION_TIME;
	if (!strnicmp(input,"SECONDSTAGE",11))
		return SECONDSTAGE;
	if (!strnicmp(input,"CSM_LV_SEP",10))
		return CSM_LV_SEP;
	if (!strnicmp(input,"TOWER_JETT",10))
		return TOWER_JETT;
	if (!strnicmp(input,"SPLASHDOWN",10))
		return SPLASHDOWN;
	if (!strnicmp(input,"CSMSTARTUP",10))
		return CSMSTARTUP;
	if (!strnicmp(input,"SIVBSTAGE",9))
		return SIVBSTAGE;
	if (!strnicmp(input,"CSM_SEP",7))
		return CSM_SEP;
	if (!strnicmp(input,"TLI",3))
		return TLI;
	return NO_TIME_DEF;
}
//Checklist Item methods.

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
		strncpy(info,cells[3].GetString(),100);
	if (cells[4].GetString())
		strncpy(item,cells[4].GetString(),100);
	position = cells[5].GetInteger();
	automatic = (cells[6].GetInteger() != 0);
	failEvent = -1;
	if (cells[7].GetString())
	{
		for (int i = 0; i < groups.size(); i ++)
		{
			if (!strnicmp(cells[7].GetString(),groups[i].name,strlen(cells[7].GetString())))
				failEvent = i;
		}
	}
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
		oapiReadScenario_nextline(scn, line);
	}
}
// Todo: Verify
void ChecklistItem::save(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn,ChecklistItemStartString,"");
	oapiWriteScenario_int(scn,"INDEX",index);
	oapiWriteScenario_int(scn,"STATUS",status);

	oapiWriteScenario_string(scn,ChecklistItemEndString,"");
}
// Todo: Verify
bool ChecklistItem::checkExec(double lastMissionTime, double checklistStart,SaturnEvents &eventController)
{
	if (!automatic)
		return false;
	if (position == -1)
		return true;
	double t = 0;
	switch(relativeEvent)
	{
	case CHECKLIST_RELATIVE:
		t = lastMissionTime - checklistStart;
		if (time > t)
			return false;
		break;
	case MISSION_TIME:
		if (time > lastMissionTime)
			return false;
		break;
	case EARTH_ORBIT_INSERT:
		if (eventController.EOI == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.EOI;
		if (time > t)
			return false;
		break;
	case SPLASHDOWN:
		if (eventController.splashdown == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.splashdown;
		if (time > t)
			return false;
		break;
	case CSMSTARTUP:
		if (eventController.CSMStartup == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.CSMStartup;
		if (time > t)
			return false;
		break;
	case SECONDSTAGE:
		if (eventController.SecondStage == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.SecondStage;
		if (time > t)
			return false;
		break;
	case SIVBSTAGE:
		if (eventController.SIVBStage == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.SIVBStage;
		if (time > t)
			return false;
		break;
	case TOWER_JETT:
		if (eventController.Tower_Jettison == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.Tower_Jettison;
		if (time > t)
			return false;
		break;
	case CSM_LV_SEP:
		if (eventController.CSM_LV_SEP == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.CSM_LV_SEP;
		if (time > t)
			return false;
		break;
	case CSM_SEP:
		if (eventController.CSM_SEP == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.CSM_SEP;
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
		if (eventController.Payload_Extraction == MINUS_INFINITY)
			return false;
		t = lastMissionTime - eventController.Payload_Extraction;
		if (time > t)
			return false;
		break;
	}
	return true;
}
//ChecklistGroup methods.

// Todo: Verify
void ChecklistGroup::init(std::vector<BasicExcelCell> &cells)
{
	if (cells[0].GetString())
		strncpy(name,cells[0].GetString(),100);
	time = cells[1].GetDouble();
	deadline = cells[2].GetDouble();
	if (cells[3].GetString())
		relativeEvent = checkEvent(cells[3].GetString(),true);
	autoSelect = (cells[4].GetInteger() != 0);
	manualSelect = (cells[5].GetInteger() != 0);
	essential = (cells[6].GetInteger() != 0);
	if (cells[7].GetString())
		strncpy(soundFile,cells[7].GetString(),100);
	autoSlow = (cells[8].GetInteger() != 0);
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
		if (time <= lastMissionTime && lastMissionTime <= deadline)
			return true;
		break;
	case EARTH_ORBIT_INSERT:
		if (eventController.EOI != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.EOI;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case SPLASHDOWN:
		if (eventController.splashdown != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.splashdown;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case CSMSTARTUP:
		if (eventController.CSMStartup != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CSMStartup;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case SECONDSTAGE:
		if (eventController.SecondStage != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CSMStartup;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case SIVBSTAGE:
		if (eventController.SIVBStage != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.SIVBStage;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case TOWER_JETT:
		if (eventController.Tower_Jettison != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.Tower_Jettison;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case CSM_LV_SEP:
		if (eventController.CSM_LV_SEP != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CSM_LV_SEP;
			if (time <= t && t <= deadline)
				return true;
		}
		break;
	case CSM_SEP:
		if (eventController.CSM_SEP != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.CSM_SEP;
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
		if (eventController.Payload_Extraction != MINUS_INFINITY)
		{
			double t = lastMissionTime - eventController.Payload_Extraction;
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
ChecklistContainer::ChecklistContainer(const ChecklistGroup &inprogram, ChecklistController &controller, double start)
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
		for (int ii = 0; ii < 8; ii++)
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
		*this = ChecklistContainer(controller.groups[integer],controller,0);
	}
	else
		return;
	while (strnicmp(line,ChecklistContainerEndString,strlen(ChecklistContainerEndString)))
	{
		bool found = false;
		if (!found && !strnicmp(line,"SEQUENCE",8))
		{
			sscanf(line+8,"%d",&integer);
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
			oapiReadScenario_nextline(scn,line);
			sscanf(line+5,"%d",&integer);
			set[integer].load(scn);
		}
		oapiReadScenario_nextline(scn,line);
	}
}
//Saturn Events methods

// Todo: Verify
SaturnEvents::SaturnEvents()
{
	splashdown = EOI = CSMStartup = SecondStage = SIVBStage = Tower_Jettison = 
		CSM_LV_SEP = CSM_SEP = TLI = Payload_Extraction = MINUS_INFINITY;
}
// Todo: Verify
void SaturnEvents::save(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn,SaturnEventStartString,"");
	oapiWriteScenario_float(scn,"splashdown",splashdown);
	oapiWriteScenario_float(scn,"EOI",EOI);
	oapiWriteScenario_float(scn,"CSMStartup",CSMStartup);
	oapiWriteScenario_float(scn,"SecondStage",SecondStage);
	oapiWriteScenario_float(scn,"SIVBStage",SIVBStage);
	oapiWriteScenario_float(scn,"Tower_Jettison",Tower_Jettison);
	oapiWriteScenario_float(scn,"CSM_LV_SEP",CSM_LV_SEP);
	oapiWriteScenario_float(scn,"CSM_SEP",CSM_SEP);
	oapiWriteScenario_float(scn,"TLI",TLI);
	oapiWriteScenario_float(scn,"Payload_Extraction",Payload_Extraction);

	oapiWriteScenario_string(scn,SaturnEventEndString,"");
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
		if (!found && !strnicmp(line,"Payload_Extraction",18))
		{
			sscanf(line+18,"%f",&fcpt);
			Payload_Extraction = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"Tower_Jettison",14))
		{
			sscanf(line+14,"%f",&fcpt);
			Tower_Jettison = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"SecondStage",11))
		{
			sscanf(line+11,"%f",&fcpt);
			SecondStage = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"splashdown",10))
		{
			sscanf(line + 10,"%f",&fcpt);
			splashdown = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"CSMStartup",10))
		{
			sscanf(line+10,"%f",&fcpt);
			CSMStartup = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"CSM_LV_SEP",10))
		{
			sscanf(line+10,"%f",&fcpt);
			CSM_LV_SEP = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"SIVBStage",9))
		{
			sscanf(line+9,"%f",&fcpt);
			SIVBStage = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"CSM_SEP",7))
		{
			sscanf(line+7,"%f",&fcpt);
			CSM_SEP = fcpt;
			found = true;
		}
		if (!found && !strnicmp(line,"EOI",3))
		{
			sscanf(line+3,"%f",&fcpt);
			EOI = fcpt;
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