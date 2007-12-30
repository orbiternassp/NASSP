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
ChecklistPanel::ChecklistPanel()
{

}
ChecklistPanel::~ChecklistPanel()
{
}
void ChecklistPanel::getChecklistItem(const ChecklistItem* input)
{
}
ChecklistGroup ChecklistPanel::*getChecklistList()
{
	return NULL;
}
bool ChecklistPanel::failChecklistItem(const ChecklistItem* input)
{
	return false;
}
bool ChecklistPanel::completeChecklistItem(const ChecklistItem* input)
{
	return false;
}
bool ChecklistPanel::autoComplete(bool)
{
	return false;
}