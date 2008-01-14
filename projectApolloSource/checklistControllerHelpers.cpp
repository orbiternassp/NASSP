#include "checklistController.h"
#include <crtdbg.h>
using namespace std;
//Checklist Item methods.
///Finds out if two checklist items refer to the same actual item.
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

//Checklist Container (program) methods.
ChecklistContainer::ChecklistContainer(const ChecklistContainer &temp)
{
	program = temp.program;
	set = vector<ChecklistItem>(temp.set);
	sequence = set.begin();
	while (sequence != set.end() && sequence->index != temp.sequence->index)
	{
		sequence ++;
	}
}
ChecklistContainer::ChecklistContainer(const ChecklistGroup &groupin, bool load)
{
	/// Since no operations actually happen on the group itself, this is safe.
	program = groupin;
	/// This needs to get initialized to the elements of the group.
	set = vector<ChecklistItem>();
	if (!load)
	{
		// Temporary debugging data, should be removed before release.
		if (program.group == 0)
		{
			ChecklistItem temp;
			temp.automatic = false;
			temp.complete = false;
			temp.failed = false;
			temp.failEvent = NULL;
			temp.group = 0;
			temp.index = 0;
			temp.info = "This is a special checklist item created to hold dummy data.";
			temp.item = "";
			temp.position=0;
			temp.relativeEvent=NO_TIME_DEF;
			temp.text = "Test item 1";
			temp.time=0;
			set.push_back(temp);
			temp.automatic = false;
			temp.complete = false;
			temp.failed = false;
			temp.failEvent = NULL;
			temp.group = 0;
			temp.index = 1;
			temp.info = "This is a special checklist item created to hold dummy data.";
			temp.item = "";
			temp.position=0;
			temp.relativeEvent=NO_TIME_DEF;
			temp.text = "Test item 2";
			temp.time=0;
			set.push_back(temp);
			temp.automatic = false;
			temp.complete = false;
			temp.failed = false;
			temp.failEvent = NULL;
			temp.group = 0;
			temp.index = 2;
			temp.info = "This is a special checklist item created to hold dummy data.";
			temp.item = "";
			temp.position=0;
			temp.relativeEvent=NO_TIME_DEF;
			temp.text = "Test item 3";
			temp.time=0;
			set.push_back(temp);
		}
	}
	// This needs to get placed at the right spot in the future.
	sequence = set.begin();
}
ChecklistContainer::~ChecklistContainer()
{
}