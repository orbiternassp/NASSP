#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "checklistController.h"

using namespace std;
ChecklistController::ChecklistController()
{
	init(false);
}
ChecklistController::~ChecklistController()
{

}
void ChecklistController::getChecklistItem(ChecklistItem* input)
{
}
ChecklistGroup *ChecklistController::getChecklistList()
{
	return NULL;
}
bool ChecklistController::failChecklistItem(ChecklistItem* input)
{
	return false;
}
bool ChecklistController::completeChecklistItem(ChecklistItem* input)
{
	return false;
}
bool ChecklistController::autoComplete(bool)
{
	return false;
}
void ChecklistController::clbkTimestep(double missionTime)
{
	lastMissionTime = missionTime;
}
bool ChecklistController::init(bool final)
{
	if (initCalled)
		return false;
	complete = false;
	initCalled = final;
	action = deque<ChecklistContainer>();
	active = NULL;
	groups = set<ChecklistGroup,ChecklistGroupcompare>();
	lastMissionTime = 0;
	return true;
	//Todo: Define the understanding that this is going to be a "inactive" controller.
}
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
void ChecklistController::init()
{
	init(true);
}