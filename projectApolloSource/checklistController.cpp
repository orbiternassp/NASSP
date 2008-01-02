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
void ChecklistController::init()
{
	complete = false;
	action = deque<ChecklistContainer>();
	active = 0;
	groups = list<ChecklistGroup>();
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