#include <list>
#include <queue>
#include "ChecklistPanel.h"
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