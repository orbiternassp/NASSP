#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "Saturn.h"
#include "checklistController.h"
using namespace std;
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
ChecklistController::ChecklistController(Saturn *saturn)
{
	init(saturn);
}
ChecklistController::ChecklistController()
{
	init();
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
void ChecklistController::init()
{
	complete = false;
	action = deque<ChecklistContainer>();
	sat = NULL;
	active = 0;
	groups = set<ChecklistGroup,ChecklistGroupcompare>();
	lastMissionTime = 0;
	//Todo: Initial loading of complete list to groups.
	//Todo: Initial loading of proper group to active.
	//Todo: figure out how to save state... potentially spit out the queue in fifo order
	//Todo: Appearance: Checklist <group name> <index>
	//Todo: Figure out how to know when a specific event has happened.
}
void ChecklistController::init(Saturn *saturn)
{
	init();
	sat = saturn;
}
void ChecklistController::LinkCraft(Saturn *saturn)
{
	init(saturn);
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
