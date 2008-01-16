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
// Todo: Verify
ChecklistContainer::ChecklistContainer(const ChecklistGroup &groupin, BasicExcel &file, const vector<ChecklistGroup> &groups)
{
	program = groupin;
	initSet(program,set,file,groups);
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
void ChecklistContainer::initSet(const ChecklistGroup &program,vector<ChecklistItem> &set, BasicExcel &file, const vector<ChecklistGroup> &groups)
{
	BasicExcelWorksheet* sheet;
	BasicExcelCell* cell;
	sheet = file.GetWorksheet(program.name);
	cell = sheet->Cell(1,0);
	char temp [100];
	temp[0] = 0;
	int i = 1;
	ChecklistItem t;
	while (cell->GetString())
	{
		strcpy(temp,cell->GetString());
		strcpy(t.text,temp);
		// Move to time column.
		cell = sheet->Cell(i,1);
		t.time = cell->GetDouble();
		//Move to Relative Event column.
		cell = sheet->Cell(i,2);
		t.relativeEvent=NO_TIME_DEF;
		if (cell->GetString())
		{
			strcpy(temp,cell->GetString());
			// Relative Event parser.
			if(!strnicmp(temp,"CHECKLIST_RELATIVE",18))
				t.relativeEvent=CHECKLIST_RELATIVE;
			else if(!strnicmp(temp,"MISSION_TIME",12))
				t.relativeEvent=MISSION_TIME;
		}
		//Move to info column.
		cell = sheet->Cell(i,3);
		if (cell->GetString())
			strcpy(t.info,cell->GetString());
		else
			t.info[0] = 0;
		//Move to item column.
		cell = sheet->Cell(i,4);
		if (cell->GetString())
			strcpy(t.item,cell->GetString());
		else
			t.item[0] = 0;
		//Move to Position column.
		cell = sheet->Cell(i,5);
		t.position = cell->GetInteger();
		//Move to automatic column.
		cell = sheet->Cell(i,6);
		t.automatic = (cell->GetInteger() != 0);
		//Move to faile event column.
		cell = sheet->Cell(i,7);
		// Find this group
		t.failEvent = -1;
		if (cell->GetString())
		{
			strcpy(temp,cell->GetString());
			if (strnicmp(temp,"__NONE",6))
			{
				for (int i = 0; i < groups.size(); i++)
				{
					if (!strnicmp(groups[i].name,temp,strlen(temp)))
						t.failEvent = i;
				}
			}
		}
		// Done finding group.
		t.index = set.size();
		t.group = program.group;
		set.push_back(t);
		t = ChecklistItem();
		//Move to next row.  Grab Text.
		i++;
		cell = sheet->Cell(i,0);
	}
}