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
ChecklistContainer::ChecklistContainer(ChecklistGroup groupin)
{
	group = groupin;
	set = vector<ChecklistItem>();
	sequence = set.begin();
}
ChecklistContainer::~ChecklistContainer()
{
}
