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

// ChecklistContainer methods
// Todo: Believed complete
ChecklistContainer::ChecklistContainer()
{
}
// Todo: Implement load-side activation.
ChecklistContainer::ChecklistContainer(const ChecklistGroup &groupin, bool load)
{
	program = groupin;
	initSet(program,set);
	sequence = set.begin();
}
// Todo: Verify
ChecklistContainer::ChecklistContainer(const ChecklistContainer &input)
{
	// Copy the group over, it is a construct of native items, so should be no problem.
	program = input.program;
	// Copy the set over.  Using the implementation of the vector to do so safely.
	set = input.set;
	// Copy the sequence.  This is the hardest step, as it has to be done precisely.
	sequence = set.begin();
	if (input.set.size() > 0)
	{
		sequence+= input.sequence->index;
	}
}
// Todo: Verify
void ChecklistContainer::operator=(const ChecklistContainer &input)
{
	// Copy the group over, it is a construct of native items, so should be no problem.
	program = input.program;
	// Copy the set over.  Using the implementation of the vector to do so safely.
	set = input.set;
	// Copy the sequence.  This is the hardest step, as it has to be done precisely.
	sequence = set.begin();
	if (input.set.size() > 0)
	{
		sequence+= input.sequence->index;
	}
}
// Todo: Implement
// Cleanup Todo: Remove unnecessary Debug only constructs.
void ChecklistContainer::initSet(const ChecklistGroup &program,vector<ChecklistItem> &set)
{
#ifdef _DEBUG //Todo: remove in cleanup.
	{
		ChecklistItem temp;
		temp.group = program.group;
		temp.index = 0;
		strcpy(temp.info,"This is a special item built for debugging.  It will NOT appear in a release build.");
		sprintf(temp.text,"%s%i","Test item 1 - group ",program.group);
		set.push_back(temp);
		temp.index = 1;
		sprintf(temp.text,"%s%i","Test item 2 - group ",program.group);
		set.push_back(temp);
		temp.index = 2;
		sprintf(temp.text,"%s%i","Test item 3 - group ",program.group);
		set.push_back(temp);
	}
#endif
}