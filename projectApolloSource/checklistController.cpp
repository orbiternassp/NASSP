#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "checklistController.h"

using namespace std;
//Todo: none
ChecklistController::ChecklistController()
{
	// This SHOULD be final implementation.  Any further initializing should be handled between init and load.
	initCalled = false;
	init(false);
}
//Todo: none
ChecklistController::~ChecklistController()
{
}
//todo: implement
bool ChecklistController::getChecklistItem(ChecklistItem* input)
{
#ifdef _DEBUG
	return true;
#endif
	return false;
}
//todo: Implement checklistgroup collection and return.
bool ChecklistController::getChecklistList(vector<ChecklistGroup>*)
{
	return false;
}
//todo: Implement failure action code.
bool ChecklistController::failChecklistItem(ChecklistItem* input)
{
	return false;
}
//todo: Implement stepping code.
bool ChecklistController::completeChecklistItem(ChecklistItem* input)
{
	return false;
}
//todo: implement with auto-complete part of timestep function.
bool ChecklistController::autoComplete(bool)
{
	return false;
}
//todo: asses and implement.
void ChecklistController::clbkTimestep(double missionTime)
{
	lastMissionTime = missionTime;
}
//todo: any further non-file specific initialization needed.
//cleanup: remove unnecessary debug-only constructs
bool ChecklistController::init(bool final)
{
	if (initCalled)
		return false;
	complete = false;
	initCalled = final;
	lastMissionTime = 0;
#ifdef _DEBUG //Todo: remove once file loading code is implemented.
	{
		ChecklistGroup temp;
		temp.autoSelect = true;
		temp.essential = false;
		temp.group = 0;
		temp.manualSelect = true;
		strcpy(temp.name,"Test Group");
		temp.relativeEvent = MISSION_TIME;
		temp.time = -3840;
		groups.push_back(temp);
	}
#endif
	return true;
	//Todo: Define the understanding that this is going to be a "inactive" controller.
}
//todo: All file specific initialization.
bool ChecklistController::init(char *checkFile)
{
	if(!init(true))
		return false;
	//Todo: Initial loading of complete list of groups.
	//Todo: Initial loading of proper group to active.
	//Todo: figure out how to save state... potentially spit out the queue in fifo order
	//Todo: Appearance: Checklist <group name> <index>
	//Todo: Figure out how to know when a specific event has happened.

	return true;
}
//todo: believed complete, verify.
void ChecklistController::init()
{
	init(true);
}
//todo: fully implement save/load functions.
void ChecklistController::save(FILEHANDLE scn)
{
	return;
	oapiWriteScenario_string(scn,ChecklistControllerStartString,"");
	oapiWriteScenario_string(scn,ChecklistControllerEndString,"");
}
//todo: fully implement save/load functions.
void ChecklistController::load(FILEHANDLE scn)
{
	char *line, buffer[100];
	line = "";
	while (strnicmp(line,ChecklistControllerEndString,strlen(ChecklistControllerEndString)))
	{
		oapiReadScenario_nextline(scn,line);
	}
}
//todo: implement
bool ChecklistController::autoExecute(bool set)
{
	bool temp = autoexecute;
	autoexecute = set;
	return temp;
}
//todo: implement
bool ChecklistController::autoExecute()
{
	return autoexecute;
}
//todo: implement
bool ChecklistController::spawnCheck(int group, bool automagic)
{
	active = ChecklistContainer(groups[group]);
	return true;
}