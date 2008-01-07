#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "checklistController.h"

using namespace std;
//Finds out if two checklist items refer to the same actual item.
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
ChecklistGroup ChecklistController::*getChecklistList()
{
	return NULL;
}
bool ChecklistController::failChecklistItem(ChecklistItem* input)
{
	input->failed = true;
	if (input->failEvent.group != -1)
	{
		active->sequence++;
		action.push_front(*active);
		active = new ChecklistContainer(input->failEvent);
	}
	else
	{
		active->sequence++;
	}
	input->group = -1;
	input->index = 0;
	getChecklistItem(input);
	return true;
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
ChecklistContainer::ChecklistContainer(ChecklistGroup groupin)
{
	group = groupin;
	set = vector<ChecklistItem>();
	sequence = set.begin();
}
ChecklistContainer::~ChecklistContainer()
{
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
void ChecklistController::init(char *checkFile)
{
	if(!init(true))
		return;
	//Todo: Initial loading of complete list to groups.
	//Todo: Initial loading of proper group to active.
	//Todo: figure out how to save state... potentially spit out the queue in fifo order
	//Todo: Appearance: Checklist <group name> <index>
	//Todo: Figure out how to know when a specific event has happened.
}
void ChecklistController::init()
{
	init(true);
}
bool ChecklistGroupcompare::operator()(const ChecklistGroup& lhs, const ChecklistGroup& rhs)
{
	if(lhs.relativeEvent < rhs.relativeEvent)
		return true;
	if(lhs.relativeEvent > rhs.relativeEvent)
		return false;
	if(lhs.time < rhs.time)
		return true;
	if(lhs.time > rhs.time)
		return false;
	return false;
}
